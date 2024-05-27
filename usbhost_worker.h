#pragma once

#include "furi_hal.h"

#define RX_BUF_SIZE (320)

typedef struct USBWorker USBWorker;

typedef enum {
    USBWorkerStop = (1 << 0),
    USBWorkerRxDone = (1 << 1),
    USBWorkerMount = (1 << 2),
    USBWorkerUmount = (1 << 3),
} USBWorkerFlags;

#define WORKER_FLAGS_MASK (USBWorkerStop | USBWorkerRxDone | USBWorkerMount | USBWorkerUmount)

USBWorker* usbhost_worker_init(USBHostApp* app);
void usbhost_worker_free(USBWorker* worker);
