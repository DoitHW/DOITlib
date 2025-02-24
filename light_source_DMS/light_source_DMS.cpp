#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <light_source_DMS/light_source_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <vector>
#include <EEPROM.h>

#ifdef PLAYER
  #include <play_DMS/play_DMS.h>
  extern DOITSOUNDS_ doitPlayer;
#endif
extern float varaux;
extern bool BCframe;


LIGHTSOURCE_::LIGHTSOURCE_() : ELEMENT_() {
            set_type(TYPE_LIGHTSOURCE);
            currentMode= DEFAULT_BASIC_MODE;
            activePattern = NO_PATTERN;
        }

void LIGHTSOURCE_::lightsource_begin(){
            colorHandler.begin(NUM_LEDS);
            delay(10);
            element->set_mode(DEFAULT_BASIC_MODE);
}



void LIGHTSOURCE_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
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

    switch(LEF.function){

        case F_SEND_COMMAND:{
            byte command= LEF.data[0];
            byte mode= DEFAULT_BASIC_MODE;
            if     (command == TEST_CMD) delay(1); 
            else if(command == START_CMD){
                element->set_mode(DEFAULT_BASIC_MODE);
                
                event_register(EV_START, 0);
                if(!BCframe){
                    send_frame(frameMaker_RETURN_ELEM_SECTOR(globalID, BROADCAST, &mode, ELEM_CMODE_SECTOR));
                    BCframe= false;
                }
                delay(10);
                event_register(EV_MODE_CHANGE, DEFAULT_BASIC_MODE);
                delay(10);
                colorHandler.set_targetColor(CRGB::White);
            }
            else if(command == BLACKOUT){
                // APAGAR FASTLED

                colorHandler.setAll(0x01, 0x01, 0x01);   
                event_register(EV_END, 0);
                if(!BCframe){
                    send_frame(frameMaker_RETURN_ELEM_SECTOR(globalID, BROADCAST, &mode, ELEM_CMODE_SECTOR));
                    BCframe= false;
                }
                delay(10);
                #ifdef DEBUG
                    print_event_log();
                #endif
                delay(10);
                calculate_and_save_statistics();
                delay(10);
                save_register();
                delay(200);
                ESP.restart();
            } 
            else if(command == SEND_REG_RF_CMD){
                delay(400);
                colorHandler.setAll(30, 30, 5);
                print_event_register_file_RF();
                delay(10);
                colorHandler.setAll(0, 0, 0);
            }
            else if(command == SEND_STATS_RF_CMD){
                delay(400);
                colorHandler.setAll(5, 10, 30);
                print_stats_file_RF();
                delay(10);
                colorHandler.setAll(0, 0, 0);
            }
            else if(command == ERR_DBG_ON){
                delay(100);
                element->set_err_dbg(true);
               
            }
            else if(command == ERR_DBG_OFF){
                delay(100);
                element->set_err_dbg(false);
            }
            else if(command == MAGIC_TEST_CMD){
                static bool magik= true;
                #ifdef PLAYER
                    if(magik) doitPlayer.play_file(99, 99);
                    else      doitPlayer.stop_file();
                    delay(3700);
                #endif
                if(magik){
                    set_mode(LIGHTSOURCE_PATTERN_MODE);
                    set_active_pattern(RAINBOW_PATT);
                }
                else set_mode(DEFAULT_BASIC_MODE);
                magik= !magik;
            }
            else if(command == MAGIC_TEST_2_CMD){
                static bool magik= true;
                #ifdef PLAYER
                    if(magik) doitPlayer.play_file(99, 98);
                    else      doitPlayer.stop_file();
                    delay(100);
                #endif
                if(magik){
                    set_mode(LIGHTSOURCE_PATTERN_MODE);
                    set_active_pattern(FIRE_PATT);
                }
                else set_mode(DEFAULT_BASIC_MODE);
                magik= !magik;
            }
            break;
        }
        case F_REQ_ELEM_SECTOR:{
            byte lang= LEF.data[0];
            byte sector= LEF.data[1];
            //element->event_register_update(EV_SECTOR_REQ, sector);
                                                                            #ifdef DEBUG
                                                                            Serial.println("lenguaje pedido: " + String(lang));   
                                                                            Serial.println("sector pedido: " + String(sector));   
                                                                            #endif
            byte sector_data[192];
            get_sector_data(sector_data, lang, sector);
                                                                            #ifdef DEBUG
                                                                            Serial.println("Sector data: " + String(sector_data[0], HEX));
                                                                            #endif
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
            delay(200);
            event_register(EV_ID_CHANGE, globalID);
            delay(200);
            break;
        }
        default: 
                                                                #ifdef DEBUG
                                                                    Serial.println("Trama recibida pero no se ha iniciado sesion.");
                                                                #endif
            break;
    }

    if(sessionActive){
        switch (LEF.function) {


            case F_SET_ELEM_MODE:{
                
                byte mode= LEF.data[0];
                byte static prevMode= 1;
                                                                            #ifdef DEBUG
                                                                            Serial.println("OJUU, LEF.data[0]= " +String(mode));
                                                                            #endif
                if(mode != LIGHTSOURCE_PATTERN_MODE) colorHandler.set_activePattern(NO_PATTERN);
                element->set_mode(mode);

                if((element->get_currentMode() == LIGHTSOURCE_PASSIVE_MODE) || 
                (element->get_currentMode() == LIGHTSOURCE_SLOW_PASSIVE_MODE)) colorHandler.set_passive(true);
                else                                                      colorHandler.set_passive(false);

                if     (element->get_currentMode() == LIGHTSOURCE_PASSIVE_MODE){
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_slow_passive(false);
                }
                else if(element->get_currentMode() == LIGHTSOURCE_SLOW_PASSIVE_MODE){
                    colorHandler.set_targetFade(SLOW_FADE);
                    colorHandler.set_slow_passive(true);
                }

                event_register(EV_MODE_CHANGE, mode);
                colorHandler.set_targetColor(0);
                colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
                if(!BCframe){
                    send_frame(frameMaker_RETURN_ELEM_SECTOR(globalID, BROADCAST, &mode, ELEM_CMODE_SECTOR));
                    BCframe= false;
                }
                prevMode= mode;
                break;
            }

            case F_SEND_COLOR: {

                byte color = LEF.data[0];
                #ifdef PLAYER
                    if(currentMode_ != LEDSTRIP_PASSIVE_MODE) doitPlayer.play_file(14, color + 11);
                #endif
                event_register(EV_COLOR_CHANGE, color);
                CRGB colorin= colorHandler.get_CRGB_from_colorList(color);
                                                                #ifdef DEBUG
                                                                    Serial.println("Color recibido: " + String(color));
                                                                #endif
                if (currentMode_ == LIGHTSOURCE_BASIC_MODE) {
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
                else if(currentMode_ == LIGHTSOURCE_SLOW_MODE){
                                                                #ifdef DEBUG
                                                                    Serial.println("Manejando en modo LIGHTSOURCE_FAST_MODE. o mix");
                                                                #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(SLOWEST_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_MOTION_LIGHT_MODE){
                                                                #ifdef DEBUG
                                                                    Serial.println("Manejando en modo LIGHTSOURCE_MOTION_MODE.");
                                                                #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_MOTION_COLOR_MODE){
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_MIX_MODE){
                                        #ifdef DEBUG
                                            Serial.println("Manejando en modo MIX MODE");
                                        #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_PULSE_MODE){
                                    #ifdef DEBUG
                                        Serial.println("Manejando en modo MIX MODE");
                                    #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(FASTEST_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_PASSIVE_MODE){

                    if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
                    else                             colorHandler.set_is_paused(true);
                                                                #ifdef DEBUG
                                                                    Serial.println("Manejando en modo LIGHTSOURCE_PASSIVE_MODE.");
                                                                #endif
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_SLOW_PASSIVE_MODE){

                    if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
                    else                             colorHandler.set_is_paused(true);
                                                                #ifdef DEBUG
                                                                    Serial.println("Manejando en modo LIGHTSOURCE_PASSIVE_MODE.");
                                                                #endif
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_LIGHT_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo MIX MODE");
                                                            #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_REVERSE_LIGHT_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo MIX MODE");
                                                            #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                
                else if(currentMode_ == LIGHTSOURCE_VOICE_COLOR_MODE){
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_BUBBLES_MODE){
                    break;
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_REVERSE_BUBBLES_MODE){
                    break;
                } 
            }
        
            case F_SEND_SENSOR_VALUE_1:{
                                                                #ifdef DEBUG
                                                                Serial.println("Se ha recibido un sensor value");
                                                                #endif
                if(currentMode_ == LIGHTSOURCE_MOTION_LIGHT_MODE){
                    byte value= get_brightness_from_sensorValue(LEF);
                    colorHandler.set_targetFade(MOTION_VAL_FADE);
                    colorHandler.set_targetBrightness(value);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                }

                else if(currentMode_ == LIGHTSOURCE_MOTION_COLOR_MODE){
                    byte value= get_color_from_sensorValue(LEF); 
                    CRGB colorin= colorHandler.get_CRGB_from_pasiveColorList(value);
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(RB_MOTION_VAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                }
                break;
            }

            case F_SEND_SENSOR_VALUE_2:{

                if     (currentMode_ == LIGHTSOURCE_VOICE_LIGHT_MODE){
         
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_REVERSE_LIGHT_MODE){
         
                }

                else if(currentMode_ == LIGHTSOURCE_VOICE_COLOR_MODE){
         
                }
                

                break;
            }

            case F_SEND_PATTERN_NUM:{
                
                if(currentMode_ == LIGHTSOURCE_PATTERN_MODE){
                    byte patt= LEF.data[0];
                    set_active_pattern(patt);
                }
                break;
            }

        

            default: {break;}
        }
    }

    stackSize = uxTaskGetStackHighWaterMark(NULL);  // ojo que esto no se que hace, pero si lo quitas se mueren 3 gatitos y 1 peruano.
                                                                #ifdef DEBUG
                                                                   Serial.println("Stack restante al final: " + String(stackSize));
                                                                #endif
}

