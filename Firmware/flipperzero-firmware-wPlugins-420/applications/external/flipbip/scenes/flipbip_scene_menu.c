#include "../flipbip.h"
#include "../helpers/flipbip_file.h"

enum SubmenuIndex {
    SubmenuIndexScene1BTC = 10,
    SubmenuIndexScene1ETH,
    SubmenuIndexScene1DOGE,
    SubmenuIndexScene1New,
    SubmenuIndexScene1Import,
    SubmenuIndexSettings,
};

void flipbip_scene_menu_submenu_callback(void* context, uint32_t index) {
    FlipBip* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void flipbip_scene_menu_on_enter(void* context) {
    FlipBip* app = context;

    if(flipbip_has_file(FlipBipFileKey, NULL, false) &&
       flipbip_has_file(FlipBipFileDat, NULL, false)) {
        submenu_add_item(
            app->submenu,
            "View BTC wallet",
            SubmenuIndexScene1BTC,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "View ETH wallet",
            SubmenuIndexScene1ETH,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "View DOGE wallet",
            SubmenuIndexScene1DOGE,
            flipbip_scene_menu_submenu_callback,
            app);
        submenu_add_item(
            app->submenu,
            "Regenerate wallet",
            SubmenuIndexScene1New,
            flipbip_scene_menu_submenu_callback,
            app);
    } else {
        submenu_add_item(
            app->submenu,
            "Generate new wallet",
            SubmenuIndexScene1New,
            flipbip_scene_menu_submenu_callback,
            app);
    }
    submenu_add_item(
        app->submenu,
        "Import from mnemonic",
        SubmenuIndexScene1Import,
        flipbip_scene_menu_submenu_callback,
        app);

    submenu_add_item(
        app->submenu, "Settings", SubmenuIndexSettings, flipbip_scene_menu_submenu_callback, app);

    submenu_set_selected_item(
        app->submenu, scene_manager_get_scene_state(app->scene_manager, FlipBipSceneMenu));

    view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdMenu);
}

bool flipbip_scene_menu_on_event(void* context, SceneManagerEvent event) {
    FlipBip* app = context;
    //UNUSED(app);
    if(event.type == SceneManagerEventTypeBack) {
        //exit app
        scene_manager_stop(app->scene_manager);
        view_dispatcher_stop(app->view_dispatcher);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexScene1BTC) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinBTC0;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1BTC);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1ETH) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinETH60;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1ETH);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1DOGE) {
            app->overwrite_saved_seed = 0;
            app->import_from_mnemonic = 0;
            app->bip44_coin = FlipBipCoinDOGE3;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1DOGE);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1New) {
            app->overwrite_saved_seed = 1;
            app->import_from_mnemonic = 0;
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexScene1New);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneScene_1);
            return true;
        } else if(event.event == SubmenuIndexScene1Import) {
            app->import_from_mnemonic = 1;
            app->input_state = FlipBipTextInputMnemonic;
            text_input_set_header_text(app->text_input, "Enter mnemonic phrase");
            view_dispatcher_switch_to_view(app->view_dispatcher, FlipBipViewIdTextInput);
            return true;
        } else if(event.event == SubmenuIndexSettings) {
            scene_manager_set_scene_state(
                app->scene_manager, FlipBipSceneMenu, SubmenuIndexSettings);
            scene_manager_next_scene(app->scene_manager, FlipBipSceneSettings);
            return true;
        }
    }
    return false;
}

void flipbip_scene_menu_on_exit(void* context) {
    FlipBip* app = context;
    submenu_reset(app->submenu);
}