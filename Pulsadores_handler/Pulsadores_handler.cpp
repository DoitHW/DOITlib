#include <Pulsadores_handler/Pulsadores_handler.h>
#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <botonera_DMS/botonera_DMS.h>

// Inicialización de pines y matriz de colores
int filas[FILAS] = {4, 5, 6, 7};
int columnas[COLUMNAS] = {1, 2, 3};
static bool lastState[FILAS][COLUMNAS];

byte pulsadorColor[FILAS][COLUMNAS] = {
    {ORANGE, GREEN, WHITE},
    {BLUE, RELAY, RED},
    {LIGHT_BLUE, YELLOW, VIOLET},
    {BLACK, BLACK, BLACK} // Relleno para futuras expansiones
};

// Constructor
PulsadoresHandler::PulsadoresHandler() {}

// Inicialización de pines
void PulsadoresHandler::begin() {
    for (int i = 0; i < FILAS; i++) {
        pinMode(filas[i], OUTPUT);
        digitalWrite(filas[i], HIGH);
    }
    for (int j = 0; j < COLUMNAS; j++) {
        pinMode(columnas[j], INPUT_PULLUP);
    }
    
}

// Lee la matriz y devuelve el color del pulsador presionado una sola vez al presionar
byte PulsadoresHandler::leerPulsador() {
    lastState[FILAS][COLUMNAS] = {false};  // Estado anterior del botón

    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW); // Activar fila actual
        delayMicroseconds(10);

        for (int j = 0; j < COLUMNAS; j++) {
            bool currentState = (digitalRead(columnas[j]) == LOW);  // Detectar botón presionado

            // Detectar flanco de bajada (presión inicial)
            if (currentState && !lastState[i][j]) { 
                lastState[i][j] = true; // Actualizar estado
                digitalWrite(filas[i], HIGH); // Desactivar fila antes de salir
                return pulsadorColor[i][j];  // Retornar el color del pulsador presionado
            } 
            else if (!currentState) {
                lastState[i][j] = false;  // Resetear cuando se suelta el botón
            }
        }

        digitalWrite(filas[i], HIGH); // Desactivar fila actual
    }

    return 0xFF; // Retornar un valor inválido si no se presionó ningún botón
}


// Muestra información sobre el pulsador presionado
void PulsadoresHandler::mostrarColor(byte color) {
    std::vector<byte> target;
    
    // Comprobar si el elemento está seleccionado antes de enviar la trama
    if (!isCurrentElementSelected()) {
        Serial.println("El elemento actual no está seleccionado. No se enviará la trama.");
        return;
    }

    byte elementID = getCurrentElementID();  // Obtener la ID solo si está seleccionado
    target.push_back(elementID);  

    static bool relay_state = false;
    const char* colorNombre = "";

    switch (color) {
        case WHITE: colorNombre = "Blanco"; break;
        case YELLOW: colorNombre = "Amarillo"; break;
        case ORANGE: colorNombre = "Naranja"; break;
        case RED: colorNombre = "Rojo"; break;
        case VIOLET: colorNombre = "Violeta"; break;
        case BLUE: colorNombre = "Azul"; break;
        case LIGHT_BLUE: colorNombre = "Azul Claro"; break;
        case GREEN: colorNombre = "Verde"; break;
        case BLACK: colorNombre = "Negro"; break;
        case RELAY:
            colorNombre = "Relay";
            relay_state = !relay_state;
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
            send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, 0xFF, SPANISH_LANG, ELEM_SERIAL_SECTOR));
            break;
        default:
            Serial.println("Ningún botón presionado.");
            return;
    }

    if (color != RELAY) {
        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, color));
    }

    Serial.print("Botón presionado y seleccionado: ");
    Serial.println(colorNombre);
}

void PulsadoresHandler::limpiarEstados() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lastState[i][j] = false; 
        }
    }
}


