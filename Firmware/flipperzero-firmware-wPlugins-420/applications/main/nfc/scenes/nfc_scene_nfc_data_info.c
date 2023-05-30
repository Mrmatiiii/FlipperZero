#include "../nfc_i.h"
#include <inttypes.h>

void nfc_scene_nfc_data_info_widget_callback(GuiButtonType result, InputType type, void* context) {
    Nfc* nfc = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, result);
    }
}

uint32_t nfc_scene_nfc_data_info_get_key(uint8_t* data) {
    uint32_t value = 0;

    for(uint32_t pos = 0; pos < 4; pos++) {
        value <<= 8;
        value |= data[pos];
    }

    return value;
}

void nfc_scene_nfc_data_info_on_enter(void* context) {
    Nfc* nfc = context;
    Widget* widget = nfc->widget;
    FuriHalNfcDevData* nfc_data = &nfc->dev->dev_data.nfc_data;
    FuriHalNfcType type = nfc_data->type;
    NfcDeviceData* dev_data = &nfc->dev->dev_data;
    NfcProtocol protocol = dev_data->protocol;
    uint8_t text_scroll_height = 0;
    if((protocol == NfcDeviceProtocolMifareDesfire) || (protocol == NfcDeviceProtocolMifareUl) ||
       (protocol == NfcDeviceProtocolMifareClassic) || (protocol == NfcDeviceProtocolNfcV)) {
        widget_add_button_element(
            widget, GuiButtonTypeRight, "More", nfc_scene_nfc_data_info_widget_callback, nfc);
        text_scroll_height = 52;
    } else {
        text_scroll_height = 64;
    }

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    // Set name if present
    if(nfc->dev->dev_name[0] != '\0') {
        furi_string_printf(temp_str, "\ec%s\n", nfc->dev->dev_name);
    }

    // Set tag type
    if(protocol == NfcDeviceProtocolEMV) {
        furi_string_cat_printf(temp_str, "\e#EMV Bank Card\n");
    } else if(protocol == NfcDeviceProtocolMRTD) {
        furi_string_cat_printf(temp_str, "\e#Passport/ID\n");
    } else if(protocol == NfcDeviceProtocolMifareUl) {
        furi_string_cat_printf(
            temp_str, "\e#%s\n", nfc_mf_ul_type(dev_data->mf_ul_data.type, true));
    } else if(protocol == NfcDeviceProtocolMifareClassic) {
        furi_string_cat_printf(
            temp_str, "\e#%s\n", nfc_mf_classic_type(dev_data->mf_classic_data.type));
    } else if(protocol == NfcDeviceProtocolMifareDesfire) {
        furi_string_cat_printf(temp_str, "\e#MIFARE DESFire\n");
    } else if(protocol == NfcDeviceProtocolNfcV) {
        switch(dev_data->nfcv_data.sub_type) {
        case NfcVTypePlain:
            furi_string_cat_printf(temp_str, "\e#ISO15693\n");
            break;
        case NfcVTypeSlix:
            furi_string_cat_printf(temp_str, "\e#ISO15693 SLIX\n");
            break;
        case NfcVTypeSlixS:
            furi_string_cat_printf(temp_str, "\e#ISO15693 SLIX-S\n");
            break;
        case NfcVTypeSlixL:
            furi_string_cat_printf(temp_str, "\e#ISO15693 SLIX-L\n");
            break;
        case NfcVTypeSlix2:
            furi_string_cat_printf(temp_str, "\e#ISO15693 SLIX2\n");
            break;
        default:
            furi_string_cat_printf(temp_str, "\e#ISO15693 (unknown)\n");
            break;
        }
    } else if(protocol == NfcDeviceProtocolFelica) {
        furi_string_cat_printf(temp_str, "\e#%s\n", nfc_felica_type(dev_data->felica_data.type));
    } else {
        furi_string_cat_printf(temp_str, "\e#Unknown ISO tag\n");
    }

    // Set tag iso data
    if(protocol == NfcDeviceProtocolNfcV) {
        NfcVData* nfcv_data = &nfc->dev->dev_data.nfcv_data;

        furi_string_cat_printf(temp_str, "UID:\n");
        for(size_t i = 0; i < nfc_data->uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc_data->uid[i]);
        }
        furi_string_cat_printf(temp_str, "\n");

        furi_string_cat_printf(
            temp_str,
            "DSFID: %02X %s\n",
            nfcv_data->dsfid,
            (nfcv_data->security_status[0] & NfcVLockBitDsfid) ? "(locked)" : "");
        furi_string_cat_printf(
            temp_str,
            "AFI: %02X %s\n",
            nfcv_data->afi,
            (nfcv_data->security_status[0] & NfcVLockBitAfi) ? "(locked)" : "");
        furi_string_cat_printf(temp_str, "IC Ref: %02X\n", nfcv_data->ic_ref);
        furi_string_cat_printf(temp_str, "Blocks: %02X\n", nfcv_data->block_num);
        furi_string_cat_printf(temp_str, "Blocksize: %02X\n", nfcv_data->block_size);

        furi_string_cat_printf(
            temp_str, "Data (%d byte)\n", nfcv_data->block_num * nfcv_data->block_size);

        int maxBlocks = nfcv_data->block_num;
        if(maxBlocks > 32) {
            maxBlocks = 32;
            furi_string_cat_printf(temp_str, "(truncated to %d blocks)\n", maxBlocks);
        }

        for(int block = 0; block < maxBlocks; block++) {
            const char* status = (nfcv_data->security_status[block] & 0x01) ? "(lck)" : "";
            for(int pos = 0; pos < nfcv_data->block_size; pos++) {
                furi_string_cat_printf(
                    temp_str, " %02X", nfcv_data->data[block * nfcv_data->block_size + pos]);
            }
            furi_string_cat_printf(temp_str, " %s\n", status);
        }
        furi_string_cat_printf(temp_str, "\n");

        switch(dev_data->nfcv_data.sub_type) {
        case NfcVTypePlain:
            furi_string_cat_printf(temp_str, "Type: Plain\n");
            break;
        case NfcVTypeSlix:
            furi_string_cat_printf(temp_str, "Type: SLIX\n");
            furi_string_cat_printf(temp_str, "Keys:\n");
            furi_string_cat_printf(
                temp_str,
                " EAS      %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_eas));
            break;
        case NfcVTypeSlixS:
            furi_string_cat_printf(temp_str, "Type: SLIX-S\n");
            furi_string_cat_printf(temp_str, "Keys:\n");
            furi_string_cat_printf(
                temp_str,
                " Read     %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_read));
            furi_string_cat_printf(
                temp_str,
                " Write    %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_write));
            furi_string_cat_printf(
                temp_str,
                " Privacy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_privacy));
            furi_string_cat_printf(
                temp_str,
                " Destroy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_destroy));
            furi_string_cat_printf(
                temp_str,
                " EAS      %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_eas));
            break;
        case NfcVTypeSlixL:
            furi_string_cat_printf(temp_str, "Type: SLIX-L\n");
            furi_string_cat_printf(temp_str, "Keys:\n");
            furi_string_cat_printf(
                temp_str,
                " Privacy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_privacy));
            furi_string_cat_printf(
                temp_str,
                " Destroy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_destroy));
            furi_string_cat_printf(
                temp_str,
                " EAS      %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_eas));
            break;
        case NfcVTypeSlix2:
            furi_string_cat_printf(temp_str, "Type: SLIX2\n");
            furi_string_cat_printf(temp_str, "Keys:\n");
            furi_string_cat_printf(
                temp_str,
                " Read     %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_read));
            furi_string_cat_printf(
                temp_str,
                " Write    %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_write));
            furi_string_cat_printf(
                temp_str,
                " Privacy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_privacy));
            furi_string_cat_printf(
                temp_str,
                " Destroy  %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_destroy));
            furi_string_cat_printf(
                temp_str,
                " EAS      %08lX\n",
                nfc_scene_nfc_data_info_get_key(nfcv_data->sub_data.slix.key_eas));
            break;
        default:
            furi_string_cat_printf(temp_str, "\e#ISO15693 (unknown)\n");
            break;
        }
        // Set tag general data
    } else if(type == FuriHalNfcTypeF) {
        // Set NFC-F data
        furi_string_cat_printf(temp_str, "ISO 18092 (NFC-F)\n");
        furi_string_cat_printf(temp_str, "CIN:");
        // NFC-F Card Identification Number (CIN) starts at "UID" byte 2.
        for(size_t i = 2; i < nfc_data->uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc_data->uid[i]);
        }
        // The first 2 bytes of the "UID" are Manufacturer Code (MC)
        furi_string_cat_printf(
            temp_str,
            "\nMC: %02X %02X  ROM: %02X  IC: %02X\n\n",
            nfc_data->uid[0],
            nfc_data->uid[1],
            nfc_data->f_data.pmm[0],
            nfc_data->f_data.pmm[1]);

        furi_string_cat_printf(temp_str, "MRT (1 node/blk):\n");
        furi_string_cat_printf(
            temp_str,
            "- ReqSvc: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_VARIABLE_MRT], 1));
        furi_string_cat_printf(
            temp_str,
            "- Fixed: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_FIXED_MRT], 0));
        furi_string_cat_printf(
            temp_str,
            "- Auth1: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_MUTUAL_AUTH_MRT], 1));
        furi_string_cat_printf(
            temp_str,
            "- Auth2: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_MUTUAL_AUTH_MRT], 0));
        furi_string_cat_printf(
            temp_str,
            "- Read: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_READ_MRT], 1));
        furi_string_cat_printf(
            temp_str,
            "- Write: %" PRIuLEAST32 "us\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_WRITE_MRT], 1));
        furi_string_cat_printf(
            temp_str,
            "- Other: %" PRIuLEAST32 "us\n\n",
            felica_estimate_timing_us(nfc_data->f_data.pmm[FELICA_PMM_OTHER_MRT], 0));

        furi_string_cat_printf(temp_str, "IDm:");
        for(size_t i = 0; i < nfc_data->uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc_data->uid[i]);
        }
        furi_string_cat_printf(temp_str, "\nPMm:");
        for(size_t i = 0; i < sizeof(nfc_data->f_data.pmm); i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc_data->f_data.pmm[i]);
        }
    } else { // FuriHalNfcTypeA
        char iso_type = FURI_BIT(nfc_data->a_data.sak, 5) ? '4' : '3';
        furi_string_cat_printf(temp_str, "ISO 14443-%c (NFC-A)\n", iso_type);
        furi_string_cat_printf(temp_str, "UID:");
        for(size_t i = 0; i < nfc_data->uid_len; i++) {
            furi_string_cat_printf(temp_str, " %02X", nfc_data->uid[i]);
        }
        furi_string_cat_printf(
            temp_str, "\nATQA: %02X %02X ", nfc_data->a_data.atqa[1], nfc_data->a_data.atqa[0]);
        furi_string_cat_printf(temp_str, " SAK: %02X", nfc_data->a_data.sak);
    }

    // Set application specific data
    if(protocol == NfcDeviceProtocolMifareDesfire) {
        MifareDesfireData* data = &dev_data->mf_df_data;
        uint32_t bytes_total = 1UL << (data->version.sw_storage >> 1);
        uint32_t bytes_free = data->free_memory ? data->free_memory->bytes : 0;
        furi_string_cat_printf(temp_str, "\n%lu", bytes_total);
        if(data->version.sw_storage & 1) {
            furi_string_push_back(temp_str, '+');
        }
        furi_string_cat_printf(temp_str, " bytes, %lu bytes free\n", bytes_free);

        uint16_t n_apps = 0;
        uint16_t n_files = 0;
        for(MifareDesfireApplication* app = data->app_head; app; app = app->next) {
            n_apps++;
            for(MifareDesfireFile* file = app->file_head; file; file = file->next) {
                n_files++;
            }
        }
        furi_string_cat_printf(temp_str, "%d Application", n_apps);
        if(n_apps != 1) {
            furi_string_push_back(temp_str, 's');
        }
        furi_string_cat_printf(temp_str, ", %d file", n_files);
        if(n_files != 1) {
            furi_string_push_back(temp_str, 's');
        }
    } else if(protocol == NfcDeviceProtocolMifareUl) {
        MfUltralightData* data = &dev_data->mf_ul_data;
        furi_string_cat_printf(
            temp_str, "\nPages Read %d/%d", data->data_read / 4, data->data_size / 4);
        if(data->data_size > data->data_read) {
            furi_string_cat_printf(temp_str, "\nPassword-protected");
        } else if(data->auth_success) {
            MfUltralightConfigPages* config_pages = mf_ultralight_get_config_pages(data);
            if(config_pages) {
                furi_string_cat_printf(
                    temp_str,
                    "\nPassword: %02X %02X %02X %02X",
                    config_pages->auth_data.pwd.raw[0],
                    config_pages->auth_data.pwd.raw[1],
                    config_pages->auth_data.pwd.raw[2],
                    config_pages->auth_data.pwd.raw[3]);
                furi_string_cat_printf(
                    temp_str,
                    "\nPACK: %02X %02X",
                    config_pages->auth_data.pack.raw[0],
                    config_pages->auth_data.pack.raw[1]);
            }
        }
    } else if(protocol == NfcDeviceProtocolMifareClassic) {
        MfClassicData* data = &dev_data->mf_classic_data;
        uint8_t sectors_total = mf_classic_get_total_sectors_num(data->type);
        uint8_t keys_total = sectors_total * 2;
        uint8_t keys_found = 0;
        uint8_t sectors_read = 0;
        mf_classic_get_read_sectors_and_keys(data, &sectors_read, &keys_found);
        furi_string_cat_printf(temp_str, "\nKeys Found %d/%d", keys_found, keys_total);
        furi_string_cat_printf(temp_str, "\nSectors Read %d/%d", sectors_read, sectors_total);
    }

    // Add text scroll widget
    widget_add_text_scroll_element(
        widget, 0, 0, 128, text_scroll_height, furi_string_get_cstr(temp_str));
    furi_string_free(temp_str);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewWidget);
}

bool nfc_scene_nfc_data_info_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    NfcProtocol protocol = nfc->dev->dev_data.protocol;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeRight) {
            if(protocol == NfcDeviceProtocolMifareDesfire) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireData);
                consumed = true;
            } else if(protocol == NfcDeviceProtocolMifareUl) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightData);
                consumed = true;
            } else if(protocol == NfcDeviceProtocolMifareClassic) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicData);
            } else if(protocol == NfcDeviceProtocolNfcV) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcVMenu);
                consumed = true;
            }
        }
    }

    return consumed;
}

void nfc_scene_nfc_data_info_on_exit(void* context) {
    Nfc* nfc = context;

    widget_reset(nfc->widget);
}
