//
//  search_station_window.c
//  PebbleTransilien
//
//  Created by CocoaBob on 25/07/15.
//  Copyright (c) 2015 CocoaBob. All rights reserved.
//

#include <pebble.h>
#include "headers.h"

static Window *s_window;
static MenuLayer *s_menu_layer;
#ifdef PBL_PLATFORM_BASALT
static StatusBarLayer *s_status_bar;
static Layer *s_status_bar_background_layer;
static Layer *s_status_bar_overlay_layer;
#endif

#ifdef PBL_BW
static InverterLayer *s_inverter_layer;
#endif

static size_t s_list_items_count;
static size_t *s_list_items;
static bool s_is_searching;

// MARK: Constants


// MARK: Drawing

static void draw_menu_layer_cell(GContext *ctx, Layer *cell_layer,
                                 GColor text_color,
#ifdef PBL_COLOR
                                 bool is_highlighed,
#endif
                                 const char * str_station) {
    graphics_context_set_text_color(ctx, text_color);
    GRect bounds = layer_get_bounds(cell_layer);
    
    GRect frame_icon = GRect(CELL_MARGIN,
                             (CELL_HEIGHT_2 - FROM_TO_ICON_WIDTH) / 2,
                             FROM_TO_ICON_WIDTH,
                             FROM_TO_ICON_WIDTH);
#ifdef PBL_COLOR
    draw_image_in_rect(ctx, is_highlighed?RESOURCE_ID_IMG_FROM_DARK:RESOURCE_ID_IMG_FROM_LIGHT, frame_icon);
#else
    draw_image_in_rect(ctx, RESOURCE_ID_IMG_FROM_LIGHT, frame_icon);
#endif
    
    // Station
    GRect frame_station = GRect(CELL_MARGIN + FROM_TO_ICON_WIDTH + CELL_MARGIN,
                                TEXT_Y_OFFSET,
                                bounds.size.w - CELL_MARGIN * 2 - FROM_TO_ICON_WIDTH,
                                CELL_HEIGHT_2);
    draw_text(ctx, str_station, FONT_KEY_GOTHIC_18, frame_station, GTextAlignmentLeft);
}

// MARK: Menu layer callbacks

static uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
    if (s_is_searching) {
        return 1;
    } else {
        return (s_list_items_count > 0)?s_list_items_count:1;
    }
}

static int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    return CELL_HEIGHT_2;
}

static void draw_row_callback(GContext *ctx, Layer *cell_layer, MenuIndex *cell_index, void *context) {
#ifdef PBL_COLOR
    MenuIndex selected_index = menu_layer_get_selected_index(s_menu_layer);
    bool is_selected = (menu_index_compare(&selected_index, cell_index) == 0);
    bool is_dark_theme = status_is_dark_theme();
    bool is_highlighed = is_dark_theme || is_selected;
    GColor text_color = (is_selected && !is_dark_theme)?curr_bg_color():curr_fg_color();
#else
    GColor text_color = curr_fg_color();
#endif
    
    if (s_is_searching) {
        graphics_context_set_text_color(ctx, text_color);
        draw_cell_title(ctx, cell_layer, "Search...");
    } else if (s_list_items_count > 0) {
        size_t station_index = s_list_items[cell_index->row];
        printf("%u",station_index);
        
        // Station
        char *str_station = malloc(sizeof(char) * STATION_NAME_MAX_LENGTH);
        stations_get_name(station_index, str_station, STATION_NAME_MAX_LENGTH);
        
        draw_menu_layer_cell(ctx, cell_layer,
                             text_color,
#ifdef PBL_COLOR
                             is_highlighed,
#endif
                             str_station);
        
        // Clean
        free(str_station);
    } else {
        graphics_context_set_text_color(ctx, text_color);
        draw_cell_title(ctx, cell_layer, "Not found.");
    }
}

static void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
    
}

static int16_t get_separator_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    return SEPARATOR_HEIGHT;
}

static void draw_separator_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context)  {
    draw_separator(ctx, cell_layer, curr_fg_color());
}

#ifdef PBL_PLATFORM_BASALT
static void draw_background_callback(GContext* ctx, const Layer *bg_layer, bool highlight, void *callback_context) {
    GRect frame = layer_get_frame(bg_layer);
    graphics_context_set_fill_color(ctx, curr_bg_color());
    graphics_fill_rect(ctx, frame, 0, GCornerNone);
}
#endif

// MARK: Window callbacks

static void window_load(Window *window) {
    //Demo data
    s_list_items_count = 5;
    NULL_FREE(s_list_items);
    s_list_items = malloc(sizeof(size_t) * s_list_items_count);
    
    s_list_items[0] = 33;
    s_list_items[1] = 101;
    s_list_items[2] = 205;
    s_list_items[3] = 356;
    s_list_items[4] = 474;
    
    // Window
    Layer *window_layer = window_get_root_layer(window);
    GRect window_bounds = layer_get_bounds(window_layer);
    
    // Add menu layer
    int16_t status_bar_height = 0;
#ifdef PBL_PLATFORM_BASALT
    status_bar_height = STATUS_BAR_LAYER_HEIGHT;
#endif
    GRect menu_layer_frame = GRect(window_bounds.origin.x,
                                   window_bounds.origin.y + status_bar_height,
                                   window_bounds.size.w,
                                   window_bounds.size.h - status_bar_height);
    s_menu_layer = menu_layer_create(menu_layer_frame);
    layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer));
    // Setup menu layer
    menu_layer_set_click_config_onto_window(s_menu_layer, window);
    menu_layer_set_callbacks(s_menu_layer, NULL, (MenuLayerCallbacks) {
        .get_num_rows = (MenuLayerGetNumberOfRowsInSectionsCallback)get_num_rows_callback,
        .get_cell_height = (MenuLayerGetCellHeightCallback)get_cell_height_callback,
        .draw_row = (MenuLayerDrawRowCallback)draw_row_callback,
        .select_click = (MenuLayerSelectCallback)select_callback,
        .get_separator_height = (MenuLayerGetSeparatorHeightCallback)get_separator_height_callback,
        .draw_separator = (MenuLayerDrawSeparatorCallback)draw_separator_callback
#ifdef PBL_PLATFORM_BASALT
        ,
        .draw_background = (MenuLayerDrawBackgroundCallback)draw_background_callback
#endif
    });

    
    // Finally, add status bar
#ifdef PBL_PLATFORM_BASALT
    window_add_status_bar(window_layer, &s_status_bar, &s_status_bar_background_layer, &s_status_bar_overlay_layer);
#endif
    
#ifdef PBL_BW
    s_inverter_layer = inverter_layer_create(window_bounds);
#endif
    
#ifdef PBL_COLOR
    setup_ui_theme_for_menu_layer(s_menu_layer);
#else
    setup_ui_theme(s_window, s_inverter_layer);
#endif
}

static void window_unload(Window *window) {
    // Data
    NULL_FREE(s_list_items);
    
    // Window
    menu_layer_destroy(s_menu_layer);
    window_destroy(window);
    s_window = NULL;
    
#ifdef PBL_PLATFORM_BASALT
    layer_destroy(s_status_bar_background_layer);
    layer_destroy(s_status_bar_overlay_layer);
    status_bar_layer_destroy(s_status_bar);
#endif
    
#ifdef PBL_BW
    inverter_layer_destroy(s_inverter_layer);
#endif
}

static void window_appear(Window *window) {
}

static void window_disappear(Window *window) {
}

// MARK: Entry point

void push_search_train_window() {
    if(!s_window) {
        s_window = window_create();
        window_set_window_handlers(s_window, (WindowHandlers) {
            .load = window_load,
            .appear = window_appear,
            .disappear = window_disappear,
            .unload = window_unload,
        });
    }
    
    window_stack_push(s_window, true);
}