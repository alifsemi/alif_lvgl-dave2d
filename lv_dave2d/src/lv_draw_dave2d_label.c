#include "lv_draw_dave2d.h"

static void lv_draw_dave2d_draw_letter_cb(lv_draw_unit_t * draw_unit, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                          lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area);

static void lv_label_render(d2_device * handle, const lv_color_t * color, lv_opa_t opa,
                            const lv_area_t * coords, lv_draw_buf_t * draw_buf);

static lv_draw_dave2d_unit_t * unit = NULL;

void lv_draw_dave2d_label(lv_draw_dave2d_unit_t * u, const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->opa <= LV_OPA_MIN) return;

    unit = u;

    lv_area_t label_coords = *coords;

#if (D2_LABEL_RENDER_EACH_LETTER == 0)
    lv_area_t clipped_area;
    bool clips = _lv_area_intersect(&clipped_area, coords, u->base_unit.clip_area);
    if (!clips) return;

    int32_t x = 0 - u->base_unit.target_layer->buf_area.x1;
    int32_t y = 0 - u->base_unit.target_layer->buf_area.y1;

    lv_area_move(&clipped_area, x, y);
    lv_area_move(&label_coords, x, y);

    const lv_area_t * current_layer_clip_area = u->base_unit.clip_area;

    lv_area_t act_area;
    act_area.x1 = clipped_area.x1;
    act_area.x2 = clipped_area.x2;
    act_area.y1 = clipped_area.y1;
    act_area.y2 = clipped_area.y1;

    u->base_unit.clip_area = &act_area;

    int32_t w = lv_area_get_width(&clipped_area);
    uint32_t max_height = D2_LABEL_BUF_SIZE / w / lv_color_format_get_size(LV_COLOR_FORMAT_A8);

    while (act_area.y2 < clipped_area.y2)
    {
        act_area.y2 = LV_MIN(act_area.y1 + max_height - 1, clipped_area.y2);

#if D2_RENDER_EACH_OPERATION
#if (D2_USE_INTERNAL_RENDERBUFFERS == 0)
    d2_selectrenderbuffer(unit->d2_handle, unit->renderbuffer);
#endif
#endif

    u->label_drawbuffer = lv_draw_buf_create(lv_area_get_width(&act_area), lv_area_get_height(&act_area),
                                             LV_COLOR_FORMAT_A8, LV_STRIDE_AUTO);
    memset(u->label_drawbuffer->data, 0, u->label_drawbuffer->data_size);
#if D2_USE_INTERNAL_RENDERBUFFERS
    d2_buf_add(u->label_drawbuffer->data);
#endif

    u->label_coords = lv_malloc(sizeof(lv_area_t));
    lv_area_copy(u->label_coords, &act_area);

    d2_framebuffer_from_layer(unit->d2_handle, unit->base_unit.target_layer);

    d2_u8 current_fillmode = d2_getfillmode(unit->d2_handle);

    d2_cliprect(unit->d2_handle, (d2_border)act_area.x1, (d2_border)act_area.y1, (d2_border)act_area.x2,
                (d2_border)act_area.y2);
#endif

    lv_draw_label_iterate_characters(&u->base_unit, dsc, &label_coords, lv_draw_dave2d_draw_letter_cb);

#if (D2_LABEL_RENDER_EACH_LETTER == 0)
    lv_label_render(unit->d2_handle, &dsc->color, dsc->opa, &act_area, u->label_drawbuffer);

    d2_setfillmode(unit->d2_handle, current_fillmode);

#if D2_RENDER_EACH_OPERATION
#if D2_USE_INTERNAL_RENDERBUFFERS
    d2_start_rendering();
#else
    d2_executerenderbuffer(unit->d2_handle, unit->renderbuffer, 0);
    d2_flushframe(unit->d2_handle);
#endif

    if (unit->label_drawbuffer)
    {
        lv_draw_buf_destroy(unit->label_drawbuffer);
        unit->label_drawbuffer = NULL;
    }

    if (unit->label_coords)
    {
        lv_free(unit->label_coords);
        unit->label_coords = NULL;
    }

#endif

        act_area.y1 = act_area.y2 + 1;
    }

    u->base_unit.clip_area = current_layer_clip_area;
#endif
}

static void lv_draw_dave2d_draw_letter_cb(lv_draw_unit_t * u, lv_draw_glyph_dsc_t * glyph_draw_dsc,
                                          lv_draw_fill_dsc_t * fill_draw_dsc, const lv_area_t * fill_area)
{
    d2_u8 current_fillmode;
    lv_area_t clip_area;
    lv_area_t letter_coords;

    int32_t x;
    int32_t y;

    letter_coords = *glyph_draw_dsc->letter_coords;

    bool is_common;
#if D2_LABEL_RENDER_EACH_LETTER
    is_common = _lv_area_intersect(&clip_area, glyph_draw_dsc->letter_coords, u->clip_area);
#else
    is_common = _lv_area_intersect(&clip_area, glyph_draw_dsc->letter_coords, unit->label_coords);
#endif
    if(!is_common) return;

#if D2_LABEL_RENDER_EACH_LETTER
    x = 0 - unit->base_unit.target_layer->buf_area.x1;
    y = 0 - unit->base_unit.target_layer->buf_area.y1;
#else
    x = 0 - unit->label_coords->x1;
    y = 0 - unit->label_coords->y1;
#endif

    lv_area_move(&clip_area, x, y);
    lv_area_move(&letter_coords, x, y);

#if LV_USE_OS
    lv_result_t  status;
    status = lv_mutex_lock(unit->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }
#endif

#if D2_LABEL_RENDER_EACH_LETTER
#if D2_RENDER_EACH_OPERATION
#if (D2_USE_INTERNAL_RENDERBUFFERS == 0)
    d2_selectrenderbuffer(unit->d2_handle, unit->renderbuffer);
#endif
#endif

    //
    // Generate render operations
    //

    d2_framebuffer_from_layer(unit->d2_handle, unit->base_unit.target_layer);

    current_fillmode = d2_getfillmode(unit->d2_handle);

    d2_cliprect(unit->d2_handle, (d2_border)clip_area.x1, (d2_border)clip_area.y1, (d2_border)clip_area.x2,
                (d2_border)clip_area.y2);
#endif

    if(glyph_draw_dsc) {
        switch(glyph_draw_dsc->format) {
            case LV_FONT_GLYPH_FORMAT_NONE: {
#if LV_USE_FONT_PLACEHOLDER
                    /* Draw a placeholder rectangle*/
                    lv_draw_border_dsc_t border_draw_dsc;
                    lv_draw_border_dsc_init(&border_draw_dsc);
                    border_draw_dsc.opa = glyph_draw_dsc->opa;
                    border_draw_dsc.color = glyph_draw_dsc->color;
                    border_draw_dsc.width = 1;
                    //lv_draw_sw_border(u, &border_draw_dsc, glyph_draw_dsc->bg_coords);
                    lv_draw_dave2d_border(unit, &border_draw_dsc, glyph_draw_dsc->bg_coords);
#endif
                }
                break;
            case LV_FONT_GLYPH_FORMAT_A1 ... LV_FONT_GLYPH_FORMAT_A8:
#if D2_LABEL_RENDER_EACH_LETTER
                {
                    lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;

                    lv_label_render(unit->d2_handle, &glyph_draw_dsc->color, glyph_draw_dsc->opa, &letter_coords, draw_buf);

                    d2_setfillmode(unit->d2_handle, current_fillmode);
                }
#else
                {
                    lv_draw_buf_t * draw_buf = glyph_draw_dsc->glyph_data;

                    lv_area_t letter_area;
                    letter_area.x1 = clip_area.x1 - letter_coords.x1;
                    letter_area.x2 = letter_area.x1 + clip_area.x2 - clip_area.x1;
                    letter_area.y1 = clip_area.y1 - letter_coords.y1;
                    letter_area.y2 = letter_area.y1 + clip_area.y2 - clip_area.y1;

                    lv_draw_buf_copy(unit->label_drawbuffer, &clip_area, draw_buf, &letter_area);
                }
#endif
                break;
            case LV_FONT_GLYPH_FORMAT_IMAGE: {
#if LV_USE_IMGFONT
                    lv_draw_image_dsc_t img_dsc;
                    lv_draw_image_dsc_init(&img_dsc);
                    img_dsc.rotation = 0;
                    img_dsc.scale_x = LV_SCALE_NONE;
                    img_dsc.scale_y = LV_SCALE_NONE;
                    img_dsc.opa = glyph_draw_dsc->opa;
                    img_dsc.src = glyph_draw_dsc->glyph_data;
                    //lv_draw_sw_image(draw_unit, &img_dsc, glyph_draw_dsc->letter_coords);
#endif
                }
                break;
            default:
                break;
        }
    }

    //
    // Execute render operations
    //
#if D2_LABEL_RENDER_EACH_LETTER
#if D2_RENDER_EACH_OPERATION
#if D2_USE_INTERNAL_RENDERBUFFERS
    d2_start_rendering();
#else
    d2_executerenderbuffer(unit->d2_handle, unit->renderbuffer, 0);
    d2_flushframe(unit->d2_handle);
#endif
#endif
#endif

    if(fill_draw_dsc && fill_area) {
        //lv_draw_sw_fill(u, fill_draw_dsc, fill_area);
        lv_draw_dave2d_fill(unit, fill_draw_dsc, fill_area);
    }

#if LV_USE_OS
    status = lv_mutex_unlock(unit->pd2Mutex);
    if(LV_RESULT_OK != status) {
        __BKPT(0);
    }
#endif
}

static void lv_label_render(d2_device * handle, const lv_color_t * color, lv_opa_t opa,
                            const lv_area_t * coords, lv_draw_buf_t * draw_buf)
{
    d1_cacheblockflush(handle, 0, draw_buf->data, draw_buf->data_size);
    d2_setalphablendmode(unit->d2_handle, d2_bm_one, d2_bm_one_minus_alpha);
    d2_settexture(handle, (void *)draw_buf->data,
                    (d2_s32)lv_draw_buf_width_to_stride((uint32_t)lv_area_get_width(coords), LV_COLOR_FORMAT_A8),
                    lv_area_get_width(coords),  lv_area_get_height(coords), d2_mode_alpha8);
    d2_settexopparam(unit->d2_handle, d2_cc_red, color->red, 0);
    d2_settexopparam(unit->d2_handle, d2_cc_green, color->green, 0);
    d2_settexopparam(unit->d2_handle, d2_cc_blue, color->blue, 0);
    d2_settexopparam(unit->d2_handle, d2_cc_alpha, opa, 0);

    d2_settextureoperation(unit->d2_handle, d2_to_multiply, d2_to_replace, d2_to_replace, d2_to_replace);

    d2_settexturemapping(unit->d2_handle, D2_FIX4(coords->x1), D2_FIX4(coords->y1), D2_FIX16(0), D2_FIX16(0),
                            D2_FIX16(1), D2_FIX16(0), D2_FIX16(0), D2_FIX16(1));

    d2_settexturemode(unit->d2_handle, d2_tm_filter);

    d2_setfillmode(unit->d2_handle, d2_fm_texture);

    d2_renderbox(unit->d2_handle, (d2_point)D2_FIX4(coords->x1),
                    (d2_point)D2_FIX4(coords->y1),
                    (d2_point)D2_FIX4(lv_area_get_width(coords)),
                    (d2_point)D2_FIX4(lv_area_get_height(coords)));
}
