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



void setup_scr_scrSetup(lv_ui *ui)
{
    //Write codes scrSetup
    ui->scrSetup = lv_obj_create(NULL);
    lv_obj_set_size(ui->scrSetup, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrSetup, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrSetup, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrSetup, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrSetup, lv_color_hex(0xF3F8FE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrSetup, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_contBG
    ui->scrSetup_contBG = lv_obj_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_contBG, 0, 0);
    lv_obj_set_size(ui->scrSetup_contBG, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrSetup_contBG, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrSetup_contBG, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrSetup_contBG, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrSetup_contBG, lv_color_hex(0xe12e2e), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrSetup_contBG, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrSetup_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_btnBack
    ui->scrSetup_btnBack = lv_button_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_btnBack, 183, 235);
    lv_obj_set_size(ui->scrSetup_btnBack, 120, 47);
    ui->scrSetup_btnBack_label = lv_label_create(ui->scrSetup_btnBack);
    lv_label_set_text(ui->scrSetup_btnBack_label, "BACK");
    lv_label_set_long_mode(ui->scrSetup_btnBack_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrSetup_btnBack_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrSetup_btnBack, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrSetup_btnBack_label, LV_PCT(100));

    //Write style for scrSetup_btnBack, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrSetup_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrSetup_btnBack, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->scrSetup_btnBack, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->scrSetup_btnBack, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->scrSetup_btnBack, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrSetup_btnBack, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrSetup_btnBack, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrSetup_btnBack, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrSetup_btnBack, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrSetup_btnBack, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrSetup_btnBack, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_labelPrompt
    ui->scrSetup_labelPrompt = lv_label_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_labelPrompt, 32, 188);
    lv_obj_set_size(ui->scrSetup_labelPrompt, 435, 31);
    lv_label_set_text(ui->scrSetup_labelPrompt, "You have no permission to change the settings");
    lv_label_set_long_mode(ui->scrSetup_labelPrompt, LV_LABEL_LONG_WRAP);

    //Write style for scrSetup_labelPrompt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrSetup_labelPrompt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrSetup_labelPrompt, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrSetup_labelPrompt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrSetup_labelPrompt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrSetup_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_imgPrinter
    ui->scrSetup_imgPrinter = lv_image_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_imgPrinter, 136, 70);
    lv_obj_set_size(ui->scrSetup_imgPrinter, 70, 80);
    lv_obj_add_flag(ui->scrSetup_imgPrinter, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrSetup_imgPrinter, &_printer2_RGB565A8_70x80);
    lv_image_set_pivot(ui->scrSetup_imgPrinter, 50,50);
    lv_image_set_rotation(ui->scrSetup_imgPrinter, 0);

    //Write style for scrSetup_imgPrinter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrSetup_imgPrinter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrSetup_imgPrinter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_imgWave
    ui->scrSetup_imgWave = lv_image_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_imgWave, 224, 64);
    lv_obj_set_size(ui->scrSetup_imgWave, 25, 27);
    lv_obj_add_flag(ui->scrSetup_imgWave, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrSetup_imgWave, &_no_internet_RGB565A8_25x27);
    lv_image_set_pivot(ui->scrSetup_imgWave, 50,50);
    lv_image_set_rotation(ui->scrSetup_imgWave, 0);

    //Write style for scrSetup_imgWave, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrSetup_imgWave, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrSetup_imgWave, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrSetup_imgCloud
    ui->scrSetup_imgCloud = lv_image_create(ui->scrSetup);
    lv_obj_set_pos(ui->scrSetup_imgCloud, 272, 35);
    lv_obj_set_size(ui->scrSetup_imgCloud, 60, 47);
    lv_obj_add_flag(ui->scrSetup_imgCloud, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrSetup_imgCloud, &_cloud_RGB565A8_60x47);
    lv_image_set_pivot(ui->scrSetup_imgCloud, 50,50);
    lv_image_set_rotation(ui->scrSetup_imgCloud, 0);

    //Write style for scrSetup_imgCloud, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrSetup_imgCloud, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrSetup_imgCloud, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of scrSetup.


    //Update current screen layout.
    lv_obj_update_layout(ui->scrSetup);

    //Init events for screen.
    events_init_scrSetup(ui);
}
