/*
* Copyright 2025 NXP
* NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl/lvgl.h"


typedef struct
{
  
	lv_obj_t *scrHome;
	bool scrHome_del;
	lv_obj_t *scrHome_contBG;
	lv_obj_t *scrHome_contText;
	lv_obj_t *scrHome_labelPrompt;
	lv_obj_t *scrHome_contColorInk;
	lv_obj_t *scrHome_barBlueInk;
	lv_obj_t *scrHome_barRedInk;
	lv_obj_t *scrHome_barYellowInk;
	lv_obj_t *scrHome_barBlackInk;
	lv_obj_t *scrHome_contTop;
	lv_obj_t *scrHome_imgIconWIFI;
	lv_obj_t *scrHome_imgIconCall;
	lv_obj_t *scrHome_imgIconEco;
	lv_obj_t *scrHome_imgIconPC;
	lv_obj_t *scrHome_labelDate;
	lv_obj_t *scrHome_contMain;
	lv_obj_t *scrHome_contCopy;
	lv_obj_t *scrHome_imgIconCopy;
	lv_obj_t *scrHome_labelCopy;
	lv_obj_t *scrHome_contScan;
	lv_obj_t *scrHome_imgIconScan;
	lv_obj_t *scrHome_labelScan;
	lv_obj_t *scrHome_contPrint;
	lv_obj_t *scrHome_imgIconPrint;
	lv_obj_t *scrHome_labelPrint;
	lv_obj_t *scrHome_contSetup;
	lv_obj_t *scrHome_imgIconSetup;
	lv_obj_t *scrHome_labelSetup;
	lv_obj_t *scrCopy;
	bool scrCopy_del;
	lv_obj_t *scrCopy_contBG;
	lv_obj_t *scrCopy_labelTitle;
	lv_obj_t *scrCopy_btnBack;
	lv_obj_t *scrCopy_btnBack_label;
	lv_obj_t *scrCopy_contPanel;
	lv_obj_t *scrCopy_imgBright;
	lv_obj_t *scrCopy_imgColor;
	lv_obj_t *scrCopy_sliderBright;
	lv_obj_t *scrCopy_sliderHue;
	lv_obj_t *scrCopy_btnNext;
	lv_obj_t *scrCopy_btnNext_label;
	lv_obj_t *scrCopy_contImage;
	lv_obj_t *scrCopy_imgScanned;
	lv_obj_t *scrCopy2;
	bool scrCopy2_del;
	lv_obj_t *scrCopy2_contBG;
	lv_obj_t *scrCopy2_labelTitle;
	lv_obj_t *scrCopy2_btnBack;
	lv_obj_t *scrCopy2_btnBack_label;
	lv_obj_t *scrCopy2_contImage;
	lv_obj_t *scrCopy2_imgScanned;
	lv_obj_t *scrCopy2_contPanel;
	lv_obj_t *scrCopy2_labelCopies;
	lv_obj_t *scrCopy2_btnMinus;
	lv_obj_t *scrCopy2_btnMinus_label;
	lv_obj_t *scrCopy2_btnPlus;
	lv_obj_t *scrCopy2_btnPlus_label;
	lv_obj_t *scrCopy2_labelCounter;
	lv_obj_t *scrCopy2_labelColor;
	lv_obj_t *scrCopy2_labelVertical;
	lv_obj_t *scrCopy2_swColor;
	lv_obj_t *scrCopy2_swVertical;
	lv_obj_t *scrCopy2_btnNext;
	lv_obj_t *scrCopy2_btnNext_label;
	lv_obj_t *scrCopy2_contDDLpanel;
	lv_obj_t *scrCopy2_ddlistMode;
	lv_obj_t *scrCopy2_ddlistDPI;
	lv_obj_t *scrPrintMenu;
	bool scrPrintMenu_del;
	lv_obj_t *scrPrintMenu_contBG;
	lv_obj_t *scrPrintMenu_labelTitle;
	lv_obj_t *scrPrintMenu_btnBack;
	lv_obj_t *scrPrintMenu_btnBack_label;
	lv_obj_t *scrPrintMenu_contMain;
	lv_obj_t *scrPrintMenu_contUSB;
	lv_obj_t *scrPrintMenu_imgUSB;
	lv_obj_t *scrPrintMenu_labelUSB;
	lv_obj_t *scrPrintMenu_contMobile;
	lv_obj_t *scrPrintMenu_imgMobile;
	lv_obj_t *scrPrintMenu_labelMobile;
	lv_obj_t *scrPrintMenu_contInternet;
	lv_obj_t *scrPrintMenu_imgInternet;
	lv_obj_t *scrPrintMenu_labelInternet;
	lv_obj_t *scrPrintMenu_contText;
	lv_obj_t *scrPrintMenu_labelPrompt;
	lv_obj_t *scrPrintUSB;
	bool scrPrintUSB_del;
	lv_obj_t *scrPrintUSB_contBG;
	lv_obj_t *scrPrintUSB_labelTitle;
	lv_obj_t *scrPrintUSB_btnBack;
	lv_obj_t *scrPrintUSB_btnBack_label;
	lv_obj_t *scrPrintUSB_contPanel;
	lv_obj_t *scrPrintUSB_labelCopies;
	lv_obj_t *scrPrintUSB_btnMinus;
	lv_obj_t *scrPrintUSB_btnMinus_label;
	lv_obj_t *scrPrintUSB_btnPlus;
	lv_obj_t *scrPrintUSB_btnPlus_label;
	lv_obj_t *scrPrintUSB_labelCounter;
	lv_obj_t *scrPrintUSB_labelColor;
	lv_obj_t *scrPrintUSB_labelVertical;
	lv_obj_t *scrPrintUSB_swColor;
	lv_obj_t *scrPrintUSB_swVertical;
	lv_obj_t *scrPrintUSB_btnNext;
	lv_obj_t *scrPrintUSB_btnNext_label;
	lv_obj_t *scrPrintUSB_listFiles;
	lv_obj_t *scrPrintUSB_listFiles_item0;
	lv_obj_t *scrPrintUSB_listFiles_item1;
	lv_obj_t *scrPrintUSB_listFiles_item2;
	lv_obj_t *scrPrintUSB_listFiles_item3;
	lv_obj_t *scrPrintUSB_listFiles_item4;
	lv_obj_t *scrPrintUSB_listFiles_item5;
	lv_obj_t *scrPrintUSB_listFiles_item6;
	lv_obj_t *scrPrintUSB_listFiles_item7;
	lv_obj_t *scrPrintUSB_listFiles_item8;
	lv_obj_t *scrPrintUSB_listFiles_item9;
	lv_obj_t *scrPrintUSB_listFiles_item10;
	lv_obj_t *scrPrintUSB_listFiles_item11;
	lv_obj_t *scrPrintUSB_listFiles_item12;
	lv_obj_t *scrPrintUSB_listFiles_item13;
	lv_obj_t *scrPrintUSB_listFiles_item14;
	lv_obj_t *scrPrintUSB_listFiles_item15;
	lv_obj_t *scrPrintUSB_listFiles_item16;
	lv_obj_t *scrPrintUSB_listFiles_item17;
	lv_obj_t *scrPrintUSB_listFiles_item18;
	lv_obj_t *scrPrintUSB_contDDLpanel;
	lv_obj_t *scrPrintUSB_ddlistMode;
	lv_obj_t *scrPrintUSB_ddlistDPI;
	lv_obj_t *scrPrintMobile;
	bool scrPrintMobile_del;
	lv_obj_t *scrPrintMobile_contBG;
	lv_obj_t *scrPrintMobile_btnNxet;
	lv_obj_t *scrPrintMobile_btnNxet_label;
	lv_obj_t *scrPrintMobile_labelPrompt;
	lv_obj_t *scrPrintMobile_imgPhone;
	lv_obj_t *scrPrintMobile_imgWave;
	lv_obj_t *scrPrintMobile_imgPrinter;
	lv_obj_t *scrSetup;
	bool scrSetup_del;
	lv_obj_t *scrSetup_contBG;
	lv_obj_t *scrSetup_btnBack;
	lv_obj_t *scrSetup_btnBack_label;
	lv_obj_t *scrSetup_labelPrompt;
	lv_obj_t *scrSetup_imgPrinter;
	lv_obj_t *scrSetup_imgWave;
	lv_obj_t *scrSetup_imgCloud;
	lv_obj_t *scrLoader;
	bool scrLoader_del;
	lv_obj_t *scrLoader_contBG;
	lv_obj_t *scrLoader_arcLoader;
	lv_obj_t *scrLoader_labelLoader;
	lv_obj_t *scrLoader_labelPrompt;
	lv_obj_t *scrFinished;
	bool scrFinished_del;
	lv_obj_t *scrFinished_contBG;
	lv_obj_t *scrFinished_imgIconOk;
	lv_obj_t *scrFinished_btnNxet;
	lv_obj_t *scrFinished_btnNxet_label;
	lv_obj_t *scrFinished_labelPrompt;
}lv_ui;

typedef void (*ui_setup_scr_t)(lv_ui * ui);

void ui_init_style(lv_style_t * style);

void ui_load_scr_animation(lv_ui *ui, lv_obj_t ** new_scr, bool new_scr_del, bool * old_scr_del, ui_setup_scr_t setup_scr,
                           lv_screen_load_anim_t anim_type, uint32_t time, uint32_t delay, bool is_clean, bool auto_del);

void ui_animation(void * var, uint32_t duration, int32_t delay, int32_t start_value, int32_t end_value, lv_anim_path_cb_t path_cb,
                  uint32_t repeat_cnt, uint32_t repeat_delay, uint32_t playback_time, uint32_t playback_delay,
                  lv_anim_exec_xcb_t exec_cb, lv_anim_start_cb_t start_cb, lv_anim_completed_cb_t ready_cb, lv_anim_deleted_cb_t deleted_cb);


void init_scr_del_flag(lv_ui *ui);

void setup_bottom_layer(void);

void setup_ui(lv_ui *ui);

void video_play(lv_ui *ui);

void init_keyboard(lv_ui *ui);

extern lv_ui guider_ui;


void setup_scr_scrHome(lv_ui *ui);
void setup_scr_scrCopy(lv_ui *ui);
void setup_scr_scrCopy2(lv_ui *ui);
void setup_scr_scrPrintMenu(lv_ui *ui);
void setup_scr_scrPrintUSB(lv_ui *ui);
void setup_scr_scrPrintMobile(lv_ui *ui);
void setup_scr_scrSetup(lv_ui *ui);
void setup_scr_scrLoader(lv_ui *ui);
void setup_scr_scrFinished(lv_ui *ui);
LV_IMAGE_DECLARE(_wifi_RGB565A8_30x24);
LV_IMAGE_DECLARE(_tel_RGB565A8_24x24);
LV_IMAGE_DECLARE(_eco_RGB565A8_29x27);
LV_IMAGE_DECLARE(_pc_RGB565A8_30x29);

LV_IMAGE_DECLARE(_btn_bg_1_RGB565A8_100x141);
LV_IMAGE_DECLARE(_copy_RGB565A8_37x47);

LV_IMAGE_DECLARE(_btn_bg_2_RGB565A8_100x141);
LV_IMAGE_DECLARE(_scan_RGB565A8_37x47);

LV_IMAGE_DECLARE(_btn_bg_3_RGB565A8_100x141);
LV_IMAGE_DECLARE(_print_RGB565A8_37x47);

LV_IMAGE_DECLARE(_btn_bg_4_RGB565A8_100x141);
LV_IMAGE_DECLARE(_setup_RGB565A8_37x47);
LV_IMAGE_DECLARE(_bright_RGB565A8_20x20);
LV_IMAGE_DECLARE(_hue_RGB565A8_18x18);
LV_IMAGE_DECLARE(_example_RGB565A8_310x228);
LV_IMAGE_DECLARE(_usb_RGB565A8_37x47);
LV_IMAGE_DECLARE(_mobile_RGB565A8_37x47);
LV_IMAGE_DECLARE(_internet_RGB565A8_37x47);
LV_IMAGE_DECLARE(_phone_RGB565A8_50x70);
LV_IMAGE_DECLARE(_wave_RGB565A8_20x41);
LV_IMAGE_DECLARE(_printer2_RGB565A8_70x80);
LV_IMAGE_DECLARE(_no_internet_RGB565A8_25x27);
LV_IMAGE_DECLARE(_cloud_RGB565A8_60x47);
LV_IMAGE_DECLARE(_ready_RGB565A8_100x100);

LV_FONT_DECLARE(lv_font_montserratMedium_14)
LV_FONT_DECLARE(lv_font_montserratMedium_16)
LV_FONT_DECLARE(lv_font_montserratMedium_20)
LV_FONT_DECLARE(lv_font_montserratMedium_25)
LV_FONT_DECLARE(lv_font_montserratMedium_12)


#ifdef __cplusplus
}
#endif
#endif
