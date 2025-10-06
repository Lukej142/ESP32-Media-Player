/**
 * @file home_gen.c
 * @description Template source file for LVGL objects
 */

/*********************
 *      INCLUDES
 *********************/
#include "home_gen.h"
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

lv_obj_t * home_create(void)
{
    LV_TRACE_OBJ_CREATE("begin");

    static lv_style_t cell;

    static bool style_inited = false;

    if (!style_inited) {
        lv_style_init(&cell);
        lv_style_set_border_side(&cell, LV_BORDER_SIDE_FULL);

        style_inited = true;
    }

    lv_obj_t * lv_obj_0 = lv_obj_create(NULL);

    lv_obj_t * lv_table_0 = lv_table_create(lv_obj_0);
    lv_table_set_column_count(lv_table_0, 2);
    lv_table_set_row_count(lv_table_0, 2);
    lv_obj_set_style_border_side(lv_table_0, LV_BORDER_SIDE_FULL, 0);
    lv_obj_add_style(lv_table_0, &cell, 0);
    lv_table_set_cell_value(lv_table_0, 0, 0, "0");



    LV_TRACE_OBJ_CREATE("finished");

    lv_obj_set_name(lv_obj_0, "home");

    return lv_obj_0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/