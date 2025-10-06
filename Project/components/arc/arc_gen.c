/**
 * @file arc_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "arc_gen.h"
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

lv_obj_t * arc_create(lv_obj_t * parent, const char * title, lv_subject_t * variable)
{
    LV_TRACE_OBJ_CREATE("begin");


    lv_obj_t * lv_arc_0 = lv_arc_create(parent);
    lv_arc_bind_value(lv_arc_0, variable);
    lv_obj_set_width(lv_arc_0, 100);
    lv_obj_set_height(lv_arc_0, 100);
    lv_obj_add_event_cb(lv_arc_0, arc_changed, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t * lv_label_0 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_0, LV_ALIGN_CENTER);
    lv_label_set_text(lv_label_0, lv_tr(title));
    lv_obj_set_style_pad_bottom(lv_label_0, 10, 0);


    lv_obj_t * lv_label_1 = lv_label_create(lv_arc_0);
    lv_obj_set_align(lv_label_1, LV_ALIGN_CENTER);
    lv_label_bind_text(lv_label_1, variable, NULL);lv_obj_set_style_pad_top(lv_label_1, 20, 0);



    LV_TRACE_OBJ_CREATE("finished");

    lv_obj_set_name(lv_arc_0, "arc_#");

    return lv_arc_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/