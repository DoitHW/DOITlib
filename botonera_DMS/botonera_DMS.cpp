#include <botonera_DMS/botonera_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <encoder_handler/encoder_handler.h>
#include <display_handler/display_handler.h>




BOTONERA_::BOTONERA_(uint16_t serialNumber) : ELEMENT_(serialNumber) {
            set_type(TYPE_BOTONERA);
        }

void BOTONERA_::botonera_begin(){

    
    colorHandler.begin(NUM_LEDS);           
}


void BOTONERA_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    if (!element) {
                                                            #ifdef DEBUG
                                                                Serial.println("Error: 'element' no está inicializado.");
                                                            #endif
        return;
    }

    // Depuración del estado de la pila
    Serial.println("Inicio de RX_main_handler");
    UBaseType_t stackSize = uxTaskGetStackHighWaterMark(NULL);
                                                            #ifdef DEBUG
                                                                Serial.println("Stack restante: " + String(stackSize));
                                                            #endif

    byte currentMode_ = element->get_currentMode();

    switch (LEF.function) {

        case F_SET_ELEM_MODE:{
          
            break;
        }
        case F_SEND_COLOR: {
            
            
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

