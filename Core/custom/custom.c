/*
* Copyright 2024 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/


/*********************
 *      INCLUDES
 *********************/
#include <stdio.h>
#include "../lvgl/lvgl.h"
#include "custom.h"
#include "../generated/gui_guider.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**
 * Create a demo application
 */

void custom_init(lv_ui *ui)
{
    /* Add your codes here */
}

void slider_adjust_img_cb(lv_obj_t * img, int32_t brightValue, int16_t hueValue)
{
    static lv_color_t recolor;

    recolor = lv_color_hsv_to_rgb(hueValue, 100, brightValue);

    lv_obj_set_style_img_recolor(img, recolor, LV_PART_MAIN|LV_STATE_DEFAULT);
    lv_obj_set_style_img_recolor_opa(img, 50, LV_PART_MAIN|LV_STATE_DEFAULT);
}

void loader_anim_cb(lv_obj_t* var, uint32_t value)
{
    lv_arc_set_value(var, value);
    lv_label_set_text_fmt(guider_ui.scrLoader_labelLoader, "%d%%", value);
}

void loader_anim_complete_cb(lv_obj_t* var)
{
    ui_load_scr_animation(&guider_ui, &guider_ui.scrFinished, guider_ui.scrFinished_del, &guider_ui.scrLoader_del, setup_scr_scrFinished, LV_SCR_LOAD_ANIM_FADE_ON, 200, 100, false, true);
}