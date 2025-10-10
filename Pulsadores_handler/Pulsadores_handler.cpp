#include <Pulsadores_handler/Pulsadores_handler.h>
#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <botonera_DMS/botonera_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <display_handler/display_handler.h>
#include <RelayManager_DMS/RelayStateManager.h>

// Inicialización de pines y matriz de colores
#if defined(BOTONERA_NEW)
/* === BOTONERAS NUEVAS === */
int filas[FILAS]       = {5, 4, 6, 7};
int columnas[COLUMNAS] = {3, 2, 1};
static bool lastState[FILAS][COLUMNAS];
byte relay_state = false;

byte pulsadorColor[FILAS][COLUMNAS] = {
    {ORANGE, GREEN,  WHITE},
    {BLUE,   RELAY,  RED},
    {VIOLET, YELLOW, LIGHT_BLUE},
    {BLACK,  BLACK,  BLACK} // Relleno
};

#elif defined(BOTONERA_OLD)
/* === BOTONERAS ANTIGUAS === */
int filas[FILAS]       = {4, 5, 6, 7};
int columnas[COLUMNAS] = {1, 2, 3};
static bool lastState[FILAS][COLUMNAS];
byte relay_state = false;

byte pulsadorColor[FILAS][COLUMNAS] = {
    {ORANGE,     GREEN,  WHITE},
    {BLUE,       RELAY,  RED},
    {LIGHT_BLUE, YELLOW, VIOLET},
    {BLACK,      BLACK,  BLACK} // Relleno
};
#endif

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
    
    auto isNSZero = [](const TARGETNS& ns) -> bool {
        return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
    };
    // Target por defecto: BROADCAST
    uint8_t  targetType = 0xFF;   // BROADCAST
    TARGETNS targetNS   = NS_ZERO;

    String currentFile = elementFiles[currentIndex];

    /*───────────────────────────
      1) MODO COGNITIVO (Consola)
    ───────────────────────────*/
    if (inCognitiveMenu) {
        static bool lastState[FILAS][COLUMNAS] = { { false } };

        for (int i = 0; i < FILAS; i++) {
            digitalWrite(filas[i], LOW);
            delayMicroseconds(10);
            for (int j = 0; j < COLUMNAS; j++) {
                byte color = pulsadorColor[i][j];
                if (color != BLUE && color != GREEN && color != YELLOW &&
                    color != RED  && color != RELAY) {
                    continue;
                }

                bool currentPressed = (digitalRead(columnas[j]) == LOW);

                // Objetivo: CONSOLA
                targetType = DEFAULT_CONSOLE; // 0xDC
                targetNS   = NS_ZERO;         // NS cero para no-dispositivo

                if (!lastState[i][j] && currentPressed) {
                    processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
                                       targetType, targetNS);
                }
                if (lastState[i][j] && !currentPressed) {
                    processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
                                       targetType, targetNS);
                }
                lastState[i][j] = currentPressed;
            }
            digitalWrite(filas[i], HIGH);
        }
        return;  // salir tras gestionar cognitivo
    }

    /*───────────────────────────
      2) Filtros de contexto
    ───────────────────────────*/
    if (currentFile == "Apagar") return;

    // Ignorar pulsaciones si es elemento normal no seleccionado
    if (currentFile != "Ambientes" && currentFile != "Fichas" &&
        !selectedStates[currentIndex]) return;

    const bool isFichas = (currentFile == "Fichas");

    /*───────────────────────────
      3) Resolución de destino (target)
    ───────────────────────────*/
        if (!isFichas) {
        if (currentFile == "Ambientes") {
            bool ambientesSeleccionado = false;
            for (size_t i = 0; i < elementFiles.size(); ++i) {
                if (elementFiles[i] == "Ambientes" && selectedStates[i]) {
                    ambientesSeleccionado = true;
                    break;
                }
            }
            if (ambientesSeleccionado) {
                // Objetivo: BROADCAST
                targetType = BROADCAST;  // 0xFF
                targetNS   = NS_ZERO;
            }
        } else if (currentFile == "Comunicador") {
            extern TARGETNS communicatorActiveNS;
            if (isNSZero(communicatorActiveNS)) {
                // Comunicador en modo broadcast
                targetType = BROADCAST;
                targetNS   = NS_ZERO;
            } else {
                // Comunicador apuntando a un dispositivo concreto
                targetType = DEFAULT_DEVICE;  // 0xDD
                targetNS   = communicatorActiveNS;
            }
        } else {
            targetType = DEFAULT_DEVICE;
            targetNS   = getCurrentElementNS();;
        }
    }


    /*───────────────────────────
      4) Flags del modo actual
    ───────────────────────────*/
    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig)) {
        DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }

    const bool hasPulse    = getModeFlag(modeConfig, HAS_PULSE);
    const bool hasPassive  = getModeFlag(modeConfig, HAS_PASSIVE);
    const bool hasRelay    = getModeFlag(modeConfig, HAS_RELAY);
    const bool hasRelayN1  = getModeFlag(modeConfig, HAS_RELAY_N1);
    const bool hasRelayN2  = getModeFlag(modeConfig, HAS_RELAY_N2);
    const bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);

    /*───────────────────────────
      5) Estado por-botón
    ───────────────────────────*/
    static bool         lastState[FILAS][COLUMNAS]       = { { false } };
    static unsigned long pressTime[FILAS][COLUMNAS]      = { { 0 } };
    static byte         currentActiveColor               = BLACK;
    static bool         blackSent                        = false;
    static bool         mixReady                         = true;

    bool        currentRelayState = false;
    bool        blueButtonState   = false;
    static int  lastModeIndex     = -1;

    // Reset al cambiar de modo visible
    if (currentModeIndex != lastModeIndex) {
        for (int i = 0; i < FILAS; i++)
            for (int j = 0; j < COLUMNAS; j++)
                pressTime[i][j] = 0;

        currentActiveColor = BLACK;
        blackSent          = false;
        mixReady           = true;
        lastModeIndex      = currentModeIndex;
    }

    /*───────────────────────────
      6) Escaneo de matriz y eventos
    ───────────────────────────*/
    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW);
        delayMicroseconds(10);
        for (int j = 0; j < COLUMNAS; j++) {
            bool currentPressed = (digitalRead(columnas[j]) == LOW);
            byte color         = pulsadorColor[i][j];

            if (color != RELAY) {
                if (!lastState[i][j] && currentPressed)      pressTime[i][j] = millis();
                else if (lastState[i][j] && !currentPressed) pressTime[i][j] = 0;
            }

            if (color == RELAY) currentRelayState |= currentPressed;
            if (color == BLUE)  blueButtonState   |= currentPressed;

            if (!lastState[i][j] && currentPressed) {
                if (isFichas) {
                    if (color == RELAY)      relayButtonPressed = true;
                    else if (color == BLUE)  blueButtonPressed  = true;
                } else {
                    processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay,
                                       targetType, targetNS);
                }
            }
            if (lastState[i][j] && !currentPressed) {
                if (isFichas) {
                    if (color == RELAY)      relayButtonPressed = false;
                    else if (color == BLUE)  blueButtonPressed  = false;
                } else {
                    processButtonEvent(i, j, BUTTON_RELEASED, hasPulse, hasPassive, hasRelay,
                                       targetType, targetNS);
                }
            }
            lastState[i][j] = currentPressed;
        }
        digitalWrite(filas[i], HIGH);
    }

    relayButtonPressed = currentRelayState;
    blueButtonPressed  = blueButtonState;

    /*───────────────────────────
      7) Envío de color (modes con HAS_PULSE)
    ───────────────────────────*/
    if (!inModesScreen && hasPulse && !isFichas) {
        if (hasAdvanced) {
            int  count   = 0;
            byte color1  = BLACK;
            byte color2  = BLACK;

            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (pulsadorColor[i][j] == RELAY) continue;
                    if (pressTime[i][j] > 0) {
                        ++count;
                        if (count == 1)      color1 = pulsadorColor[i][j];
                        else if (count == 2) color2 = pulsadorColor[i][j];
                    }
                }
            }

            if (count < 2) {
                mixReady = true;
                if (count == 0) {
                    if (!blackSent) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                        currentActiveColor = BLACK;
                        blackSent          = true;
                    }
                } else if (count == 1 && currentActiveColor != color1) {
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, color1));
                    currentActiveColor = color1;
                    blackSent          = false;
                }
            } else if (count == 2 && mixReady) {
                byte mixColor;
                if (colorHandler.color_mix_handler(color1, color2, &mixColor)) {
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, mixColor));
                    currentActiveColor = mixColor;
                    blackSent          = false;
                    mixReady           = false;
                } else {
                    if (!blackSent) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                        currentActiveColor = BLACK;
                        blackSent          = true;
                        mixReady           = false;
                    }
                }
            }
        } else {
            unsigned long maxTime        = 0;
            byte          newActiveColor = BLACK;
            bool          activeValid    = false;

            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (pulsadorColor[i][j] == RELAY) continue;
                    if (pressTime[i][j] > maxTime) {
                        maxTime        = pressTime[i][j];
                        newActiveColor = pulsadorColor[i][j];
                        activeValid    = true;
                    }
                }
            }

            if (activeValid && currentActiveColor != newActiveColor) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, newActiveColor));
                currentActiveColor = newActiveColor;
                blackSent          = false;
            } else if (!activeValid && !blackSent) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                currentActiveColor = BLACK;
                blackSent          = true;
            }
        }
    }
}

std::vector<uint8_t> idsSPIFFS;   // IDs ordenadas               (solo Comunicador)
int  relayStep = -1;              // -1 = BROADCAST encendido
TARGETNS communicatorActiveNS = NS_ZERO;   // 0xFF = broadcast

void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           uint8_t targetType, const TARGETNS& targetNS)
{
    const byte buttonColor = pulsadorColor[i][j];
    const String currentFile = elementFiles[currentIndex];

    static bool     lastWasComunicador = false;
    static uint8_t  lastLegacyColor    = 0xFF; // sentinel (ningún color válido)
    static bool     sameColorParity    = false;

    auto isNSZero = [](const TARGETNS& ns) -> bool {
        return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
    };

    // ──────────────────────────────────────
    // 0) Menú Cognitivo → siempre a CONSOLA
    //    (esperamos targetType=DEFAULT_CONSOLE, targetNS=NS_ZERO)
    // ──────────────────────────────────────
    if (inCognitiveMenu) {
        if (event == BUTTON_PRESSED) {
            if (buttonColor != RELAY) {
                // COLOR hacia la consola
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_CONSOLE, NS_ZERO, buttonColor));
            }
        } else if (event == BUTTON_RELEASED && buttonColor == RELAY) {
            // RELAY → flag 1 (no toggle) hacia la consola
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_CONSOLE, NS_ZERO, 0x01));
        }
        return;
    }

    // 1) Fichas no se procesan aquí
    if (currentFile == "Fichas")
        return;

    // ─────────────────────────────────────────────────────────────
    // 2) COMUNICADOR · elemento sólo-relé → filtra botones
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Comunicador") {
        extern TARGETNS communicatorActiveNS;  // NS del elemento apuntado por el comunicador

        // ¿apunta a un dispositivo real?
        const bool communicatorHasTarget = !isNSZero(communicatorActiveNS);

        // ¿ese dispositivo es “solo-relé”?
        bool soloRele = false;
        if (communicatorHasTarget) {
            uint8_t cfg[2] = {0};
            if (RelayStateManager::getModeConfigForNS(communicatorActiveNS, cfg)) {
                const bool hasCol = getModeFlag(cfg, HAS_BASIC_COLOR) || getModeFlag(cfg, HAS_ADVANCED_COLOR);
                const bool hasRel = getModeFlag(cfg, HAS_RELAY);
                soloRele = (!hasCol && hasRel);
            }
        }

        if (soloRele) {
            // En solo-relé, sólo aceptamos AZUL y RELAY
            if (buttonColor != BLUE && buttonColor != RELAY) return;

            // Botón AZUL → toggle ON/OFF del relé en el NS del comunicador
            if (buttonColor == BLUE && event == BUTTON_PRESSED && communicatorHasTarget) {
                const bool cur  = RelayStateManager::get(communicatorActiveNS);
                const bool next = !cur;
                send_frame(frameMaker_SEND_FLAG_BYTE(
                    DEFAULT_BOTONERA, DEFAULT_DEVICE, communicatorActiveNS, next ? 0x01 : 0x00
                ));
                RelayStateManager::set(communicatorActiveNS, next);
                return;
            }
        }
    }

    // ─────────────────────────────────────────────────────────────
    // 3) AMBIENTES (broadcast patterns cuando está seleccionado)
    //    (esperamos targetType=BROADCAST, targetNS=NS_ZERO al venir del caller)
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Ambientes") {
        if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
            const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;

            // Mandamos patrón a broadcast
            send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
            delay(100);

            if (sendColorByte != BLACK) doitPlayer.play_file(8, sendColorByte);
            else                         doitPlayer.stop_file();
            return;
        }
    }

    // ─────────────────────────────────────────────────────────────
    // 4) Cargar configuración del modo del elemento visible
    // ─────────────────────────────────────────────────────────────
    uint8_t modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig)) {
        #ifdef DEBUG
        DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual.");
        #endif
        return;
    }

    const bool hasBasic      = getModeFlag(modeConfig, HAS_BASIC_COLOR);
    const bool hasAdvanced   = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
    const bool hasPatterns   = getModeFlag(modeConfig, HAS_PATTERNS);
    const bool hasRelayN1    = getModeFlag(modeConfig, HAS_RELAY_N1);
    const bool hasRelayN2    = getModeFlag(modeConfig, HAS_RELAY_N2);
    const bool isMultiRelay  = (hasRelayN1 || hasRelayN2);
    const bool isAromaterapia= (!hasRelay && hasRelayN1 && hasRelayN2);

    // Si es multi-relé/aromaterapia, sólo valen botones de color (BLUE/GREEN/YELLOW/RED)
    if ((isAromaterapia || isMultiRelay) &&
        buttonColor != BLUE && buttonColor != GREEN &&
        buttonColor != YELLOW && buttonColor != RED) {
        return;
    }

    int relayIndex = -1;
    if      (buttonColor == BLUE)   relayIndex = 0;
    else if (buttonColor == GREEN)  relayIndex = 1;
    else if (buttonColor == YELLOW) relayIndex = 2;
    else if (buttonColor == RED)    relayIndex = 3;

    // ─────────────────────────────────────────────────────────────
    // 5) MULTIRRELÉ (botones de color)
    //    (para dispositivos → targetType debe ser DEFAULT_DEVICE y targetNS válido)
    // ─────────────────────────────────────────────────────────────
    if ((isMultiRelay || isAromaterapia) && (relayIndex >= 0 && relayIndex <= 3)) {
        if (event == BUTTON_PRESSED) {
            uint8_t mask = (1u << relayIndex);

            if (isAromaterapia) {
                // Exclusivo
                relay_state = (relay_state == mask) ? 0x00 : mask;
            } else {
                // Independiente (toggle)
                relay_state ^= mask;
            }

            // 1º feedback de color al mismo target
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
            delay(20);
            // 2º flags compuestos
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, relay_state));

            #ifdef DEBUG
            DEBUG__________printf("[RELAY %s] Botón color activado - Index: %d → Estado: 0x%02X\n",
                                  isAromaterapia ? "AROMATERAPIA" : "MULTI",
                                  relayIndex, relay_state);
            #endif
        }
        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 6) COMUNICADOR · RELÉ DE “CICLO” (botón RELAY)
    //    recorre dispositivos: negro → blanco
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Comunicador" && buttonColor == RELAY) {
        if (event != BUTTON_PRESSED) return;                   // Sólo actuamos en PRESSED
        if (digitalRead(ENC_BUTTON) == LOW) return;            // Evitar interferir con el encoder

        std::vector<TARGETNS> nsSPIFFS;  // Sustituto de idsSPIFFS (global)
        if (nsSPIFFS.empty()) {
            // Construir lista de NS una sola vez
            for (const String &f : elementFiles) {
                if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar")
                    continue;
                nsSPIFFS.push_back(getNSFromFile(f));
            }
            std::sort(nsSPIFFS.begin(), nsSPIFFS.end(),
                      [](const TARGETNS& a, const TARGETNS& b){
                          return memcmp(&a, &b, sizeof(TARGETNS)) < 0;
                      });
        }

        // Determinar objetivos black / white
        uint8_t blackType, whiteType;
        TARGETNS blackNS = NS_ZERO, whiteNS = NS_ZERO;

        if (nsSPIFFS.empty()) {
            // sin elementos → broadcast ↔ broadcast
            blackType = BROADCAST; blackNS = NS_ZERO;
            whiteType = BROADCAST; whiteNS = NS_ZERO;
        } else {
            if (relayStep == -1) {
                blackType = BROADCAST;      blackNS = NS_ZERO;
                whiteType = DEFAULT_DEVICE; whiteNS = nsSPIFFS[0];
                relayStep = 0;
            } else if (relayStep < static_cast<int>(nsSPIFFS.size()) - 1) {
                blackType = DEFAULT_DEVICE; blackNS = nsSPIFFS[relayStep];
                ++relayStep;
                whiteType = DEFAULT_DEVICE; whiteNS = nsSPIFFS[relayStep];
            } else {
                blackType = DEFAULT_DEVICE; blackNS = nsSPIFFS[relayStep];
                whiteType = BROADCAST;      whiteNS = NS_ZERO;
                relayStep = -1;
            }
        }

        // Enviar negro (OFF) al black target
        if (blackType == BROADCAST) {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACK));
            send_old_color(BLACK);
        } else {
            uint8_t cfgB[2] = {0};
            bool hasColorB = false, hasRelayB = false;
            if (RelayStateManager::getModeConfigForNS(blackNS, cfgB)) {
                hasColorB = getModeFlag(cfgB, HAS_BASIC_COLOR) || getModeFlag(cfgB, HAS_ADVANCED_COLOR);
                hasRelayB = getModeFlag(cfgB, HAS_RELAY);
            }
            if (hasColorB) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACK));
            } else if (hasRelayB) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, 0x00));
            } else {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACK));
            }
        }
        delay(30);

        // Enviar blanco (ON) al white target
        if (whiteType == BROADCAST) {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, WHITE));
        } else {
            uint8_t cfgW[2] = {0};
            bool hasColorW = false, hasRelayW = false;
            if (RelayStateManager::getModeConfigForNS(whiteNS, cfgW)) {
                hasColorW = getModeFlag(cfgW, HAS_BASIC_COLOR) || getModeFlag(cfgW, HAS_ADVANCED_COLOR);
                hasRelayW = getModeFlag(cfgW, HAS_RELAY);
            }
            if (hasColorW) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, WHITE));
            } else if (hasRelayW) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01));
            } else {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, WHITE));
            }
        }

        // Actualiza el NS activo del comunicador (para botones de color)
        ::communicatorActiveNS = (whiteType == DEFAULT_DEVICE) ? whiteNS : NS_ZERO;

        colorHandler.setCurrentFile("Comunicador");
        colorHandler.setPatternBotonera(currentModeIndex, ledManager);

        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 7) RELÉ ÚNICO (botón RELAY, sin multirrelé/aromaterapia)
    // ─────────────────────────────────────────────────────────────
    if (buttonColor == RELAY) {
        if (!hasRelay || isMultiRelay || isAromaterapia)
            return;
        if (digitalRead(ENC_BUTTON) == LOW)
            return;

        // Para dispositivos, exigimos NS válido
        if (targetType == DEFAULT_DEVICE && isNSZero(targetNS))
            return;

        if (event == BUTTON_PRESSED) {
            if (hasPulse) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, 0x01));
                RelayStateManager::set(targetNS, true);
            } else {
                const bool cur  = RelayStateManager::get(targetNS);
                const bool next = !cur;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, next ? 0x01 : 0x00));
                RelayStateManager::set(targetNS, next);
            }
        } else if (event == BUTTON_RELEASED && hasPulse) {
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, 0x00));
            RelayStateManager::set(targetNS, false);
        }
        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 8) MODO PASIVO: SOLO AZUL
    // ─────────────────────────────────────────────────────────────
    if (hasPassive && (buttonColor != BLUE))
        return;

    // Validación: si target es dispositivo, NS debe ser válido
    if (targetType == DEFAULT_DEVICE && isNSZero(targetNS))
        return;

    // ─────────────────────────────────────────────────────────────
    // 9) PATRONES
    // ─────────────────────────────────────────────────────────────
    if (hasPatterns && event == BUTTON_PRESSED) {
        send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 10) ADVANCED NO PULSE (mezclas)
    // ─────────────────────────────────────────────────────────────
    if (hasAdvanced && !hasPulse) {
        static int  lastModeForAdvanced = -1;
        static bool advancedMixed       = false;

        if (currentModeIndex != lastModeForAdvanced) {
            lastBasicColor      = BLACK;
            advancedMixed       = false;
            lastModeForAdvanced = currentModeIndex;
        }

        if (event == BUTTON_PRESSED) {
            if (advancedMixed) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                lastBasicColor = BLACK;
                advancedMixed  = false;
            } else {
                if (lastBasicColor == BLACK) {
                    lastBasicColor = buttonColor;
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
                } else {
                    uint8_t mixColor;
                    if (colorHandler.color_mix_handler(lastBasicColor, buttonColor, &mixColor)) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, mixColor));
                        lastBasicColor = mixColor;
                        advancedMixed  = true;
                    } else {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                        lastBasicColor = BLACK;
                        advancedMixed  = false;
                    }
                }
            }
        }
        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 11) MODO BÁSICO (no pulse)
    // ─────────────────────────────────────────────────────────────
    if (hasPulse)
    {
        // En pulse: el envío de color se maneja al soltar / por escaneo, no aquí
        return;
    }
    else
    {
        if ((hasBasic || hasAdvanced) && event == BUTTON_PRESSED)
        {
            // Siempre enviamos la trama NUEVA
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));

            // Legacy SOLO en "Comunicador"
            if (currentFile == "Comunicador")
            {
                // Si acabamos de entrar en Comunicador, resetea
                if (!lastWasComunicador)
                {
                    lastWasComunicador = true;
                    lastLegacyColor = 0xFF;  // sin color previo
                    sameColorParity = false; // primera repetición sería COLOR
                }

                if (lastLegacyColor != buttonColor)
                {
                    // Color distinto al anterior → siempre COLOR
                    send_old_color(buttonColor);
                    lastLegacyColor = buttonColor;
                    sameColorParity = true; // la próxima vez que se repita ESTE color → BLACK
                }
                else
                {
                    // Mismo color consecutivo → alterna entre COLOR y BLACK
                    if (sameColorParity)
                    {
                        send_old_color(BLACK);   // BLACK = 0x08
                        sameColorParity = false; // próxima repetición → COLOR
                    }
                    else
                    {
                        send_old_color(buttonColor);
                        sameColorParity = true; // próxima repetición → BLACK
                    }
                }
            }
            else
            {
                // Salimos de "Comunicador": forzar reset al volver a entrar
                lastWasComunicador = false;
            }
        }
        return;
    }
}
