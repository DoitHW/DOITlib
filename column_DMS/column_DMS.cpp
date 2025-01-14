#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <column_DMS/column_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <vector>
#include <EEPROM.h>

extern float varaux;


COLUMN_::COLUMN_() : ELEMENT_() {
            set_type(TYPE_COLUMN);
            currentMode= DEFAULT_BASIC_MODE;
            activePattern = NO_PATTERN;
        }

void COLUMN_::column_begin(){
            colorHandler.begin(NUM_LEDS);
            delay(10);
            pinMode(COLUMN_RELAY_PIN, OUTPUT);
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
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                               Serial.println("Stack restante: " + String(stackSize));
                                                            #endif
    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

        case F_REQ_ELEM_SECTOR:{
            byte lang= LEF.data[0];
            byte sector= LEF.data[1];
            Serial.println("lenguaje pedido: " + String(lang));   
            Serial.println("sector pedido: " + String(sector));   
            byte sector_data[192];
            get_sector_data(sector_data, lang, sector);
            Serial.println("Sector data: " + String(sector_data[0], HEX));
            FRAME_T frame= frameMaker_RETURN_ELEM_SECTOR(globalID, LEF.origin, sector_data, sector);
            send_frame(frame);
                                                            #ifdef DEBUG
                                                             Serial.println("Info devuelta en un Return");
                                                             Serial.println("Recibido F_REQ_ELEM_INFO, lang= " +String(lang));
                                                             Serial.println("Recibido F_REQ_ELEM_INFO, sector= " +String(sector));
                                                            #endif
            break;
        }
        case F_SET_ELEM_ID:{
            element->set_ID(LEF.data[0]);
            globalID= LEF.data[0];
            break;
        }
        case F_SET_ELEM_MODE:{
            
            byte mode= LEF.data[0];
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJUU, LEF.data[0]= " +String(mode));
                                                                        #endif
            if(mode != COLUMN_PATTERN_MODE) colorHandler.set_activePattern(NO_PATTERN);
            element->set_mode(mode);
            if(element->get_currentMode() == COLUMN_PASSIVE_MODE) colorHandler.set_passive(true);
            else                                                  colorHandler.set_passive(false);
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJITO, que passem a modo: " +String(element->get_currentMode()));
                                                                        #endif
            break;
        }
        case F_SEND_TEST:{
            byte testin= LEF.data[0];
            if     (testin == HELLO_TEST) delay(1);// fer algo}
            else if(testin == COLOR_TEST) delay(1);// fer algo}
            else if(testin == BLACKOUT){
                element->work_time_handler(8);
                delay(300);
                ESP.restart();
            } // fer algo}
            break;
        }
        case F_SEND_COLOR:{
            byte color = LEF.data[0];
            element->work_time_handler(color);
            CRGB colorin= colorHandler.get_CRGB_from_colorList(color);
                                                            #ifdef DEBUG
                                                                Serial.println("Color recibido: " + String(color));
                                                            #endif
            if (currentMode_ == COLUMN_BASIC_MODE) {
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif

            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
            }
        else if(currentMode_ == COLUMN_SLOW_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_FAST_MODE. o mix");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(SLOWEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
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
            break;
            }
        else if(currentMode_ == COLUMN_MIX_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            colorHandler.set_targetColor(colorin);
            colorHandler.set_targetFade(FASTEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
            }
        else if(currentMode_ == COLUMN_PASSIVE_MODE){
            if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
            else                             colorHandler.set_is_paused(true);
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                            #endif
            break;
            }
        }
       
        case F_SEND_SENSOR_VALUE:{
                                                            #ifdef DEBUG
                                                            Serial.println("Se ha recibido un sensor value");
                                                            #endif
            if(currentMode_ == COLUMN_MOTION_MODE){
                byte value= get_brightness_from_sensorValue(LEF);
                colorHandler.set_targetFade(MOTION_VAL_FADE);
                colorHandler.set_targetBrightness(value);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == COLUMN_RB_MOTION_MODE){
                byte value= get_color_from_sensorValue(LEF); 
                CRGB colorin= colorHandler.get_CRGB_from_pasiveColorList(value);
                colorHandler.set_targetColor(colorin);
                colorHandler.set_targetFade(RB_MOTION_VAL_FADE);
                colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == LEDSTRIP_PATTERN_MODE){
                varaux= get_aux_var_01_from_sensorValue(LEF);
            }
            break;
        }
        case F_SEND_PATTERN_NUM:{
                byte numPattern= LEF.data[0];
                if(numPattern != NO_PATTERN) element->work_time_handler(1);
                else                         element->work_time_handler(8);
                colorHandler.set_activePattern(numPattern);                                           
            break;
        }
        case F_SEND_FLAG_BYTE:{
            bool bubbles= LEF.data[0] & 0x01;
                                                                #ifdef DEBUG
                                                                    Serial.println(" he recibido un flag byte: " );
                                                                    Serial.println(bubbles);
                                                                #endif
           
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

        default:{
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

