//
//  status_center.h
//  PebbleTransilien
//
//  Created by CocoaBob on 10/07/15.
//  Copyright (c) 2015 CocoaBob. All rights reserved.
//

#pragma once

void load_status();
void unload_status();

bool status_is_dark_theme();
char *status_curr_locale();

GColor curr_fg_color();
GColor curr_bg_color();