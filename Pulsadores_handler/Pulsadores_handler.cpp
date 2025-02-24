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

void PulsadoresHandler::limpiarEstados() {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            lastState[i][j] = false; 
        }
    }
}


// Función que recorre la matriz y procesa cada pulsador de forma independiente.
// Esta función debe llamarse desde el loop principal en cada iteración.
void PulsadoresHandler::procesarPulsadores() {
    // --- Construir la lista de destino (target) para botones de color ---
    // NOTA: Para el botón relé no es necesario, ya que se procesará de todas formas.
    std::vector<byte> target;
    String currentFile = elementFiles[currentIndex];
    
    if (currentFile == "Ambientes") {
        bool ambientesSeleccionado = false;
        for (size_t i = 0; i < elementFiles.size(); i++) {
            if (elementFiles[i] == "Ambientes" && selectedStates[i]) {
                ambientesSeleccionado = true;
                break;
            }
        }
        if (ambientesSeleccionado) {
            for (size_t i = 0; i < elementFiles.size(); i++) {
                if (selectedStates[i] && elementFiles[i] != "Apagar" && elementFiles[i] != "Fichas") {
                    byte elementID = 0;
                    fs::File f = SPIFFS.open(elementFiles[i], "r");
                    if (f) {
                        f.seek(OFFSET_ID, SeekSet);
                        f.read(&elementID, 1);
                        f.close();
                    }
                    if (elementID != 0) {
                        target.push_back(elementID);
                    }
                }
            }
        }
        // Si no hay elemento seleccionado en "Ambientes", target quedará vacío.
    }
    else {
        // Para otras pantallas, se añade el elemento solo si está seleccionado.
        if (isCurrentElementSelected()) {
            target.push_back(getCurrentElementID());
        }
    }
    // No retornamos si target está vacío, para seguir actualizando el estado del relé.

    // --- Leer la configuración actual (aplicable a todos los botones) ---
    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig)) {
        Serial.println("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }
    bool hasPulse   = getModeFlag(modeConfig, HAS_PULSE);    // Si activo: comportamiento "mantener"
    bool hasPassive = getModeFlag(modeConfig, HAS_PASSIVE);    // Si activo: solo se procesa el botón AZUL
    bool hasRelay   = getModeFlag(modeConfig, HAS_RELAY_1);      // Se atiende el botón RELAY solo si está activo

    // --- Variables estáticas para almacenar el estado anterior de cada pulsador ---
    static bool lastState[FILAS][COLUMNAS] = { { false } };

    // --- Variable para almacenar el estado actual del botón relé ---
    bool currentRelayState = false;
    
    // --- Escanear la matriz de pulsadores ---
    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW);
        delayMicroseconds(10);
        for (int j = 0; j < COLUMNAS; j++) {
            bool currentPressed = (digitalRead(columnas[j]) == LOW);
            
            // Si este botón es el RELAY (por ejemplo, si pulsadorColor[i][j] == RELAY)
            if (pulsadorColor[i][j] == RELAY) {
                currentRelayState |= currentPressed;
            }
            
            // Detectar eventos: PRESIÓN y LIBERACIÓN
            if (!lastState[i][j] && currentPressed) {
                processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay, target);
            }
            if (lastState[i][j] && !currentPressed) {
                processButtonEvent(i, j, BUTTON_RELEASED, hasPulse, hasPassive, hasRelay, target);
            }
            lastState[i][j] = currentPressed;
        }
        digitalWrite(filas[i], HIGH);
    }
    
    // Actualizar el estado del botón relé (siempre, independientemente de la selección)
    relayButtonPressed = currentRelayState;
}

// Función auxiliar para procesar el evento de un botón en la posición (i, j)
void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           std::vector<byte> &target)
{
    byte buttonColor = pulsadorColor[i][j];

    // 1. Procesar el botón RELAY (botón 9)
    if (buttonColor == RELAY)
    {
        if (!hasRelay)
            return; // Si el flag de relé no está activo, se ignora.

        // Agregamos: si el encoder está presionado, NO enviamos comando (para evitar el flagByte en combinación)
        if (digitalRead(ENC_BUTTON) == LOW)
            return;

        if (event == BUTTON_PRESSED)
        {
            relayButtonPressed = true;
            if (hasPulse)
            {
                // Modo PULSE para RELAY: presionar → relay_state true
                relay_state = true;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.println("RELAY PULSE: PRESIÓN, enviando relay_state = TRUE");
#endif
            }
            else
            {
                // Modo BÁSICO: toggle al presionar
                relay_state = !relay_state;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.printf("RELAY BÁSICO: PRESIÓN, toggle relay_state a %d\n", relay_state);
#endif
            }
        }
        else if (event == BUTTON_RELEASED)
        {
            relayButtonPressed = false;
            if (hasPulse)
            {
                // En modo PULSE, al soltar → relay_state false
                relay_state = false;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.println("RELAY PULSE: LIBERACIÓN, enviando relay_state = FALSE");
#endif
            }
            // En modo básico no se envía nada al liberar.
        }
        return;
    }

    // 2. Si HAS_PASSIVE está activo, solo procesar el botón AZUL (para botones de color)
    if (hasPassive && (buttonColor != BLUE))
        return;

    // 3. Procesar botones de color (no RELAY)
    // Para estos botones, enviamos comandos solo si hay un elemento seleccionado (target no vacío)
    if (target.empty())
        return; // No se envían comandos si no hay elemento seleccionado
    if (event == BUTTON_PRESSED)
    {
        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
#ifdef DEBUG
        Serial.printf("COLOR: PRESIÓN, enviando color %d\n", buttonColor);
#endif
    }
    else if (event == BUTTON_RELEASED)
    {
        if (hasPulse)
        {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
#ifdef DEBUG
            Serial.println("COLOR PULSE: LIBERACIÓN, enviando Negro");
#endif
        }
    }
}
