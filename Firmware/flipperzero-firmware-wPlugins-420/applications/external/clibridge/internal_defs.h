#pragma once
#include <furi.h>
#include <furi_hal.h>
#include <m-dict.h>
#include <m-bptree.h>
#include <m-array.h>
#include <cli/cli.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/menu.h>
#include <gui/modules/submenu.h>

typedef struct {
    FuriThreadStdoutWriteCallback write_callback;
    FuriString* buffer;
} FuriThreadStdout_internal;

typedef struct {
    bool is_service;
    FuriThreadState state;
    int32_t ret;

    FuriThreadCallback callback;
    void* context;

    FuriThreadStateCallback state_callback;
    void* state_context;

    char* name;
    configSTACK_DEPTH_TYPE stack_size;
    FuriThreadPriority priority;

    TaskHandle_t task_handle;
    bool heap_trace_enabled;
    size_t heap_size;

    FuriThreadStdout_internal output;
} FuriThread_internal;

DICT_DEF2(ViewDict, uint32_t, M_DEFAULT_OPLIST, View*, M_PTR_OPLIST)
typedef struct {
    FuriMessageQueue* queue;
    Gui* gui;
    ViewPort* view_port;
    ViewDict_t views;

    View* current_view;

    View* ongoing_input_view;
    uint8_t ongoing_input;

    ViewDispatcherCustomEventCallback custom_event_callback;
    ViewDispatcherNavigationEventCallback navigation_event_callback;
    ViewDispatcherTickEventCallback tick_event_callback;
    uint32_t tick_period;
    void* event_context;
} ViewDispatcher_internal;

typedef struct {
    Gui* gui;
    bool is_enabled;
    ViewPortOrientation orientation;

    uint8_t width;
    uint8_t height;

    ViewPortDrawCallback draw_callback;
    void* draw_callback_context;

    ViewPortInputCallback input_callback;
    void* input_callback_context;
} ViewPort_internal;

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    Menu* primary_menu;
    Submenu* settings_menu;

    void (*closed_callback)(void*);
    void* closed_callback_context;

    void (*click_callback)(const char*, void*);
    void* click_callback_context;

    FuriThread* thread;
} LoaderMenu_internal;

typedef struct {
    char* args;
    char* name;
    FuriThread* thread;
    bool insomniac;
} LoaderAppData_internal;

typedef struct {
    FuriPubSub* pubsub;
    FuriMessageQueue* queue;
    LoaderMenu_internal* loader_menu;
    LoaderAppData_internal app;
} Loader_internal;

typedef struct {
    CliCallback callback;
    void* context;
    uint32_t flags;
} CliCommand_internal;

#define CLI_COMMANDS_TREE_RANK 4
BPTREE_DEF2(
    CliCommandTree_internal,
    CLI_COMMANDS_TREE_RANK,
    FuriString*,
    FURI_STRING_OPLIST,
    CliCommand_internal,
    M_POD_OPLIST)

#define M_OPL_CliCommandTree_internal_t() BPTREE_OPLIST(CliCommandTree_internal, M_POD_OPLIST)

typedef struct {
    CliCommandTree_internal_t commands;
    void* mutex;
    void* idle_sem;
    void* last_line;
    void* line;
    void* session;

    size_t cursor_position;
} Cli_internal;