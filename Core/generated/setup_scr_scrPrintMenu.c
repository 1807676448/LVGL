/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "../lvgl/lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"



void setup_scr_scrPrintMenu(lv_ui *ui)
{
    //Write codes scrPrintMenu
    ui->scrPrintMenu = lv_obj_create(NULL);
    lv_obj_set_size(ui->scrPrintMenu, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintMenu, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu, lv_color_hex(0xF3F8FE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contBG
    ui->scrPrintMenu_contBG = lv_obj_create(ui->scrPrintMenu);
    lv_obj_set_pos(ui->scrPrintMenu_contBG, 0, 0);
    lv_obj_set_size(ui->scrPrintMenu_contBG, 480, 58);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contBG, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contBG, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contBG, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contBG, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contBG, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_labelTitle
    ui->scrPrintMenu_labelTitle = lv_label_create(ui->scrPrintMenu);
    lv_obj_set_pos(ui->scrPrintMenu_labelTitle, 121, 23);
    lv_obj_set_size(ui->scrPrintMenu_labelTitle, 232, 32);
    lv_label_set_text(ui->scrPrintMenu_labelTitle, "PRINT MENU");
    lv_label_set_long_mode(ui->scrPrintMenu_labelTitle, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMenu_labelTitle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_labelTitle, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_labelTitle, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_labelTitle, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_labelTitle, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_btnBack
    ui->scrPrintMenu_btnBack = lv_button_create(ui->scrPrintMenu);
    lv_obj_set_pos(ui->scrPrintMenu_btnBack, 20, 11);
    lv_obj_set_size(ui->scrPrintMenu_btnBack, 51, 44);
    ui->scrPrintMenu_btnBack_label = lv_label_create(ui->scrPrintMenu_btnBack);
    lv_label_set_text(ui->scrPrintMenu_btnBack_label, "" LV_SYMBOL_LEFT " ");
    lv_label_set_long_mode(ui->scrPrintMenu_btnBack_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintMenu_btnBack_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintMenu_btnBack, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintMenu_btnBack_label, LV_PCT(100));

    //Write style for scrPrintMenu_btnBack, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintMenu_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_btnBack, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_btnBack, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_btnBack, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_btnBack, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_btnBack, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contMain
    ui->scrPrintMenu_contMain = lv_obj_create(ui->scrPrintMenu);
    lv_obj_set_pos(ui->scrPrintMenu_contMain, 20, 50);
    lv_obj_set_size(ui->scrPrintMenu_contMain, 440, 164);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contMain, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contMain, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contMain, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contMain, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contMain, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contMain, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contMain, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contUSB
    ui->scrPrintMenu_contUSB = lv_obj_create(ui->scrPrintMenu_contMain);
    lv_obj_set_pos(ui->scrPrintMenu_contUSB, 22, 9);
    lv_obj_set_size(ui->scrPrintMenu_contUSB, 100, 141);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contUSB, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contUSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contUSB, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contUSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contUSB, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contUSB, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->scrPrintMenu_contUSB, &_btn_bg_2_RGB565A8_100x141, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->scrPrintMenu_contUSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_imgUSB
    ui->scrPrintMenu_imgUSB = lv_image_create(ui->scrPrintMenu_contUSB);
    lv_obj_set_pos(ui->scrPrintMenu_imgUSB, 42, 20);
    lv_obj_set_size(ui->scrPrintMenu_imgUSB, 37, 47);
    lv_obj_add_flag(ui->scrPrintMenu_imgUSB, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(ui->scrPrintMenu_imgUSB, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMenu_imgUSB, &_usb_RGB565A8_37x47);
    lv_image_set_pivot(ui->scrPrintMenu_imgUSB, 50,50);
    lv_image_set_rotation(ui->scrPrintMenu_imgUSB, 0);

    //Write style for scrPrintMenu_imgUSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMenu_imgUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMenu_imgUSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_labelUSB
    ui->scrPrintMenu_labelUSB = lv_label_create(ui->scrPrintMenu_contUSB);
    lv_obj_set_pos(ui->scrPrintMenu_labelUSB, 11, 104);
    lv_obj_set_size(ui->scrPrintMenu_labelUSB, 58, 23);
    lv_obj_add_flag(ui->scrPrintMenu_labelUSB, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_label_set_text(ui->scrPrintMenu_labelUSB, "USB");
    lv_label_set_long_mode(ui->scrPrintMenu_labelUSB, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMenu_labelUSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_labelUSB, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_labelUSB, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_labelUSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_labelUSB, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_labelUSB, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contMobile
    ui->scrPrintMenu_contMobile = lv_obj_create(ui->scrPrintMenu_contMain);
    lv_obj_set_pos(ui->scrPrintMenu_contMobile, 169, 9);
    lv_obj_set_size(ui->scrPrintMenu_contMobile, 100, 141);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contMobile, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contMobile, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contMobile, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contMobile, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contMobile, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contMobile, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->scrPrintMenu_contMobile, &_btn_bg_3_RGB565A8_100x141, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->scrPrintMenu_contMobile, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_imgMobile
    ui->scrPrintMenu_imgMobile = lv_image_create(ui->scrPrintMenu_contMobile);
    lv_obj_set_pos(ui->scrPrintMenu_imgMobile, 42, 20);
    lv_obj_set_size(ui->scrPrintMenu_imgMobile, 37, 47);
    lv_obj_add_flag(ui->scrPrintMenu_imgMobile, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(ui->scrPrintMenu_imgMobile, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMenu_imgMobile, &_mobile_RGB565A8_37x47);
    lv_image_set_pivot(ui->scrPrintMenu_imgMobile, 50,50);
    lv_image_set_rotation(ui->scrPrintMenu_imgMobile, 0);

    //Write style for scrPrintMenu_imgMobile, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMenu_imgMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMenu_imgMobile, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_labelMobile
    ui->scrPrintMenu_labelMobile = lv_label_create(ui->scrPrintMenu_contMobile);
    lv_obj_set_pos(ui->scrPrintMenu_labelMobile, 13, 105);
    lv_obj_set_size(ui->scrPrintMenu_labelMobile, 70, 23);
    lv_obj_add_flag(ui->scrPrintMenu_labelMobile, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_label_set_text(ui->scrPrintMenu_labelMobile, "MOBILE");
    lv_label_set_long_mode(ui->scrPrintMenu_labelMobile, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMenu_labelMobile, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_labelMobile, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_labelMobile, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_labelMobile, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_labelMobile, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_labelMobile, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contInternet
    ui->scrPrintMenu_contInternet = lv_obj_create(ui->scrPrintMenu_contMain);
    lv_obj_set_pos(ui->scrPrintMenu_contInternet, 316, 9);
    lv_obj_set_size(ui->scrPrintMenu_contInternet, 100, 141);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contInternet, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contInternet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contInternet, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contInternet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contInternet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contInternet, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_src(ui->scrPrintMenu_contInternet, &_btn_bg_4_RGB565A8_100x141, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_opa(ui->scrPrintMenu_contInternet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_image_recolor_opa(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_imgInternet
    ui->scrPrintMenu_imgInternet = lv_image_create(ui->scrPrintMenu_contInternet);
    lv_obj_set_pos(ui->scrPrintMenu_imgInternet, 42, 20);
    lv_obj_set_size(ui->scrPrintMenu_imgInternet, 37, 47);
    lv_obj_add_flag(ui->scrPrintMenu_imgInternet, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_add_flag(ui->scrPrintMenu_imgInternet, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMenu_imgInternet, &_internet_RGB565A8_37x47);
    lv_image_set_pivot(ui->scrPrintMenu_imgInternet, 50,50);
    lv_image_set_rotation(ui->scrPrintMenu_imgInternet, 0);

    //Write style for scrPrintMenu_imgInternet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMenu_imgInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMenu_imgInternet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_labelInternet
    ui->scrPrintMenu_labelInternet = lv_label_create(ui->scrPrintMenu_contInternet);
    lv_obj_set_pos(ui->scrPrintMenu_labelInternet, 8, 104);
    lv_obj_set_size(ui->scrPrintMenu_labelInternet, 83, 23);
    lv_obj_add_flag(ui->scrPrintMenu_labelInternet, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_label_set_text(ui->scrPrintMenu_labelInternet, "INTERNET");
    lv_label_set_long_mode(ui->scrPrintMenu_labelInternet, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMenu_labelInternet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_labelInternet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_labelInternet, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_labelInternet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_labelInternet, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_labelInternet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_contText
    ui->scrPrintMenu_contText = lv_obj_create(ui->scrPrintMenu);
    lv_obj_set_pos(ui->scrPrintMenu_contText, 20, 235);
    lv_obj_set_size(ui->scrPrintMenu_contText, 440, 52);
    lv_obj_set_scrollbar_mode(ui->scrPrintMenu_contText, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMenu_contText, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_contText, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_contText, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_contText, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_contText, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_contText, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMenu_labelPrompt
    ui->scrPrintMenu_labelPrompt = lv_label_create(ui->scrPrintMenu_contText);
    lv_obj_set_pos(ui->scrPrintMenu_labelPrompt, 108, 15);
    lv_obj_set_size(ui->scrPrintMenu_labelPrompt, 242, 21);
    lv_label_set_text(ui->scrPrintMenu_labelPrompt, "What do you want to do today?");
    lv_label_set_long_mode(ui->scrPrintMenu_labelPrompt, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMenu_labelPrompt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMenu_labelPrompt, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMenu_labelPrompt, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMenu_labelPrompt, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMenu_labelPrompt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMenu_labelPrompt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMenu_labelPrompt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMenu_labelPrompt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMenu_labelPrompt, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMenu_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of scrPrintMenu.
    lv_obj_set_style_opa(ui->scrPrintMenu_contUSB, 0, 0);
    lv_obj_align(ui->scrPrintMenu_imgUSB, LV_ALIGN_TOP_RIGHT, -15, 15);
    lv_obj_align(ui->scrPrintMenu_labelUSB, LV_ALIGN_BOTTOM_LEFT, 5, -10);
    lv_obj_set_style_transform_pivot_x(ui->scrPrintMenu_contUSB, lv_obj_get_width(ui->scrPrintMenu_contUSB) / 2, 0);
    lv_obj_set_style_transform_pivot_y(ui->scrPrintMenu_contUSB, lv_obj_get_height(ui->scrPrintMenu_contUSB) / 2, 0);

    lv_obj_set_style_opa(ui->scrPrintMenu_contMobile, 0, 0);
    lv_obj_align(ui->scrPrintMenu_imgMobile, LV_ALIGN_TOP_RIGHT, -15, 15);
    lv_obj_align(ui->scrPrintMenu_labelMobile, LV_ALIGN_BOTTOM_LEFT, 5, -10);
    lv_obj_set_style_transform_pivot_x(ui->scrPrintMenu_contMobile, lv_obj_get_width(ui->scrPrintMenu_contMobile) / 2, 0);
    lv_obj_set_style_transform_pivot_y(ui->scrPrintMenu_contMobile, lv_obj_get_height(ui->scrPrintMenu_contMobile) / 2, 0);

    lv_obj_set_style_opa(ui->scrPrintMenu_contInternet, 0, 0);
    lv_obj_align(ui->scrPrintMenu_imgInternet, LV_ALIGN_TOP_RIGHT, -15, 15);
    lv_obj_align(ui->scrPrintMenu_labelInternet, LV_ALIGN_BOTTOM_LEFT, 5, -10);
    lv_obj_set_style_transform_pivot_x(ui->scrPrintMenu_contInternet, lv_obj_get_width(ui->scrPrintMenu_contInternet) / 2, 0);
    lv_obj_set_style_transform_pivot_y(ui->scrPrintMenu_contInternet, lv_obj_get_height(ui->scrPrintMenu_contInternet) / 2, 0);

    //Update current screen layout.
    lv_obj_update_layout(ui->scrPrintMenu);

    //Init events for screen.
    events_init_scrPrintMenu(ui);
}
