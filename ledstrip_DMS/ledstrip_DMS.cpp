#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <ledstrip_DMS/ledstrip_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <vector>
#include <EEPROM.h>

extern float varaux;

LEDSTRIP_::LEDSTRIP_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_LEDSTRIP);
            currentMode= DEFAULT_BASIC_MODE;
            activePattern = NO_PATTERN;
        }

void LEDSTRIP_::ledstrip_begin(){
            colorHandler.begin(NUM_LEDS);
            delay(10);

            element->set_mode(DEFAULT_BASIC_MODE);
}

void ELEMENT_::work_time_handler(byte colorin){
    if (colorin != 8) {
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
}

void LEDSTRIP_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }
                                                            #ifdef DEBUG
                                                                Serial.println("Inicio de RX_main_handler, provando F_RQ_NFO");
                                                            #endif
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                               Serial.println("Stack restante: " + String(stackSize));
                                                            #endif

    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

        case F_REQ_ELEM_INFO:{
            byte lang= LEF.data[0];
            
            Serial.println("Recibido F_REQ_ELEM_INFO, lang= " +String(lang));
            
            Serial.println("Info obtenida, creado en infopack.");
           // FRAME_T frame= frameMaker_RETURN_ELEM_INFO(element->ID, LEF.origin, info);
           // send_frame(frame);
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

        case F_SET_ELEM_MODE:{
            byte mode= LEF.data[0];
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJUU, LEF.data[0]= " +String(mode));
                                                                        #endif
            if(mode != LEDSTRIP_PATTERN_MODE) colorHandler.set_activePattern(NO_PATTERN);
            element->set_mode(mode);
            if(element->get_currentMode() == LEDSTRIP_PASSIVE_MODE) colorHandler.set_passive(true);
            else                                                    colorHandler.set_passive(false);
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJITO, que passem a modo: " +String(element->get_currentMode()));
                                                                        #endif
            break;
        }
        case F_SEND_COLOR: {
            byte color = LEF.data[0];
            element->work_time_handler(color);
            CRGB colorin= colorHandler.get_CRGB_from_colorList(color);
                                                            #ifdef DEBUG
                                                                Serial.println("Color recibido: " + String(color));
                                                            #endif
            if (currentMode_ == LEDSTRIP_BASIC_MODE) {
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
        else if(currentMode_ == LEDSTRIP_SLOW_MODE){
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
        else if(currentMode_ == LEDSTRIP_MOTION_MODE){
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
        else if(currentMode_ == LEDSTRIP_MIX_MODE){
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
        else if(currentMode_ == LEDSTRIP_PASSIVE_MODE){

            if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
            else                             colorHandler.set_is_paused(true);
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                            #endif
            
            break;
            }
        else if(currentMode_ == LEDSTRIP_PATTERN_MODE){
            // OJO de moment, no fer res.
            break;
            }
        else if(currentMode_ == LEDSTRIP_TEST_ZONE_MODE){
            break;
            }
        break;
        }
        
        case F_SEND_SENSOR_VALUE:{
                                                            #ifdef DEBUG
                                                            Serial.println("Se ha recibido un sensor value");
                                                            #endif
            if(currentMode_ == LEDSTRIP_MOTION_MODE){
                byte value= get_brightness_from_sensorValue(LEF);
                colorHandler.set_targetFade(MOTION_VAL_FADE);
                colorHandler.set_targetBrightness(value);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == LEDSTRIP_RB_MOTION_MODE){
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
