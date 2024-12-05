#include "defines_DMS.h"
#include "Element_DMS.h"
#include "light_source_DMS.h"
#include "Frame_DMS.h"
#include "Color_DMS.h"
#include <vector>
#include <EEPROM.h>


void LIGHTSOURCE_::inic_elem_config(){

    CRGB leds[NUM_LEDS];
    FastLED.addLeds<WS2815, LED_DATA_PIN, GRB>(leds, NUM_LEDS);
    delay(10);
    byte id= EEPROM.read(EEPROM_ID_ADDRESS);
    element->set_ID(id);
    element->set_mode(DEFAULT_BASIC_MODE);
                                                                #ifdef DEBUG
                                                                  Serial.println("Se inicializa FASTLED.");
                                                                  Serial.println("RELAY pin configurado como salida.");
                                                                  Serial.println("Se lee EEPROM para asignar element->ID.");
                                                                  Serial.println("currentMode = BASIC MODE.");
                                                                #endif
}


void LIGHTSOURCE_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    byte currentMode_ = element->get_currentMode();
    switch (LEF.function) {

        case F_REQ_ELEM_INFO: {
            byte type = element->get_type();
            INFO_PACK_T infopack = element->get_info_pack(TYPE_LIGHTSOURCE, LEF.data[0]);
                                                                #ifdef DEBUG
                                                                  Serial.println("Se ha recibido un REQ_INFO, se procede a devolver la info.");
                                                                #endif
            send_frame(frameMaker_RETURN_ELEM_INFO(LEF.origin, infopack));
            break;
        }

        case F_SEND_TEST: {
            /*
            hacer test
            */
            break;
        }

        case F_SEND_COLOR: {
            byte color = LEF.data[0];
            
            if(color != BLACK) startTime= millis();
            else {
                uint64_t finishTime= millis();
                uint64_t time= finishTime - startTime;
                uint64_t registeredTime;
                EEPROM.get(EEPROM_WORKING_TIME_ADDRESS, registeredTime);
                time += registeredTime;
                element->register_working_time(time);
            }
            
                                                                #ifdef DEBUG
                                                                  Serial.println("Se ha recibido el color." + String(color)); // OJO... revisar
                                                                #endif
            if (currentMode_ == LIGHTSOURCE_BASIC_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_FAST_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_RB_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_MIX_MODE) {
                // Código específico
            }
            break;
        }

        case F_SEND_SENSOR_VALUE: {
            byte sensorVal = get_mapped_sensor_value(LEF.data[0], LEF.data[1], LEF.data[2], LEF.data[3], LEF.data[4], LEF.data[5]);
                                                                            #ifdef DEBUG
                                                                              Serial.println("Se ha recibido un SENSOR_VALUE, se procede a hacer magia con este numero: " + String(sensorVal));
                                                                            #endif
            if (currentMode_ == LIGHTSOURCE_BASIC_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_FAST_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_RB_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == LIGHTSOURCE_MIX_MODE) {
                // Código específico
            }
            break;
        }

        case F_SEND_FLAG_BYTE: {
              
            break;
        }

        default:
// pam
            break;
    }
}



void LIGHTSOURCE_::element_action(){

    /*
    update_leds();
    update_Relay();
    */

}