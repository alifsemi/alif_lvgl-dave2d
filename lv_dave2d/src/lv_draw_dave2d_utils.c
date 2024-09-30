/**
 * @file lv_draw_dave2d_utils.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_dave2d.h"
#include "dave_base.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void d2_buf_clear_cb(void * node);
static void d2_buf_switch(void);
static void d2_buf_clear(void);
static bool d2_buf_contains(d2_buf * buf, void * ptr);

/**********************
 *  STATIC VARIABLES
 **********************/

static d2_device * _d2_handle;
static lv_ll_t _d2_buf_1, _d2_buf_2;
static lv_ll_t * _d2_buf_act;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

d2_color  lv_draw_dave2d_lv_colour_to_d2_colour(lv_color_t color)
{
    uint8_t alpha, red, green, blue;

    alpha = 0x00;
    red = color.red ;
    green = color.green ;
    blue = color.blue;
    /*Color depth: 8 (A8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
    switch(LV_COLOR_DEPTH) {
        case(8):
            __BKPT(0);
            break;
        case(16):
            break;
        case(24):
            break;
        case(32):
            break;

        default:
            break;
    }

    return (alpha) << 24UL
           | (red) << 16UL
           | (green) << 8UL
           | (blue) << 0UL;
}

/**
 * @todo
 * This function is used in _dave2d_buf_copy() that's currently commented out
 */
#if 0
d2_s32 lv_draw_dave2d_cf_fb_get(void)
{
    d2_s32 d2_fb_mode = 0;
    switch(g_display0_cfg.input->format) {
        case    DISPLAY_IN_FORMAT_16BITS_RGB565: ///< RGB565,   16 bits
            d2_fb_mode = d2_mode_rgb565;
            break;
        case DISPLAY_IN_FORMAT_32BITS_ARGB8888: ///< ARGB8888, 32 bits
            d2_fb_mode = d2_mode_argb8888;
            break;
        case DISPLAY_IN_FORMAT_32BITS_RGB888: ///< RGB888,   32 bits
            d2_fb_mode = d2_mode_rgb888;
            break;
        case  DISPLAY_IN_FORMAT_16BITS_ARGB4444: ///< ARGB4444, 16 bits
            d2_fb_mode = d2_mode_argb4444;
            break;
        case DISPLAY_IN_FORMAT_16BITS_ARGB1555: ///< ARGB1555, 16 bits
        case DISPLAY_IN_FORMAT_CLUT8 : ///< CLUT8
        case DISPLAY_IN_FORMAT_CLUT4  : ///< CLUT4
        case  DISPLAY_IN_FORMAT_CLUT1  : ///< CLUT1
            //Not supported as a FB format by Dave2D
            break;

        default:
            break;
    }
    return d2_fb_mode;
}
#endif

d2_u32 lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(lv_color_format_t colour_format)
{
    d2_u32 d2_lvgl_mode = 0;

    switch(colour_format) {
        case(LV_COLOR_FORMAT_A8):
            d2_lvgl_mode = d2_mode_alpha8; //?
            break;
        case(LV_COLOR_FORMAT_RGB565):
            d2_lvgl_mode = d2_mode_rgb565;
            break;
        case(LV_COLOR_FORMAT_RGB888):
            d2_lvgl_mode = d2_mode_argb8888; //?
            break;
        case(LV_COLOR_FORMAT_ARGB8888):
        case(LV_COLOR_FORMAT_XRGB8888):
            d2_lvgl_mode = d2_mode_argb8888;
            break;

        default:
            __BKPT(0);
            break;

    }
    return d2_lvgl_mode;
}

void d2_framebuffer_from_layer(d2_device * handle, lv_layer_t * layer)
{
    lv_draw_buf_t * draw_buf = layer->draw_buf;
    lv_area_t buffer_area = layer->buf_area;
    lv_area_move(&buffer_area, -layer->buf_area.x1, -layer->buf_area.y1);

    d2_framebuffer(handle, draw_buf->data,
                   (d2_s32) draw_buf->header.stride / lv_color_format_get_size(layer->color_format),
                   (d2_u32)lv_area_get_width(&buffer_area),
                   (d2_u32)lv_area_get_height(&buffer_area),
                   lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(layer->color_format));
}

void d2_utils_init(d2_device * handle)
{
    _lv_ll_init(&_d2_buf_1, sizeof(void *));
    _lv_ll_init(&_d2_buf_2, sizeof(void *));
    _d2_buf_act = &_d2_buf_1;
    _d2_handle = handle;
}

void * d2_buf_alloc(uint32_t size)
{
    return d1_allocvidmem(d2_level1interface(_d2_handle), d1_mem_any, size);
}

void d2_buf_free(void * ptr)
{

    d1_freevidmem(d2_level1interface(_d2_handle), d1_mem_any, ptr);
}

void d2_buf_copy(void * ptr, uint32_t size)
{
    d1_copytovidmem(d2_level1interface(_d2_handle), ptr, ptr, size, 0);
}

void d2_buf_add(void * ptr)
{
    void ** new_node = _lv_ll_ins_tail(_d2_buf_act);
    *new_node = ptr;
}

bool d2_buf_on_rendering(void * ptr)
{
    return d2_buf_contains(&_d2_buf_1, ptr) || d2_buf_contains(&_d2_buf_2, ptr);
}

void d2_start_rendering(void)
{
    // Wait for previous rendering to finish
    d2_endframe(_d2_handle);

    // Switch and clear buf
    d2_buf_switch();
    d2_buf_clear();

    // Execute the next render buffer
    d2_startframe(_d2_handle);
}

void d2_finish_rendering(void)
{
    d2_endframe(_d2_handle);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void d2_buf_clear_cb(void * node)
{
    _lv_ll_remove(_d2_buf_act, node);
    d1_freevidmem(d2_level1interface(_d2_handle), d1_mem_any, *(void **)node);
    lv_free(node);
}

static void d2_buf_switch(void)
{
    _d2_buf_act = _d2_buf_act == &_d2_buf_1 ? &_d2_buf_2 : &_d2_buf_1;
}

static void d2_buf_clear(void)
{
    _lv_ll_clear_custom(_d2_buf_act, d2_buf_clear_cb);
}

static bool d2_buf_contains(d2_buf * buf, void * ptr)
{
    void ** next = _lv_ll_get_head(buf);

    while (next != NULL)
    {
        if (*next == ptr)
        {
            return true;
        }

        next = _lv_ll_get_next(buf, next);
    }

    return false;
}
