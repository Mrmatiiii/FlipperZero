
#pragma once

#include <math.h>
#include <furi.h>
#include <m-list.h>
#include <furi_hal.h>
#include <lib/flipper_format/flipper_format.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SUBGHZ_SETTING_FILE_TYPE "Flipper SubGhz Setting File"
#define SUBGHZ_SETTING_FILE_VERSION 1

#define SUBGHZ_SETTING_DEFAULT_PRESET_COUNT 4

LIST_DEF(FrequencyList, uint32_t)

#define M_OPL_FrequencyList_t() LIST_OPLIST(FrequencyList)

typedef struct SubGhzSetting SubGhzSetting;

SubGhzSetting* subghz_setting_alloc(void);

void subghz_setting_free(SubGhzSetting* instance);

void subghz_setting_load(SubGhzSetting* instance, const char* file_path);

size_t subghz_setting_get_frequency_count(SubGhzSetting* instance);

size_t subghz_setting_get_hopper_frequency_count(SubGhzSetting* instance);

size_t subghz_setting_get_preset_count(SubGhzSetting* instance);

const char* subghz_setting_get_preset_name(SubGhzSetting* instance, size_t idx);

int subghz_setting_get_inx_preset_by_name(SubGhzSetting* instance, const char* preset_name);

uint8_t* subghz_setting_get_preset_data(SubGhzSetting* instance, size_t idx);

size_t subghz_setting_get_preset_data_size(SubGhzSetting* instance, size_t idx);

uint8_t* subghz_setting_get_preset_data_by_name(SubGhzSetting* instance, const char* preset_name);

bool subghz_setting_load_custom_preset(
    SubGhzSetting* instance,
    const char* preset_name,
    FlipperFormat* fff_data_file);

bool subghz_setting_delete_custom_preset(SubGhzSetting* instance, const char* preset_name);

uint32_t subghz_setting_get_frequency(SubGhzSetting* instance, size_t idx);

uint32_t subghz_setting_get_hopper_frequency(SubGhzSetting* instance, size_t idx);

uint32_t subghz_setting_get_frequency_default_index(SubGhzSetting* instance);

uint32_t subghz_setting_get_default_frequency(SubGhzSetting* instance);

void subghz_setting_set_default_frequency(SubGhzSetting* instance, uint32_t frequency_to_setup);

#ifdef __cplusplus
}
#endif