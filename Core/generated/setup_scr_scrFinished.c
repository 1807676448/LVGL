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



void setup_scr_scrFinished(lv_ui *ui)
{
    //Write codes scrFinished
    ui->scrFinished = lv_obj_create(NULL);
    lv_obj_set_size(ui->scrFinished, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrFinished, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrFinished, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrFinished, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrFinished, lv_color_hex(0xF3F8FE), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrFinished, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrFinished_contBG
    ui->scrFinished_contBG = lv_obj_create(ui->scrFinished);
    lv_obj_set_pos(ui->scrFinished_contBG, 0, 0);
    lv_obj_set_size(ui->scrFinished_contBG, 480, 320);
    lv_obj_set_scrollbar_mode(ui->scrFinished_contBG, LV_SCROLLBAR_MODE_OFF);

    //Write style for scrFinished_contBG, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrFinished_contBG, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(ui->scrFinished_contBG, lv_color_hex(0x28b620), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_dir(ui->scrFinished_contBG, LV_GRAD_DIR_NONE, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrFinished_contBG, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrFinished_imgIconOk
    ui->scrFinished_imgIconOk = lv_image_create(ui->scrFinished);
    lv_obj_set_pos(ui->scrFinished_imgIconOk, 186, 23);
    lv_obj_set_size(ui->scrFinished_imgIconOk, 100, 100);
    lv_obj_add_flag(ui->scrFinished_imgIconOk, LV_OBJ_FLAG_CLICKABLE);
    lv_image_set_src(ui->scrFinished_imgIconOk, &_ready_RGB565A8_100x100);
    lv_image_set_pivot(ui->scrFinished_imgIconOk, 50,50);
    lv_image_set_rotation(ui->scrFinished_imgIconOk, 0);

    //Write style for scrFinished_imgIconOk, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_image_recolor_opa(ui->scrFinished_imgIconOk, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_image_opa(ui->scrFinished_imgIconOk, 255, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrFinished_btnNxet
    ui->scrFinished_btnNxet = lv_button_create(ui->scrFinished);
    lv_obj_set_pos(ui->scrFinished_btnNxet, 167, 235);
    lv_obj_set_size(ui->scrFinished_btnNxet, 140, 47);
    ui->scrFinished_btnNxet_label = lv_label_create(ui->scrFinished_btnNxet);
    lv_label_set_text(ui->scrFinished_btnNxet_label, "CONTINUE");
    lv_label_set_long_mode(ui->scrFinished_btnNxet_label, LV_LABEL_LONG_WRAP);
    lv_obj_align(ui->scrFinished_btnNxet_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(ui->scrFinished_btnNxet, 0, LV_STATE_DEFAULT);
    lv_obj_set_width(ui->scrFinished_btnNxet_label, LV_PCT(100));

    //Write style for scrFinished_btnNxet, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_bg_opa(ui->scrFinished_btnNxet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(ui->scrFinished_btnNxet, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_opa(ui->scrFinished_btnNxet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_color(ui->scrFinished_btnNxet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_border_side(ui->scrFinished_btnNxet, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrFinished_btnNxet, 23, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrFinished_btnNxet, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrFinished_btnNxet, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrFinished_btnNxet, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrFinished_btnNxet, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrFinished_btnNxet, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);

    //Write codes scrFinished_labelPrompt
    ui->scrFinished_labelPrompt = lv_label_create(ui->scrFinished);
    lv_obj_set_pos(ui->scrFinished_labelPrompt, 126, 188);
    lv_obj_set_size(ui->scrFinished_labelPrompt, 217, 32);
    lv_label_set_text(ui->scrFinished_labelPrompt, "Printing finished");
    lv_label_set_long_mode(ui->scrFinished_labelPrompt, LV_LABEL_LONG_WRAP);

    //Write style for scrFinished_labelPrompt, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
    lv_obj_set_style_border_width(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_radius(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(ui->scrFinished_labelPrompt, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(ui->scrFinished_labelPrompt, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(ui->scrFinished_labelPrompt, 255, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_line_space(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_text_align(ui->scrFinished_labelPrompt, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui->scrFinished_labelPrompt, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

    //The custom code of scrFinished.


    //Update current screen layout.
    lv_obj_update_layout(ui->scrFinished);

    //Init events for screen.
    events_init_scrFinished(ui);
}
