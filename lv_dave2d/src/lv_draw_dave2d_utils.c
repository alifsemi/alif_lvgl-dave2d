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

static bool d2_buf_contains(d2_buf * buf, void * ptr);

/**********************
 *  STATIC VARIABLES
 **********************/

static d2_device * _d2_handle;
static lv_ll_t _d2_buf_1, _d2_buf_2;

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
            LV_ASSERT(0);
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

d2_u32 lv_draw_dave2d_lv_colour_fmt_to_d2_fmt(lv_color_format_t colour_format)
{
    d2_u32 d2_lvgl_mode = 0;

    switch(colour_format) {
        case(LV_COLOR_FORMAT_I1):
            d2_lvgl_mode = d2_mode_i1;
            break;
        case(LV_COLOR_FORMAT_I2):
            d2_lvgl_mode = d2_mode_i2;
            break;
        case(LV_COLOR_FORMAT_I4):
            d2_lvgl_mode = d2_mode_i4;
            break;
        case(LV_COLOR_FORMAT_I8):
            d2_lvgl_mode = d2_mode_i8;
            break;
        case(LV_COLOR_FORMAT_A8):
            d2_lvgl_mode = d2_mode_alpha8;
            break;
        case(LV_COLOR_FORMAT_RGB565):
            d2_lvgl_mode = d2_mode_rgb565;
            break;
        case(LV_COLOR_FORMAT_ARGB1555):
            d2_lvgl_mode = d2_mode_argb1555;
            break;
        case(LV_COLOR_FORMAT_ARGB4444):
            d2_lvgl_mode = d2_mode_argb4444;
            break;
        case(LV_COLOR_FORMAT_ARGB8888):
            d2_lvgl_mode = d2_mode_argb8888;
            break;
        case(LV_COLOR_FORMAT_XRGB8888):
            d2_lvgl_mode = d2_mode_argb8888;
            break;

        case(LV_COLOR_FORMAT_RGB888): //LV_COLOR_FORMAT_RGB888 is a 3 byte format, d2_mode_rgb888 is a 4 byte format, not supported
        default:
            LV_ASSERT(0);
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

bool lv_draw_dave2d_is_dest_cf_supported(lv_color_format_t cf)
{
    bool result;

    switch(cf) {
        case LV_COLOR_FORMAT_A8:
        case LV_COLOR_FORMAT_RGB565:
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB4444:
            result = true;
            break;

        default:
            result = false;
            break;
    }

    return result;
}

void d2_utils_init(d2_device * handle)
{
    lv_ll_init(&_d2_buf_1, sizeof(void *));
    lv_ll_init(&_d2_buf_2, sizeof(void *));
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

bool d2_buf_on_rendering(void * ptr)
{
    return d2_buf_contains(&_d2_buf_1, ptr) || d2_buf_contains(&_d2_buf_2, ptr);
}

void d2_finish_rendering(void)
{
    d2_endframe(_d2_handle);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool d2_buf_contains(d2_buf * buf, void * ptr)
{
    void ** next = lv_ll_get_head(buf);

    while (next != NULL)
    {
        if (*next == ptr)
        {
            return true;
        }

        next = lv_ll_get_next(buf, next);
    }

    return false;
}
