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
            colorHandler.begin(NUM_LEDS);
            delay(10);
            pinMode(COLUMN_RELAY_PIN, OUTPUT);
}

void COLUMN_::inic_elem_config(){

    colorHandler.begin(NUM_LEDS);
    delay(10);
    pinMode(COLUMN_RELAY_PIN, OUTPUT);
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
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }

    // Depuración del estado de la pila
    Serial.println("Inicio de RX_main_handler");
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            /*
                                                            #ifdef DEBUG
                                                               Serial.println("Stack restante: " + String(stackSize));
                                                            #endif
                                                            */

    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

       case F_REQ_ELEM_INFO:{
            INFO_PACK_T info= get_info_pack(LEF.data[0]);
            FRAME_T frame= frameMaker_RETURN_ELEM_INFO(element->ID, LEF.origin, &info);
            send_frame(frame);
                                                        /*
                                                        #ifdef DEBUG
                                                            Serial.println("Info devuelta en un Return");
                                                        #endif
                                                        */
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

  /*    case F_SEND_TEST:{


            break;
        }*/

        case F_SET_ELEM_MODE:{
            byte mode= LEF.data[0];
            element->currentMode= mode;
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
                    Serial.println("Iniciando cronómetro.");
                    start_working_time();
                } else {
                    Serial.println("El cronómetro ya está activo.");
                }
            } else {
                if (stopwatchRunning) {
                    Serial.println("Deteniendo y guardando el cronómetro.");
                    stopAndSave_working_time();
                } else {
                    Serial.println("El cronómetro ya está detenido.");
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
        else if(currentMode_ == COLUMN_SLOW_MODE || currentMode_ == COLUMN_MIX_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(SLOWEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_MOTION_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_RB_MOTION_MODE){   // OJO
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_MIX_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            }
        else if(currentMode_ == COLUMN_PASSIVE_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
/* OJITO*/
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
                                                                    Serial.println(" he recibido un flag byte: " );
                                                                #endif
            bool bubbles= LEF.data[0] & 0x01;
            Serial.println(bubbles);
            if(bubbles){
                digitalWrite(COLUMN_RELAY_PIN, HIGH);
                element->set_flag(RELAY_1_FLAG, SET_RELAY);
            }
            else{
                digitalWrite(COLUMN_RELAY_PIN, LOW);
                element->set_flag(RELAY_1_FLAG, RESET_RELAY);
            }
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
    stackSize = uxTaskGetStackHighWaterMark(NULL);  // ojo que esto no se que hace, pero si lo quitas se mueren 3 gatitos y 1 peruano.
                                                                #ifdef DEBUG
                                                                   Serial.println("Stack restante al final: " + String(stackSize));
                                                                #endif
}

