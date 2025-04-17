#include <Pulsadores_handler/Pulsadores_handler.h>
#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <botonera_DMS/botonera_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <display_handler/display_handler.h>

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

bool PulsadoresHandler::isButtonPressed(byte color) {
    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW); // Activamos la fila
        delayMicroseconds(1); // Pequeña pausa para asegurar estabilidad en la lectura
        
        for (int j = 0; j < COLUMNAS; j++) {
            if (pulsadorColor[i][j] == color && digitalRead(columnas[j]) == LOW) {
                digitalWrite(filas[i], HIGH); // Restauramos la fila antes de salir
                return true; // Se encontró el botón presionado
            }
        }
        
        digitalWrite(filas[i], HIGH); // Restauramos la fila antes de continuar
    }
    return false; // Ningún botón con ese color está presionado
}

void PulsadoresHandler::procesarPulsadores() {
    // --- Construir el vector de destino (target) ---
    std::vector<byte> target;
    String currentFile = elementFiles[currentIndex];
    bool isFichas = (currentFile == "Fichas");

    if (!isFichas) {
        if (currentFile == "Ambientes") {
            bool ambientesSeleccionado = false;
            for (size_t i = 0; i < elementFiles.size(); i++) {
                if (elementFiles[i] == "Ambientes" && selectedStates[i]) {
                    ambientesSeleccionado = true;
                    break;
                }
            }
            if (ambientesSeleccionado) {
                target.push_back(BROADCAST);
            }
        } else {
            // Elemento dinámico (ni Fichas, ni Ambientes, ni Apagar)
            byte elementID = BROADCAST;
            if (currentFile == "Apagar") {
                elementID = apagarSala.ID;
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_ID, SeekSet);
                    f.read(&elementID, 1);
                    f.close();
                } else {
                    Serial.printf("❌ Error al leer la ID del archivo %s\n", currentFile.c_str());
                }
            }
            target.push_back(elementID);
        }
    }
    // --- Fin construcción target ---

    // --- Leer configuración del modo actual ---
    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig)) {
        Serial.println("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }
    bool hasPulse    = getModeFlag(modeConfig, HAS_PULSE);
    bool hasPassive  = getModeFlag(modeConfig, HAS_PASSIVE);
    bool hasRelay    = getModeFlag(modeConfig, HAS_RELAY);
    bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);

    // Si se tiene HAS_ADVANCED_COLOR y HAS_PULSE pero NO el modo alternativo activo, se ignora el PULSE
    if (hasAdvanced && hasPulse && !modeAlternateActive) {
        hasPulse = false;
    }

    // --- Variables estáticas para el escaneo de pulsadores ---
    static bool lastState[FILAS][COLUMNAS] = { { false } };
    static unsigned long pressTime[FILAS][COLUMNAS] = { { 0 } };
    static byte currentActiveColor = BLACK; // Color enviado en la última trama
    static bool blackSent = false;            // Para evitar reenvío redundante de BLACK
    static bool mixReady = true;              // Nueva variable: true cuando se han soltado botones (count<2)

    // --- Estados especiales para botones RELAY y BLUE ---
    bool currentRelayState = false;
    bool blueButtonState = false;
    static int lastModeIndex = -1;
    if (currentModeIndex != lastModeIndex) {
        for (int i = 0; i < FILAS; i++) {
            for (int j = 0; j < COLUMNAS; j++) {
                pressTime[i][j] = 0;
            }
        }
        currentActiveColor = BLACK;
        blackSent = false;
        mixReady = true; // Reiniciamos la bandera mixReady al cambiar de modo
        lastModeIndex = currentModeIndex;
    }

    // --- Escaneo de la matriz ---
    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW);
        delayMicroseconds(10);
        for (int j = 0; j < COLUMNAS; j++) {
            bool currentPressed = (digitalRead(columnas[j]) == LOW);

            // Actualiza el tiempo de pulsación (para botones que no son RELAY)
            if (pulsadorColor[i][j] != RELAY) {
                if (!lastState[i][j] && currentPressed) {
                    pressTime[i][j] = millis();
                } else if (lastState[i][j] && !currentPressed) {
                    pressTime[i][j] = 0;
                }
            }

            // Actualiza estados para botones especiales
            if (pulsadorColor[i][j] == RELAY) {
                currentRelayState |= currentPressed;
            }
            if (pulsadorColor[i][j] == BLUE) {
                blueButtonState |= currentPressed;
            }

            // Detecta eventos de pulsación y liberación
            if (!lastState[i][j] && currentPressed) {
                if (isFichas) {
                    if (pulsadorColor[i][j] == RELAY)
                        relayButtonPressed = true;
                    else if (pulsadorColor[i][j] == BLUE)
                        blueButtonPressed = true;
                } else {
                    processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay, target);
                }
            }
            if (lastState[i][j] && !currentPressed) {
                if (isFichas) {
                    if (pulsadorColor[i][j] == RELAY)
                        relayButtonPressed = false;
                    else if (pulsadorColor[i][j] == BLUE)
                        blueButtonPressed = false;
                } else {
                    processButtonEvent(i, j, BUTTON_RELEASED, hasPulse, hasPassive, hasRelay, target);
                }
            }
            lastState[i][j] = currentPressed;
        }
        digitalWrite(filas[i], HIGH);
    }
    relayButtonPressed = currentRelayState;
    blueButtonPressed = blueButtonState;

    // --- Lógica para enviar trama en modo PULSE ---
    if (!inModesScreen && hasPulse && !isFichas) {
        if (hasAdvanced) {
            // Modo ADVANCED + PULSE: se cuentan hasta 2 botones no RELAY pulsados
            int count = 0;
            byte color1 = BLACK, color2 = BLACK;
            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (pulsadorColor[i][j] == RELAY) continue;
                    if (pressTime[i][j] > 0) {
                        count++;
                        if (count == 1) {
                            color1 = pulsadorColor[i][j];
                        } else if (count == 2) {
                            color2 = pulsadorColor[i][j];
                        }
                    }
                }
            }
            if (count < 2) {
                // Si hay uno o ningún botón pulsado, se reinicia el estado de la mezcla
                mixReady = true;
                if (count == 0) {
                    if (!blackSent) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
#ifdef DEBUG
                        Serial.println("ADVANCED PULSE: Ningún botón activo, enviando Negro.");
#endif
                        currentActiveColor = BLACK;
                        blackSent = true;
                    }
                } else if (count == 1) {
                    if (currentActiveColor != color1) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, color1));
#ifdef DEBUG
                        Serial.printf("ADVANCED PULSE: Un botón activo, enviando color %d.\n", color1);
#endif
                        currentActiveColor = color1;
                        blackSent = false;
                    }
                }
            } else if (count == 2) {
                // Si hay dos botones pulsados y mixReady está activado (es decir, es la primera vez en esta combinación)
                if (mixReady) {
                    byte mixColor;
                    if (colorHandler.color_mix_handler(color1, color2, &mixColor)) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, mixColor));
#ifdef DEBUG
                        Serial.printf("ADVANCED PULSE: Dos botones activos, enviando mezcla %d.\n", mixColor);
#endif
                        currentActiveColor = mixColor;
                        blackSent = false;
                        mixReady = false;  // Marcar que ya se envió la mezcla para esta combinación
                    } else {
                        if (!blackSent) {
                            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
#ifdef DEBUG
                            Serial.println("ADVANCED PULSE: Combinación no definida, enviando Negro.");
#endif
                            currentActiveColor = BLACK;
                            blackSent = true;
                            mixReady = false;
                        }
                    }
                }
                // Si mixReady es false, se evita el reenvío continuo.
            }
        } else {
            // Lógica para modo PULSE sin advanced (igual que en el código original)
            unsigned long maxTime = 0;
            byte newActiveColor = BLACK;
            bool activeColorValid = false;
            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (pulsadorColor[i][j] == RELAY) continue;
                    if (pressTime[i][j] > maxTime) {
                        maxTime = pressTime[i][j];
                        newActiveColor = pulsadorColor[i][j];
                        activeColorValid = true;
                    }
                }
            }
            if (activeColorValid && currentActiveColor != newActiveColor) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, newActiveColor));
                currentActiveColor = newActiveColor;
                blackSent = false;
            } else if (!activeColorValid && !blackSent) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                currentActiveColor = BLACK;
                blackSent = true;
            }
        }
    }
}


// Función auxiliar para procesar el evento de un botón en la posición (i, j)
void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           std::vector<byte> &target)
{
    byte buttonColor = pulsadorColor[i][j];


    // Verificar si estamos en "Fichas"
    String currentFile = elementFiles[currentIndex];
    if (currentFile == "Fichas") {
        // No hacemos nada aquí, ya que manejamos "Fichas" directamente en procesarPulsadores()
        return;
    }

    if (currentFile == "Ambientes") {
        if (event == BUTTON_PRESSED){
            if (isCurrentElementSelected()) {
                byte buttonColor = pulsadorColor[i][j];
                if (buttonColor == RELAY) buttonColor = BLACK;
                
                // 🎯 Tramas especiales
                send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, target, buttonColor));
                delay(100);

                // 🔊 Reproducción de audio
                if (buttonColor != BLACK) {
                    //  if (buttonColor == BEACH || buttonColor == OCEAN) doitPlayer.play_file(98, buttonColor);
                    //  else doitPlayer.play_file(99, 8);
                     doitPlayer.play_file(8, buttonColor +1);
                }else doitPlayer.stop_file();

                return; // ⚠️ Evita seguir procesando la pulsación con lógica normal
            }
        }
    }

    // 1. Procesar el botón RELAY (botón especial)
    if (buttonColor == RELAY)
    {
        if (!hasRelay)
            return;
        if (digitalRead(ENC_BUTTON) == LOW)
            return;
        if (event == BUTTON_PRESSED)
        {
            relayButtonPressed = true;
            if (hasPulse)
            {
                relay_state = true;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.println("RELAY PULSE: PRESIÓN, enviando relay_state = TRUE");
#endif
            }
            else
            {
                if (currentFile != "Ambientes" && currentFile != "Fichas" && currentFile != "Apagar"){
                relay_state = !relay_state;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.printf("RELAY BÁSICO: PRESIÓN, toggle relay_state a %d\n", relay_state);
#endif
                }
            }
        }
        else if (event == BUTTON_RELEASED)
        {
            relayButtonPressed = false;
            if (hasPulse)
            {
                relay_state = false;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
#ifdef DEBUG
                Serial.println("RELAY PULSE: LIBERACIÓN, enviando relay_state = FALSE");
#endif
            }
        }
        return;
    }

    // 2. Si HAS_PASSIVE está activo, procesar únicamente el botón AZUL.
    if (hasPassive && (buttonColor != BLUE))
        return;

    if (target.empty())
        return;

    // --- Nuevo: Comprobar el flag HAS_PATTERNS ---
    // Se obtiene el modo actual para poder leer este flag
    currentFile = elementFiles[currentIndex];
    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig))
    {
        Serial.println("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }
    bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
    bool hasPatterns = getModeFlag(modeConfig, HAS_PATTERNS);
    // Si el modo tiene HAS_PATTERNS, cada pulsación envía la trama de patrón y se retorna.
    if (hasPatterns && event == BUTTON_PRESSED)
    {
        send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, target, buttonColor));
        return;
    }
    // --- Fin nuevo flag HAS_PATTERNS ---

    // --- Rama para modo ADVANCED sin pulso ---
    if (hasAdvanced && !hasPulse)
    {
        static int lastModeForAdvanced = -1;
        if (currentModeIndex != lastModeForAdvanced)
        {
            lastBasicColor = BLACK;
            static bool advancedMixed = false;
            advancedMixed = false;
            lastModeForAdvanced = currentModeIndex;
#ifdef DEBUG
            Serial.println("ADVANCED NON-PULSE: Cambio de modo detectado. Reiniciando estado avanzado.");
#endif
        }
        if (event == BUTTON_PRESSED)
        {
            static bool advancedMixed = false; // Variable para saber si ya se envió mezcla
            if (advancedMixed)
            {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
#ifdef DEBUG
                Serial.println("ADVANCED NON-PULSE: Nueva pulsación tras mezcla, enviando Negro.");
#endif
                lastBasicColor = BLACK;
                advancedMixed = false;
            }
            else
            {
                if (lastBasicColor == BLACK)
                {
                    lastBasicColor = buttonColor;
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
#ifdef DEBUG
                    Serial.printf("ADVANCED NON-PULSE: Primer botón, enviando color %d\n", buttonColor);
#endif
                }
                else
                {
                    byte mixColor;
                    if (colorHandler.color_mix_handler(lastBasicColor, buttonColor, &mixColor))
                    {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, mixColor));
#ifdef DEBUG
                        Serial.printf("ADVANCED NON-PULSE: Enviando color mezcla %d\n", mixColor);
#endif
                        lastBasicColor = mixColor;
                        advancedMixed = true;
                    }
                    else
                    {
#ifdef DEBUG
                        Serial.println("ADVANCED NON-PULSE: Combinación no definida, enviando Negro.");
#endif
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                        lastBasicColor = BLACK;
                        advancedMixed = false;
                    }
                }
            }
        }
        // En BUTTON_RELEASED no se hace nada.
        return;
    }
    // --- Fin rama ADVANCED sin pulso ---

    // --- Para modo PULSE (con o sin advanced) ---
    if (hasPulse)
    {
#ifdef DEBUG
        Serial.println("COLOR: Evento en modo PULSE, envío diferido al escaneo.");
#endif
        return;
    }
    else
    {
        // Modo BÁSICO (sin advanced ni pulso)
        if (event == BUTTON_PRESSED)
        {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
#ifdef DEBUG
            Serial.printf("COLOR BÁSICO: PRESIÓN, enviando color %d\n", buttonColor);
#endif
        }
        else if (event == BUTTON_RELEASED)
        {
#ifdef DEBUG
            Serial.println("COLOR BÁSICO: LIBERACIÓN, sin acción.");
#endif
        }
    }
}
