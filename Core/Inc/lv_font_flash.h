/**
 ******************************************************************************
 * @file    lv_font_flash.h
 * @brief   LVGL Flash 字库模块 — 从 W25Q64 外部 Flash 读取字形位图
 *
 * @note    使用方法:
 *          1. 用 lv_font_conv 生成字库 C 文件
 *          2. 将 glyph_bitmap[] 数组数据烧录到 W25Q64 中 (地址由用户指定)
 *          3. 将生成的 lv_font_t 结构体中的:
 *             - .get_glyph_bitmap = lv_font_get_bitmap_flash  (替换默认回调)
 *             - .user_data = (void *)(uintptr_t)flash_addr   (位图在Flash中的起始地址)
 *          4. 字形描述表 (glyph_dsc, cmaps) 留在固件中编译
 ******************************************************************************
 */
#ifndef LV_FONT_FLASH_H
#define LV_FONT_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl/lvgl.h"

/**
 * @brief  Flash 字库位图读取回调（替代 lv_font_get_bitmap_fmt_txt）
 * @param  g_dsc     LVGL 传入的字形描述
 * @param  draw_buf  LVGL 提供的绘制缓冲区 (A8 格式输出)
 * @return 指向位图数据的指针 (通常返回 draw_buf->data)
 *
 * @note   使用前需设置 font->user_data 为 Flash 中位图数据的起始地址 (uint32_t)
 *         font->dsc 仍需指向有效的 lv_font_fmt_txt_dsc_t (不含 glyph_bitmap 指针)
 *         字形描述表在固件中，位图数据在外部 W25Q64 中
 */
const void * lv_font_get_bitmap_flash(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf);

#ifdef __cplusplus
}
#endif

#endif /* LV_FONT_FLASH_H */
