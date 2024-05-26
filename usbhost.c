#include "usbhost_i.h"

#define TAG "USBHost"

static bool usbhost_app_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    USBHostApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool usbhost_app_back_event_callback(void* context) {
    furi_assert(context);
    USBHostApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

static void usbhost_app_tick_event_callback(void* context) {
    furi_assert(context);
    USBHostApp* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

USBHostApp* usbhost_app_alloc() {
    USBHostApp* app = malloc(sizeof(USBHostApp));

    app->gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager = scene_manager_alloc(&usbhost_scene_handlers, app);
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, usbhost_app_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, usbhost_app_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        app->view_dispatcher, usbhost_app_tick_event_callback, 100);

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, USBHostAppViewOutput, text_box_get_view(app->text_box));
    app->text_box_store = furi_string_alloc();
    furi_string_reserve(app->text_box_store, USBHOST_TEXT_BOX_STORE_SIZE);

    furi_hal_power_enable_otg();
    // max3421e_init();
    // furi_hal_spi_bus_handle_init(USBHOST_SPI_HANDLE);

    scene_manager_next_scene(app->scene_manager, USBHostSceneStart);

    return app;
}

void usbhost_app_free(USBHostApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, USBHostAppViewOutput);

    text_box_free(app->text_box);
    furi_string_free(app->text_box_store);

    // View dispatcher
    view_dispatcher_free(app->view_dispatcher);
    scene_manager_free(app->scene_manager);

    // Close records
    furi_record_close(RECORD_GUI);

    // furi_hal_spi_bus_handle_deinit(USBHOST_SPI_HANDLE);
    // max3421e_deinit();
    furi_hal_power_disable_otg();

    free(app);
}

int32_t usbhost_app(void* p) {
    UNUSED(p);

    USBHostApp* app = usbhost_app_alloc();

    view_dispatcher_run(app->view_dispatcher);

    usbhost_app_free(app);

    return 0;
}
