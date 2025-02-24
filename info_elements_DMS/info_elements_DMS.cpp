#include <defines_DMS/defines_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Element_DMS/Element_DMS.h>
// intentar millorar control de memoria per que el elements no carreguin data inutil, y recorda comprar cafe. Sergi truchilla !!!

uint16_t get_info_num(int count, ...) {
    uint16_t result = 0;                        // si esta no funciona es culpa del Gepetto....
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int bit_position = va_arg(args, int);
        result |= (1 << bit_position);
    }
    va_end(args);
    return result;
}
    

#ifdef COLUMNA

        uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if     (modein == COLUMN_BASIC_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_MOTION_LIGHT_MODE) config= get_info_num(4, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        HAS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_MOTION_COLOR_MODE) config= get_info_num(4, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        HAS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(3, 
                                                                        HAS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_PULSE_MODE) config= get_info_num(4, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_PULSE,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(3, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);
                
                else if(modein == COLUMN_SLOW_PASSIVE_MODE) config= get_info_num(3, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1,
                                                                        MODE_EXIST);

                 else if(modein == COLUMN_VOICE_LIGHT_MODE) config= get_info_num(4, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_VOICE_REVERSE_LIGHT_MODE)  config= get_info_num(4, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_VOICE_COLOR_MODE)  config= get_info_num(5, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        HAS_BINARY_SENSORS,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_VOICE_BUBBLES_MODE) config= get_info_num(3, 
                                                                        HAS_SENS_VAL_2,
                                                                        HAS_BINARY_SENSORS,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_VOICE_REVERSE_BUBBLES_MODE) config= get_info_num(3, 
                                                                        HAS_SENS_VAL_2,
                                                                        HAS_BINARY_SENSORS,
                                                                        MODE_EXIST);

                else if(modein == COLUMN_PATTERN_MODE) config= get_info_num(2, 
                                                                        HAS_PATTERNS,
                                                                        MODE_EXIST);                                                                                                  

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");      
                                                        #endif
        // uint16_t res= 0;
        // int tam= sizeof(config) * 8;
        // for (int i = 0; i < tam; i++) {
        //         if (config & (1 << i)) {
        //             res |= (1 << (tam - 1 - i));
        //         }
        //     }                                        
        return config;
        }

        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "COLUMNA BETA";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "BASICO";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "BASICO LENTO";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "MUEVETE BRILLO";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "MUEVETE COLOR";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "MIX DE COLORES";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "PULSO";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "AUTOMATICO";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= "AUTO. LENTO";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= "VOZ BRILLO";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= "VOZ NO BRILLO";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= "VOZ COLOR";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= "VOZ BURBUJAS";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= "VOZ NO BURBUJAS";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= "EFECTOS";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= "---";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
        return info;
        }
#endif



#ifdef FIBRAS

        uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if     (modein == LIGHTSOURCE_BASIC_MODE) config= get_info_num(2, 
                                                                        HAS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_SLOW_MODE) config= get_info_num(2, 
                                                                        HAS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_MOTION_LIGHT_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_MOTION_COLOR_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_MIX_MODE) config= get_info_num(2, 
                                                                        HAS_ADVANCED_COLOR,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_PULSE_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_PULSE,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        MODE_EXIST);
                
                else if(modein == LIGHTSOURCE_SLOW_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_VOICE_LIGHT_MODE) config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_VOICE_REVERSE_LIGHT_MODE)  config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == LIGHTSOURCE_VOICE_COLOR_MODE)  config= get_info_num(3, 
                                                                        HAS_BASIC_COLOR,
                                                                        HAS_SENS_VAL_2,
                                                                        MODE_EXIST);


                else if(modein == LIGHTSOURCE_PATTERN_MODE) config= get_info_num(2, 
                                                                        HAS_PATTERNS,
                                                                        MODE_EXIST);                                                                                                  

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");      
                                                        #endif
        return config;
        }

        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                        
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS DE LUZ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "BASICO";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "BASICO LENTO";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "MUEVETE BRILLO";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "MUEVETE COLOR";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "MIX DE COLORES";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "PULSO";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "AUTOMATICO";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= "AUTO. LENTO";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= "VOZ BRILLO";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= "VOZ SOMBRA";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= "VOZ COLOR";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= "JUEGO SIMON";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= "JUEGO MUSICA";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= "";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif


#ifdef WALLWASHER

        uint16_t get_config_flag_mode(byte modein){
                uint16_t config= 0;

                if(modein == LEDSTRIP_HIDDEN_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH);

                else if(modein == LEDSTRIP_BASIC_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_SLOW_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_MOTION_MODE) config= get_info_num(4, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        ACCEPTS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_RB_MOTION_MODE) config= get_info_num(4, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        ACCEPTS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_MIX_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        SITUATED_HIGH,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_PASSIVE_MODE) config= get_info_num(3, 
                                                                        HAS_PASSIVE,
                                                                        SITUATED_HIGH,
                                                                        MODE_EXIST);

                else if(modein == LEDSTRIP_PATTERN_MODE) config= get_info_num(4, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_PATTERNS,
                                                                        SITUATED_HIGH,
                                                                        MODE_EXIST);
                else if(modein == LEDSTRIP_MIC_MODE) config= get_info_num(4, 
                                                                                ACCEPTS_BASIC_COLOR,
                                                                                ACCEPTS_SENS_VAL_2,
                                                                                SITUATED_HIGH,
                                                                                MODE_EXIST);

                                                                        


                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.println(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.println(" para el elemento Wallwasher");     
                                                        #endif
                return config;
                }

        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "BARRA LED";
                                else if(fieldin == ELEM_DESC)           info= "HOLA TESTING THE BEST OF THE BESTES SWITCHS";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "S. PEPE"; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "SOY DON CHIFLEIN...."; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "BASICO";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "ERAN TRES MOTORISTAS QUE ERAN MOTOS MIENTRAS QUE CHOCABAN CON UN BANCO.";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "LENTO";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "ESTE AÑO NO HAY REYES POR MALOS Y POR FUMAR ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "MOVIMIENTO";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= "ESTO NO ES UN JUEGO EH";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "MOVIMIENTO (color)";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "COLOR MIX";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "AUTOMATICO";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "EFECTOS";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= "SABES NO?";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= "VOZ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= "HEMOS VUELTO HIJOS DE LA GRAN PUTAAAAAAAA";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= "PERO LA QUERIA TANTO";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= "";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= "CALLATE LA BOCA TONTITOOO";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= "";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= "";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "Doit LED Strip";
                                else if(fieldin == ELEM_DESC)           info= "Lights your room and create an immersive Fx!";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Basic";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Changes the color.";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "Slow";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "Changes the color slowly";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef LLUMNEGRA

        uint16_t get_config_flag_mode(byte modein){
                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");       
                                                        #endif
                return config;
                }

        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " 69 guapa";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef VENTILADOR

        uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                               
                                                        #endif
                return config;
                }

        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef ESCALERA

        uint16_t get_config_flag_mode(byte modein){
                uint16_t config= 0;
                if(modein == VUMETER_HIDDEN_MODE) config= get_info_num(1, 
                                                                        ACCEPTS_BASIC_COLOR);

                else if(modein == VUMETER_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_SENS_VAL_1,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_PATTERN_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_MODE_8) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_MODE_9) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_SIMON_GAME_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_SECUENCER_GAME_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_SPEAK_GAME_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_SENS_VAL_2,
                                                                        MODE_EXIST);

                else if(modein == VUMETER_BLOCK_SPEAK_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_SENS_VAL_2,
                                                                        MODE_EXIST);
                                                                        

                else if(modein == VUMETER_TONE_DETECT_MODE) config= get_info_num(1, 
                                                                        MODE_EXIST);

                else if(modein == VUMETER_METEOR_VOICE_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_SENS_VAL_2,
                                                                        MODE_EXIST
                                                                        );
                                                                                            

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                                
                                                        #endif
                return config;
                }
        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "SUPERESCALA";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Oculto"; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Basic";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "Slow";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "Motion";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "Color Motion";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "Mix";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "Pasive";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "Secuencias";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= "Modo 8";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= "Este es un modo que no hace nada... todabia, pero hara cosas superguays en un futuro muy lejano";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= "ESTE ES EL JUEGO DEL SIMON EL COLOMBIANO LOCO";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= "JUEGO MUSICA";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= "JUEGO HABLA";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= "HABLA BLOQUES";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= "DETECTA TONO";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= "METEORITOS";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef TUNEL
uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                              
                                                        #endif
                return config;
                }
        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef TORMENTA
uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                             
                                                        #endif
                return config;
                }
        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef PISCINA
uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                               
                                                        #endif
                return config;
                }
        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
                return info;
        }
#endif

#ifdef AROMATERAPIA
uint16_t get_config_flag_mode(byte modein){

                uint16_t config= 0;
                if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        HAS_RELAY_1,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == COLUMN_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        HAS_RELAY_1);

                else if(modein == COLUMN_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        HAS_RELAY_1);

                                                        #ifdef DEBUG
                                                                Serial.print("Se devuelve un 16bit config -> " );
                                                                Serial.print(config, BIN);
                                                                Serial.print(" para el modo ");
                                                                Serial.print(modein);
                                                                Serial.print(" para el elemento ");
                                                             
                                                        #endif
                return config;
                }
        String get_string_from_info_DB(byte fieldin, byte languajein){
                
                String info;
                switch(languajein){

                        case SPANISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= "FIBRAS";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case ENGLISH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case FRENCH_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GERMAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case CATALAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case MEXICAN_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case EUSKERA_LANG:
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_8_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_9_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_9_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_10_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_10_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_11_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_11_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_12_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_12_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_13_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_13_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_14_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_14_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_NAME)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                
                }
        return info;
        }
#endif