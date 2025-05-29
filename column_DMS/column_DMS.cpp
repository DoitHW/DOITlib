#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <column_DMS/column_DMS.h>
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


COLUMN_::COLUMN_() : ELEMENT_() {
            set_type(TYPE_COLUMN);
            currentMode= DEFAULT_BASIC_MODE;
            activePattern = NO_PATTERN;
        }

void COLUMN_::column_begin(){
            colorHandler.begin(NUM_LEDS);
            delay(10);
            pinMode(COLUMN_RELAY_PIN, OUTPUT);
            digitalWrite(COLUMN_RELAY_PIN, LOW);
            element->set_mode(DEFAULT_BASIC_MODE);
}



void COLUMN_::relay_handler(bool actionin){

    if(actionin) digitalWrite(COLUMN_RELAY_PIN, HIGH);
    else         digitalWrite(COLUMN_RELAY_PIN, LOW);
}

void COLUMN_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                DEBUG__________ln("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                               DEBUG__________ln("Stack restante: " + String(stackSize));
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
            else if(command == TEST_CMD){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido NOP_");
                #endif
                for(int i= 0; i < 10; i++){
                    colorHandler.setAll(0, 0, 0);
                    delay(100);
                    colorHandler.setAll(5*i, 200, 100 - 2*i);
                    delay(100);
                }
            }
            else if(command == SEND_REG_RF_CMD){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido REG");
                #endif
                delay(400);
                colorHandler.setAll(30, 30, 5);
                print_event_register_file_RF();
                delay(10);
                colorHandler.setAll(0, 0, 0);
            }
            else if(command == SEND_STATS_RF_CMD){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido STATS");
                #endif
                delay(400);
                colorHandler.setAll(5, 10, 30);
                print_stats_file_RF();
                delay(10);
                colorHandler.setAll(0, 0, 0);
            }
            else if(command == ERR_DBG_ON){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido ERR ON");
                #endif
                delay(100);
                element->set_err_dbg(true);
               
            }
            else if(command == ERR_DBG_OFF){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido ERR OFF");
                #endif
                delay(100);
                element->set_err_dbg(false);
            }
            else if(command == SET_ELEM_DEAF){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido DEAF SHORT");
                #endif
                Serial1.end();
                delay(MIN_DEAF_TIME);
                configurar_RF(RF_BAUD_RATE);
                colorHandler.setAll(0x00, 0xFF, 0x10);
                delay(300);
                colorHandler.setAll(0x00, 0x00, 0x00);
            }
            else if(command == SET_ELEM_LONG_DEAF){
                #ifdef DEBUG
                    DEBUG__________ln("Recibido DEAF LONG");
                #endif
                Serial1.end();
                delay(MAX_DEAF_TIME);
                configurar_RF(RF_BAUD_RATE);
                colorHandler.setAll(0x00, 0xFF, 0x10);
                delay(300);
                colorHandler.setAll(0x00, 0x00, 0x00);
            }
            else if(command == MAGIC_TEST_CMD){
                _ERR_THROW_START_ "PUDDI PUDDI" _ERR_THROW_END_
                #ifdef DEBUG
                    DEBUG__________ln("Recibido PUDDI PUDDI");
                #endif
                static bool magik= true;
                #ifdef PLAYER
                    if(magik) doitPlayer.play_file(99, 99);
                    else      doitPlayer.stop_file();
                    delay(3700);
                #endif
                if(magik){
                    set_mode(COLUMN_PATTERN_MODE);
                    set_active_pattern(RAINBOW_PATT);
                }
                else set_mode(DEFAULT_BASIC_MODE);
                set_flag(RELAY_1_FLAG, magik);
                digitalWrite(COLUMN_RELAY_PIN, magik);
                magik= !magik;
            }
            else if(command == MAGIC_TEST_2_CMD){
                _ERR_THROW_START_ "MISION IMPOSIBOL" _ERR_THROW_END_
                #ifdef DEBUG
                    DEBUG__________ln("Recibido MISION IMPOSIBLE");
                #endif
                static bool magik= true;
                #ifdef PLAYER
                    if(magik) doitPlayer.play_file(99, 98);
                    else      doitPlayer.stop_file();
                    delay(100);
                #endif
                if(magik){
                    set_mode(COLUMN_PATTERN_MODE);
                    set_active_pattern(FIRE_PATT);
                }
                else set_mode(DEFAULT_BASIC_MODE);
                set_flag(RELAY_1_FLAG, magik);
                digitalWrite(COLUMN_RELAY_PIN, magik);
                magik= !magik;

            }
            break;
        }
        case F_REQ_ELEM_SECTOR:{
            byte lang= LEF.data[0];
            byte sector= LEF.data[1];
            //element->event_register_update(EV_SECTOR_REQ, sector);
                                                                            #ifdef DEBUG
                                                                            DEBUG__________ln("lenguaje pedido: " + String(lang));   
                                                                            DEBUG__________ln("sector pedido: " + String(sector));   
                                                                            #endif
            byte sector_data[192];
            get_sector_data(sector_data, lang, sector);
                                                                            #ifdef DEBUG
                                                                            DEBUG__________ln("Sector data: " + String(sector_data[0], HEX));
                                                                            #endif
            FRAME_T frame= frameMaker_RETURN_ELEM_SECTOR(globalID, LEF.origin, sector_data, sector);
            send_frame(frame);
            
                                                            #ifdef DEBUG
                                                             DEBUG__________ln("Info devuelta en un Return");
                                                             DEBUG__________ln("Recibido F_REQ_ELEM_INFO, lang= " +String(lang));
                                                             DEBUG__________ln("Recibido F_REQ_ELEM_INFO, sector= " +String(sector));
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
                                                                    DEBUG__________ln("Trama recibida pero no se ha iniciado sesion.");
                                                                #endif
            break;
    }

    if(sessionActive){
        switch (LEF.function) {


            case F_SET_ELEM_MODE:{
                
                byte mode= LEF.data[0];
                byte static prevMode= 1;
                                                                            #ifdef DEBUG
                                                                            DEBUG__________ln("OJUU, LEF.data[0]= " +String(mode));
                                                                            #endif
                if(mode != COLUMN_PATTERN_MODE) colorHandler.set_activePattern(NO_PATTERN);
                element->set_mode(mode);

                if((element->get_currentMode() == COLUMN_PASSIVE_MODE) || 
                (element->get_currentMode() == COLUMN_SLOW_PASSIVE_MODE)) colorHandler.set_passive(true);
                else                                                      colorHandler.set_passive(false);

                if     (element->get_currentMode() == COLUMN_PASSIVE_MODE){
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_slow_passive(false);
                }
                else if(element->get_currentMode() == COLUMN_SLOW_PASSIVE_MODE){
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
                                                                    DEBUG__________ln("Color recibido: " + String(color));
                                                                #endif
                if (currentMode_ == COLUMN_BASIC_MODE) {
                                                                #ifdef DEBUG
                                                                    DEBUG__________ln("Manejando en modo BASIC_MODE.");
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
                                                                    DEBUG__________ln("Manejando en modo COLUMN_FAST_MODE. o mix");
                                                                #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(SLOWEST_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == COLUMN_MOTION_LIGHT_MODE){
                                                                #ifdef DEBUG
                                                                    DEBUG__________ln("Manejando en modo COLUMN_MOTION_MODE.");
                                                                #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == COLUMN_MOTION_COLOR_MODE){
                    break;
                }

                else if(currentMode_ == COLUMN_MIX_MODE){
                                        #ifdef DEBUG
                                            DEBUG__________ln("Manejando en modo MIX MODE");
                                        #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == COLUMN_PULSE_MODE){
                                    #ifdef DEBUG
                                        DEBUG__________ln("Manejando en modo MIX MODE");
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
                                                                    DEBUG__________ln("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                                #endif
                    break;
                }

                else if(currentMode_ == COLUMN_SLOW_PASSIVE_MODE){

                    if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
                    else                             colorHandler.set_is_paused(true);
                                                                #ifdef DEBUG
                                                                    DEBUG__________ln("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                                #endif
                    break;
                }

                else if(currentMode_ == COLUMN_VOICE_LIGHT_MODE){
                                                            #ifdef DEBUG
                                                                DEBUG__________ln("Manejando en modo MIX MODE");
                                                            #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                else if(currentMode_ == COLUMN_VOICE_REVERSE_LIGHT_MODE){
                                                            #ifdef DEBUG
                                                                DEBUG__________ln("Manejando en modo MIX MODE");
                                                            #endif
                    colorHandler.set_targetColor(colorin);
                    colorHandler.set_targetFade(NORMAL_FADE);
                    colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                    break;
                }

                
                else if(currentMode_ == COLUMN_VOICE_COLOR_MODE){
                    break;
                }

                else if(currentMode_ == COLUMN_VOICE_BUBBLES_MODE){
                    break;
                }

                else if(currentMode_ == COLUMN_VOICE_REVERSE_BUBBLES_MODE){
                    break;
                } 
            }
        
            case F_SEND_SENSOR_VALUE_1:{
                                                                #ifdef DEBUG
                                                                DEBUG__________ln("Se ha recibido un sensor value");
                                                                #endif
                if(currentMode_ == COLUMN_MOTION_LIGHT_MODE){
                    byte value= get_brightness_from_sensorValue(LEF);
                    colorHandler.set_targetFade(MOTION_VAL_FADE);
                    colorHandler.set_targetBrightness(value);
                    colorHandler.transitionStartTime= millis();
                    colorHandler.transitioning= true;
                }

                else if(currentMode_ == COLUMN_MOTION_COLOR_MODE){
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

                if     (currentMode_ == COLUMN_VOICE_LIGHT_MODE){
         
                }

                else if(currentMode_ == COLUMN_VOICE_REVERSE_LIGHT_MODE){
         
                }

                else if(currentMode_ == COLUMN_VOICE_COLOR_MODE){
         
                }
                
                else if(currentMode_ == COLUMN_VOICE_BUBBLES_MODE){
         
                }

                else if(currentMode_ == COLUMN_VOICE_REVERSE_BUBBLES_MODE){
         
                }
                

                break;
            }
            case F_SEND_FLAG_BYTE:{
                bool bubbles= LEF.data[0] & 0x01;
                                                                    #ifdef DEBUG
                                                                        DEBUG__________ln(" he recibido un flag byte: " );
                                                                        DEBUG__________ln(bubbles);
                                                                    #endif
                if( currentMode_ == COLUMN_BASIC_MODE ||
                    currentMode_ == COLUMN_SLOW_MODE ||
                    currentMode_ == COLUMN_MOTION_LIGHT_MODE ||
                    currentMode_ == COLUMN_MOTION_COLOR_MODE ||
                    currentMode_ == COLUMN_MIX_MODE ||
                    currentMode_ == COLUMN_PULSE_MODE ||
                    currentMode_ == COLUMN_PASSIVE_MODE ||
                    currentMode_ == COLUMN_SLOW_PASSIVE_MODE ||
                    currentMode_ == COLUMN_VOICE_LIGHT_MODE ||
                    currentMode_ == COLUMN_VOICE_REVERSE_LIGHT_MODE ||
                    currentMode_ == COLUMN_VOICE_COLOR_MODE ||
                    currentMode_ == COLUMN_PATTERN_MODE){

                        if(bubbles){
                            digitalWrite(COLUMN_RELAY_PIN, HIGH);
                            element->set_flag(RELAY_1_FLAG, SET_RELAY);
                        }
                        else{
                            digitalWrite(COLUMN_RELAY_PIN, LOW);
                            element->set_flag(RELAY_1_FLAG, RESET_RELAY);
                        }
                }
                    break;
            }

            case F_SEND_PATTERN_NUM:{
                
                if(currentMode_ == COLUMN_PATTERN_MODE){
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
                                                                   DEBUG__________ln("Stack restante al final: " + String(stackSize));
                                                                #endif
}

