#include <Pulsadores_handler/Pulsadores_handler.h>
#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <botonera_DMS/botonera_DMS.h>
#include <encoder_handler/encoder_handler.h>

// Inicialización de pines y matriz de colores
int filas[FILAS] = {4, 5, 6, 7};
int columnas[COLUMNAS] = {1, 2, 3};
static bool lastState[FILAS][COLUMNAS];
bool relay_state = false;

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
    String currentFile = elementFiles[currentIndex];

    // Verificar si el elemento actual es "Ambientes"
    if (currentFile == "Ambientes") {
        Serial.println("Modo Ambientes detectado. Verificando si está seleccionado...");

        // Si "Ambientes" no está seleccionado, no se envía ninguna trama
        bool ambientesSeleccionado = false;
        for (size_t i = 0; i < elementFiles.size(); i++) {
            if (elementFiles[i] == "Ambientes" && selectedStates[i]) {
                ambientesSeleccionado = true;
                break;
            }
        }
        if (!ambientesSeleccionado) {
            Serial.println("⚠️ El elemento 'Ambientes' no está seleccionado. No se enviará ninguna trama.");
            return;
        }

        // Recorrer la lista de elementos en SPIFFS y agregar sus IDs si están seleccionados
        for (size_t i = 0; i < elementFiles.size(); i++) {
            if (selectedStates[i] && elementFiles[i] != "Apagar" && elementFiles[i] != "Fichas") {
                byte elementID = 0;

                // Leer la ID desde SPIFFS
                fs::File f = SPIFFS.open(elementFiles[i], "r");
                if (f) {
                    f.seek(OFFSET_ID, SeekSet);
                    f.read(&elementID, 1);
                    f.close();
                }

                // Agregar al target si la ID es válida
                if (elementID != 0) {
                    target.push_back(elementID);
                    Serial.printf("Elemento seleccionado agregado a la trama: ID %d\n", elementID);
                }
            }
        }
    } else {
        // Si no es "Ambientes", se comporta de manera normal (envío a un solo elemento seleccionado)
        if (!isCurrentElementSelected()) {
            Serial.println("El elemento actual no está seleccionado. No se enviará la trama.");
            return;
        }
        byte elementID = getCurrentElementID();
        target.push_back(elementID);
    }

    if (target.empty()) {
        Serial.println("⚠️ No hay elementos seleccionados para recibir la trama.");
        return;
    }

    // Obtener el nombre del color
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
            delay(5);
            break;
        default:
            Serial.println("Ningún botón presionado.");
            return;
    }

    if (color != RELAY) {
        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, color));
    }

    Serial.printf("🎨 Botón presionado y seleccionado: %s\n", colorNombre);
}


void PulsadoresHandler::limpiarEstados() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lastState[i][j] = false; 
        }
    }
}


