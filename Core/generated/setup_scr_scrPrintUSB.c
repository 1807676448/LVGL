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



void setup_scr_scrPrintUSB(lv_ui *ui)
{
    //Write codes scrPrintUSB
    ui->scrPrintUSB = lv_obj_create(NULL);
    lv_obj_set_size(ui->scrPrintUSB, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrPrintUSB, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintUSB, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB, lv_color_hex(0xF3F8FE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_contBG
    ui->scrPrintUSB_contBG = lv_obj_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_contBG, 0, 0);
    lv_obj_set_size(ui->scrPrintUSB_contBG, 480, 58);
    lv_obj_set_scrollbar_mode(ui->scrPrintUSB_contBG, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintUSB_contBG, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_contBG, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_contBG, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_contBG, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_labelTitle
    ui->scrPrintUSB_labelTitle = lv_label_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_labelTitle, 120, 11);
    lv_obj_set_size(ui->scrPrintUSB_labelTitle, 232, 32);
    lv_label_set_text(ui->scrPrintUSB_labelTitle, "SCANNING IMAGE");
    lv_label_set_long_mode(ui->scrPrintUSB_labelTitle, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintUSB_labelTitle, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_labelTitle, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_labelTitle, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_labelTitle, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_labelTitle, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_labelTitle, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_btnBack
    ui->scrPrintUSB_btnBack = lv_button_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_btnBack, 20, 0);
    lv_obj_set_size(ui->scrPrintUSB_btnBack, 51, 44);
    ui->scrPrintUSB_btnBack_label = lv_label_create(ui->scrPrintUSB_btnBack);
    lv_label_set_text(ui->scrPrintUSB_btnBack_label, "" LV_SYMBOL_LEFT " ");
    lv_label_set_long_mode(ui->scrPrintUSB_btnBack_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintUSB_btnBack_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintUSB_btnBack, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintUSB_btnBack_label, LV_PCT(100));

    //Write style for scrPrintUSB_btnBack, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_btnBack, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_btnBack, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_btnBack, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_btnBack, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_btnBack, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_contPanel
    ui->scrPrintUSB_contPanel = lv_obj_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_contPanel, 280, 50);
    lv_obj_set_size(ui->scrPrintUSB_contPanel, 180, 167);
    lv_obj_set_scrollbar_mode(ui->scrPrintUSB_contPanel, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintUSB_contPanel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_contPanel, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_contPanel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_contPanel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_contPanel, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_contPanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_labelCopies
    ui->scrPrintUSB_labelCopies = lv_label_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_labelCopies, 38, 9);
    lv_obj_set_size(ui->scrPrintUSB_labelCopies, 100, 32);
    lv_label_set_text(ui->scrPrintUSB_labelCopies, "Copies");
    lv_label_set_long_mode(ui->scrPrintUSB_labelCopies, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintUSB_labelCopies, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_labelCopies, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_labelCopies, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_labelCopies, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_labelCopies, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_labelCopies, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_btnMinus
    ui->scrPrintUSB_btnMinus = lv_button_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_btnMinus, 14, 50);
    lv_obj_set_size(ui->scrPrintUSB_btnMinus, 35, 35);
    ui->scrPrintUSB_btnMinus_label = lv_label_create(ui->scrPrintUSB_btnMinus);
    lv_label_set_text(ui->scrPrintUSB_btnMinus_label, "-");
    lv_label_set_long_mode(ui->scrPrintUSB_btnMinus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintUSB_btnMinus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintUSB_btnMinus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintUSB_btnMinus_label, LV_PCT(100));

    //Write style for scrPrintUSB_btnMinus, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_btnMinus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_btnMinus, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_btnMinus, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_btnMinus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_btnMinus, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_btnMinus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_btnMinus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_btnMinus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_btnMinus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_btnMinus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_btnPlus
    ui->scrPrintUSB_btnPlus = lv_button_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_btnPlus, 127, 50);
    lv_obj_set_size(ui->scrPrintUSB_btnPlus, 35, 35);
    ui->scrPrintUSB_btnPlus_label = lv_label_create(ui->scrPrintUSB_btnPlus);
    lv_label_set_text(ui->scrPrintUSB_btnPlus_label, "+");
    lv_label_set_long_mode(ui->scrPrintUSB_btnPlus_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintUSB_btnPlus_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintUSB_btnPlus, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintUSB_btnPlus_label, LV_PCT(100));

    //Write style for scrPrintUSB_btnPlus, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_btnPlus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_btnPlus, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_btnPlus, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_btnPlus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_btnPlus, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_btnPlus, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_btnPlus, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_btnPlus, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_btnPlus, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_btnPlus, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_labelCounter
    ui->scrPrintUSB_labelCounter = lv_label_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_labelCounter, 56, 50);
    lv_obj_set_size(ui->scrPrintUSB_labelCounter, 64, 35);
    lv_label_set_text(ui->scrPrintUSB_labelCounter, "1");
    lv_label_set_long_mode(ui->scrPrintUSB_labelCounter, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintUSB_labelCounter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_labelCounter, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->scrPrintUSB_labelCounter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->scrPrintUSB_labelCounter, lv_color_hex(0x909090), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->scrPrintUSB_labelCounter, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_labelCounter, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_labelCounter, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_labelCounter, &lv_font_montserratMedium_25, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_labelCounter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_labelCounter, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_labelCounter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_labelColor
    ui->scrPrintUSB_labelColor = lv_label_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_labelColor, 10, 95);
    lv_obj_set_size(ui->scrPrintUSB_labelColor, 70, 30);
    lv_label_set_text(ui->scrPrintUSB_labelColor, "Color");
    lv_label_set_long_mode(ui->scrPrintUSB_labelColor, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintUSB_labelColor, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_labelColor, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_labelColor, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_labelColor, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_labelColor, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_labelColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_labelVertical
    ui->scrPrintUSB_labelVertical = lv_label_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_labelVertical, 90, 95);
    lv_obj_set_size(ui->scrPrintUSB_labelVertical, 88, 29);
    lv_label_set_text(ui->scrPrintUSB_labelVertical, "Vertical");
    lv_label_set_long_mode(ui->scrPrintUSB_labelVertical, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintUSB_labelVertical, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_labelVertical, lv_color_hex(0x000000), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_labelVertical, &lv_font_montserratMedium_20, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_labelVertical, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_labelVertical, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_labelVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_swColor
    ui->scrPrintUSB_swColor = lv_switch_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_swColor, 24, 129);
    lv_obj_set_size(ui->scrPrintUSB_swColor, 44, 25);

    //Write style for scrPrintUSB_swColor, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swColor, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swColor, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swColor, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_swColor, 11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_swColor, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for scrPrintUSB_swColor, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swColor, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swColor, lv_color_hex(0x28b620), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swColor, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swColor, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for scrPrintUSB_swColor, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swColor, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swColor, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swColor, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swColor, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_swColor, 11, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_swVertical
    ui->scrPrintUSB_swVertical = lv_switch_create(ui->scrPrintUSB_contPanel);
    lv_obj_set_pos(ui->scrPrintUSB_swVertical, 113, 129);
    lv_obj_set_size(ui->scrPrintUSB_swVertical, 44, 25);

    //Write style for scrPrintUSB_swVertical, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swVertical, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swVertical, lv_color_hex(0xe6e2e6), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swVertical, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_swVertical, 11, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_swVertical, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style for scrPrintUSB_swVertical, Part: LV_PART_INDICATOR, State: LV_STATE_CHECKED.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swVertical, 255, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swVertical, lv_color_hex(0x28b620), LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swVertical, LV_GRAD_DIR_NONE, LV_PART_INDICATOR|LV_STATE_CHECKED);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swVertical, 0, LV_PART_INDICATOR|LV_STATE_CHECKED);

    //Write style for scrPrintUSB_swVertical, Part: LV_PART_KNOB, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_swVertical, 255, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_swVertical, lv_color_hex(0xffffff), LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_swVertical, LV_GRAD_DIR_NONE, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_swVertical, 0, LV_PART_KNOB|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_swVertical, 11, LV_PART_KNOB|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_btnNext
    ui->scrPrintUSB_btnNext = lv_button_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_btnNext, 290, 247);
    lv_obj_set_size(ui->scrPrintUSB_btnNext, 160, 41);
    ui->scrPrintUSB_btnNext_label = lv_label_create(ui->scrPrintUSB_btnNext);
    lv_label_set_text(ui->scrPrintUSB_btnNext_label, "NEXT");
    lv_label_set_long_mode(ui->scrPrintUSB_btnNext_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintUSB_btnNext_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintUSB_btnNext, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintUSB_btnNext_label, LV_PCT(100));

    //Write style for scrPrintUSB_btnNext, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_btnNext, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_btnNext, lv_color_hex(0x28b620), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_btnNext, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_btnNext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_btnNext, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_btnNext, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintUSB_btnNext, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_btnNext, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_btnNext, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintUSB_btnNext, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_listFiles
    ui->scrPrintUSB_listFiles = lv_list_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_listFiles, 20, 50);
    lv_obj_set_size(ui->scrPrintUSB_listFiles, 235, 170);
    lv_obj_set_scrollbar_mode(ui->scrPrintUSB_listFiles, LV_SCROLLBAR_MODE_OFF);
    ui->scrPrintUSB_listFiles_item0 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Rent Contract.pdf");
    ui->scrPrintUSB_listFiles_item1 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Scanned_08_02.pdf");
    ui->scrPrintUSB_listFiles_item2 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "photo_240514.jpg");
    ui->scrPrintUSB_listFiles_item3 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "photo_240519.jpg");
    ui->scrPrintUSB_listFiles_item4 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "photo_230526.jpg");
    ui->scrPrintUSB_listFiles_item5 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "photo_230731.jpg");
    ui->scrPrintUSB_listFiles_item6 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "screenshot_0426.png");
    ui->scrPrintUSB_listFiles_item7 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "screenshot_0624.png");
    ui->scrPrintUSB_listFiles_item8 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "screenshot_0702.png");
    ui->scrPrintUSB_listFiles_item9 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Meeting_Summary.txt");
    ui->scrPrintUSB_listFiles_item10 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "untitled.txt");
    ui->scrPrintUSB_listFiles_item11 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "New File.txt");
    ui->scrPrintUSB_listFiles_item12 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "untitled (1).txt");
    ui->scrPrintUSB_listFiles_item13 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_1.jpg");
    ui->scrPrintUSB_listFiles_item14 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_2.jpg");
    ui->scrPrintUSB_listFiles_item15 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_3.jpg");
    ui->scrPrintUSB_listFiles_item16 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_4.jpg");
    ui->scrPrintUSB_listFiles_item17 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_5.jpg");
    ui->scrPrintUSB_listFiles_item18 = lv_list_add_button(ui->scrPrintUSB_listFiles, LV_SYMBOL_FILE, "Show_6.jpg");

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_listFiles_main_main_default
    static lv_style_t style_scrPrintUSB_listFiles_main_main_default;
    ui_init_style(&style_scrPrintUSB_listFiles_main_main_default);

    lv_style_set_pad_top(&style_scrPrintUSB_listFiles_main_main_default, 5);
    lv_style_set_pad_left(&style_scrPrintUSB_listFiles_main_main_default, 5);
    lv_style_set_pad_right(&style_scrPrintUSB_listFiles_main_main_default, 5);
    lv_style_set_pad_bottom(&style_scrPrintUSB_listFiles_main_main_default, 5);
    lv_style_set_bg_opa(&style_scrPrintUSB_listFiles_main_main_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_listFiles_main_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_listFiles_main_main_default, LV_GRAD_DIR_NONE);
    lv_style_set_border_width(&style_scrPrintUSB_listFiles_main_main_default, 0);
    lv_style_set_radius(&style_scrPrintUSB_listFiles_main_main_default, 9);
    lv_style_set_shadow_width(&style_scrPrintUSB_listFiles_main_main_default, 0);
    lv_obj_add_style(ui->scrPrintUSB_listFiles, &style_scrPrintUSB_listFiles_main_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_listFiles_main_scrollbar_default
    static lv_style_t style_scrPrintUSB_listFiles_main_scrollbar_default;
    ui_init_style(&style_scrPrintUSB_listFiles_main_scrollbar_default);

    lv_style_set_radius(&style_scrPrintUSB_listFiles_main_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_scrPrintUSB_listFiles_main_scrollbar_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_listFiles_main_scrollbar_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_listFiles_main_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->scrPrintUSB_listFiles, &style_scrPrintUSB_listFiles_main_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_listFiles_extra_btns_main_default
    static lv_style_t style_scrPrintUSB_listFiles_extra_btns_main_default;
    ui_init_style(&style_scrPrintUSB_listFiles_extra_btns_main_default);

    lv_style_set_pad_top(&style_scrPrintUSB_listFiles_extra_btns_main_default, 5);
    lv_style_set_pad_left(&style_scrPrintUSB_listFiles_extra_btns_main_default, 5);
    lv_style_set_pad_right(&style_scrPrintUSB_listFiles_extra_btns_main_default, 5);
    lv_style_set_pad_bottom(&style_scrPrintUSB_listFiles_extra_btns_main_default, 5);
    lv_style_set_border_width(&style_scrPrintUSB_listFiles_extra_btns_main_default, 0);
    lv_style_set_text_color(&style_scrPrintUSB_listFiles_extra_btns_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_scrPrintUSB_listFiles_extra_btns_main_default, &lv_font_montserratMedium_16);
    lv_style_set_text_opa(&style_scrPrintUSB_listFiles_extra_btns_main_default, 255);
    lv_style_set_radius(&style_scrPrintUSB_listFiles_extra_btns_main_default, 3);
    lv_style_set_bg_opa(&style_scrPrintUSB_listFiles_extra_btns_main_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_listFiles_extra_btns_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_listFiles_extra_btns_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item18, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item17, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item16, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item15, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item14, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item13, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item12, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item11, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item10, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item9, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item8, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item7, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item6, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item5, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item4, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item3, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item2, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item1, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_add_style(ui->scrPrintUSB_listFiles_item0, &style_scrPrintUSB_listFiles_extra_btns_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_listFiles_extra_texts_main_default
    static lv_style_t style_scrPrintUSB_listFiles_extra_texts_main_default;
    ui_init_style(&style_scrPrintUSB_listFiles_extra_texts_main_default);

    lv_style_set_pad_top(&style_scrPrintUSB_listFiles_extra_texts_main_default, 5);
    lv_style_set_pad_left(&style_scrPrintUSB_listFiles_extra_texts_main_default, 5);
    lv_style_set_pad_right(&style_scrPrintUSB_listFiles_extra_texts_main_default, 5);
    lv_style_set_pad_bottom(&style_scrPrintUSB_listFiles_extra_texts_main_default, 5);
    lv_style_set_border_width(&style_scrPrintUSB_listFiles_extra_texts_main_default, 0);
    lv_style_set_text_color(&style_scrPrintUSB_listFiles_extra_texts_main_default, lv_color_hex(0x0D3055));
    lv_style_set_text_font(&style_scrPrintUSB_listFiles_extra_texts_main_default, &lv_font_montserratMedium_12);
    lv_style_set_text_opa(&style_scrPrintUSB_listFiles_extra_texts_main_default, 255);
    lv_style_set_radius(&style_scrPrintUSB_listFiles_extra_texts_main_default, 3);
    lv_style_set_transform_width(&style_scrPrintUSB_listFiles_extra_texts_main_default, 0);
    lv_style_set_bg_opa(&style_scrPrintUSB_listFiles_extra_texts_main_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_listFiles_extra_texts_main_default, lv_color_hex(0xffffff));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_listFiles_extra_texts_main_default, LV_GRAD_DIR_NONE);

    //Write codes scrPrintUSB_contDDLpanel
    ui->scrPrintUSB_contDDLpanel = lv_obj_create(ui->scrPrintUSB);
    lv_obj_set_pos(ui->scrPrintUSB_contDDLpanel, 20, 241);
    lv_obj_set_size(ui->scrPrintUSB_contDDLpanel, 235, 52);
    lv_obj_set_scrollbar_mode(ui->scrPrintUSB_contDDLpanel, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintUSB_contDDLpanel, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_contDDLpanel, 9, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_contDDLpanel, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_contDDLpanel, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_contDDLpanel, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_contDDLpanel, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_ddlistMode
    ui->scrPrintUSB_ddlistMode = lv_dropdown_create(ui->scrPrintUSB_contDDLpanel);
    lv_obj_set_pos(ui->scrPrintUSB_ddlistMode, 10, 8);
    lv_obj_set_size(ui->scrPrintUSB_ddlistMode, 100, 35);
    lv_dropdown_set_options(ui->scrPrintUSB_ddlistMode, "Best\nNormal\nDraft");

    //Write style for scrPrintUSB_ddlistMode, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->scrPrintUSB_ddlistMode, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_ddlistMode, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_ddlistMode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_ddlistMode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_ddlistMode, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_ddlistMode, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_ddlistMode, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_ddlistMode, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_ddlistMode, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_ddlistMode, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_ddlistMode, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_ddlistMode, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_scrPrintUSB_ddlistMode_extra_list_selected_checked
    static lv_style_t style_scrPrintUSB_ddlistMode_extra_list_selected_checked;
    ui_init_style(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked);

    lv_style_set_border_width(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked, lv_color_hex(0x2f3243));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistMode_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistMode), &style_scrPrintUSB_ddlistMode_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_ddlistMode_extra_list_main_default
    static lv_style_t style_scrPrintUSB_ddlistMode_extra_list_main_default;
    ui_init_style(&style_scrPrintUSB_ddlistMode_extra_list_main_default);

    lv_style_set_max_height(&style_scrPrintUSB_ddlistMode_extra_list_main_default, 90);
    lv_style_set_text_color(&style_scrPrintUSB_ddlistMode_extra_list_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_scrPrintUSB_ddlistMode_extra_list_main_default, &lv_font_montserratMedium_14);
    lv_style_set_text_opa(&style_scrPrintUSB_ddlistMode_extra_list_main_default, 255);
    lv_style_set_border_width(&style_scrPrintUSB_ddlistMode_extra_list_main_default, 0);
    lv_style_set_radius(&style_scrPrintUSB_ddlistMode_extra_list_main_default, 4);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistMode_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistMode_extra_list_main_default, lv_color_hex(0x2f3243));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistMode_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistMode), &style_scrPrintUSB_ddlistMode_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default
    static lv_style_t style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default;
    ui_init_style(&style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default);

    lv_style_set_radius(&style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default, lv_color_hex(0xc1c1c1));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistMode), &style_scrPrintUSB_ddlistMode_extra_list_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //Write codes scrPrintUSB_ddlistDPI
    ui->scrPrintUSB_ddlistDPI = lv_dropdown_create(ui->scrPrintUSB_contDDLpanel);
    lv_obj_set_pos(ui->scrPrintUSB_ddlistDPI, 125, 8);
    lv_obj_set_size(ui->scrPrintUSB_ddlistDPI, 100, 35);
    lv_dropdown_set_options(ui->scrPrintUSB_ddlistDPI, "70 DPI\n96 DPI\n150 DPI\n300 DPI\n600 DPI\n900 DPI\n1200 DPI");

    //Write style for scrPrintUSB_ddlistDPI, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_text_color(ui->scrPrintUSB_ddlistDPI, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintUSB_ddlistDPI, &lv_font_montserratMedium_14, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintUSB_ddlistDPI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintUSB_ddlistDPI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintUSB_ddlistDPI, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintUSB_ddlistDPI, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintUSB_ddlistDPI, 6, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintUSB_ddlistDPI, 4, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintUSB_ddlistDPI, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintUSB_ddlistDPI, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintUSB_ddlistDPI, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintUSB_ddlistDPI, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_CHECKED for &style_scrPrintUSB_ddlistDPI_extra_list_selected_checked
    static lv_style_t style_scrPrintUSB_ddlistDPI_extra_list_selected_checked;
    ui_init_style(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked);

    lv_style_set_border_width(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, 0);
    lv_style_set_radius(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, 0);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, lv_color_hex(0x2f3243));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistDPI), &style_scrPrintUSB_ddlistDPI_extra_list_selected_checked, LV_PART_SELECTED|LV_STATE_CHECKED);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_ddlistDPI_extra_list_main_default
    static lv_style_t style_scrPrintUSB_ddlistDPI_extra_list_main_default;
    ui_init_style(&style_scrPrintUSB_ddlistDPI_extra_list_main_default);

    lv_style_set_max_height(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, 90);
    lv_style_set_text_color(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, &lv_font_montserratMedium_14);
    lv_style_set_text_opa(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, 255);
    lv_style_set_border_width(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, 0);
    lv_style_set_radius(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, 4);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, lv_color_hex(0x2f3243));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistDPI_extra_list_main_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistDPI), &style_scrPrintUSB_ddlistDPI_extra_list_main_default, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write style state: LV_STATE_DEFAULT for &style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default
    static lv_style_t style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default;
    ui_init_style(&style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default);

    lv_style_set_radius(&style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default, 3);
    lv_style_set_bg_opa(&style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default, lv_color_hex(0xc1c1c1));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default, LV_GRAD_DIR_NONE);
    lv_obj_add_style(lv_dropdown_get_list(ui->scrPrintUSB_ddlistDPI), &style_scrPrintUSB_ddlistDPI_extra_list_scrollbar_default, LV_PART_SCROLLBAR|LV_STATE_DEFAULT);

    //The custom code of scrPrintUSB.
    uint32_t i;
    static lv_style_t style_scrPrintUSB_listFiles_extra_btns_main_checked;
    ui_init_style(&style_scrPrintUSB_listFiles_extra_btns_main_checked);

    lv_style_set_pad_top(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 5);
    lv_style_set_pad_left(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 5);
    lv_style_set_pad_right(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 5);
    lv_style_set_pad_bottom(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 5);
    lv_style_set_border_width(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 0);
    lv_style_set_text_color(&style_scrPrintUSB_listFiles_extra_btns_main_checked, lv_color_hex(0xffffff));
    lv_style_set_text_font(&style_scrPrintUSB_listFiles_extra_btns_main_checked, &lv_font_montserratMedium_16);
    lv_style_set_text_opa(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 255);
    lv_style_set_radius(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 3);
    lv_style_set_bg_opa(&style_scrPrintUSB_listFiles_extra_btns_main_checked, 255);
    lv_style_set_bg_color(&style_scrPrintUSB_listFiles_extra_btns_main_checked, lv_color_hex(0x2f3243));
    lv_style_set_bg_grad_dir(&style_scrPrintUSB_listFiles_extra_btns_main_checked, LV_GRAD_DIR_NONE);

    for(i = 0; i < lv_obj_get_child_cnt(ui->scrPrintUSB_listFiles); i++) {
        lv_obj_t * button = lv_obj_get_child(ui->scrPrintUSB_listFiles, i);
        lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);
        lv_obj_add_style(button, &style_scrPrintUSB_listFiles_extra_btns_main_checked, LV_PART_MAIN|LV_STATE_CHECKED);
    }


    //Update current screen layout.
    lv_obj_update_layout(ui->scrPrintUSB);

    //Init events for screen.
    events_init_scrPrintUSB(ui);
}
