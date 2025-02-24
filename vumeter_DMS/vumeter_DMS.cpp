#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <vumeter_DMS/vumeter_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <driver/i2s.h>
#include <ctime> 
#include <arduinoFFT.h>
#include <algorithm>

#ifdef PLAYER
  #include <play_DMS/play_DMS.h>
  extern DOITSOUNDS_ doitPlayer;
#endif
extern bool BCframe;
extern float varaux;
extern CRGB* leds;
extern byte numColorRec;
INFO_PACK_T info;

VUMETER_::VUMETER_() {
    set_type(TYPE_ESCALERA);
    currentMode = VUMETER_BASIC_MODE;
    activePattern = NO_PATTERN;
}

void VUMETER_::vumeter_begin(){
    colorHandler.begin(NUM_LEDS);
    delay(10);
    element->set_mode(DEFAULT_BASIC_MODE);

    pinMode(VUMETER_SEL_MODE_PIN_01, INPUT_PULLUP);
    pinMode(VUMETER_SEL_MODE_PIN_02, INPUT_PULLUP);
    pinMode(VUMETER_SEL_MODE_PIN_03, INPUT_PULLUP);
    pinMode(VUMETER_SEL_MODE_PIN_04, INPUT_PULLUP);
    pinMode(VUMETER_SEL_MODE_PIN_05, INPUT_PULLUP);
    pinMode(VUMETER_SEL_MODE_PIN_06, INPUT_PULLUP);
}



void VUMETER_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
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

        case F_REQ_ELEM_SECTOR:{
            byte lang= LEF.data[0];
            byte sector= LEF.data[1];
            //element->event_register_update(EV_SECTOR_REQ, sector);
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
           // event_register(EV_ID_CHANGE, globalID);
            break;
        }

        case F_SET_ELEM_MODE:{
            byte mode= LEF.data[0];
            byte static prevMode= 1;
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJUU, LEF.data[0]= " +String(mode));
                                                                        #endif
            if(mode != VUMETER_PATTERN_MODE) colorHandler.set_activePattern(NO_PATTERN);
            element->set_mode(mode);
            if(element->get_currentMode() == VUMETER_PASSIVE_MODE)  colorHandler.set_passive(true);
            else                                                    colorHandler.set_passive(false);
            event_register(EV_MODE_CHANGE, mode);
            if(mode != prevMode) {colorHandler.setAll(0, 0, 0); Serial.println("Se apaga la llus al cambiar de modo!!!!!!!!!!!!!!!!!!");}
            delay(10);
                                                                        #ifdef DEBUG
                                                                        Serial.println("OJITO, que passem a modo: " +String(element->get_currentMode()));
                                                                        #endif
            if(!BCframe){
                send_frame(frameMaker_RETURN_ELEM_SECTOR(globalID, BROADCAST, &mode, ELEM_CMODE_SECTOR));
                BCframe= false;
            }
            prevMode= mode;
            break;
        }

        case F_SEND_COMMAND:{
            byte testin= LEF.data[0];
            if     (testin == TEST_CMD) delay(1); 
            else if(testin == START_CMD){

                //colorHandler.fade_in_out(0x00, 0xFF, 0x20);
                colorHandler.setAll(0 , 0xFF, 0x20);
                delay(500);
                colorHandler.setAll(0 , 0, 0);
                event_register(EV_START, 0);
            
            }
            else if(testin == BLACKOUT){
                // APAGAR FASTLED
                colorHandler.setAll(0x00, 0x00, 0x00);
                event_register(EV_END, 0);
                delay(200);
            
                delay(2000);
                ESP.restart();
            } 
            break;
        }
        case F_SEND_COLOR: {
            byte color = LEF.data[0];
            numColorRec= color;
            colorReceived= true;
            #ifdef PLAYER
            if( (digitalRead(VUMETER_SEL_MODE_PIN_01)  == HIGH) &&
                (digitalRead(VUMETER_SEL_MODE_PIN_02)  == HIGH) &&
                (digitalRead(VUMETER_SEL_MODE_PIN_03)  == HIGH) &&
                (digitalRead(VUMETER_SEL_MODE_PIN_04)  == HIGH) &&
                (digitalRead(VUMETER_SEL_MODE_PIN_05)  == HIGH)
             ){
                 if(currentMode_ != LEDSTRIP_PASSIVE_MODE) doitPlayer.play_file(14, color + 11);
             }
            #endif
             
            event_register(EV_COLOR_CHANGE, color);
            CRGB colorin= colorHandler.get_CRGB_from_colorList(color);
            colorHandler.set_targetColor(colorin);
                                                            #ifdef DEBUG
                                                                Serial.println("Color recibido: " + String(color));
                                                            #endif
        if (currentMode_ == VUMETER_BASIC_MODE) {
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
        else if(currentMode_ == VUMETER_SLOW_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_FAST_MODE. o mix");
                                                            #endif
            
            colorHandler.set_targetFade(SLOWEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
            }
        else if(currentMode_ == VUMETER_MOTION_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_MOTION_MODE.");
                                                            #endif
            
            colorHandler.set_targetFade(NORMAL_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
            }
        else if(currentMode_ == VUMETER_MIX_MODE){
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo BASIC_MODE.");
                                                            #endif
            
            colorHandler.set_targetFade(FASTEST_FADE);
            colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
            colorHandler.transitionStartTime= millis();
            colorHandler.transitioning= true;
            break;
            }
        else if(currentMode_ == VUMETER_PASSIVE_MODE){

            if(colorHandler.get_is_paused()) colorHandler.set_is_paused(false);
            else                             colorHandler.set_is_paused(true);
                                                            #ifdef DEBUG
                                                                Serial.println("Manejando en modo COLUMN_PASSIVE_MODE.");
                                                            #endif
            break;
            }
        }
        case F_SEND_SENSOR_VALUE_1:{
                                                            #ifdef DEBUG
                                                            Serial.println("Se ha recibido un sensor value");
                                                            #endif
            if(currentMode_ == VUMETER_MOTION_MODE){
                byte value= get_brightness_from_sensorValue(LEF);
                colorHandler.set_targetFade(MOTION_VAL_FADE);
                colorHandler.set_targetBrightness(value);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == VUMETER_RB_MOTION_MODE){
                byte value= get_color_from_sensorValue(LEF); 
                CRGB colorin= colorHandler.get_CRGB_from_pasiveColorList(value);
                colorHandler.set_targetColor(colorin);
                colorHandler.set_targetFade(RB_MOTION_VAL_FADE);
                colorHandler.set_targetBrightness(MAX_BRIGHTNESS);
                colorHandler.transitionStartTime= millis();
                colorHandler.transitioning= true;
            }
            else if(currentMode_ == VUMETER_PATTERN_MODE){
                varaux= get_aux_var_01_from_sensorValue(LEF);
            }
            break;
        }

        case F_SEND_PATTERN_NUM:{
                byte numPattern= LEF.data[0];

                colorHandler.set_activePattern(numPattern);                                           
            break;
        }

        default: {
                                                                #ifdef DEBUG
                                                                    Serial.println(" LEDSTRIP Se ha recibido una función desconocida.");
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






void VUMETER_::update_vum() {

    switch(currentMode){


        case VUMETER_SIMON_GAME_MODE:
            //colorHandler.matrix_draw_circle(4, 5, 7, CRGB::White);
            colorHandler.simon_game(numColorRec);
            break;

        case VUMETER_SECUENCER_GAME_MODE:
            colorHandler.sequencer_game(numColorRec);
            break;

        case VUMETER_SPEAK_GAME_MODE:
            #ifdef MIC
                colorHandler.speak_game();
            #endif
            break;

        case VUMETER_BLOCK_SPEAK_MODE:{
            #ifdef MIC
                colorHandler.block_speak(colorHandler.targetColor);
                //colorHandler.BouncingBalls(0x90, 0xFF, 0xBB);
            #endif
            break;
        }
        case VUMETER_TONE_DETECT_MODE:
            #ifdef MIC

                //colorHandler.tone_color();
            #endif
            break;

        case VUMETER_METEOR_VOICE_MODE:
            #ifdef MIC
                colorHandler.voice_meteors();
            #endif
            break;

        default: break;
        }
}






