#include "usbhost_i.h"
#include "usbhost_worker.h"

#include "helpers/tu_helpers.h"
#include "glue.h"

#define TAG "USBHostWorker"

struct USBWorker {
    USBHostApp* app;
    FuriThread* thread;
    FuriStreamBuffer* stream;
    uint8_t buffer[RX_BUF_SIZE + 1];
    // uint8_t daddr;
};

tusb_desc_device_t desc_device;

/*
void uart_terminal_uart_set_handle_rx_data_cb(
    UART_TerminalUart* uart,
    void (*handle_rx_data_cb)(uint8_t* buf, size_t len, void* context)) {
    furi_assert(uart);
    uart->handle_rx_data_cb = handle_rx_data_cb;
}

#define WORKER_FLAGS_MASK (WorkerEvtStop | WorkerEvtRxDone)

void uart_terminal_uart_on_irq_cb(
    FuriHalSerialHandle* handle,
    FuriHalSerialRxEvent event,
    void* context) {
    UART_TerminalUart* uart = (UART_TerminalUart*)context;

    if(event == FuriHalSerialRxEventData) {
        uint8_t data = furi_hal_serial_async_rx(handle);
        furi_stream_buffer_send(uart->rx_stream, &data, 1, 0);
        furi_thread_flags_set(furi_thread_get_id(uart->rx_thread), WorkerEvtRxDone);
    }
}
*/

/*------------- TinyUSB Callbacks -------------*/

// Invoked when device is mounted (configured)
void tuh_mount_cb(uint8_t daddr) {
    FURI_LOG_I(TAG, "Device attached, address = %d\r\n", daddr);
    furi_thread_flags_set(furi_thread_get_current_id(), USBWorkerMount);

    // Get Device Descriptor
    // TODO: invoking control transfer now has issue with mounting hub with multiple devices attached, fix later
    //tuh_descriptor_get_device(daddr, &desc_device, 18, print_device_descriptor, 0);
}

/// Invoked when device is unmounted (bus reset/unplugged)
void tuh_umount_cb(uint8_t daddr) {
    FURI_LOG_I(TAG, "Device removed, address = %d\r\n", daddr);
    furi_thread_flags_set(furi_thread_get_current_id(), USBWorkerUmount);

    // free_hid_buf(daddr);
}

/*------------- Worker Code -------------*/

static int32_t usbhost_worker(void* p) {
    USBWorker* worker = p;
    UNUSED(worker);
    FURI_LOG_I(TAG, "usbhost_worker() started");

    while(1) {
        tuh_task();
        uint32_t flags = furi_thread_flags_wait(WORKER_FLAGS_MASK, FuriFlagWaitAny, 100);
        // FuriWaitForever
        if(flags & FuriFlagErrorTimeout) continue;
        FURI_LOG_T(TAG, "usbhost_worker() flags: %lx", flags);
        furi_check((flags & FuriFlagError) == 0);

        if(flags & USBWorkerStop) break;

        if(flags & USBWorkerRxDone) {
            FURI_LOG_I(TAG, "Worker flag: RX done");
            //size_t len =
            //    furi_stream_buffer_receive(worker->stream, worker->buffer, RX_BUF_SIZE, 0);
            //if(len > 0) {
            //if(worker->handle_rx_data_cb) worker->handle_rx_data_cb(worker->buffer, len, worker->app);
            //}
        }

        if(flags & USBWorkerMount) {
            FURI_LOG_I(TAG, "Worker flag: device mounted");
        }
        if(flags & USBWorkerUmount) {
            FURI_LOG_I(TAG, "Worker flag: device unmounted");
        }
    }

    FURI_LOG_I(TAG, "usbhost_worker() loop exited");

    furi_stream_buffer_free(worker->stream);

    return 0;
}

USBWorker* usbhost_worker_init(USBHostApp* app) {
    FURI_LOG_I(TAG, "usbhost_worker_init() started");
    USBWorker* worker = malloc(sizeof(USBWorker));

    worker->app = app;
    worker->stream = furi_stream_buffer_alloc(RX_BUF_SIZE, 1);
    worker->thread = furi_thread_alloc();

    furi_thread_set_name(worker->thread, "USBHostWorkerThread");
    furi_thread_set_stack_size(worker->thread, 1024);
    furi_thread_set_context(worker->thread, worker);
    furi_thread_set_callback(worker->thread, usbhost_worker);
    // furi_thread_set_priority(worker->thread, FuriThreadPriorityIdle);

    board_init();
    FURI_LOG_I(TAG, "board_init() finished");
    tuh_init(BOARD_TUH_RHPORT);
    FURI_LOG_I(TAG, "tuh_init() finished");

    furi_thread_start(worker->thread);

    return worker;
}

void usbhost_worker_free(USBWorker* worker) {
    furi_assert(worker);

    // furi_hal_serial_async_rx_stop(uart->serial_handle);
    // furi_hal_serial_deinit(uart->serial_handle);
    // furi_hal_serial_control_release(uart->serial_handle);

    furi_thread_flags_set(furi_thread_get_id(worker->thread), USBWorkerStop);
    furi_thread_join(worker->thread);
    furi_thread_free(worker->thread);

    free(worker);

    tuh_deinit(BOARD_TUH_RHPORT);
    board_deinit();
    FURI_LOG_I(TAG, "usbhost worker freed");
}
