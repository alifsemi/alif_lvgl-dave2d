/**
 * @file 'lv_dave2_cfg.h'
 *
 * @brief Config for LVGL D/AVE2D integration layer
*/

#ifndef LV_DRAW_DAVE2D_CFG_H
#define LV_DRAW_DAVE2D_CFG_H

/**
 * Use internal render double buffering
*/
#define D2_USE_INTERNAL_RENDERBUFFERS (1)

/**
 * Render each letter separately or use a buffer
*/
#define D2_LABEL_RENDER_EACH_LETTER   (0)
#if (D2_LABEL_RENDER_EACH_LETTER == 0)
#define D2_LABEL_BUF_SIZE   (128 * 1024U)
#endif

#endif /*LV_DRAW_DAVE2D_CFG_H*/