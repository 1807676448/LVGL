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



void setup_scr_scrPrintMobile(lv_ui *ui)
{
    //Write codes scrPrintMobile
    ui->scrPrintMobile = lv_obj_create(NULL);
    lv_obj_set_size(ui->scrPrintMobile, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrPrintMobile, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMobile, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintMobile, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMobile, lv_color_hex(0xF3F8FE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMobile, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_contBG
    ui->scrPrintMobile_contBG = lv_obj_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_contBG, 0, 0);
    lv_obj_set_size(ui->scrPrintMobile_contBG, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrPrintMobile_contBG, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrPrintMobile_contBG, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMobile_contBG, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrPrintMobile_contBG, lv_color_hex(0x2f3243), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrPrintMobile_contBG, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMobile_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_btnNxet
    ui->scrPrintMobile_btnNxet = lv_button_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_btnNxet, 167, 235);
    lv_obj_set_size(ui->scrPrintMobile_btnNxet, 140, 47);
    ui->scrPrintMobile_btnNxet_label = lv_label_create(ui->scrPrintMobile_btnNxet);
    lv_label_set_text(ui->scrPrintMobile_btnNxet_label, "CONTINUE");
    lv_label_set_long_mode(ui->scrPrintMobile_btnNxet_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrPrintMobile_btnNxet_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrPrintMobile_btnNxet, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrPrintMobile_btnNxet_label, LV_PCT(100));

    //Write style for scrPrintMobile_btnNxet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrPrintMobile_btnNxet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrPrintMobile_btnNxet, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->scrPrintMobile_btnNxet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->scrPrintMobile_btnNxet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->scrPrintMobile_btnNxet, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMobile_btnNxet, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMobile_btnNxet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMobile_btnNxet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMobile_btnNxet, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMobile_btnNxet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMobile_btnNxet, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_labelPrompt
    ui->scrPrintMobile_labelPrompt = lv_label_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_labelPrompt, 90, 188);
    lv_obj_set_size(ui->scrPrintMobile_labelPrompt, 299, 28);
    lv_label_set_text(ui->scrPrintMobile_labelPrompt, "Put your phone near to the printer");
    lv_label_set_long_mode(ui->scrPrintMobile_labelPrompt, LV_LABEL_LONG_WRAP);

    //Write style for scrPrintMobile_labelPrompt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrPrintMobile_labelPrompt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrPrintMobile_labelPrompt, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrPrintMobile_labelPrompt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrPrintMobile_labelPrompt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrPrintMobile_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_imgPhone
    ui->scrPrintMobile_imgPhone = lv_image_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_imgPhone, 270, 70);
    lv_obj_set_size(ui->scrPrintMobile_imgPhone, 50, 70);
    lv_obj_add_flag(ui->scrPrintMobile_imgPhone, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMobile_imgPhone, &_phone_RGB565A8_50x70);
    lv_image_set_pivot(ui->scrPrintMobile_imgPhone, 50,50);
    lv_image_set_rotation(ui->scrPrintMobile_imgPhone, 0);

    //Write style for scrPrintMobile_imgPhone, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMobile_imgPhone, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMobile_imgPhone, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_imgWave
    ui->scrPrintMobile_imgWave = lv_image_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_imgWave, 231, 94);
    lv_obj_set_size(ui->scrPrintMobile_imgWave, 20, 41);
    lv_obj_add_flag(ui->scrPrintMobile_imgWave, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMobile_imgWave, &_wave_RGB565A8_20x41);
    lv_image_set_pivot(ui->scrPrintMobile_imgWave, 50,50);
    lv_image_set_rotation(ui->scrPrintMobile_imgWave, 0);

    //Write style for scrPrintMobile_imgWave, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMobile_imgWave, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMobile_imgWave, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrPrintMobile_imgPrinter
    ui->scrPrintMobile_imgPrinter = lv_image_create(ui->scrPrintMobile);
    lv_obj_set_pos(ui->scrPrintMobile_imgPrinter, 140, 70);
    lv_obj_set_size(ui->scrPrintMobile_imgPrinter, 70, 80);
    lv_obj_add_flag(ui->scrPrintMobile_imgPrinter, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrPrintMobile_imgPrinter, &_printer2_RGB565A8_70x80);
    lv_image_set_pivot(ui->scrPrintMobile_imgPrinter, 50,50);
    lv_image_set_rotation(ui->scrPrintMobile_imgPrinter, 0);

    //Write style for scrPrintMobile_imgPrinter, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrPrintMobile_imgPrinter, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrPrintMobile_imgPrinter, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of scrPrintMobile.


    //Update current screen layout.
    lv_obj_update_layout(ui->scrPrintMobile);

    //Init events for screen.
    events_init_scrPrintMobile(ui);
}
