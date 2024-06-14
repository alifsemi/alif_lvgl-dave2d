#ifndef LV_DRAW_DAVE2D_H
#define LV_DRAW_DAVE2D_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "src/lv_conf_internal.h"
#include "src/draw/lv_draw.h"
#include "dave_driver.h"
#include "lv_draw_dave2d_utils.h"
#include "lv_dave2d_cfg.h"

/*********************
 *      DEFINES
 *********************/

#define D2_RENDER_EACH_OPERATION      (1)

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
#if LV_USE_OS
    lv_thread_sync_t sync;
    lv_thread_t thread;
#endif
    uint32_t idx;
    d2_device * d2_handle;
#if (D2_USE_INTERNAL_RENDERBUFFERS == 0)
    d2_renderbuffer * renderbuffer;
#endif
#if (D2_LABEL_RENDER_EACH_LETTER == 0)
    lv_draw_buf_t * label_drawbuffer;
    lv_area_t * label_coords;
#endif
#if LV_USE_OS
    lv_mutex_t * pd2Mutex;
#endif
} lv_draw_dave2d_unit_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

void lv_draw_dave2d_init(void);

void lv_draw_dave2d_image(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_dave2d_fill(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_border(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_border_dsc_t * dsc,
                           const lv_area_t * coords);

void lv_draw_dave2d_box_shadow(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_box_shadow_dsc_t * dsc,
                               const lv_area_t * coords);

void lv_draw_dave2d_label(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_arc(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);

void lv_draw_dave2d_line(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_line_dsc_t * dsc);

void lv_draw_dave2d_layer(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_image_dsc_t * draw_dsc,
                          const lv_area_t * coords);

void lv_draw_dave2d_triangle(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_triangle_dsc_t * dsc);

void lv_draw_dave2d_mask_rect(lv_draw_dave2d_unit_t * draw_unit, const lv_draw_mask_rect_dsc_t * dsc,
                              const lv_area_t * coords);

void lv_draw_dave2d_transform(lv_draw_dave2d_unit_t * draw_unit, const lv_area_t * dest_area, const void * src_buf,
                              int32_t src_w, int32_t src_h, int32_t src_stride,
                              const lv_draw_image_dsc_t * draw_dsc, const lv_draw_image_sup_t * sup, lv_color_format_t cf, void * dest_buf);

/***********************
 * GLOBAL VARIABLES
 ***********************/

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_DRAW_DAVE2D_H*/
