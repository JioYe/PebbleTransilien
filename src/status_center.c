//
//  status_center.c
//  PebbleTransilien
//
//  Created by CocoaBob on 10/07/15.
//  Copyright (c) 2015 CocoaBob. All rights reserved.
//

#include <pebble.h>
#include "headers.h"

static bool s_is_dark_theme;
static char* s_curr_locale;

void load_status() {
    unload_status();
    
    // Get current theme
    s_is_dark_theme = get_theme_setting();
    
    // Get current locale
    s_curr_locale = malloc(sizeof(char) * 5);
    if (!get_locale_setting(s_curr_locale)) {
        s_curr_locale = NULL;
    }
    setlocale(LC_ALL, s_curr_locale);
}

void unload_status() {
    if (s_curr_locale != NULL) {
        free(s_curr_locale);
        s_curr_locale = NULL;
    }
}


bool status_is_dark_theme() {
    return s_is_dark_theme;
}

char *status_curr_locale() {
    return s_curr_locale;
}

GColor curr_fg_color() {
    return s_is_dark_theme?GColorWhite:GColorBlack;
}

GColor curr_bg_color() {
    return s_is_dark_theme?GColorBlack:GColorWhite;
}