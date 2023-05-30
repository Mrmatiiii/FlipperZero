#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <notification/notification_messages.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/scene_manager.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include "scenes/flipbip_scene.h"
#include "views/flipbip_startscreen.h"
#include "views/flipbip_scene_1.h"

#define FLIPBIP_VERSION "v0.0.9"

#define COIN_BTC 0
#define COIN_DOGE 3
#define COIN_ETH 60

#define TEXT_BUFFER_SIZE 256

typedef struct {
    Gui* gui;
    NotificationApp* notification;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    SceneManager* scene_manager;
    VariableItemList* variable_item_list;
    TextInput* text_input;
    FlipBipStartscreen* flipbip_startscreen;
    FlipBipScene1* flipbip_scene_1;
    // Settings options
    int haptic;
    int led;
    int bip39_strength;
    int passphrase;
    // Main menu options
    int bip44_coin;
    int overwrite_saved_seed;
    int import_from_mnemonic;
    // Text input
    int input_state;
    char passphrase_text[TEXT_BUFFER_SIZE];
    char import_mnemonic_text[TEXT_BUFFER_SIZE];
    char input_text[TEXT_BUFFER_SIZE];
} FlipBip;

typedef enum {
    FlipBipViewIdStartscreen,
    FlipBipViewIdMenu,
    FlipBipViewIdScene1,
    FlipBipViewIdSettings,
    FlipBipViewIdTextInput,
} FlipBipViewId;

typedef enum {
    FlipBipHapticOff,
    FlipBipHapticOn,
} FlipBipHapticState;

typedef enum {
    FlipBipLedOff,
    FlipBipLedOn,
} FlipBipLedState;

typedef enum {
    FlipBipStrength128,
    FlipBipStrength192,
    FlipBipStrength256,
} FlipBipStrengthState;

typedef enum {
    FlipBipPassphraseOff,
    FlipBipPassphraseOn,
} FlipBipPassphraseState;

typedef enum {
    FlipBipCoinBTC0,
    FlipBipCoinETH60,
    FlipBipCoinDOGE3,
} FlipBipCoin;

typedef enum {
    FlipBipTextInputDefault,
    FlipBipTextInputPassphrase,
    FlipBipTextInputMnemonic
} FlipBipTextInputState;

typedef enum {
    FlipBipStatusSuccess = 0,
    FlipBipStatusReturn = 10,
    FlipBipStatusLoadError = 11,
    FlipBipStatusSaveError = 12,
    FlipBipStatusMnemonicCheckError = 13,
} FlipBipStatus;
