//
//  data_model.h
//  PebbleTransilien
//
//  Created by CocoaBob on 21/07/15.
//  Copyright (c) 2015 CocoaBob. All rights reserved.
//

#pragma once

// MARK: DataModel

typedef struct DataModelFromTo {
    size_t from;
    size_t to;
} DataModelFromTo;

typedef DataModelFromTo Favorite;

#define TRAIN_NUMBER_LENGTH 6

typedef struct DataModelNextTrain {
    char number[TRAIN_NUMBER_LENGTH];   // Train number, e.g. "133871"
    char code[5];                       // Train mission code, null-terminated string, e.g. "POBI"
    time_t hour;                        // Train hour, 8 Bytes unsigned integer, seconds since January 1st 1970, e.g. "1437738167"
    char platform[3];                   // Train dock or Train lane, null-terminated string, e.g. "C"
    size_t terminus;                    // Train terminus station index, 2 Bytes unsigned integer, e.g. 354 (PSL: Paris Saint-Lazare)
} DataModelNextTrain;

typedef struct DataModelTrainDetail {
    time_t time;                        // time, the time of arriving, 8 Bytes unsigned integer, seconds since January 1st 1970, e.g. "1437738167"
    size_t station;                     // codeGare, Station index, e.g. 354 (PSL: Paris Saint-Lazare)
} DataModelTrainDetail;