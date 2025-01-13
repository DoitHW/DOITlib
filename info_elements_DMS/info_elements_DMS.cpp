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
                                if     (fieldin == ELEM_NAME)           info= "Columna";
                                else if(fieldin == ELEM_DESC)           info= "Tubo de burbujas que hace cosas chulisimas";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " "; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " "; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color, mantener pulsado un color cambia su intensidad!";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "Modo basico de colores pero con cambios más rapidos!";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "Modo Muevete! ";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= "Mueve tu Doit PlayPad para ajustar la intensidad del color de la columna! Los botones de colores funcionaran en modo BASICO.";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "Modo Muevete! (Colores)";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= "Mueve tu Doit PlayPad para cambiar de colores la columna! Los botones de colores funcionaran en modo BASICO.";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "Modo Mezcla color ";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= "Pulsa dos botones a la vez para crear colores nuevos!";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "Modo pasivo";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= "Los colores cambian lentamente, pulsa cualquier boton para pausarlo";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "Modo pasivo(Relax)";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= "Los colores cambian MUY lentamente, pulsa cualquier boton para pausarlo.";
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



#ifdef FIBRAS

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


#ifdef WALLWASHER

        uint16_t get_config_flag_mode(byte modein){
                uint16_t config= 0;
                if(modein == LEDSTRIP_BASIC_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH);

                else if(modein == LEDSTRIP_SLOW_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH);

                else if(modein == LEDSTRIP_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == LEDSTRIP_RB_MOTION_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        SITUATED_HIGH,
                                                                        ACCEPTS_SENS_VAL_1);

                else if(modein == LEDSTRIP_MIX_MODE) config= get_info_num(2, 
                                                                        ACCEPTS_ADVANCED_COLOR,
                                                                        SITUATED_HIGH);

                else if(modein == LEDSTRIP_PASSIVE_MODE) config= get_info_num(2, 
                                                                        HAS_PASSIVE,
                                                                        SITUATED_HIGH);

                else if(modein == LEDSTRIP_PATTERN_MODE) config= get_info_num(3, 
                                                                        ACCEPTS_BASIC_COLOR,
                                                                        ACCEPTS_PATTERNS,
                                                                        SITUATED_HIGH);


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
                                if     (fieldin == ELEM_NAME)           info= "ILLOSTRIPS";
                                else if(fieldin == ELEM_DESC)           info= "HOLA TESTING THE BEST OF THE BESTES SWITCHS";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= ""; // modo oculto
                                else if(fieldin == ELEM_MODE_0_DESC)    info= ""; // 
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "MODO GIGACHAD";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "ERAN TRES MOTORISTAS QUE ERAN MOTOS MIENTRAS QUE CHOCABAN CON UN BANCO.";
                                else if(fieldin == ELEM_MODE_2_NAME)    info= "MODO GITANO";
                                else if(fieldin == ELEM_MODE_2_DESC)    info= "ESTE AÑO NO HAY REYES POR MALOS Y POR FUMAR ";
                                else if(fieldin == ELEM_MODE_3_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_3_DESC)    info= "ESTO NO ES UN JUEGO EH";
                                else if(fieldin == ELEM_MODE_4_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_4_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_5_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_5_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_6_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_6_DESC)    info= "";
                                else if(fieldin == ELEM_MODE_7_NAME)    info= "";
                                else if(fieldin == ELEM_MODE_7_DESC)    info= "SABES NO?";
                                else if(fieldin == ELEM_MODE_8_NAME)    info= "";
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