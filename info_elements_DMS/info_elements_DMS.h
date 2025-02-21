#pragma once

#ifndef INFO_ELEMENTS_DMS_H
#define INFO_ELEMENTS_DMS_H
#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <String>

/*
FLAGS de modo:
1a- Acepta color basico
2a- Acepta colores avanzados    
3a- tiene RELE A
4a- tiene RELE B
5a- acepta sensor_val 1
6a- acepta sensor_val 2
7a- esta en un lugar alto
8a- tiene modo pasivo
--
1b- el elemento responde afirmativo / negativo
2b- acepta numero de BANK / FILE
3b- acepta numero de PATTERN
4b- JUEGO GUESS COLOR
5b- juego SIMON
6b- Not impl. yet
7b- Not impl. yet
8b- Not impl. yet

*/

enum MODE_CONFIGS{
    HAS_BASIC_COLOR= 0, //b0
    HAS_PULSE,          //b1
    HAS_ADVANCED_COLOR, //b2
    HAS_RELAY_1,        //b3
    HAS_RELAY_2,        //b4
    HAS_RELAY_3,        //b5
    HAS_RELAY_4,        //b6
    HAS_SENS_VAL_1,     //b7
    
    HAS_SENS_VAL_2,     //b8
    SITUATED_HIGH,      //b9
    HAS_PASSIVE,        //b10
    CAN_ANSWER,         //b11
    HAS_BANK_FILE,      //b12
    HAS_PATTERNS,       //b13
    NOP_1,              //b14
    MODE_EXIST          //b15
};



String get_string_from_info_DB(byte fieldin, byte languajein);
uint16_t get_config_flag_mode(byte modein);
uint16_t get_info_num(int count, ...);

#endif