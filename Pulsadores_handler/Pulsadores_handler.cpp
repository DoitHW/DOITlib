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

// Mapear color lógico a LED index físico [0..8]. Devuelve -1 si no aplica (p.ej. BLACK/relleno)
static inline int colorToLedIdx(byte color) {
    switch (color) {
        case RELAY:       return 0;
        case WHITE:       return 1;
        case RED:         return 2;
        case LIGHT_BLUE:  return 3; // CELESTE
        case YELLOW:      return 4;
        case ORANGE:      return 5;
        case GREEN:       return 6;
        case VIOLET:      return 7;
        case BLUE:        return 8;
        default:          return -1; // BLACK / relleno u otros no mapeados
    }
}


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
// ===== Estado global de habilitación por LED (0..8) =====
static bool g_btnEnabled[9] = { true,true,true,true,true,true,true,true,true };

bool PulsadoresHandler::isButtonEnabled(uint8_t ledIdx) {
    return (ledIdx < 9) ? g_btnEnabled[ledIdx] : true;
}

void PulsadoresHandler::setButtonActiveMask(const bool mask[9]) {
    for (int i = 0; i < 9; ++i) g_btnEnabled[i] = mask[i];
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
        digitalWrite(filas[i], LOW);
        delayMicroseconds(1);
        for (int j = 0; j < COLUMNAS; j++) {
            if (pulsadorColor[i][j] != color) continue;

            const int ledIdx = colorToLedIdx(color);
            if (ledIdx >= 0 && !PulsadoresHandler::isButtonEnabled(ledIdx)) {
                continue; // inhabilitado → ignorar como si NO estuviera pulsado
            }

            if (digitalRead(columnas[j]) == LOW) {
                digitalWrite(filas[i], HIGH);
                return true;
            }
        }
        digitalWrite(filas[i], HIGH);
    }
    return false; // ← faltaba: si no se encontró pulsado, devolver false
}

void PulsadoresHandler::procesarPulsadores() {
    
    auto isNSZero = [](const TARGETNS& ns) -> bool {
        return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
    };
    // Target por defecto: BROADCAST
    uint8_t  targetType = 0xFF;   // BROADCAST
    TARGETNS targetNS   = getOwnNS();

    String currentFile = elementFiles[currentIndex];
    const bool respMode = PulsadoresHandler::isResponseRouteActive();
    const bool noInputGlobal = PulsadoresHandler::isGlobalFxNoInput();

    /*───────────────────────────
      1) MODO COGNITIVO (Consola)
    ───────────────────────────*/
    if (inCognitiveMenu && !respMode && !noInputGlobal) {
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

                // Filtrado por máscara 'active'
                int ledIdxGate = colorToLedIdx(color);
                if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
                    const bool communicatorBroadcast =
                        (currentFile == "Comunicador") &&
                        (communicatorActiveNS.mac01==0 && communicatorActiveNS.mac02==0 &&
                        communicatorActiveNS.mac03==0 && communicatorActiveNS.mac04==0 &&
                        communicatorActiveNS.mac05==0);
                   
                    if (!(respMode || communicatorBroadcast)) {
                        currentPressed = false;
                    }
                }

      
                // Objetivo: CONSOLA
                targetType = DEFAULT_CONSOLE; // 0xDC
                targetNS   = getOwnNS();      

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
    if (!respMode && currentFile != "Ambientes" && currentFile != "Fichas" &&
    currentFile != "Comunicador" && !selectedStates[currentIndex]) return;


    const bool isFichas = (currentFile == "Fichas");

    /*───────────────────────────
    3) Resolución de destino (target)
    ───────────────────────────*/
    if (respMode) {
        // En modo respuesta el destino lo fija processButtonEvent(F_SEND_RESPONSE)
        targetType = PulsadoresHandler::getResponseTargetType();
        targetNS   = getOwnNS();
    } else if (!isFichas) {
        if (currentFile == "Ambientes") {
            bool ambientesSeleccionado = false;
            for (size_t i = 0; i < elementFiles.size(); ++i) {
                if (elementFiles[i] == "Ambientes" && selectedStates[i]) { ambientesSeleccionado = true; break; }
            }
            if (ambientesSeleccionado) {
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
            targetNS   = getCurrentElementNS();
        }
    } // (si isFichas, el target se resuelve en su flujo propio)

    /*───────────────────────────
    4) Flags del modo actual (del TARGET real)
    ───────────────────────────*/
    byte modeConfig[2] = {0};
    bool cfgOK = false;

    auto isSpecialFile = [](const String& name)->bool{
        return (name == "Ambientes" || name == "Fichas" || name == "Comunicador" || name == "Apagar" || name == "Dado");
    };

    if (isSpecialFile(currentFile)) {
        // RAM: igual que haces en handleEncoder()
        INFO_PACK_T* opt = nullptr;
        if      (currentFile == "Ambientes")   opt = &ambientesOption;
        else if (currentFile == "Fichas")      opt = &fichasOption;
        else if (currentFile == "Comunicador") opt = &comunicadorOption;
        else if (currentFile == "Apagar")      opt = &apagarSala;
        else if (currentFile == "Dado")        opt = &dadoOption;

        if (opt) {
            const int realModeIndex = opt->currentMode;
            memcpy(modeConfig, opt->mode[realModeIndex].config, 2);
            cfgOK = true;
        }
    } else {
        // DISPOSITIVO: pedir la config por su NS actual (robusto)
        const TARGETNS ns = getCurrentElementNS();
        cfgOK = RelayStateManager::getModeConfigForNS(ns, modeConfig); // ← clave
        if (!cfgOK) {
            // Fallback: leer OFFSET_CURRENTMODE del archivo (como en handleEncoder)
            fs::File f = SPIFFS.open(currentFile, "r");
            if (f) {
                int realModeIndex = 0;
                f.seek(OFFSET_CURRENTMODE, SeekSet);
                realModeIndex = f.read();
                f.seek(OFFSET_MODES + realModeIndex * SIZE_MODE + 216, SeekSet);
                f.read(modeConfig, 2);
                f.close();
                cfgOK = true;
            }
        }
    }

    if (!cfgOK) {
        DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual (NS/FILE).");
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
            
            if (noInputGlobal) {                                          // <-- Nueva línea
                currentPressed = false;                                   // <-- Nueva línea
            } 
            // === Filtrado por máscara 'active' ===
            int ledIdxGate = colorToLedIdx(color);
            if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
                const bool communicatorBroadcast =
                    (currentFile == "Comunicador") &&
                    (communicatorActiveNS.mac01==0 && communicatorActiveNS.mac02==0 &&
                    communicatorActiveNS.mac03==0 && communicatorActiveNS.mac04==0 &&
                    communicatorActiveNS.mac05==0);
                // En modo respuesta o en Comunicador→broadcast no bloqueamos el evento
                if (!(respMode || communicatorBroadcast)) {
                    currentPressed = false;
                }
            }


            if (color != RELAY) {
                if (!lastState[i][j] && currentPressed)      pressTime[i][j] = millis();
                else if (lastState[i][j] && !currentPressed) pressTime[i][j] = 0;
            }

            if (color == RELAY) currentRelayState |= currentPressed;
            if (color == BLUE)  blueButtonState   |= currentPressed;

            if (!lastState[i][j] && currentPressed) {
            if (respMode) {
                targetType = PulsadoresHandler::getResponseTargetType(); // <-- Nueva línea
                targetNS   = getOwnNS();     
                // En modo respuesta, siempre disparamos evento (flags no se usan en la rama 1-bis)
                processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
                                targetType, targetNS);
            } else if (isFichas) {
                if (color == RELAY)      relayButtonPressed = true;
                else if (color == BLUE)  blueButtonPressed  = true;
            } else {
                processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay,
                                targetType, targetNS);
            }
        }
        if (lastState[i][j] && !currentPressed) {
            if (respMode) {
                targetType = PulsadoresHandler::getResponseTargetType();   // <-- Nueva línea
                targetNS   = getOwnNS();  
                processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
                                targetType, targetNS);
            } else if (isFichas) {
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
    if (!respMode && !inModesScreen && hasPulse && !isFichas) {
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
    const byte   buttonColor  = pulsadorColor[i][j];
    const String currentFile  = elementFiles[currentIndex];

    const bool communicatorBroadcast =
        (currentFile == "Comunicador") &&
        (targetType == BROADCAST) &&
        (targetNS.mac01==0 && targetNS.mac02==0 && targetNS.mac03==0 &&
         targetNS.mac04==0 && targetNS.mac05==0);

    static bool     lastWasComunicador = false;
    static uint8_t  lastLegacyColor    = 0xFF; // sentinel (ningún color válido)
    static bool     sameColorParity    = false;

    auto isNSZero = [](const TARGETNS& ns) -> bool {
        return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
    };

    // ──────────────────────────────────────
    // 0) Menú Cognitivo → PRIORIDAD EXTMAP (respMode)
    // ──────────────────────────────────────
    if (inCognitiveMenu && !PulsadoresHandler::isResponseRouteActive()) { 
        if (event == BUTTON_PRESSED) {
            if (buttonColor != RELAY) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_CONSOLE, getOwnNS(), buttonColor));
            }
        } else if (event == BUTTON_RELEASED && buttonColor == RELAY) {
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_CONSOLE, getOwnNS(), 0x01));
        }
        return;
    }


    // 1) Fichas no se procesan aquí
    if (currentFile == "Fichas")
        return;

    /* ─────────────────────────────────────────────────────────────
     * 1 bis) MODO RESPUESTA
     * ───────────────────────────────────────────────────────────── */
    if (event == BUTTON_PRESSED && PulsadoresHandler::isResponseRouteActive()) {
        int ledIdx = colorToLedIdx(buttonColor);
        if (ledIdx < 0) return;
        if (!PulsadoresHandler::isButtonEnabled((uint8_t)ledIdx)) return;

        auto ledIdxToButtonId = [](uint8_t idx) -> uint8_t {
            static const uint8_t kMap[9] = { 5, 9, 4, 8, 3, 7, 2, 6, 1 };
            return (idx < 9) ? kMap[idx] : 0;
        };
        const uint8_t response = ledIdxToButtonId((uint8_t)ledIdx);
        if (response == 0) return;

        send_frame(frameMaker_SEND_RESPONSE(DEFAULT_BOTONERA, s_respTargetType, getOwnNS(), response));

    return; // ← no continuar con la lógica estándar

    }

    // ─────────────────────────────────────────────────────────────
    // 2) COMUNICADOR · elemento sólo-relé → filtra botones
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Comunicador") {
        // ¡IMPORTANTE! usar la GLOBAL, no crear una variable local
        extern TARGETNS communicatorActiveNS;

        const bool communicatorHasTarget = !isNSZero(communicatorActiveNS);
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
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Ambientes") {
        if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
            const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;
            send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
            delay(100);
            if (sendColorByte != BLACK) doitPlayer.play_file(8, sendColorByte);
            else                         doitPlayer.stop_file();
            return;
        }
    }

    // ─────────────────────────────────────────────────────────────
    // 4) Cargar configuración del modo del TARGET real (por NS)
    //    (comportamiento conservador si no hay flags)
    // ─────────────────────────────────────────────────────────────
    uint8_t modeConfig[2] = {0};
    bool    cfgOK = false;

    auto isSpecialFile = [](const String& name)->bool{
        return (name == "Ambientes" || name == "Fichas" || name == "Comunicador" || name == "Apagar" || name == "Dado");
    };

    if (currentFile == "Comunicador") {
        // Si apunta a dispositivo concreto, usa los flags de ese NS;
        // si está en broadcast, no bloquees por falta de flags.
        extern TARGETNS communicatorActiveNS;
        if (!isNSZero(communicatorActiveNS)) {
            cfgOK = RelayStateManager::getModeConfigForNS(communicatorActiveNS, modeConfig);
        } else {
            cfgOK = true; // broadcast: no necesitas flags para permitir colores básicos
        }
    }
    else if (!isSpecialFile(currentFile)) {
        if (targetType == DEFAULT_DEVICE && !isNSZero(targetNS)) {
            cfgOK = RelayStateManager::getModeConfigForNS(targetNS, modeConfig);
        } else {
            cfgOK = true; // broadcast/console
        }
    }
    else {
        // RAM: Ambientes/Fichas/Apagar/Dado → opera sin bloquear si no hay flags cargados
        cfgOK = true;
    }

    if (!cfgOK) {
        // Sin config explícita → todos los flags en 0 (se usará la lógica conservadora más abajo)
        modeConfig[0] = modeConfig[1] = 0;
    }

    const bool hasBasic    = getModeFlag(modeConfig, HAS_BASIC_COLOR);
    const bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
    const bool hasPatterns = getModeFlag(modeConfig, HAS_PATTERNS);

    // Capacidades de relé del TARGET (para RELÉ ÚNICO / MULTI)
    const bool cfgHasRelay    = getModeFlag(modeConfig, HAS_RELAY);
    const bool cfgHasN1       = getModeFlag(modeConfig, HAS_RELAY_N1);
    const bool cfgHasN2       = getModeFlag(modeConfig, HAS_RELAY_N2);
    const bool isMultiRelay   = (cfgHasN1 || cfgHasN2);
    const bool isAromaterapia = (!cfgHasRelay && cfgHasN1 && cfgHasN2);

    // Si es multi-relé/aromaterapia, sólo valen botones BLUE/GREEN/YELLOW/RED
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
            // 1º feedback de color
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
    //    recorre dispositivos: negro → blanco → … → broadcast
    // ─────────────────────────────────────────────────────────────
    if (currentFile == "Comunicador" && buttonColor == RELAY) {
        if (event != BUTTON_PRESSED) return;            // Sólo en PRESSED
        if (digitalRead(ENC_BUTTON) == LOW) return;     // Evitar interferir con el encoder

        extern TARGETNS communicatorActiveNS;

        auto isNSZero2 = [](const TARGETNS& n){
            return n.mac01==0 && n.mac02==0 && n.mac03==0 && n.mac04==0 && n.mac05==0;
        };
        auto eqNS = [](const TARGETNS& a, const TARGETNS& b){
            return a.mac01==b.mac01 && a.mac02==b.mac02 && a.mac03==b.mac03 && a.mac04==b.mac04 && a.mac05==b.mac05;
        };
        auto cmpNS = [&](const TARGETNS& a, const TARGETNS& b){
            if (a.mac01 != b.mac01) return a.mac01 < b.mac01;
            if (a.mac02 != b.mac02) return a.mac02 < b.mac02;
            if (a.mac03 != b.mac03) return a.mac03 < b.mac03;
            if (a.mac04 != b.mac04) return a.mac04 < b.mac04;
            return a.mac05 < b.mac05;
        };

        // Construir SIEMPRE la lista actual de NS
        std::vector<TARGETNS> nsSPIFFS;
        nsSPIFFS.reserve(elementFiles.size());
        for (const String &f : elementFiles) {
            if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar") continue;
            TARGETNS ns = getNSFromFile(f);
            if (!isNSZero2(ns)) nsSPIFFS.push_back(ns);
        }
        std::sort(nsSPIFFS.begin(), nsSPIFFS.end(), cmpNS);

        // Realineación/Acotado de relayStep
        if (!nsSPIFFS.empty() && !isNSZero2(communicatorActiveNS)) {
            int idx = -1;
            for (size_t k = 0; k < nsSPIFFS.size(); ++k) {
                if (eqNS(nsSPIFFS[k], communicatorActiveNS)) { idx = (int)k; break; }
            }
            if (idx != -1) relayStep = idx;
        }
        if (relayStep < -1 || relayStep >= (int)nsSPIFFS.size()) {
            relayStep = -1;
        }

        // Determinar objetivos black / white
        uint8_t  blackType, whiteType;
        TARGETNS blackNS = NS_ZERO, whiteNS = NS_ZERO;

        if (nsSPIFFS.empty()) {
            blackType = BROADCAST;  blackNS = NS_ZERO;
            whiteType = BROADCAST;  whiteNS = NS_ZERO;
        } else {
            if (relayStep == -1) {
                blackType = BROADCAST;       blackNS = NS_ZERO;
                whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[0];
                relayStep = 0;
            } else if (relayStep < (int)nsSPIFFS.size() - 1) {
                blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
                ++relayStep;
                whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[relayStep];
            } else {
                blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
                whiteType = BROADCAST;       whiteNS = NS_ZERO;
                relayStep = -1;
            }
        }

        // OFF al black target
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

        // ON al white target
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

        // Actualiza NS activo del comunicador
        ::communicatorActiveNS = (whiteType == DEFAULT_DEVICE) ? whiteNS : NS_ZERO;

        // Refresca patrón local
        colorHandler.setCurrentFile("Comunicador");
        colorHandler.setPatternBotonera(currentModeIndex, ledManager);
        return;
    }

    // ─────────────────────────────────────────────────────────────
    // 7) RELÉ ÚNICO (botón RELAY)
    // ─────────────────────────────────────────────────────────────
    if (buttonColor == RELAY) {
        if (!cfgHasRelay || isMultiRelay || isAromaterapia) return;
        if (digitalRead(ENC_BUTTON) == LOW) return;
        if (targetType == DEFAULT_DEVICE && isNSZero(targetNS)) return;

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
    if (hasPulse) {
        // En pulse: lo gestiona el escaneo
        return;
    } else {
        // Permite color básico aunque no haya flags… si estamos en Comunicador en BROADCAST
        const bool allowBasicSend = communicatorBroadcast || hasBasic || hasAdvanced;
        if (allowBasicSend && event == BUTTON_PRESSED && buttonColor != RELAY) {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));

            // Legacy SOLO en "Comunicador"
            if (currentFile == "Comunicador") {
                if (!lastWasComunicador) {
                    lastWasComunicador = true;
                    lastLegacyColor = 0xFF;
                    sameColorParity = false;
                }
                if (lastLegacyColor != buttonColor) {
                    send_old_color(buttonColor);
                    lastLegacyColor = buttonColor;
                    sameColorParity = true;
                } else {
                    if (sameColorParity) {
                        send_old_color(BLACK);   // 0x08
                        sameColorParity = false;
                    } else {
                        send_old_color(buttonColor);
                        sameColorParity = true;
                    }
                }
            } else {
                lastWasComunicador = false;
            }
        }
        return;
    }
}


// void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
//                                            bool hasPulse, bool hasPassive, bool hasRelay,
//                                            uint8_t targetType, const TARGETNS& targetNS)
// {
//     const byte buttonColor = pulsadorColor[i][j];
//     const String currentFile = elementFiles[currentIndex];

//     const bool communicatorBroadcast =
//     (currentFile == "Comunicador") &&
//     (targetType == BROADCAST) &&
//     (targetNS.mac01==0 && targetNS.mac02==0 && targetNS.mac03==0 &&
//      targetNS.mac04==0 && targetNS.mac05==0);


//     static bool     lastWasComunicador = false;
//     static uint8_t  lastLegacyColor    = 0xFF; // sentinel (ningún color válido)
//     static bool     sameColorParity    = false;

//     auto isNSZero = [](const TARGETNS& ns) -> bool {
//         return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
//     };

//     // ──────────────────────────────────────
//     // 0) Menú Cognitivo → siempre a CONSOLA
//     //    (esperamos targetType=DEFAULT_CONSOLE, targetNS=NS_ZERO)
//     // ──────────────────────────────────────
//     if (inCognitiveMenu) {
//         if (event == BUTTON_PRESSED) {
//             if (buttonColor != RELAY) {
//                 // COLOR hacia la consola
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_CONSOLE, NS_ZERO, buttonColor));
//             }
//         } else if (event == BUTTON_RELEASED && buttonColor == RELAY) {
//             // RELAY → flag 1 (no toggle) hacia la consola
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_CONSOLE, NS_ZERO, 0x01));
//         }
//         return;
//     }

//     // 1) Fichas no se procesan aquí
//     if (currentFile == "Fichas")
//         return;

//     /* ─────────────────────────────────────────────────────────────
//     * 1 bis) MODO RESPUESTA (activado tras F_SET_BUTTONS_EXTMAP)
//     *       - Si está activo: cualquier botón PRESSED envía F_SEND_RESPONSE
//     *         al origen que configuró (DC→NS_ZERO, DD→su NS) y SALIMOS.
//     *       - Se asume que el escaneo ya filtró .active; por robustez, revalidamos.
//     * ───────────────────────────────────────────────────────────── */
//     if (event == BUTTON_PRESSED && PulsadoresHandler::isResponseRouteActive()) {

//         // Mapeo color lógico → índice LED 0..8 (ya lo usas en el proyecto)
//         int ledIdx = colorToLedIdx(buttonColor);
//         if (ledIdx < 0) return;

//         if (!PulsadoresHandler::isButtonEnabled((uint8_t)ledIdx)) {
//             return; // deshabilitado por .active → no responde
//         }

//         // ledIdx (0..8) → buttonId (1..9), inversa de tu tabla canónica
//         auto ledIdxToButtonId = [](uint8_t idx) -> uint8_t {
//             static const uint8_t kMap[9] = { 5, 9, 4, 8, 3, 7, 2, 6, 1 };
//             return (idx < 9) ? kMap[idx] : 0;
//         };
//         const uint8_t response = ledIdxToButtonId((uint8_t)ledIdx);
//         if (response == 0) return; // fuera de rango

//         // Construir y enviar RESPUESTA al “origen que configuró”
//         FRAME_T fr = frameMaker_SEND_RESPONSE(
//             DEFAULT_BOTONERA,
//             s_respTargetType,   // (static en PulsadoresHandler)
//             s_respTargetNS,     // (NS_ZERO si DC, NS origen si DD)
//             response
//         );
//         send_frame(fr);

//         return; // ← importante: no sigas con la lógica antigua (color/patrones/relé)
//     }
    
//     // ─────────────────────────────────────────────────────────────
//     // 2) COMUNICADOR · elemento sólo-relé → filtra botones
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Comunicador") {
//         TARGETNS communicatorActiveNS;  // NS del elemento apuntado por el comunicador

//         // ¿apunta a un dispositivo real?
//         const bool communicatorHasTarget = !isNSZero(communicatorActiveNS);

//         // ¿ese dispositivo es “solo-relé”?
//         bool soloRele = false;
//         if (communicatorHasTarget) {
//             uint8_t cfg[2] = {0};
//             if (RelayStateManager::getModeConfigForNS(communicatorActiveNS, cfg)) {
//                 const bool hasCol = getModeFlag(cfg, HAS_BASIC_COLOR) || getModeFlag(cfg, HAS_ADVANCED_COLOR);
//                 const bool hasRel = getModeFlag(cfg, HAS_RELAY);
//                 soloRele = (!hasCol && hasRel);
//             }
//         }

//         if (soloRele) {
//             // En solo-relé, sólo aceptamos AZUL y RELAY
//             if (buttonColor != BLUE && buttonColor != RELAY) return;

//             // Botón AZUL → toggle ON/OFF del relé en el NS del comunicador
//             if (buttonColor == BLUE && event == BUTTON_PRESSED && communicatorHasTarget) {
//                 const bool cur  = RelayStateManager::get(communicatorActiveNS);
//                 const bool next = !cur;
//                 send_frame(frameMaker_SEND_FLAG_BYTE(
//                     DEFAULT_BOTONERA, DEFAULT_DEVICE, communicatorActiveNS, next ? 0x01 : 0x00
//                 ));
//                 RelayStateManager::set(communicatorActiveNS, next);
//                 return;
//             }
//         }
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 3) AMBIENTES (broadcast patterns cuando está seleccionado)
//     //    (esperamos targetType=BROADCAST, targetNS=NS_ZERO al venir del caller)
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Ambientes") {
//         if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
//             const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;

//             // Mandamos patrón a broadcast
//             send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
//             delay(100);

//             if (sendColorByte != BLACK) doitPlayer.play_file(8, sendColorByte);
//             else                         doitPlayer.stop_file();
//             return;
//         }
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 4) Cargar configuración del modo del TARGET real (por NS)
//     // ─────────────────────────────────────────────────────────────
//     uint8_t modeConfig[2] = {0};
//     bool cfgOK = false;

//     auto isSpecialFile = [](const String& name)->bool{
//         return (name == "Ambientes" || name == "Fichas" || name == "Comunicador" || name == "Apagar" || name == "Dado");
//     };

//     if (currentFile == "Comunicador") {
//         // Para lógica general (no el ciclo, que ya lo trataste arriba):
//         if (!isNSZero(communicatorActiveNS)) {
//             cfgOK = RelayStateManager::getModeConfigForNS(communicatorActiveNS, modeConfig);
//         } else {
//             cfgOK = true; // broadcast: no necesitas flags para colores/patrones
//         }
//     }
//     else if (!isSpecialFile(currentFile)) {
//         // Elemento SPIFFS → usa el NS resuelto por el caller
//         if (targetType == DEFAULT_DEVICE) {
//             cfgOK = RelayStateManager::getModeConfigForNS(targetNS, modeConfig);
//         } else {
//             cfgOK = true; // broadcast/console
//         }
//     }
//     else {
//         // RAM (Ambientes, Fichas, Apagar, Dado): puedes operar sin flags o replicar lectura RAM si los necesitas
//         cfgOK = true;
//     }

//     if (!cfgOK) {
//         // Si no hay config, sigue pero con flags a 0 (comportamiento conservador)
//         // DEBUG__________ln("ℹ️ No config via NS; se asumen flags 0.");
//     }

//     const bool hasBasic    = getModeFlag(modeConfig, HAS_BASIC_COLOR);
//     const bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
//     const bool hasPatterns = getModeFlag(modeConfig, HAS_PATTERNS);

//     // **DE AQUÍ** calculas las capacidades reales de relé del TARGET:
//     const bool cfgHasRelay  = getModeFlag(modeConfig, HAS_RELAY);
//     const bool cfgHasN1     = getModeFlag(modeConfig, HAS_RELAY_N1);
//     const bool cfgHasN2     = getModeFlag(modeConfig, HAS_RELAY_N2);
//     const bool isMultiRelay = (cfgHasN1 || cfgHasN2);
//     const bool isAromaterapia = (!cfgHasRelay && cfgHasN1 && cfgHasN2);


//     // Si es multi-relé/aromaterapia, sólo valen botones de color (BLUE/GREEN/YELLOW/RED)
//     if ((isAromaterapia || isMultiRelay) &&
//         buttonColor != BLUE && buttonColor != GREEN &&
//         buttonColor != YELLOW && buttonColor != RED) {
//         return;
//     }

//     int relayIndex = -1;
//     if      (buttonColor == BLUE)   relayIndex = 0;
//     else if (buttonColor == GREEN)  relayIndex = 1;
//     else if (buttonColor == YELLOW) relayIndex = 2;
//     else if (buttonColor == RED)    relayIndex = 3;

//     // ─────────────────────────────────────────────────────────────
//     // 5) MULTIRRELÉ (botones de color)
//     //    (para dispositivos → targetType debe ser DEFAULT_DEVICE y targetNS válido)
//     // ─────────────────────────────────────────────────────────────
//     if ((isMultiRelay || isAromaterapia) && (relayIndex >= 0 && relayIndex <= 3)) {
//         if (event == BUTTON_PRESSED) {
//             uint8_t mask = (1u << relayIndex);

//             if (isAromaterapia) {
//                 // Exclusivo
//                 relay_state = (relay_state == mask) ? 0x00 : mask;
//             } else {
//                 // Independiente (toggle)
//                 relay_state ^= mask;
//             }

//             // 1º feedback de color al mismo target
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
//             delay(20);
//             // 2º flags compuestos
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, relay_state));

//             #ifdef DEBUG
//             DEBUG__________printf("[RELAY %s] Botón color activado - Index: %d → Estado: 0x%02X\n",
//                                   isAromaterapia ? "AROMATERAPIA" : "MULTI",
//                                   relayIndex, relay_state);
//             #endif
//         }
//         return;
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 6) COMUNICADOR · RELÉ DE “CICLO” (botón RELAY)
//     //    recorre dispositivos: negro → blanco
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Comunicador" && buttonColor == RELAY) {
//         if (event != BUTTON_PRESSED) return;            // Sólo actuamos en PRESSED
//         if (digitalRead(ENC_BUTTON) == LOW) return;     // Evitar interferir con el encoder

//         auto isNSZero = [](const TARGETNS& n){
//             return n.mac01==0 && n.mac02==0 && n.mac03==0 && n.mac04==0 && n.mac05==0;
//         };
//         auto eqNS = [](const TARGETNS& a, const TARGETNS& b){
//             return a.mac01==b.mac01 && a.mac02==b.mac02 && a.mac03==b.mac03 && a.mac04==b.mac04 && a.mac05==b.mac05;
//         };
//         auto cmpNS = [&](const TARGETNS& a, const TARGETNS& b){
//             if (a.mac01 != b.mac01) return a.mac01 < b.mac01;
//             if (a.mac02 != b.mac02) return a.mac02 < b.mac02;
//             if (a.mac03 != b.mac03) return a.mac03 < b.mac03;
//             if (a.mac04 != b.mac04) return a.mac04 < b.mac04;
//             return a.mac05 < b.mac05;
//         };

//         // Construir SIEMPRE la lista actual de NS (robusto ante cambios de lista/orden)
//         std::vector<TARGETNS> nsSPIFFS;
//         nsSPIFFS.reserve(elementFiles.size());
//         for (const String &f : elementFiles) {
//             if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar") continue;
//             TARGETNS ns = getNSFromFile(f);
//             if (!isNSZero(ns)) nsSPIFFS.push_back(ns);
//         }
//         std::sort(nsSPIFFS.begin(), nsSPIFFS.end(), cmpNS);

//         // === REALINEACIÓN Y ACOTADO DE relayStep ===
//         // 1) Alinear con el NS activo del Comunicador si está presente
//         if (!nsSPIFFS.empty() && !isNSZero(communicatorActiveNS)) {
//             int idx = -1;
//             for (size_t k = 0; k < nsSPIFFS.size(); ++k) {
//                 if (eqNS(nsSPIFFS[k], communicatorActiveNS)) { idx = (int)k; break; }
//             }
//             if (idx != -1) relayStep = idx;
//         }
//         // 2) Acotar siempre el índice (tras mapas, listas cambiadas, etc.)
//         if (relayStep < -1 || relayStep >= (int)nsSPIFFS.size()) {
//             relayStep = -1;  // fuerza inicio correcto del ciclo (broadcast → primero)
//         }

//         // Determinar objetivos black / white
//         uint8_t  blackType, whiteType;
//         TARGETNS blackNS = NS_ZERO, whiteNS = NS_ZERO;

//         if (nsSPIFFS.empty()) {
//             // Sin elementos → broadcast ↔ broadcast (sigue siendo útil)
//             blackType = BROADCAST;  blackNS = NS_ZERO;
//             whiteType = BROADCAST;  whiteNS = NS_ZERO;
//         } else {
//             if (relayStep == -1) {
//                 // Inicio de ciclo: broadcast → primer elemento
//                 blackType = BROADCAST;       blackNS = NS_ZERO;
//                 whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[0];
//                 relayStep = 0;
//             } else if (relayStep < (int)nsSPIFFS.size() - 1) {
//                 // Avance normal
//                 blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
//                 ++relayStep;
//                 whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[relayStep];
//             } else {
//                 // Último → broadcast y cerramos ciclo
//                 blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
//                 whiteType = BROADCAST;       whiteNS = NS_ZERO;
//                 relayStep = -1;
//             }
//         }

//         // Enviar negro (OFF) al black target
//         if (blackType == BROADCAST) {
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACK));
//             send_old_color(BLACK); // legado si aplicaba en tu rama
//         } else {
//             uint8_t cfgB[2] = {0};
//             bool hasColorB = false, hasRelayB = false;
//             if (RelayStateManager::getModeConfigForNS(blackNS, cfgB)) {
//                 hasColorB = getModeFlag(cfgB, HAS_BASIC_COLOR) || getModeFlag(cfgB, HAS_ADVANCED_COLOR);
//                 hasRelayB = getModeFlag(cfgB, HAS_RELAY);
//             }
//             if (hasColorB) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACK));
//             } else if (hasRelayB) {
//                 send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, 0x00));
//             } else {
//                 // fallback seguro
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACK));
//             }
//         }
//         delay(30);

//         // Enviar blanco (ON) al white target
//         if (whiteType == BROADCAST) {
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, WHITE));
//         } else {
//             uint8_t cfgW[2] = {0};
//             bool hasColorW = false, hasRelayW = false;
//             if (RelayStateManager::getModeConfigForNS(whiteNS, cfgW)) {
//                 hasColorW = getModeFlag(cfgW, HAS_BASIC_COLOR) || getModeFlag(cfgW, HAS_ADVANCED_COLOR);
//                 hasRelayW = getModeFlag(cfgW, HAS_RELAY);
//             }
//             if (hasColorW) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, WHITE));
//             } else if (hasRelayW) {
//                 send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01));
//             } else {
//                 // fallback seguro
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, WHITE));
//             }
//         }

//         // Actualiza el NS activo del comunicador (para botones de color)
//         ::communicatorActiveNS = (whiteType == DEFAULT_DEVICE) ? whiteNS : NS_ZERO;

//         // Reaplica el patrón local por si el mapa visual debe reflejar el “activo”
//         colorHandler.setCurrentFile("Comunicador");
//         colorHandler.setPatternBotonera(currentModeIndex, ledManager);

//         return;  // no continuar con lógica estándar
//     }


//     // ─────────────────────────────────────────────────────────────
//     // 7) RELÉ ÚNICO (botón RELAY, sin multirrelé/aromaterapia)
//     // ─────────────────────────────────────────────────────────────
//     if (buttonColor == RELAY) {
//         if (!cfgHasRelay || isMultiRelay || isAromaterapia)
//             return;
//         if (digitalRead(ENC_BUTTON) == LOW)
//             return;

//         // Para dispositivos, exigimos NS válido
//         if (targetType == DEFAULT_DEVICE && isNSZero(targetNS))
//             return;

//         if (event == BUTTON_PRESSED) {
//             if (hasPulse) {
//                 send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, 0x01));
//                 RelayStateManager::set(targetNS, true);
//             } else {
//                 const bool cur  = RelayStateManager::get(targetNS);
//                 const bool next = !cur;
//                 send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, next ? 0x01 : 0x00));
//                 RelayStateManager::set(targetNS, next);
//             }
//         } else if (event == BUTTON_RELEASED && hasPulse) {
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, 0x00));
//             RelayStateManager::set(targetNS, false);
//         }
//         return;
//     }


//     // ─────────────────────────────────────────────────────────────
//     // 8) MODO PASIVO: SOLO AZUL
//     // ─────────────────────────────────────────────────────────────
//     if (hasPassive && (buttonColor != BLUE))
//         return;

//     // Validación: si target es dispositivo, NS debe ser válido
//     if (targetType == DEFAULT_DEVICE && isNSZero(targetNS))
//         return;

//     // ─────────────────────────────────────────────────────────────
//     // 9) PATRONES
//     // ─────────────────────────────────────────────────────────────
//     if (hasPatterns && event == BUTTON_PRESSED) {
//         send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
//         return;
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 10) ADVANCED NO PULSE (mezclas)
//     // ─────────────────────────────────────────────────────────────
//     if (hasAdvanced && !hasPulse) {
//         static int  lastModeForAdvanced = -1;
//         static bool advancedMixed       = false;

//         if (currentModeIndex != lastModeForAdvanced) {
//             lastBasicColor      = BLACK;
//             advancedMixed       = false;
//             lastModeForAdvanced = currentModeIndex;
//         }

//         if (event == BUTTON_PRESSED) {
//             if (advancedMixed) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
//                 lastBasicColor = BLACK;
//                 advancedMixed  = false;
//             } else {
//                 if (lastBasicColor == BLACK) {
//                     lastBasicColor = buttonColor;
//                     send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
//                 } else {
//                     uint8_t mixColor;
//                     if (colorHandler.color_mix_handler(lastBasicColor, buttonColor, &mixColor)) {
//                         send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, mixColor));
//                         lastBasicColor = mixColor;
//                         advancedMixed  = true;
//                     } else {
//                         send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
//                         lastBasicColor = BLACK;
//                         advancedMixed  = false;
//                     }
//                 }
//             }
//         }
//         return;
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 11) MODO BÁSICO (no pulse)
//     // ─────────────────────────────────────────────────────────────
//     if (hasPulse)
//     {
//         // En pulse: el envío de color se maneja al soltar / por escaneo, no aquí
//         return;
//     }
//     else
//     {
//         const bool allowBasicSend = communicatorBroadcast || hasBasic || hasAdvanced;
//         if (allowBasicSend && event == BUTTON_PRESSED && buttonColor != RELAY)
//         {
//             // Siempre enviamos la trama NUEVA
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));

//             // Legacy SOLO en "Comunicador"
//             if (currentFile == "Comunicador")
//             {
//                 // Si acabamos de entrar en Comunicador, resetea
//                 if (!lastWasComunicador)
//                 {
//                     lastWasComunicador = true;
//                     lastLegacyColor = 0xFF;  // sin color previo
//                     sameColorParity = false; // primera repetición sería COLOR
//                 }

//                 if (lastLegacyColor != buttonColor)
//                 {
//                     // Color distinto al anterior → siempre COLOR
//                     send_old_color(buttonColor);
//                     lastLegacyColor = buttonColor;
//                     sameColorParity = true; // la próxima vez que se repita ESTE color → BLACK
//                 }
//                 else
//                 {
//                     // Mismo color consecutivo → alterna entre COLOR y BLACK
//                     if (sameColorParity)
//                     {
//                         send_old_color(BLACK);   // BLACK = 0x08
//                         sameColorParity = false; // próxima repetición → COLOR
//                     }
//                     else
//                     {
//                         send_old_color(buttonColor);
//                         sameColorParity = true; // próxima repetición → BLACK
//                     }
//                 }
//             }
//             else
//             {
//                 // Salimos de "Comunicador": forzar reset al volver a entrar
//                 lastWasComunicador = false;
//             }
//         }
//         return;
//     }
// }

// Pulsadores_handler.cpp
bool     PulsadoresHandler::s_responseModeEnabled = false;
uint8_t  PulsadoresHandler::s_respTargetType = 0;
TARGETNS PulsadoresHandler::s_respTargetNS   = {0,0,0,0,0};

void PulsadoresHandler::setResponseRoute(uint8_t targetType, const TARGETNS& targetNS){
    s_respTargetType     = targetType;
    s_respTargetNS       = targetNS;
    s_responseModeEnabled = true;
}

void PulsadoresHandler::clearResponseRoute(){
    s_responseModeEnabled = false;
    s_respTargetType      = 0;
    s_respTargetNS        = TARGETNS{0,0,0,0,0};
}

bool PulsadoresHandler::isResponseRouteActive(){
    return s_responseModeEnabled;
}

uint8_t PulsadoresHandler::getResponseTargetType() {      
    return s_respTargetType;                              
}                             

TARGETNS PulsadoresHandler::getResponseTargetNS() {       
    return s_respTargetNS;                                
} 

bool PulsadoresHandler::s_globalFxNoInput = false;          // <-- Nueva línea

void PulsadoresHandler::setGlobalFxNoInput(bool enable) {   // <-- Nueva línea
    s_globalFxNoInput = enable;                              // <-- Nueva línea
}                                                            // <-- Nueva línea

bool PulsadoresHandler::isGlobalFxNoInput() {               // <-- Nueva línea
    return s_globalFxNoInput;                                // <-- Nueva línea
}                                                            // <-- Nueva línea
