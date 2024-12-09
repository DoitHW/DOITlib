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

uint16_t get_config_flag_mode(byte typein, byte modein){

        uint16_t config= 0;
        switch(typein){

                case TYPE_COLUMN:
                        if(modein == COLUMN_BASIC_MODE) config= get_info_num(2, 
                                                                                ACCEPTS_BASIC_COLOR,
                                                                                HAS_RELAY_1);

                   else if(modein == COLUMN_FAST_MODE) config= get_info_num(2, 
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
/*
---------------------------------CONTINUARA--------------------------------------------------------
*/

                        break;

                default: break;
        }
                                                                #ifdef DEBUG
                                                                  Serial.print("Se devuelve un 16bit config -> " );
                                                                  Serial.print(config, BIN);
                                                                  Serial.print(" para el modo ");
                                                                  Serial.print(modein);
                                                                  Serial.print(" para el elemento ");
                                                                  Serial.println(typein);
                                                                #endif
        return config;
}

String get_string_from_info_DB(byte typein, byte fieldin, byte languajein){

    String info;

    switch(languajein){

        case SPANISH_LANG:
                switch(typein){

                        case TYPE_COLUMN: 
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


                        case TYPE_LIGHTSOURCE: 
                                if     (fieldin == ELEM_NAME)           info= "Fibras";
                                else if(fieldin == ELEM_DESC)           info= "Fuente de luz para mazos de fibra optica, trabaja el tacto y la vista.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LEDSTRIP: 
                                if     (fieldin == ELEM_NAME)           info= "Tira LED";
                                else if(fieldin == ELEM_DESC)           info= "Tiras Led de colores que crean un ambiente de color en la sala, tienen efectos y juegos variados.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_ESCALERA: 
                                if     (fieldin == ELEM_NAME)           info= "Escalera de color";
                                else if(fieldin == ELEM_DESC)           info= "Modulo de efectos tipo escalera con diferentes juegos \"causa-efecto\"";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                                
                        case TYPE_BLACKLIGHT: 
                                if     (fieldin == ELEM_NAME)           info= "Luz negra";
                                else if(fieldin == ELEM_DESC)           info= "Luz UV para trabajar con elementos reactivos a la luz ultravioleta.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Encendido y apagado";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Aleatorio";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Se enciende y se apaga de modo aleatorio.";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_FAN: 
                                if     (fieldin == ELEM_NAME)           info= "Ventilador";
                                else if(fieldin == ELEM_DESC)           info= "Ventilador que crea un ambiente frio y rafagas de viento";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GENERIC_TYPE: 
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                        default: break;
                }
        case ENGLISH_LANG:
                switch(typein){

                        case TYPE_COLUMN: 
                                if     (fieldin == ELEM_NAME)           info= "Columna";
                                else if(fieldin == ELEM_DESC)           info= "Tubo de burbujas que hace cosas chulisimas";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LIGHTSOURCE: 
                                if     (fieldin == ELEM_NAME)           info= "Fibras";
                                else if(fieldin == ELEM_DESC)           info= "Fuente de luz para mazos de fibra optica";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LEDSTRIP: 
                                if     (fieldin == ELEM_NAME)           info= "Tira LED";
                                else if(fieldin == ELEM_DESC)           info= "Tiras Led de colores que crean un ambiente de color en la sala, tienen efectos y juegos variados.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_ESCALERA: 
                                if     (fieldin == ELEM_NAME)           info= "Escalera de color";
                                else if(fieldin == ELEM_DESC)           info= "Modulo de efectos tipo escalera con diferentes juegos \"causa-efecto\"";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                                
                        case TYPE_BLACKLIGHT: 
                                if     (fieldin == ELEM_NAME)           info= "Luz negra";
                                else if(fieldin == ELEM_DESC)           info= "Luz UV para trabajar con elementos reactivos a la luz ultravioleta.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Encendido y apagado";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Aleatorio";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Se enciende y se apaga de modo aleatorio.";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_FAN: 
                                if     (fieldin == ELEM_NAME)           info= "Ventilador";
                                else if(fieldin == ELEM_DESC)           info= "Ventilador que crea un ambiente frio y rafagas de viento";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GENERIC_TYPE: 
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                        default: break;
                }

        case GERMAN_LANG:
                switch(typein){

                        case TYPE_COLUMN: 
                                if     (fieldin == ELEM_NAME)           info= "Columna";
                                else if(fieldin == ELEM_DESC)           info= "Tubo de burbujas que hace cosas chulisimas";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LIGHTSOURCE: 
                                if     (fieldin == ELEM_NAME)           info= "Fibras";
                                else if(fieldin == ELEM_DESC)           info= "Fuente de luz para mazos de fibra optica";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LEDSTRIP: 
                                if     (fieldin == ELEM_NAME)           info= "Tira LED";
                                else if(fieldin == ELEM_DESC)           info= "Tiras Led de colores que crean un ambiente de color en la sala, tienen efectos y juegos variados.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_ESCALERA: 
                                if     (fieldin == ELEM_NAME)           info= "Escalera de color";
                                else if(fieldin == ELEM_DESC)           info= "Modulo de efectos tipo escalera con diferentes juegos \"causa-efecto\"";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                                
                        case TYPE_BLACKLIGHT: 
                                if     (fieldin == ELEM_NAME)           info= "Luz negra";
                                else if(fieldin == ELEM_DESC)           info= "Luz UV para trabajar con elementos reactivos a la luz ultravioleta.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Encendido y apagado";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Aleatorio";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Se enciende y se apaga de modo aleatorio.";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_FAN: 
                                if     (fieldin == ELEM_NAME)           info= "Ventilador";
                                else if(fieldin == ELEM_DESC)           info= "Ventilador que crea un ambiente frio y rafagas de viento";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GENERIC_TYPE: 
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                        default: break;
                }
        case FRENCH_LANG:
                switch(typein){

                        case TYPE_COLUMN: 
                                if     (fieldin == ELEM_NAME)           info= "Columna";
                                else if(fieldin == ELEM_DESC)           info= "Tubo de burbujas que hace cosas chulisimas";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LIGHTSOURCE: 
                                if     (fieldin == ELEM_NAME)           info= "Fibras";
                                else if(fieldin == ELEM_DESC)           info= "Fuente de luz para mazos de fibra optica";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_LEDSTRIP: 
                                if     (fieldin == ELEM_NAME)           info= "Tira LED";
                                else if(fieldin == ELEM_DESC)           info= "Tiras Led de colores que crean un ambiente de color en la sala, tienen efectos y juegos variados.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_ESCALERA: 
                                if     (fieldin == ELEM_NAME)           info= "Escalera de color";
                                else if(fieldin == ELEM_DESC)           info= "Modulo de efectos tipo escalera con diferentes juegos \"causa-efecto\"";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Tabla de colores normal, pulsar el mismo boton dos veces apaga el color";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Modo basico de colores pero con cambios más rápidos!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                                
                        case TYPE_BLACKLIGHT: 
                                if     (fieldin == ELEM_NAME)           info= "Luz negra";
                                else if(fieldin == ELEM_DESC)           info= "Luz UV para trabajar con elementos reactivos a la luz ultravioleta.";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= "Encendido y apagado";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Aleatorio";
                                else if(fieldin == ELEM_MODE_1_DESC)    info= "Se enciende y se apaga de modo aleatorio.";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case TYPE_FAN: 
                                if     (fieldin == ELEM_NAME)           info= "Ventilador";
                                else if(fieldin == ELEM_DESC)           info= "Ventilador que crea un ambiente frio y rafagas de viento";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= "Modo BASICO";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
                                else if(fieldin == ELEM_MODE_1_NAME)    info= "Modo Rapido!";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;

                        case GENERIC_TYPE: 
                                if     (fieldin == ELEM_NAME)           info= " ";
                                else if(fieldin == ELEM_DESC)           info= " ";
                                else if(fieldin == ELEM_MODE_0_NAME)    info= " ";
                                else if(fieldin == ELEM_MODE_0_DESC)    info= " ";
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
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                else if(fieldin == ELEM_MODE_15_DESC)   info= " ";
                                break;
                        default: break;
                }
    }

    return info;
}