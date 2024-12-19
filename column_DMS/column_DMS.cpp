#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <column_DMS/column_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <vector>
#include <EEPROM.h>


COLUMN_::COLUMN_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_COLUMN);
            currentMode= DEFAULT_BASIC_MODE;
        }

void COLUMN_::column_begin(){
            colorHandler.begin(COLUMN_NUM_LEDS);
            delay(10);
            pinMode(COLUMN_RELAY_PIN, OUTPUT);
            element->set_mode(DEFAULT_BASIC_MODE);
}

void COLUMN_::relay_handler(bool actionin){

    if(actionin) digitalWrite(COLUMN_RELAY_PIN, HIGH);
    else         digitalWrite(COLUMN_RELAY_PIN, LOW);
}

void COLUMN_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }
                                                            #ifdef DEBUG
                                                                Serial.println("Inicio de RX_main_handler");
                                                            #endif
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                                Serial.println("Stack restante: " + String(stackSize));
                                                            #endif

    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

        case F_REQ_ELEM_INFO:{
            INFO_PACK_T info= get_info_pack(TYPE_COLUMN, LEF.data[0]);
            FRAME_T frame= frameMaker_RETURN_ELEM_INFO(LEF.origin, info);
            send_frame(frame);
                                                        #ifdef DEBUG
                                                            Serial.println("Info devuelta en un Return");
                                                        #endif
            break;
        }

        case F_REQ_ELEM_STATE:{


            break;
        }

        case F_SET_ELEM_ID:{
            element->ID= LEF.data[0];
            File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r+");
            configFile.readStringUntil('\n');
            configFile.seek(configFile.position());
            configFile.println(element->ID); 
            configFile.close();
            break;
        }

        case F_SEND_TEST:{


            break;
        }

        case F_SET_ELEM_MODE:{
            byte mode= LEF.data[0];
            element->currentMode= mode;
            if(element->get_currentMode() == COLUMN_PASSIVE_MODE) colorHandler.set_passive(true);
            else                                                  colorHandler.set_passive(false);
            break;
        }
        case F_SEND_COLOR: {
            byte color = LEF.data[0];
            CRGB colorin= colorHandler.get_CRGB_from_colorList(color);
                                                            #ifdef DEBUG
                                                                Serial.println("Color recibido: " + String(color));
                                                            #endif

                if (color != 8) {
                if (!stopwatchRunning) {
                                                            #ifdef DEBUG
                                                                Serial.println("Iniciando cronómetro.");
                                                            #endif
                    start_working_time();
                } else {
                                                            #ifdef DEBUG
                                                                Serial.println("El cronómetro ya está activo.");
                                                            #endif
                }
            } else {
                if (stopwatchRunning) {
                                                            #ifdef DEBUG
                                                                Serial.println("Deteniendo y guardando el cronómetro.");
                                                            #endif
                    stopAndSave_working_time();
                } else {
                                                            #ifdef DEBUG
                                                                Serial.println("El cronómetro ya está detenido.");
                                                            #endif
                }
            }

            if (currentMode_ == COLUMN_BASIC_MODE) {
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif

            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_FAST_MODE || currentMode_ == COLUMN_MIX_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_FAST_MODE. o mix");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(FASTEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_MOTION_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_MOTION_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_PASSIVE_MODE){

            if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
            else                             colorHandler.set_is_paused(true);
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                            #endif
            }
            break;
        }
        case F_SEND_SENSOR_VALUE:{
                                                            #ifdef DEBUG
                                                            Serial.println("Se ha recibido un sensor value");
                                                            #endif
            if(currentMode_ == COLUMN_MOTION_MODE){
                byte value= get_brightness_from_sensorValue(LEF);
                colorHandler.set_targetFade(FASTEST_FADE);
                colorHandler.set_targetBrightness(value);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == COLUMN_RB_MOTION_MODE){
                byte value= get_color_from_sensorValue(LEF); 
                colorHandler.set_targetColor(value);
                colorHandler.set_targetFade(FASTEST_FADE);
                colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            break;
        }

        case F_SEND_FLAG_BYTE:{
                                                                #ifdef DEBUG
                                                                    Serial.println("");
                                                                #endif
            bool bubbles= LEF.data[0] & 0x01;
            if(bubbles) digitalWrite(COLUMN_RELAY_PIN, HIGH);
            else        digitalWrite(COLUMN_RELAY_PIN, LOW);
            break;
        }

        default: {
                                                                #ifdef DEBUG
                                                                    Serial.println("Se ha recibido una función desconocida.");
                                                                #endif
            break;
        }
    }

    // Depuración al final de la función
    stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                                #ifdef DEBUG
                                                                    Serial.println("Stack restante al final: " + String(stackSize));
                                                                #endif
}
