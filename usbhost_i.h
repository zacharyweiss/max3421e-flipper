#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_box.h>
#include <dialogs/dialogs.h>

#include "scenes/usbhost_scene.h"

#include "tusb.h"
#include "class/hid/hid.h"

// English
#define LANGUAGE_ID 0x0409
#define BUF_COUNT 4

// #include "MAX3421E.h"

#define USBHOST_TEXT_BOX_STORE_SIZE (4096)
// #define USBHOST_SPI_HANDLE &furi_hal_spi_bus_handle_external

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    TextBox* text_box;
    FuriString* text_box_store;
    size_t text_box_store_strlen;
} USBHostApp;

typedef enum {
    USBHostEventRefreshConsoleOutput = 0,
    USBHostEventStartConsole,
} USBHostCustomEvent;

typedef enum {
    USBHostAppViewOutput,
} USBHostAppView;
