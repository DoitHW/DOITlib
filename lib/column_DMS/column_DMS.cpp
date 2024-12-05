#include "defines_DMS.h"
#include "Element_DMS.h"
#include "column_DMS.h"
#include "Frame_DMS.h"
#include "Color_DMS.h"
#include <vector>
#include <EEPROM.h>

COLORHANDLER_ colorHandler;

void COLUMN_::inic_elem_config(){

    colorHandler.begin(COLUMN_NUM_LEDS);
    delay(10);

    pinMode(COLUMN_RELAY_PIN, OUTPUT);
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

void COLUMN_::relay_handler(bool actionin){

    if(actionin) digitalWrite(COLUMN_RELAY_PIN, HIGH);
    else         digitalWrite(COLUMN_RELAY_PIN, LOW);
}

void COLUMN_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    byte currentMode_ = element->get_currentMode();
    switch (LEF.function) {

        case F_REQ_ELEM_INFO: {
           // byte type = element->get_type();
            INFO_PACK_T infopack = element->get_info_pack(TYPE_COLUMN, LEF.data[0]);  // OJO
                                                                #ifdef DEBUG
                                                                  Serial.println("Se ha recibido un REQ_INFO, se procede a devolver la info.");
                                                                #endif
            send_frame(frameMaker_RETURN_ELEM_INFO(LEF.origin, infopack));
            break;
        }

        case F_REQ_ELEM_STATE: {

            break;
        }


        case F_SEND_TEST: {
            Serial.println("TESTING");
            delay(1000);
            Serial.println("Test 7");
            break;
        }

        case F_SEND_COLOR: {
            byte color = LEF.data[0];
            Serial.println("DATA " + String(LEF.data[0], HEX));
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
            if (currentMode_ == COLUMN_BASIC_MODE) {
                 CRGB aux= colorHandler.get_color(color);
                 colorHandler.set_target_color(aux);
                 colorHandler.set_target_brightness(MAX_BRIGHTNESS);
                 colorHandler.set_target_transition_time(NORMAL_FADE);

            } else if (currentMode_ == COLUMN_FAST_MODE) {
                 CRGB aux= colorHandler.get_color(color);
                 colorHandler.set_target_color(aux);
                 colorHandler.set_target_brightness(MAX_BRIGHTNESS);
                 colorHandler.set_target_transition_time(FASTEST_FADE);
            } else if (currentMode_ == COLUMN_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_RB_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_MIX_MODE) {
                // Código específico
            }

            
            break;
        }

        case F_SEND_SENSOR_VALUE: {
            byte sensorVal = get_mapped_sensor_value(LEF.data[0], LEF.data[1], LEF.data[2], LEF.data[3], LEF.data[4], LEF.data[5]);
                                                                            #ifdef DEBUG
                                                                              Serial.println("Se ha recibido un SENSOR_VALUE, se procede a hacer magia con este numero: " + String(sensorVal));
                                                                            #endif
            if (currentMode_ == COLUMN_BASIC_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_FAST_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_RB_MOTION_MODE) {
                // Código específico
            } else if (currentMode_ == COLUMN_MIX_MODE) {
                // Código específico
            }
            break;
        }

        case F_SEND_FLAG_BYTE: {
                                                                                    #ifdef DEBUG
                                                                                      Serial.println("Se ha recibido un FLAG_BYTE, se comprueba el LSB y se procede a encender o apagar el rele.");
                                                                                    #endif
            byte action = LEF.data[0] & 0x01;
            relay_handler(action);
                                                                                    #ifdef DEBUG
                                                                                      if (action) Serial.println("Rele ON.");
                                                                                      else        Serial.println("Rele OFF.");
                                                                                    #endif
            break;
        }

        default:
// pam
            break;
    }
}



void COLUMN_::element_action(){



}