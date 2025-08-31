/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "events_init.h"
#include <stdio.h>
#include "../lvgl/lvgl.h"

#if LV_USE_GUIDER_SIMULATOR && LV_USE_FREEMASTER
#include "freemaster_client.h"
#endif

static bool isBackHome = false;
static bool isPrint = false;
static bool isSetup = false;
static bool useUSB = false;
static uint32_t copiesCnt = 1;
#define TRANS_SCALE_SMALL 240
#include "custom.h"

static void scrHome_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrHome_contMain, 200, 0, lv_obj_get_y(guider_ui.scrHome_contMain), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contTop, 200, 0, lv_obj_get_y(guider_ui.scrHome_contTop), 10, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contColorInk, 200, 0, lv_obj_get_y(guider_ui.scrHome_contColorInk), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contText, 200, 0, lv_obj_get_y(guider_ui.scrHome_contText), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contBG, 300, 0, lv_obj_get_height(guider_ui.scrHome_contBG), 100, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        lv_obj_fade_in(guider_ui.scrHome_contCopy, 400, 100);
        lv_obj_fade_in(guider_ui.scrHome_contScan, 400, 100);
        lv_obj_fade_in(guider_ui.scrHome_contPrint, 400, 100);
        lv_obj_fade_in(guider_ui.scrHome_contSetup, 400, 100);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrHome_contMain, 200, 0, lv_obj_get_y(guider_ui.scrHome_contMain), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contTop, 200, 0, lv_obj_get_y(guider_ui.scrHome_contTop), 0, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contColorInk, 200, 0, lv_obj_get_y(guider_ui.scrHome_contColorInk), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrHome_contText, 200, 0, lv_obj_get_y(guider_ui.scrHome_contText), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (isSetup) {
            ui_animation(guider_ui.scrHome_contBG, 300, 0, lv_obj_get_height(guider_ui.scrHome_contBG), 272, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
            lv_obj_set_style_bg_color(guider_ui.scrHome_contBG, lv_color_hex(0xe12e2e), LV_PART_MAIN);
        } else {
            ui_animation(guider_ui.scrHome_contBG, 300, 0, lv_obj_get_height(guider_ui.scrHome_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        }
        break;
    }
    default:
        break;
    }
}

static void scrHome_contCopy_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSED:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contCopy, TRANS_SCALE_SMALL, 0);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contCopy, 256, 0);
        isPrint = true;
        isSetup = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrCopy, guider_ui.scrCopy_del, &guider_ui.scrHome_del, setup_scr_scrCopy, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrHome_contScan_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSING:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contScan, TRANS_SCALE_SMALL, 0);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contScan, LV_SCALE_NONE, 0);
        isPrint = false;
        isSetup = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrCopy, guider_ui.scrCopy_del, &guider_ui.scrHome_del, setup_scr_scrCopy, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrHome_contPrint_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSING:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contPrint, TRANS_SCALE_SMALL, 0);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contPrint, LV_SCALE_NONE, 0);
        isPrint = true;
        isSetup = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrPrintMenu, guider_ui.scrPrintMenu_del, &guider_ui.scrHome_del, setup_scr_scrPrintMenu, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrHome_contSetup_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_PRESSING:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contSetup, TRANS_SCALE_SMALL, 0);
        break;
    }
    case LV_EVENT_RELEASED:
    {
        lv_obj_set_style_transform_scale(guider_ui.scrHome_contSetup, LV_SCALE_NONE, 0);
        isSetup = true;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrSetup, guider_ui.scrSetup_del, &guider_ui.scrHome_del, setup_scr_scrSetup, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrHome (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrHome, scrHome_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrHome_contCopy, scrHome_contCopy_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrHome_contScan, scrHome_contScan_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrHome_contPrint, scrHome_contPrint_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrHome_contSetup, scrHome_contSetup_event_handler, LV_EVENT_ALL, ui);
}

static void scrCopy_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrCopy_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy_contBG), 100, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrCopy_labelTitle), 20, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrCopy_btnBack), 10, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_btnNext, 200, 0, lv_obj_get_y(guider_ui.scrCopy_btnNext), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy_contPanel), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_contImage, 200, 0, lv_obj_get_y(guider_ui.scrCopy_contImage), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (isPrint) {
            lv_label_set_text(guider_ui.scrCopy_btnNext_label, "NEXT");
            lv_obj_set_style_bg_color(guider_ui.scrCopy_btnNext, lv_color_hex(0x2f3243), LV_PART_MAIN);
        } else {
            lv_label_set_text(guider_ui.scrCopy_btnNext_label, "SEND");
            lv_obj_set_style_bg_color(guider_ui.scrCopy_btnNext, lv_color_hex(0x28b620), LV_PART_MAIN);
        }
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrCopy_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrCopy_btnBack), 0, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrCopy_labelTitle), 10, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_btnNext, 200, 0, lv_obj_get_y(guider_ui.scrCopy_btnNext), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy_contPanel), 73, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (isBackHome) {
            ui_animation(guider_ui.scrCopy_contImage, 200, 0, lv_obj_get_y(guider_ui.scrCopy_contImage), 73, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        }
        if (isPrint || isBackHome) {
            ui_animation(guider_ui.scrCopy_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        } else {
            ui_animation(guider_ui.scrCopy_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy_contBG), 272, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        }
        break;
    }
    default:
        break;
    }
}

static void scrCopy_btnBack_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrCopy_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        isBackHome = true;
        break;
    }
    default:
        break;
    }
}

static void scrCopy_sliderBright_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        slider_adjust_img_cb(guider_ui.scrCopy_imgScanned, lv_slider_get_value(guider_ui.scrCopy_sliderBright), lv_slider_get_value(guider_ui.scrCopy_sliderHue));
        break;
    }
    default:
        break;
    }
}

static void scrCopy_sliderHue_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_VALUE_CHANGED:
    {
        slider_adjust_img_cb(guider_ui.scrCopy_imgScanned, lv_slider_get_value(guider_ui.scrCopy_sliderBright), lv_slider_get_value(guider_ui.scrCopy_sliderHue));
        break;
    }
    default:
        break;
    }
}

static void scrCopy_btnNext_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        isBackHome = false;
        if (isPrint) {
            ui_load_scr_animation(&guider_ui, &guider_ui.scrCopy2, guider_ui.scrCopy2_del, &guider_ui.scrCopy_del, setup_scr_scrCopy2, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        } else {
            ui_load_scr_animation(&guider_ui, &guider_ui.scrFinished, guider_ui.scrFinished_del, &guider_ui.scrCopy_del, setup_scr_scrFinished, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        }
        break;
    }
    default:
        break;
    }
}

void events_init_scrCopy (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrCopy, scrCopy_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy_btnBack, scrCopy_btnBack_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy_sliderBright, scrCopy_sliderBright_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy_sliderHue, scrCopy_sliderHue_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy_btnNext, scrCopy_btnNext_event_handler, LV_EVENT_ALL, ui);
}

static void scrCopy2_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrCopy2_contDDLpanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_contDDLpanel), 215, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_btnNext, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_btnNext), 220, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_contPanel), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_btnBack), 10, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_labelTitle), 18, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_imgScanned, 300, 0, lv_image_get_scale(guider_ui.scrCopy2_imgScanned), 200, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_image_set_scale, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy2_contBG), 100, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        copiesCnt = 1;
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrCopy2_contDDLpanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_contDDLpanel), 225, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_contPanel), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_contImage, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_contImage), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_btnBack), 0, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrCopy2_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrCopy2_labelTitle), 10, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (isBackHome) {
            ui_animation(guider_ui.scrCopy2_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy2_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        } else {
            ui_animation(guider_ui.scrCopy2_contBG, 300, 0, lv_obj_get_height(guider_ui.scrCopy2_contBG), 272, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        }
        break;
    }
    default:
        break;
    }
}

static void scrCopy2_btnBack_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrCopy2_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        isBackHome = true;
        break;
    }
    default:
        break;
    }
}

static void scrCopy2_btnMinus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        copiesCnt = (copiesCnt > 1) ? (copiesCnt - 1) : 1;
        lv_label_set_text_fmt(guider_ui.scrCopy2_labelCounter, "%d", copiesCnt);
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        copiesCnt = (copiesCnt > 1) ? (copiesCnt - 1) : 1;
        lv_label_set_text_fmt(guider_ui.scrCopy2_labelCounter, "%d", copiesCnt);
        break;
    }
    default:
        break;
    }
}

static void scrCopy2_btnPlus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        copiesCnt++;
        lv_label_set_text_fmt(guider_ui.scrCopy2_labelCounter, "%d", copiesCnt);
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        copiesCnt++;
        lv_label_set_text_fmt(guider_ui.scrCopy2_labelCounter, "%d", copiesCnt);
        break;
    }
    default:
        break;
    }
}

static void scrCopy2_btnNext_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrLoader, guider_ui.scrLoader_del, &guider_ui.scrCopy2_del, setup_scr_scrLoader, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        isBackHome = false;
        break;
    }
    default:
        break;
    }
}

void events_init_scrCopy2 (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrCopy2, scrCopy2_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy2_btnBack, scrCopy2_btnBack_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy2_btnMinus, scrCopy2_btnMinus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy2_btnPlus, scrCopy2_btnPlus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrCopy2_btnNext, scrCopy2_btnNext_event_handler, LV_EVENT_ALL, ui);
}

static void scrPrintMenu_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrPrintMenu_contText, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_contText), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_contMain, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_contMain), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_btnBack), 10, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_labelTitle), 20, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintMenu_contBG), 100, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        lv_obj_fade_in(guider_ui.scrPrintMenu_contUSB, 400, 100);
        lv_obj_fade_in(guider_ui.scrPrintMenu_contMobile, 400, 100);
        lv_obj_fade_in(guider_ui.scrPrintMenu_contInternet, 400, 100);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrPrintMenu_contText, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_contText), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_contMain, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_contMain), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_btnBack), 0, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMenu_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrPrintMenu_labelTitle), 10, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (useUSB || isBackHome) {
            ui_animation(guider_ui.scrPrintMenu_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintMenu_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        } else {
            ui_animation(guider_ui.scrPrintMenu_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintMenu_contBG), 272, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        }
        break;
    }
    default:
        break;
    }
}

static void scrPrintMenu_btnBack_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        isBackHome = true;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrPrintMenu_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrPrintMenu_contUSB_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        useUSB = true;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrPrintUSB, guider_ui.scrPrintUSB_del, &guider_ui.scrPrintMenu_del, setup_scr_scrPrintUSB, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrPrintMenu_contMobile_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        useUSB = false;
        isBackHome = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrPrintMobile, guider_ui.scrPrintMobile_del, &guider_ui.scrPrintMenu_del, setup_scr_scrPrintMobile, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrPrintMenu_contInternet_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        isSetup = false;
        useUSB = false;
        isBackHome = false;
        ui_load_scr_animation(&guider_ui, &guider_ui.scrSetup, guider_ui.scrSetup_del, &guider_ui.scrPrintMenu_del, setup_scr_scrSetup, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrPrintMenu (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrPrintMenu, scrPrintMenu_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintMenu_btnBack, scrPrintMenu_btnBack_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintMenu_contUSB, scrPrintMenu_contUSB_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintMenu_contMobile, scrPrintMenu_contMobile_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintMenu_contInternet, scrPrintMenu_contInternet_event_handler, LV_EVENT_ALL, ui);
}

static void scrPrintUSB_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrPrintUSB_contDDLpanel, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_contDDLpanel), 215, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_btnNext, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_btnNext), 220, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_contPanel), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_btnBack), 10, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_labelTitle), 20, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintUSB_contBG), 100, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_listFiles, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_listFiles), 53, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        copiesCnt = 1;
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrPrintUSB_contDDLpanel, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_contDDLpanel), 225, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_btnNext, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_btnNext), 230, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_contPanel, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_contPanel), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_btnBack), 0, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_labelTitle, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_labelTitle), 10, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintUSB_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintUSB_listFiles, 200, 0, lv_obj_get_y(guider_ui.scrPrintUSB_listFiles), 63, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void scrPrintUSB_btnBack_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrPrintUSB_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

static void scrPrintUSB_btnMinus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        copiesCnt = (copiesCnt > 1) ? (copiesCnt - 1) : 1;
        lv_label_set_text_fmt(guider_ui.scrPrintUSB_labelCounter, "%d", copiesCnt);
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        copiesCnt = (copiesCnt > 1) ? (copiesCnt - 1) : 1;
        lv_label_set_text_fmt(guider_ui.scrPrintUSB_labelCounter, "%d", copiesCnt);
        break;
    }
    default:
        break;
    }
}

static void scrPrintUSB_btnPlus_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_CLICKED:
    {
        copiesCnt++;
        lv_label_set_text_fmt(guider_ui.scrPrintUSB_labelCounter, "%d", copiesCnt);
        break;
    }
    case LV_EVENT_LONG_PRESSED_REPEAT:
    {
        copiesCnt++;
        lv_label_set_text_fmt(guider_ui.scrPrintUSB_labelCounter, "%d", copiesCnt);
        break;
    }
    default:
        break;
    }
}

static void scrPrintUSB_btnNext_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrFinished, guider_ui.scrFinished_del, &guider_ui.scrPrintUSB_del, setup_scr_scrFinished, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrPrintUSB (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrPrintUSB, scrPrintUSB_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintUSB_btnBack, scrPrintUSB_btnBack_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintUSB_btnMinus, scrPrintUSB_btnMinus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintUSB_btnPlus, scrPrintUSB_btnPlus_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintUSB_btnNext, scrPrintUSB_btnNext_event_handler, LV_EVENT_ALL, ui);
}

static void scrPrintMobile_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrPrintMobile_imgPrinter, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgPrinter), 70, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_imgWave, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgWave), 90, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_imgPhone, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgPhone), 70, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_labelPrompt), 170, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_btnNxet, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_btnNxet), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrPrintMobile_imgPrinter, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgPrinter), 80, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_imgWave, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgWave), 100, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_imgPhone, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_imgPhone), 80, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_labelPrompt), 180, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_btnNxet, 200, 0, lv_obj_get_y(guider_ui.scrPrintMobile_btnNxet), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrPrintMobile_contBG, 300, 0, lv_obj_get_height(guider_ui.scrPrintMobile_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void scrPrintMobile_btnNxet_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrPrintMobile_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrPrintMobile (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrPrintMobile, scrPrintMobile_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrPrintMobile_btnNxet, scrPrintMobile_btnNxet_event_handler, LV_EVENT_ALL, ui);
}

static void scrSetup_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrSetup_imgCloud, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgCloud), 40, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_imgWave, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgWave), 65, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_imgPrinter, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgPrinter), 70, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrSetup_labelPrompt), 170, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrSetup_btnBack), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrSetup_imgCloud, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgCloud), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_imgWave, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgWave), 75, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_imgPrinter, 200, 0, lv_obj_get_y(guider_ui.scrSetup_imgPrinter), 80, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrSetup_labelPrompt), 180, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_btnBack, 200, 0, lv_obj_get_y(guider_ui.scrSetup_btnBack), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrSetup_contBG, 300, 0, lv_obj_get_height(guider_ui.scrSetup_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        lv_obj_set_style_bg_color(guider_ui.scrSetup_contBG, lv_color_hex(0x2f3243), LV_PART_MAIN);
        break;
    }
    default:
        break;
    }
}

static void scrSetup_btnBack_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrSetup_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrSetup (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrSetup, scrSetup_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrSetup_btnBack, scrSetup_btnBack_event_handler, LV_EVENT_ALL, ui);
}

static void scrLoader_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrLoader_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrLoader_labelPrompt), 190, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrLoader_labelLoader, 200, 0, lv_obj_get_y(guider_ui.scrLoader_labelLoader), 90, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrLoader_arcLoader, 200, 0, lv_obj_get_y(guider_ui.scrLoader_arcLoader), 40, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrLoader_arcLoader, 2000, 200, 0, 100, &lv_anim_path_linear, 0, 0, 0, 0, (lv_anim_exec_xcb_t)loader_anim_cb, NULL, (lv_anim_completed_cb_t)loader_anim_complete_cb, NULL);
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrLoader_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrLoader_labelPrompt), 200, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrLoader_labelLoader, 200, 0, lv_obj_get_y(guider_ui.scrLoader_labelLoader), 100, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrLoader_arcLoader, 200, 0, lv_obj_get_y(guider_ui.scrLoader_arcLoader), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

void events_init_scrLoader (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrLoader, scrLoader_event_handler, LV_EVENT_ALL, ui);
}

static void scrFinished_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_SCREEN_LOAD_START:
    {
        ui_animation(guider_ui.scrFinished_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrFinished_labelPrompt), 170, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrFinished_btnNxet, 200, 0, lv_obj_get_y(guider_ui.scrFinished_btnNxet), 210, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrFinished_imgIconOk, 200, 0, lv_obj_get_y(guider_ui.scrFinished_imgIconOk), 30, &lv_anim_path_ease_in, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        if (isPrint) {
            lv_label_set_text(guider_ui.scrFinished_labelPrompt, "Printing finished");
        } else {
            lv_label_set_text(guider_ui.scrFinished_labelPrompt, "Scanning finished");
        }
        break;
    }
    case LV_EVENT_SCREEN_UNLOAD_START:
    {
        ui_animation(guider_ui.scrFinished_labelPrompt, 200, 0, lv_obj_get_y(guider_ui.scrFinished_labelPrompt), 180, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrFinished_btnNxet, 200, 0, lv_obj_get_y(guider_ui.scrFinished_btnNxet), 220, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrFinished_imgIconOk, 200, 0, lv_obj_get_y(guider_ui.scrFinished_imgIconOk), 40, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_y, NULL, NULL, NULL);
        ui_animation(guider_ui.scrFinished_contBG, 200, 0, lv_obj_get_height(guider_ui.scrFinished_contBG), 50, &lv_anim_path_ease_out, 0, 0, 0, 0, (lv_anim_exec_xcb_t)lv_obj_set_height, NULL, NULL, NULL);
        break;
    }
    default:
        break;
    }
}

static void scrFinished_btnNxet_event_handler (lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    switch (code) {
    case LV_EVENT_RELEASED:
    {
        ui_load_scr_animation(&guider_ui, &guider_ui.scrHome, guider_ui.scrHome_del, &guider_ui.scrFinished_del, setup_scr_scrHome, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
        break;
    }
    default:
        break;
    }
}

void events_init_scrFinished (lv_ui *ui)
{
    lv_obj_add_event_cb(ui->scrFinished, scrFinished_event_handler, LV_EVENT_ALL, ui);
    lv_obj_add_event_cb(ui->scrFinished_btnNxet, scrFinished_btnNxet_event_handler, LV_EVENT_ALL, ui);
}


void events_init(lv_ui *ui)
{

}
