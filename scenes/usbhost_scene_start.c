#include "usbhost_i.h"

#define TAG "UsbHostSceneStart"

void usbhost_scene_start_on_enter(void* context) {
    USBHostApp* app = context;

    TextBox* text_box = app->text_box;
    text_box_reset(app->text_box);
    text_box_set_font(text_box, TextBoxFontText);
    text_box_set_focus(text_box, TextBoxFocusEnd);

    // Set starting text
    text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));

    scene_manager_set_scene_state(app->scene_manager, USBHostSceneStart, 0);
    view_dispatcher_switch_to_view(app->view_dispatcher, USBHostAppViewOutput);

    app->worker = usbhost_worker_init(app);
    // while(true) max3421e_process();
    // Register callback to receive data
    //usbhost_set_handle_rx_data_cb(
    //    app->uart, uart_terminal_console_output_handle_rx_data_cb); // setup callback for rx thread
}

bool usbhost_scene_start_on_event(void* context, SceneManagerEvent event) {
    USBHostApp* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        text_box_set_text(app->text_box, furi_string_get_cstr(app->text_box_store));
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }

    return consumed;
}

void usbhost_scene_start_on_exit(void* context) {
    USBHostApp* app = context;

    usbhost_worker_free(app->worker);
    // Unregister rx callback
    //uart_terminal_uart_set_handle_rx_data_cb(app->uart, NULL);
}