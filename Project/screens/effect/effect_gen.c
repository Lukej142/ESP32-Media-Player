/**
 * @file effect_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "effect_gen.h"
#include "ui.h"

/*********************
 *      DEFINES
 *********************/



/**********************
 *      TYPEDEFS
 **********************/

/***********************
 *  STATIC VARIABLES
 **********************/

/***********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * effect_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");


    static bool style_inited = false;

    if (!style_inited) {

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);
    lv_obj_set_style_pad_hor(lv_obj_0, 5, 0);
    lv_obj_set_style_pad_top(lv_obj_0, 5, 0);
    lv_obj_set_flag(lv_obj_0, LV_OBJ_FLAG_SCROLLABLE, false);

    lv_obj_t * title_0 = title_create(lv_obj_0, "Gain");
    lv_obj_set_align(title_0, LV_ALIGN_TOP_LEFT);


    lv_obj_t * column_0 = column_create(lv_obj_0);
    lv_obj_set_height(column_0, lv_pct(100));
    lv_obj_set_width(column_0, lv_pct(100));
    lv_obj_set_y(column_0, 25);

    lv_obj_t * lv_checkbox_0 = lv_checkbox_create(column_0);
    lv_checkbox_set_text(lv_checkbox_0, "Check");
    lv_obj_bind_checked(lv_checkbox_0, &check);


    lv_obj_t * lv_slider_0 = lv_slider_create(column_0);
    lv_slider_bind_value(lv_slider_0, &arc1);

    lv_obj_t * lv_label_0 = lv_label_create(lv_slider_0);
    lv_label_set_text(lv_label_0, "asd");




    lv_obj_t * lv_button_0 = lv_button_create(lv_obj_0);
    lv_obj_set_style_align(lv_button_0, LV_ALIGN_BOTTOM_MID, 0);
    lv_obj_set_style_text_font(lv_button_0, font_subtitle, 0);
    lv_obj_set_align(lv_button_0, LV_ALIGN_TOP_RIGHT);

    lv_obj_t * lv_label_1 = lv_label_create(lv_button_0);
    lv_label_set_text(lv_label_1, lv_tr("back"));

    lv_obj_add_screen_load_event(lv_button_0, LV_EVENT_CLICKED, home, LV_SCREEN_LOAD_ANIM_NONE, 0, 0);


    lv_obj_t * arc_0 = arc_create(lv_obj_0, "GAIN", &arc1);
    lv_obj_set_align(arc_0, LV_ALIGN_BOTTOM_LEFT);


    lv_obj_t * arc_1 = arc_create(lv_obj_0, "GAIN", &arc2);
    lv_obj_set_align(arc_1, LV_ALIGN_BOTTOM_RIGHT);



    LV_TRACE_OBJ_CREATE("finished");

    lv_obj_set_name(lv_obj_0, "effect");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/