/**
 ******************************************************************************
 * @file    lv_font_flash.c
 * @brief   LVGL Flash 字库位图读取 — 从 W25Q64 SPI Flash 读取字形位图
 *
 *          核心思路:
 *          1. 字形描述表 (glyph_dsc, cmaps) 在 MCU 内部 Flash 中编译
 *          2. 位图数据 (glyph_bitmap[]) 存储在 W25Q64 外部 Flash 中
 *          3. 本文件实现 get_glyph_bitmap 回调, 从 SPI Flash 读取位图
 *
 *          用法:
 *          font->get_glyph_bitmap = lv_font_get_bitmap_flash;
 *          font->user_data = (void *)(uintptr_t)FLASH_BITMAP_BASE_ADDR;
 ******************************************************************************
 */
#include "lv_font_flash.h"
#include "w25q64.h"
#include "../lvgl/lvgl.h"
#include <stdio.h>

/* 同 lv_font_fmt_txt.c 的查找表 */
static const uint8_t opa4_table[16] = {0,  17, 34,  51,
                                       68, 85, 102, 119,
                                       136, 153, 170, 187,
                                       204, 221, 238, 255
                                      };
static const uint8_t opa2_table[4] = {0, 85, 170, 255};

/* 最大单字形原始位图字节数 (48px × 48px × 8bpp = 2304) */
#define MAX_GLYPH_RAW_BYTES  2304

/**
 * @brief  Flash 字库位图读取回调
 *
 * 先从 Flash 读取完整的原始位图到栈缓冲区,
 * 再按标准逻辑将 bpp 格式转换为 A8 并写入 draw_buf.
 */
const void * lv_font_get_bitmap_flash(lv_font_glyph_dsc_t * g_dsc, lv_draw_buf_t * draw_buf)
{
    const lv_font_t * font;
    uint32_t flash_base;
    lv_font_fmt_txt_dsc_t * fdsc;
    uint32_t gid;
    const lv_font_fmt_txt_glyph_dsc_t * gdsc;
    int32_t gsize;

    /* 一次性诊断: 确认回调被调用 */
    {
        static int once = 0;
        if (!once) {
            once = 1;
            printf("[FONT_CB] callback fired, font=%p\r\n", (void*)g_dsc->resolved_font);
        }
    }
    uint16_t stride_in;
    uint8_t bpp;
    uint32_t bitmap_offset;
    uint32_t raw_bytes;
    uint8_t raw_buf[MAX_GLYPH_RAW_BYTES];
    const uint8_t * bitmap_in;
    uint8_t * bitmap_out;
    uint32_t stride_out;
    int32_t x, y;
    int32_t i;
    uint16_t line_rem;

    font = g_dsc->resolved_font;

    /* Flash 位图基地址存在 user_data 中 */
    flash_base = (uint32_t)(uintptr_t)font->user_data;

    fdsc = (lv_font_fmt_txt_dsc_t *)font->dsc;
    gid = g_dsc->gid.index;
    if (!gid) return NULL;

    gdsc = &fdsc->glyph_dsc[gid];

    gsize = (int32_t)gdsc->box_w * gdsc->box_h;
    if (gsize == 0) return NULL;

    stride_in = fdsc->stride;
    bpp       = fdsc->bpp;
    bitmap_offset = gdsc->bitmap_index;

    /* 计算原始位图字节数 */
    if (stride_in != 0) {
        raw_bytes = stride_in * gdsc->box_h;
    } else {
        raw_bytes = ((uint32_t)gdsc->box_w * bpp * gdsc->box_h + 7) / 8;
    }

    /* 如果请求原始位图, 直接读入 draw_buf */
    if (g_dsc->req_raw_bitmap) {
        W25Q64_ReadData(flash_base + bitmap_offset, (uint8_t *)draw_buf->data, raw_bytes);
        return draw_buf->data;
    }

    /* 从 Flash 读取完整的原始位图 (CPU轮询读取, 无需D-Cache操作) */
    if (raw_bytes > MAX_GLYPH_RAW_BYTES) return NULL;
    W25Q64_ReadData(flash_base + bitmap_offset, raw_buf, raw_bytes);

    /* === 诊断: 打印前3个有效字形的原始数据与解码结果 === */
    {
        static int diag_cnt = 0;
        if (diag_cnt < 3 && gsize > 4) {
            diag_cnt++;
            printf("[FONT] gid=%lu w=%d h=%d bpp=%d stride=%d off=0x%lX raw=%luB\r\n",
                   gid, gdsc->box_w, gdsc->box_h, bpp, stride_in, bitmap_offset, raw_bytes);
            printf("  raw[0..%d]: ", (int)(raw_bytes > 16 ? 16 : raw_bytes) - 1);
            for (uint32_t k = 0; k < raw_bytes && k < 16; k++) printf("%02X ", raw_buf[k]);
            printf("\r\n");
        }
    }

    bitmap_in = raw_buf;
    bitmap_out = draw_buf->data;
    stride_out = lv_draw_buf_width_to_stride(gdsc->box_w, LV_COLOR_FORMAT_A8);

    /* 同 lv_font_fmt_txt.c 的位图格式转换 */
    if (fdsc->bitmap_format == LV_FONT_FMT_TXT_PLAIN) {
        i = 0;

        if (bpp == 1) {
            for (y = 0; y < gdsc->box_h; y++) {
                line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for (x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x7;
                    if (i == 0) bitmap_out[x] = (*bitmap_in) & 0x80 ? 0xff : 0x00;
                    else if (i == 1) bitmap_out[x] = (*bitmap_in) & 0x40 ? 0xff : 0x00;
                    else if (i == 2) bitmap_out[x] = (*bitmap_in) & 0x20 ? 0xff : 0x00;
                    else if (i == 3) bitmap_out[x] = (*bitmap_in) & 0x10 ? 0xff : 0x00;
                    else if (i == 4) bitmap_out[x] = (*bitmap_in) & 0x08 ? 0xff : 0x00;
                    else if (i == 5) bitmap_out[x] = (*bitmap_in) & 0x04 ? 0xff : 0x00;
                    else if (i == 6) bitmap_out[x] = (*bitmap_in) & 0x02 ? 0xff : 0x00;
                    else if (i == 7) {
                        bitmap_out[x] = (*bitmap_in) & 0x01 ? 0xff : 0x00;
                        line_rem--;
                        bitmap_in++;
                    }
                }
                if (stride_in) {
                    i = 0;
                    bitmap_in += line_rem;
                }
                bitmap_out += stride_out;
            }
        }
        else if (bpp == 2) {
            for (y = 0; y < gdsc->box_h; y++) {
                line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for (x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x3;
                    if (i == 0) bitmap_out[x] = opa2_table[(*bitmap_in) >> 6];
                    else if (i == 1) bitmap_out[x] = opa2_table[((*bitmap_in) >> 4) & 0x3];
                    else if (i == 2) bitmap_out[x] = opa2_table[((*bitmap_in) >> 2) & 0x3];
                    else if (i == 3) {
                        bitmap_out[x] = opa2_table[((*bitmap_in) >> 0) & 0x3];
                        line_rem--;
                        bitmap_in++;
                    }
                }
                if (stride_in) {
                    i = 0;
                    bitmap_in += line_rem;
                }
                bitmap_out += stride_out;
            }
        }
        else if (bpp == 4) {
            for (y = 0; y < gdsc->box_h; y++) {
                line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                for (x = 0; x < gdsc->box_w; x++, i++) {
                    i = i & 0x1;
                    if (i == 0) {
                        bitmap_out[x] = opa4_table[(*bitmap_in) >> 4];
                    }
                    else if (i == 1) {
                        bitmap_out[x] = opa4_table[(*bitmap_in) & 0xF];
                        line_rem--;
                        bitmap_in++;
                    }
                }
                if (stride_in) {
                    i = 0;
                    bitmap_in += line_rem;
                }
                bitmap_out += stride_out;
            }
        }
        else if (bpp == 8) {
            for (y = 0; y < gdsc->box_h; y++) {
                for (x = 0; x < gdsc->box_w; x++) {
                    bitmap_out[x] = *bitmap_in++;
                }
                line_rem = stride_in != 0 ? stride_in : gdsc->box_w;
                bitmap_out += stride_out;
                bitmap_in += line_rem;
            }
        }

        lv_draw_buf_flush_cache(draw_buf, NULL);

        /* === 诊断: 打印解码后的 A8 像素 === */
        {
            static int diag2_cnt = 0;
            if (diag2_cnt < 3) {
                diag2_cnt++;
                int pw = gdsc->box_w < 8 ? gdsc->box_w : 8;
                int ph = gdsc->box_h < 4 ? gdsc->box_h : 4;
                printf("  A8 decoded (%dx%d top-left):\r\n", gdsc->box_w, gdsc->box_h);
                uint8_t *p = draw_buf->data;
                uint32_t so = lv_draw_buf_width_to_stride(gdsc->box_w, LV_COLOR_FORMAT_A8);
                for (int r = 0; r < ph; r++) {
                    printf("    row%d: ", r);
                    for (int c = 0; c < pw; c++) printf("%02X ", p[r * so + c]);
                    printf("\r\n");
                }
            }
        }
    }

    return draw_buf;
}
