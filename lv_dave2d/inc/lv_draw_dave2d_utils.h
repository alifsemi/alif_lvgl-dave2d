/**
 * @file lv_draw_dave2d_utils.h
 *
 */

#ifndef LV_DRAW_DAVE2D_UTILS_H
#define LV_DRAW_DAVE2D_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef lv_ll_t d2_buf;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

d2_color lv_draw_dave2d_lv_colour_to_d2_colour(lv_color_t color);

d2_s32 lv_draw_dave2d_cf_fb_get(void);

d2_u32 lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(lv_color_format_t colour_format);

void d2_framebuffer_from_layer(d2_device * handle, lv_layer_t * layer);

void d2_utils_init(d2_device * handle);

void * d2_buf_alloc(uint32_t size);

void d2_buf_free(void * ptr);

void d2_buf_copy(void * ptr, uint32_t size);

void d2_buf_add(void * ptr);

bool d2_buf_on_rendering(void * ptr);

void d2_start_rendering(void);

void d2_finish_rendering(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DAVE2D_UTILS_H*/
