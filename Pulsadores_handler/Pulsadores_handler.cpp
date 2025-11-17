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

// void PulsadoresHandler::procesarPulsadores() {
    
//     auto isNSZero = [](const TARGETNS& ns) -> bool {
//         return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
//     };
//     // Target por defecto: BROADCAST
//     uint8_t  targetType = 0xFF;   // BROADCAST
//     TARGETNS targetNS   = getOwnNS();;

//     String currentFile = elementFiles[currentIndex];
//     const bool respMode = PulsadoresHandler::isResponseRouteActive();
//     const bool noInputGlobal = PulsadoresHandler::isGlobalFxNoInput();

//     if (currentFile == "Comunicador" && !selectedStates[currentIndex]) {
//     // Evita estados pegados de estas flags
//     relayButtonPressed = false;
//     blueButtonPressed  = false;
//     return;
// }

//     /*───────────────────────────
//       1) MODO COGNITIVO (Consola)
//     ───────────────────────────*/
//     if (inCognitiveMenu && !respMode && !noInputGlobal) {
//         static bool lastState[FILAS][COLUMNAS] = { { false } };

//         for (int i = 0; i < FILAS; i++) {
//             digitalWrite(filas[i], LOW);
//             delayMicroseconds(10);
//             for (int j = 0; j < COLUMNAS; j++) {
//                 byte color = pulsadorColor[i][j];
//                 if (color != BLUE && color != GREEN && color != YELLOW &&
//                     color != RED  && color != RELAY) {
//                     continue;
//                 }

//                 bool currentPressed = (digitalRead(columnas[j]) == LOW);

//                 // Filtrado por máscara 'active'
//                 int ledIdxGate = colorToLedIdx(color);
//                 if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
//                     const bool communicatorBroadcast =
//                         (currentFile == "Comunicador") &&
//                         (communicatorActiveNS.mac01==0 && communicatorActiveNS.mac02==0 &&
//                         communicatorActiveNS.mac03==0 && communicatorActiveNS.mac04==0 &&
//                         communicatorActiveNS.mac05==0);

//                     if (color == RELAY) {
//                         // RELAY se lee SIEMPRE si el modo lo soporta (no lo bloquea la máscara 'active')
//                         // no cambiamos currentPressed
//                     } else if (!(respMode || communicatorBroadcast)) {
//                         currentPressed = false;
//                     }
//                 }


      
//                 // Objetivo: CONSOLA
//                 targetType = DEFAULT_CONSOLE; // 0xDC
//                 targetNS   = NS_ZERO;         // NS cero para no-dispositivo

//                 if (!lastState[i][j] && currentPressed) {
//                     processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
//                                        targetType, targetNS);
//                 }
//                 if (lastState[i][j] && !currentPressed) {
//                     processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
//                                        targetType, targetNS);
//                 }
//                 lastState[i][j] = currentPressed;
//             }
//             digitalWrite(filas[i], HIGH);
//         }
//         return;  // salir tras gestionar cognitivo
//     }

//     /*───────────────────────────
//       2) Filtros de contexto
//     ───────────────────────────*/
//     if (currentFile == "Apagar") return;

//     // Ignorar pulsaciones si es elemento normal no seleccionado
//     if (!respMode && currentFile != "Ambientes" && currentFile != "Fichas" &&
//     !selectedStates[currentIndex]) return;

//     const bool isFichas = (currentFile == "Fichas");

//     /*───────────────────────────
//     3) Resolución de destino (target)
//     ───────────────────────────*/
//     if (respMode) {
//         // En modo respuesta el destino lo fija processButtonEvent(F_SEND_RESPONSE)
//         targetType = PulsadoresHandler::getResponseTargetType();
//         targetNS   = getOwnNS();
//     } else if (!isFichas) {
//         if (currentFile == "Ambientes") {
//             bool ambientesSeleccionado = false;
//             for (size_t i = 0; i < elementFiles.size(); ++i) {
//                 if (elementFiles[i] == "Ambientes" && selectedStates[i]) { ambientesSeleccionado = true; break; }
//             }
//             if (ambientesSeleccionado) {
//                 targetType = BROADCAST;  // 0xFF
//                 targetNS   = NS_ZERO;
//             }
//         } else if (currentFile == "Comunicador") {
//             extern TARGETNS communicatorActiveNS;
//             if (isNSZero(communicatorActiveNS)) {
//                 // Comunicador en modo broadcast
//                 targetType = BROADCAST;
//                 targetNS   = NS_ZERO;
//             } else {
//                 // Comunicador apuntando a un dispositivo concreto
//                 targetType = DEFAULT_DEVICE;  // 0xDD
//                 targetNS   = communicatorActiveNS;
//             }
//         } else {
//             targetType = DEFAULT_DEVICE;
//             targetNS   = getCurrentElementNS();
//         }
//     } // (si isFichas, el target se resuelve en su flujo propio)



//     /*───────────────────────────
//     4) Flags del modo actual (del TARGET real)
//     ───────────────────────────*/
//     byte modeConfig[2] = {0};
//     bool cfgOK = false;

//     auto isSpecialFile = [](const String& name)->bool{
//         return (name == "Ambientes" || name == "Fichas" || name == "Comunicador" || name == "Apagar" || name == "Dado");
//     };

//     if (isSpecialFile(currentFile)) {
//         // RAM: igual que haces en handleEncoder()
//         INFO_PACK_T* opt = nullptr;
//         if      (currentFile == "Ambientes")   opt = &ambientesOption;
//         else if (currentFile == "Fichas")      opt = &fichasOption;
//         else if (currentFile == "Comunicador") opt = &comunicadorOption;
//         else if (currentFile == "Apagar")      opt = &apagarSala;
//         else if (currentFile == "Dado")        opt = &dadoOption;

//         if (opt) {
//             const int realModeIndex = opt->currentMode;
//             memcpy(modeConfig, opt->mode[realModeIndex].config, 2);
//             cfgOK = true;
//         }
//     } else {
//         // DISPOSITIVO: pedir la config por su NS actual (robusto)
//         const TARGETNS ns = getCurrentElementNS();
//         cfgOK = RelayStateManager::getModeConfigForNS(ns, modeConfig); // ← clave
//         if (!cfgOK) {
//             // Fallback: leer OFFSET_CURRENTMODE del archivo (como en handleEncoder)
//             fs::File f = SPIFFS.open(currentFile, "r");
//             if (f) {
//                 int realModeIndex = 0;
//                 f.seek(OFFSET_CURRENTMODE, SeekSet);
//                 realModeIndex = f.read();
//                 f.seek(OFFSET_MODES + realModeIndex * SIZE_MODE + 216, SeekSet);
//                 f.read(modeConfig, 2);
//                 f.close();
//                 cfgOK = true;
//             }
//         }
//     }

//     if (!cfgOK) {
//         DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual (NS/FILE).");
//         return;
//     }

//     const bool hasPulse    = getModeFlag(modeConfig, HAS_PULSE);
//     const bool hasPassive  = getModeFlag(modeConfig, HAS_PASSIVE);
//     const bool hasRelay    = getModeFlag(modeConfig, HAS_RELAY);
//     const bool hasRelayN1  = getModeFlag(modeConfig, HAS_RELAY_N1);
//     const bool hasRelayN2  = getModeFlag(modeConfig, HAS_RELAY_N2);
//     const bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);


//     /*───────────────────────────
//       5) Estado por-botón
//     ───────────────────────────*/
//     static bool         lastState[FILAS][COLUMNAS]       = { { false } };
//     static unsigned long pressTime[FILAS][COLUMNAS]      = { { 0 } };
//     static byte         currentActiveColor               = BLACK;
//     static bool         blackSent                        = false;
//     static bool         mixReady                         = true;

//     bool        currentRelayState = false;
//     bool        blueButtonState   = false;
//     static int  lastModeIndex     = -1;

//     // Reset al cambiar de modo visible
//     if (currentModeIndex != lastModeIndex) {
//         for (int i = 0; i < FILAS; i++)
//             for (int j = 0; j < COLUMNAS; j++)
//                 pressTime[i][j] = 0;

//         currentActiveColor = BLACK;
//         blackSent          = false;
//         mixReady           = true;
//         lastModeIndex      = currentModeIndex;
//     }

//     /*───────────────────────────
//       6) Escaneo de matriz y eventos
//     ───────────────────────────*/
//     for (int i = 0; i < FILAS; i++) {
//         digitalWrite(filas[i], LOW);
//         delayMicroseconds(10);
//         for (int j = 0; j < COLUMNAS; j++) {
//             bool currentPressed = (digitalRead(columnas[j]) == LOW);
//             byte color         = pulsadorColor[i][j];
            
//             if (noInputGlobal) {                                          // <-- Nueva línea
//                 currentPressed = false;                                   // <-- Nueva línea
//             } 
//             // === Filtrado por máscara 'active' ===
//             int ledIdxGate = colorToLedIdx(color);
//             if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
//                 const bool communicatorBroadcast =
//                     (currentFile == "Comunicador") &&
//                     (communicatorActiveNS.mac01==0 && communicatorActiveNS.mac02==0 &&
//                     communicatorActiveNS.mac03==0 && communicatorActiveNS.mac04==0 &&
//                     communicatorActiveNS.mac05==0);

//             const bool allowRelay = (color == RELAY) && hasRelay;   // ← usa el hasRelay que ya calculaste arriba
//             if (!(respMode || communicatorBroadcast || allowRelay)) {
//                 currentPressed = false;
//             }
//             }

//             if (color != RELAY) {
//                 if (!lastState[i][j] && currentPressed)      pressTime[i][j] = millis();
//                 else if (lastState[i][j] && !currentPressed) pressTime[i][j] = 0;
//             }

//             if (color == RELAY) currentRelayState |= currentPressed;
//             if (color == BLUE)  blueButtonState   |= currentPressed;

//             if (!lastState[i][j] && currentPressed) {
//             if (respMode) {
//                 targetType = PulsadoresHandler::getResponseTargetType(); // <-- Nueva línea
//                 targetNS   = getOwnNS();     
//                 // En modo respuesta, siempre disparamos evento (flags no se usan en la rama 1-bis)
//                 processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
//                                 targetType, targetNS);
//             } else if (isFichas) {
//                 if (color == RELAY)      relayButtonPressed = true;
//                 else if (color == BLUE)  blueButtonPressed  = true;
//             } else {
//                 processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay,
//                                 targetType, targetNS);
//             }
//         }
//         if (lastState[i][j] && !currentPressed) {
//             if (respMode) {
//                 targetType = PulsadoresHandler::getResponseTargetType();   // <-- Nueva línea
//                 targetNS   = getOwnNS();  
//                 processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
//                                 targetType, targetNS);
//             } else if (isFichas) {
//                 if (color == RELAY)      relayButtonPressed = false;
//                 else if (color == BLUE)  blueButtonPressed  = false;
//             } else {
//                 processButtonEvent(i, j, BUTTON_RELEASED, hasPulse, hasPassive, hasRelay,
//                                 targetType, targetNS);
//             }
//         }
//             lastState[i][j] = currentPressed;
//         }
//         digitalWrite(filas[i], HIGH);
//     }

//     relayButtonPressed = currentRelayState;
//     blueButtonPressed  = blueButtonState;

//     /*───────────────────────────
//       7) Envío de color (modes con HAS_PULSE)
//     ───────────────────────────*/
//     if (!respMode && !inModesScreen && hasPulse && !isFichas) {
//         if (hasAdvanced) {
//             int  count   = 0;
//             byte color1  = BLACK;
//             byte color2  = BLACK;

//             for (int i = 0; i < FILAS; i++) {
//                 for (int j = 0; j < COLUMNAS; j++) {
//                     if (pulsadorColor[i][j] == RELAY) continue;
//                     if (pressTime[i][j] > 0) {
//                         ++count;
//                         if (count == 1)      color1 = pulsadorColor[i][j];
//                         else if (count == 2) color2 = pulsadorColor[i][j];
//                     }
//                 }
//             }

//             if (count < 2) {
//                 mixReady = true;
//                 if (count == 0) {
//                     if (!blackSent) {
//                         send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
//                         currentActiveColor = BLACK;
//                         blackSent          = true;
//                     }
//                 } else if (count == 1 && currentActiveColor != color1) {
//                     send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, color1));
//                     currentActiveColor = color1;
//                     blackSent          = false;
//                 }
//             } else if (count == 2 && mixReady) {
//                 byte mixColor;
//                 if (colorHandler.color_mix_handler(color1, color2, &mixColor)) {
//                     send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, mixColor));
//                     currentActiveColor = mixColor;
//                     blackSent          = false;
//                     mixReady           = false;
//                 } else {
//                     if (!blackSent) {
//                         send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
//                         currentActiveColor = BLACK;
//                         blackSent          = true;
//                         mixReady           = false;
//                     }
//                 }
//             }
//         } else {
//             unsigned long maxTime        = 0;
//             byte          newActiveColor = BLACK;
//             bool          activeValid    = false;

//             for (int i = 0; i < FILAS; i++) {
//                 for (int j = 0; j < COLUMNAS; j++) {
//                     if (pulsadorColor[i][j] == RELAY) continue;
//                     if (pressTime[i][j] > maxTime) {
//                         maxTime        = pressTime[i][j];
//                         newActiveColor = pulsadorColor[i][j];
//                         activeValid    = true;
//                     }
//                 }
//             }

//             if (activeValid && currentActiveColor != newActiveColor) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, newActiveColor));
//                 currentActiveColor = newActiveColor;
//                 blackSent          = false;
//             } else if (!activeValid && !blackSent) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
//                 currentActiveColor = BLACK;
//                 blackSent          = true;
//             }
//         }
//     }
// }

std::vector<uint8_t> idsSPIFFS;   // IDs ordenadas               (solo Comunicador)
int  relayStep = -1;              // -1 = BROADCAST encendido
TARGETNS communicatorActiveNS = NS_ZERO;   // 0xFF = broadcast

// void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
//                                            bool hasPulse, bool hasPassive, bool hasRelay,
//                                            uint8_t targetType, const TARGETNS& targetNS)
// {
//     const byte   buttonColor  = pulsadorColor[i][j];
//     const String currentFile  = elementFiles[currentIndex];

//     const bool communicatorBroadcast =
//         (currentFile == "Comunicador") &&
//         (targetType == BROADCAST) &&
//         (targetNS.mac01==0 && targetNS.mac02==0 && targetNS.mac03==0 &&
//          targetNS.mac04==0 && targetNS.mac05==0);

//     static bool     lastWasComunicador = false;
//     static uint8_t  lastLegacyColor    = 0xFF; // sentinel (ningún color válido)
//     static bool     sameColorParity    = false;

//     auto isNSZero = [](const TARGETNS& ns) -> bool {
//         return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
//     };

//     // ──────────────────────────────────────
//     // 0) Menú Cognitivo → PRIORIDAD EXTMAP (respMode)
//     // ──────────────────────────────────────
//     if (inCognitiveMenu && !PulsadoresHandler::isResponseRouteActive()) { 
//         if (event == BUTTON_PRESSED) {
//             if (buttonColor != RELAY) {
//                 send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, DEFAULT_CONSOLE, getOwnNS(), buttonColor));
//             }
//         } else if (event == BUTTON_RELEASED && buttonColor == RELAY) {
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_CONSOLE, getOwnNS(), 0x01));
//         }
//         return;
//     }


//     // 1) Fichas no se procesan aquí
//     if (currentFile == "Fichas")
//         return;

//     /* ─────────────────────────────────────────────────────────────
//      * 1 bis) MODO RESPUESTA
//      * ───────────────────────────────────────────────────────────── */
//     if (event == BUTTON_PRESSED && PulsadoresHandler::isResponseRouteActive()) {
//         int ledIdx = colorToLedIdx(buttonColor);
//         if (ledIdx < 0) return;
//         if (!PulsadoresHandler::isButtonEnabled((uint8_t)ledIdx)) return;

//         auto ledIdxToButtonId = [](uint8_t idx) -> uint8_t {
//             static const uint8_t kMap[9] = { 5, 9, 4, 8, 3, 7, 2, 6, 1 };
//             return (idx < 9) ? kMap[idx] : 0;
//         };
//         const uint8_t response = ledIdxToButtonId((uint8_t)ledIdx);
//         if (response == 0) return;

//         send_frame(frameMaker_SEND_RESPONSE(DEFAULT_BOTONERA, s_respTargetType, getOwnNS(), response));

//     return; // ← no continuar con la lógica estándar

//     }

//     // ─────────────────────────────────────────────────────────────
//     // 2) COMUNICADOR · elemento sólo-relé → filtra botones
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Comunicador") {
//         // ¡IMPORTANTE! usar la GLOBAL, no crear una variable local
//         extern TARGETNS communicatorActiveNS;

//         const bool communicatorHasTarget = !isNSZero(communicatorActiveNS);
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
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Ambientes") {
//         if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
//             const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;
//             send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
//             delay(100);
//             if (sendColorByte != BLACK) doitPlayer.play_file(AMBIENTS_BANK, sendColorByte + 1);
//             else                         doitPlayer.stop_file();
//             return;
//         }
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 4) Cargar configuración del modo del TARGET real (por NS)
//     //    (comportamiento conservador si no hay flags)
//     // ─────────────────────────────────────────────────────────────
//     uint8_t modeConfig[2] = {0};
//     bool    cfgOK = false;

//     auto isSpecialFile = [](const String& name)->bool{
//         return (name == "Ambientes" || name == "Fichas" || name == "Comunicador" || name == "Apagar" || name == "Dado");
//     };

//     if (currentFile == "Comunicador") {
//         // Si apunta a dispositivo concreto, usa los flags de ese NS;
//         // si está en broadcast, no bloquees por falta de flags.
//         extern TARGETNS communicatorActiveNS;
//         if (!isNSZero(communicatorActiveNS)) {
//             cfgOK = RelayStateManager::getModeConfigForNS(communicatorActiveNS, modeConfig);
//         } else {
//             cfgOK = true; // broadcast: no necesitas flags para permitir colores básicos
//         }
//     }
//     else if (!isSpecialFile(currentFile)) {
//         if (targetType == DEFAULT_DEVICE && !isNSZero(targetNS)) {
//             cfgOK = RelayStateManager::getModeConfigForNS(targetNS, modeConfig);
//         } else {
//             cfgOK = true; // broadcast/console
//         }
//     }
//     else {
//         // RAM: Ambientes/Fichas/Apagar/Dado → opera sin bloquear si no hay flags cargados
//         cfgOK = true;
//     }

//     if (!cfgOK) {
//         // Sin config explícita → todos los flags en 0 (se usará la lógica conservadora más abajo)
//         modeConfig[0] = modeConfig[1] = 0;
//     }

//     const bool hasBasic    = getModeFlag(modeConfig, HAS_BASIC_COLOR);
//     const bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
//     const bool hasPatterns = getModeFlag(modeConfig, HAS_PATTERNS);

//     // Capacidades de relé del TARGET (para RELÉ ÚNICO / MULTI)
//     const bool cfgHasRelay    = getModeFlag(modeConfig, HAS_RELAY);
//     const bool cfgHasN1       = getModeFlag(modeConfig, HAS_RELAY_N1);
//     const bool cfgHasN2       = getModeFlag(modeConfig, HAS_RELAY_N2);
//     const bool isMultiRelay   = (cfgHasN1 || cfgHasN2);
//     const bool isAromaterapia = (!cfgHasRelay && cfgHasN1 && cfgHasN2);

//     // Si es multi-relé/aromaterapia, sólo valen botones BLUE/GREEN/YELLOW/RED
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
//             // 1º feedback de color
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
//             delay(20);
//             // 2º flags compuestos
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, relay_state));
// #ifdef DEBUG
//             DEBUG__________printf("[RELAY %s] Botón color activado - Index: %d → Estado: 0x%02X\n",
//                                   isAromaterapia ? "AROMATERAPIA" : "MULTI",
//                                   relayIndex, relay_state);
// #endif
//         }
//         return;
//     }

//     // ─────────────────────────────────────────────────────────────
//     // 6) COMUNICADOR · RELÉ DE “CICLO” (botón RELAY)
//     //    Por pulsación: BLACKOUT(anterior) → [CONSEC_DELAY_MS] → START(siguiente)
//     //    Primer paso desde broadcast: BLACKOUT(broadcast) → [FIRST_ON_DELAY_MS] → START(primer NS)
//     //    Último paso a broadcast:     START(broadcast) + FLAG(0x01)
//     //    Además: tras START a un dispositivo, si HAS_RELAY ⇒ enviar FLAG(0x01)
//     // ─────────────────────────────────────────────────────────────
//     if (currentFile == "Comunicador" && buttonColor == RELAY) {
//         if (event != BUTTON_PRESSED) return;            // Sólo en PRESSED
//         if (digitalRead(ENC_BUTTON) == LOW) return;     // Evitar interferir con el encoder

//         const unsigned CONSEC_DELAY_MS    = 100;   // gap entre BLACKOUT y START durante el ciclo
//         const unsigned FIRST_ON_DELAY_MS  = 3000;  // espera especial al arrancar primer NS o al volver a broadcast

//         extern TARGETNS communicatorActiveNS;

//         auto isNSZero2 = [](const TARGETNS& n){
//             return n.mac01==0 && n.mac02==0 && n.mac03==0 && n.mac04==0 && n.mac05==0;
//         };
//         auto eqNS = [](const TARGETNS& a, const TARGETNS& b){
//             return a.mac01==b.mac01 && a.mac02==b.mac02 && a.mac03==b.mac03 &&
//                 a.mac04==b.mac04 && a.mac05==b.mac05;
//         };
//         auto cmpNS = [&](const TARGETNS& a, const TARGETNS& b){
//             if (a.mac01 != b.mac01) return a.mac01 < b.mac01;
//             if (a.mac02 != b.mac02) return a.mac02 < b.mac02;
//             if (a.mac03 != b.mac03) return a.mac03 < b.mac03;
//             if (a.mac04 != b.mac04) return a.mac04 < b.mac04;
//             return a.mac05 < b.mac05;
//         };

//         // 1) Construir SIEMPRE la lista actual de NS (solo elementos reales)
//         std::vector<TARGETNS> nsSPIFFS;
//         nsSPIFFS.reserve(elementFiles.size());
//         for (const String &f : elementFiles) {
//             if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar") continue;
//             TARGETNS ns = getNSFromFile(f);
//             if (!isNSZero2(ns)) nsSPIFFS.push_back(ns);
//         }
//         std::sort(nsSPIFFS.begin(), nsSPIFFS.end(), cmpNS);

//         // 2) Realinear relayStep con el NS activo actual (si lo hay)
//         if (!nsSPIFFS.empty() && !isNSZero2(communicatorActiveNS)) {
//             int idx = -1;
//             for (size_t k = 0; k < nsSPIFFS.size(); ++k) {
//                 if (eqNS(nsSPIFFS[k], communicatorActiveNS)) { idx = (int)k; break; }
//             }
//             if (idx != -1) relayStep = idx;
//         }
//         if (relayStep < -1 || relayStep >= (int)nsSPIFFS.size()) relayStep = -1;

//         // 3) Determinar blanco/negro (anterior/siguiente) y si es primera pulsación
//         bool firstCycleKick = false;
//         uint8_t  blackType, whiteType;
//         TARGETNS blackNS = NS_ZERO, whiteNS = NS_ZERO;

//         if (nsSPIFFS.empty()) {
//             // No hay elementos: ciclo sobre broadcast (apaga y enciende todo)
//             blackType = BROADCAST;  blackNS = NS_ZERO;
//             whiteType = BROADCAST;  whiteNS = NS_ZERO;
//             // relayStep se mantiene en -1
//         } else {
//             if (relayStep == -1) {
//                 // Venimos de broadcast: siguiente será el primer NS
//                 blackType = BROADCAST;       blackNS = NS_ZERO;
//                 whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[0];
//                 relayStep = 0;
//                 firstCycleKick = true;
//             } else if (relayStep < (int)nsSPIFFS.size() - 1) {
//                 // Paso intermedio: apagar actual y encender el siguiente
//                 blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
//                 ++relayStep;
//                 whiteType = DEFAULT_DEVICE;  whiteNS = nsSPIFFS[relayStep];
//             } else {
//                 // Último → volver a broadcast (sin BLACKOUT previo)
//                 blackType = DEFAULT_DEVICE;  blackNS = nsSPIFFS[relayStep];
//                 whiteType = BROADCAST;       whiteNS = NS_ZERO;
//                 relayStep = -1;
//             }
//         }

//         // 4) Ejecutar comandos según el caso
//         if (firstCycleKick) {
//             // BLACKOUT global, esperar 3s y luego START al primer NS (+ FLAG si tiene relé)
//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT
//             ));
//             delay(FIRST_ON_DELAY_MS);

//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD
//             ));
//             delay(CONSEC_DELAY_MS);

//             // Si el elemento tiene relé, activar flag 0x01
//             {
//                 uint8_t cfgW[2] = {0,0};
//                 if (RelayStateManager::getModeConfigForNS(whiteNS, cfgW)) {
//                     if (getModeFlag(cfgW, HAS_RELAY)) {
//                         send_frame(frameMaker_SEND_FLAG_BYTE(
//                             DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01
//                         ));
//                         delay(CONSEC_DELAY_MS);
//                     }
//                 }
//             }

//         } else if (whiteType == DEFAULT_DEVICE && blackType == DEFAULT_DEVICE) {
//             // Paso intermedio normal: BLACKOUT(prev) → START(next) (+ FLAG si tiene relé)
//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACKOUT
//             ));
//             delay(CONSEC_DELAY_MS);

//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD
//             ));
//             delay(CONSEC_DELAY_MS);

//             // Si el siguiente elemento tiene relé, activar flag 0x01
//             {
//                 uint8_t cfgW[2] = {0,0};
//                 if (RelayStateManager::getModeConfigForNS(whiteNS, cfgW)) {
//                     if (getModeFlag(cfgW, HAS_RELAY)) {
//                         send_frame(frameMaker_SEND_FLAG_BYTE(
//                             DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01
//                         ));
//                         delay(CONSEC_DELAY_MS);
//                     }
//                 }
//             }

//         } else if (whiteType == BROADCAST && blackType == DEFAULT_DEVICE) {
//             // Último → salto directo a broadcast: START(broadcast) + FLAG(0x01)
//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, START_CMD
//             ));
//             delay(CONSEC_DELAY_MS);

//             send_frame(frameMaker_SEND_FLAG_BYTE(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, 0x01
//             ));
//             delay(CONSEC_DELAY_MS);

//         } else if (whiteType == DEFAULT_DEVICE && blackType == BROADCAST) {
//             // Caso raro (por seguridad): tratar como primer paso
//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT
//             ));
//             delay(FIRST_ON_DELAY_MS);

//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD
//             ));
//             delay(CONSEC_DELAY_MS);

//             // Si el elemento tiene relé, activar flag 0x01
//             {
//                 uint8_t cfgW[2] = {0,0};
//                 if (RelayStateManager::getModeConfigForNS(whiteNS, cfgW)) {
//                     if (getModeFlag(cfgW, HAS_RELAY)) {
//                         send_frame(frameMaker_SEND_FLAG_BYTE(
//                             DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01
//                         ));
//                         delay(CONSEC_DELAY_MS);
//                     }
//                 }
//             }

//         } else if (whiteType == BROADCAST && blackType == BROADCAST) {
//             // Sin elementos: ciclo sobre broadcast (apaga todo y vuelve a encender tras 3s) + FLAG(0x01)
//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT
//             ));
//             delay(FIRST_ON_DELAY_MS);

//             send_frame(frameMaker_SEND_COMMAND(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, START_CMD
//             ));
//             delay(CONSEC_DELAY_MS);

//             send_frame(frameMaker_SEND_FLAG_BYTE(
//                 DEFAULT_BOTONERA, BROADCAST, NS_ZERO, 0x01
//             ));
//             delay(CONSEC_DELAY_MS);
//         }

//         // 5) Actualizar NS activo del comunicador
//         ::communicatorActiveNS = (whiteType == DEFAULT_DEVICE) ? whiteNS : NS_ZERO;

//         // 6) Refrescar patrón local
//         colorHandler.setCurrentFile("Comunicador");
//         colorHandler.setPatternBotonera(currentModeIndex, ledManager);
//         return;
//     }





//     // ─────────────────────────────────────────────────────────────
//     // 7) RELÉ ÚNICO (botón RELAY)
//     // ─────────────────────────────────────────────────────────────
//     if (buttonColor == RELAY) {
//         if (!cfgHasRelay || isMultiRelay || isAromaterapia) return;
//         if (digitalRead(ENC_BUTTON) == LOW) return;
//         if (targetType == DEFAULT_DEVICE && isNSZero(targetNS)) return;

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
//     if (hasPulse) {
//         // En pulse: lo gestiona el escaneo
//         return;
//     } else {
//         // Permite color básico aunque no haya flags… si estamos en Comunicador en BROADCAST
//         const bool allowBasicSend = communicatorBroadcast || hasBasic || hasAdvanced;
//         if (allowBasicSend && event == BUTTON_PRESSED && buttonColor != RELAY) {
//             send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));

//             // Legacy SOLO en "Comunicador"
//             if (currentFile == "Comunicador") {
//                 if (!lastWasComunicador) {
//                     lastWasComunicador = true;
//                     lastLegacyColor = 0xFF;
//                     sameColorParity = false;
//                 }
//                 if (lastLegacyColor != buttonColor) {
//                     #ifdef LEGACY_COLOR_SUPPORT
//                         send_old_color(buttonColor);
//                     #endif
//                     lastLegacyColor = buttonColor;
//                     sameColorParity = true;
//                 } else {
//                     if (sameColorParity) {
//                     #ifdef LEGACY_COLOR_SUPPORT
//                         send_old_color(BLACK);
//                     #endif
//                         sameColorParity = false;
//                     } else {
//                     #ifdef LEGACY_COLOR_SUPPORT
//                         send_old_color(buttonColor);
//                     #endif
//                         sameColorParity = true;
//                     }
//                 }
//             } else {
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

// ─────────────────────────────────────────────────────────────
// Helpers de caché locales (ligeros y sin dependencias externas)
// ─────────────────────────────────────────────────────────────
struct _BtnCtx {
    // Claves de invalidación
    String   file;                 // elementFiles[currentIndex]
    TARGETNS ns;                   // NS real del target (si aplica)
    bool     isRespMode = false;   // ruta de respuesta
    bool     isFichas   = false;   // fichero especial
    bool     isCom      = false;   // Comunicador
    bool     isAmb      = false;   // Ambientes
    bool     isApagar   = false;   // Apagar
    bool     isDado     = false;   // Dado
    bool     isBcastCom = false;   // Comunicador en broadcast
    int      visibleModeIdx = -1;  // currentModeIndex
    int      realModeIdx    = -1;  // índice real del modo en INFO_PACK_T / archivo
    uint8_t  targetType     = BROADCAST; // resuelto
    TARGETNS targetNS       = NS_ZERO;   // resuelto

    // Flags cacheados (del modo actual del TARGET real)
    bool cfgOK       = false;
    uint8_t cfg[2]   = {0,0};
    bool hasPulse    = false;
    bool hasPassive  = false;
    bool hasRelay    = false;
    bool hasRelayN1  = false;
    bool hasRelayN2  = false;
    bool hasBasic    = false;
    bool hasAdvanced = false;
    bool hasPatterns = false;
};

static inline bool _isNSZero(const TARGETNS& ns) {
    return memcmp(&ns, &NS_ZERO, sizeof(TARGETNS)) == 0;
}

static inline void _deriveFlags(_BtnCtx &c) {
    c.hasPulse    = getModeFlag(c.cfg, HAS_PULSE);
    c.hasPassive  = getModeFlag(c.cfg, HAS_PASSIVE);
    c.hasRelay    = getModeFlag(c.cfg, HAS_RELAY);
    c.hasRelayN1  = getModeFlag(c.cfg, HAS_RELAY_N1);
    c.hasRelayN2  = getModeFlag(c.cfg, HAS_RELAY_N2);
    c.hasAdvanced = getModeFlag(c.cfg, HAS_ADVANCED_COLOR);
    c.hasBasic    = getModeFlag(c.cfg, HAS_BASIC_COLOR);
    c.hasPatterns = getModeFlag(c.cfg, HAS_PATTERNS);
}

static bool _isSpecial(const String& f) {
    return (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar" || f == "Dado");
}

static inline void _fillFlagsFromSpecial(const String& file, _BtnCtx& c) {
    INFO_PACK_T* opt = nullptr;
    if      (file == "Ambientes")   opt = &ambientesOption;
    else if (file == "Fichas")      opt = &fichasOption;
    else if (file == "Comunicador") opt = &comunicadorOption;
    else if (file == "Apagar")      opt = &apagarSala;
    else if (file == "Dado")        opt = &dadoOption;
    if (opt) {
        c.realModeIdx = opt->currentMode;
        memcpy(c.cfg, opt->mode[c.realModeIdx].config, 2);
        c.cfgOK = true;
        _deriveFlags(c);
    } else {
        c.cfgOK = false;
        c.cfg[0]=c.cfg[1]=0;
        _deriveFlags(c);
    }
}

// Caché minúsculo por NS para evitar tocar SPIFFS si la librería no lo tiene cacheado
struct _NSCfgEntry { TARGETNS ns; uint8_t cfg[2]; bool ok; };
static _NSCfgEntry _nsCache[4]; // 4 entradas LRU muy simple
static inline bool _getModeCfgCached(const TARGETNS& ns, uint8_t out[2], bool& ok) {
    // hit?
    for (int k=0; k<4; ++k) {
        if (!_isNSZero(_nsCache[k].ns) && memcmp(&_nsCache[k].ns, &ns, sizeof(ns))==0) {
            out[0] = _nsCache[k].cfg[0]; out[1] = _nsCache[k].cfg[1]; ok = _nsCache[k].ok; return true;
        }
    }
    ok = RelayStateManager::getModeConfigForNS(ns, out); // puede ser cacheada internamente o tocar SPIFFS 1 vez
    // insértalo (LRU muy burro: desplaza y mete en 0)
    for (int k=3; k>0; --k) _nsCache[k] = _nsCache[k-1];
    _nsCache[0].ns  = ns;
    _nsCache[0].cfg[0] = out[0];
    _nsCache[0].cfg[1] = out[1];
    _nsCache[0].ok  = ok;
    return false;
}

// Resuelve/actualiza el contexto sólo cuando cambie algo
static inline void _refreshBtnCtx(_BtnCtx& c, bool respMode) {
    String file = elementFiles[currentIndex];
    const bool isFichas   = (file == "Fichas");
    const bool isCom      = (file == "Comunicador");
    const bool isAmb      = (file == "Ambientes");
    const bool isApagar   = (file == "Apagar");
    const bool isDado     = (file == "Dado");
    extern TARGETNS communicatorActiveNS;

    TARGETNS curNS = NS_ZERO;
    uint8_t  tgtType = BROADCAST;
    TARGETNS tgtNS   = NS_ZERO;
    int      realModeIdx = -1;

    // 1) Resolver target
    if (respMode) {
        tgtType = PulsadoresHandler::getResponseTargetType();
        tgtNS   = getOwnNS();
        curNS   = tgtNS;
    } else if (isAmb) {
        // broadcast sólo si Ambientes está seleccionado (como tenías)
        bool ambientesSeleccionado = false;
        for (size_t i = 0; i < elementFiles.size(); ++i) {
            if (elementFiles[i] == "Ambientes" && selectedStates[i]) { ambientesSeleccionado = true; break; }
        }
        tgtType = ambientesSeleccionado ? BROADCAST : BROADCAST;
        tgtNS   = NS_ZERO;
        curNS   = NS_ZERO;
    } else if (isCom) {
        if (_isNSZero(communicatorActiveNS)) {
            tgtType = BROADCAST; tgtNS = NS_ZERO;
        } else {
            tgtType = DEFAULT_DEVICE; tgtNS = communicatorActiveNS;
        }
        curNS = tgtNS;
    } else if (isFichas) {
        // Fichas: lo resolverá su flujo específico si lo necesitas
        tgtType = BROADCAST; tgtNS = NS_ZERO; curNS = NS_ZERO;
    } else if (isApagar || isDado) {
        tgtType = BROADCAST; tgtNS = NS_ZERO; curNS = NS_ZERO;
    } else {
        // Dispositivo normal
        tgtType = DEFAULT_DEVICE;
        tgtNS   = getCurrentElementNS();
        curNS   = tgtNS;
    }

    // 2) ¿Debemos recalcular?
    const bool needRecalc =
        (file           != c.file)            ||
        (respMode       != c.isRespMode)      ||
        (currentModeIndex != c.visibleModeIdx)||
        (memcmp(&curNS,  &c.ns, sizeof(curNS)) != 0) ||
        (tgtType        != c.targetType)      ||
        (memcmp(&tgtNS,  &c.targetNS, sizeof(tgtNS)) != 0);

    if (!needRecalc) return;

    // 3) Rellenar claves
    c.file          = file;
    c.ns            = curNS;
    c.isRespMode    = respMode;
    c.isFichas      = isFichas;
    c.isCom         = isCom;
    c.isAmb         = isAmb;
    c.isApagar      = isApagar;
    c.isDado        = isDado;
    c.isBcastCom    = (isCom && _isNSZero(communicatorActiveNS));
    c.visibleModeIdx= currentModeIndex;
    c.targetType    = tgtType;
    c.targetNS      = tgtNS;

    // 4) Flags del modo (ligeros y cacheados)
    if (isApagar) {
        c.cfgOK=false; c.cfg[0]=c.cfg[1]=0; _deriveFlags(c);
        return;
    }

    if (_isSpecial(file)) {
        _fillFlagsFromSpecial(file, c);
        return;
    }

    if (!respMode && !_isNSZero(curNS)) {
        bool ok=false;
        _getModeCfgCached(curNS, c.cfg, ok);  // 1ª vez puede tocar SPIFFS; luego cache
        c.cfgOK = ok;
        _deriveFlags(c);
        if (!c.cfgOK) { // fallback opcional: obtener del archivo UNA sola vez (no por escaneo)
            fs::File f = SPIFFS.open(file, "r");
            if (f) {
                f.seek(OFFSET_CURRENTMODE, SeekSet);
                c.realModeIdx = f.read();
                f.seek(OFFSET_MODES + c.realModeIdx * SIZE_MODE + 216, SeekSet);
                f.read(c.cfg, 2);
                f.close();
                c.cfgOK = true;
                _deriveFlags(c);
                // mete también en caché
                for (int k=3; k>0; --k) _nsCache[k] = _nsCache[k-1];
                _nsCache[0].ns = curNS; _nsCache[0].cfg[0]=c.cfg[0]; _nsCache[0].cfg[1]=c.cfg[1]; _nsCache[0].ok=true;
            }
        }
    } else {
        // RespMode o NS nulo → no bloquear por falta de flags
        c.cfgOK=false; c.cfg[0]=c.cfg[1]=0; _deriveFlags(c);
    }
}

void PulsadoresHandler::procesarPulsadores() {
    // Contexto global cacheado (persiste entre llamadas)
    static _BtnCtx ctx;

    const String currentFile = elementFiles[currentIndex];
    const bool   respMode    = PulsadoresHandler::isResponseRouteActive();
    const bool   noInput     = PulsadoresHandler::isGlobalFxNoInput();

    // Ignorar entradas si Comunicador NO está seleccionado (limpia flags pegados)
    if (currentFile == "Comunicador" && !selectedStates[currentIndex]) {
        relayButtonPressed = false;
        blueButtonPressed  = false;
        return;
    }

    // 1) Menú Cognitivo → escaneo mínimo, sin cálculos caros
    if (inCognitiveMenu && !respMode && !noInput) {
        static bool lastState[FILAS][COLUMNAS] = {{false}};
        for (int i = 0; i < FILAS; i++) {
            digitalWrite(filas[i], LOW);
            delayMicroseconds(10);
            for (int j = 0; j < COLUMNAS; j++) {
                const byte color = pulsadorColor[i][j];
                if (color != BLUE && color != GREEN && color != YELLOW && color != RED && color != RELAY) continue;

                bool currentPressed = (digitalRead(columnas[j]) == LOW);

                // Máscara 'active' (salvo RELAY, salvo rutas permitidas)
                const int ledIdxGate = colorToLedIdx(color);
                if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
                    const bool communicatorBroadcast =
                        (currentFile == "Comunicador") && _isNSZero(communicatorActiveNS);
                    if (color != RELAY && !(respMode || communicatorBroadcast)) currentPressed = false;
                }

                const uint8_t targetType = DEFAULT_CONSOLE;
                const TARGETNS targetNS  = NS_ZERO;

                if (!lastState[i][j] && currentPressed) {
                    processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
                                       targetType, targetNS);
                } else if (lastState[i][j] && !currentPressed) {
                    processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/true, /*hasPassive*/false, /*hasRelay*/true,
                                       targetType, targetNS);
                }
                lastState[i][j] = currentPressed;
            }
            digitalWrite(filas[i], HIGH);
        }
        return;
    }

    // 2) Filtros de contexto rápidos
    if (currentFile == "Apagar") return;

    // Elemento normal NO seleccionado → ignorar (regla del usuario)
    if (!respMode && currentFile != "Ambientes" && currentFile != "Fichas" &&
        currentFile != "Comunicador" && !selectedStates[currentIndex]) {
        return;
    }

    // 3) Refrescar contexto SOLO si cambió algo relevante
    _refreshBtnCtx(ctx, respMode);

    // 4) Estado por botón (solo datos de tiempo/mezcla; nada caro)
    static bool          lastState[FILAS][COLUMNAS]  = {{false}};
    static unsigned long pressTime[FILAS][COLUMNAS] = {{0}};
    static byte          currentActiveColor         = BLACK;
    static bool          blackSent                  = false;
    static bool          mixReady                   = true;

    bool currentRelayState = false;
    bool blueButtonState   = false;
    static int lastVisibleMode = -1;

    if (ctx.visibleModeIdx != lastVisibleMode) {
        for (int i = 0; i < FILAS; i++)
            for (int j = 0; j < COLUMNAS; j++)
                pressTime[i][j] = 0;
        currentActiveColor = BLACK;
        blackSent          = false;
        mixReady           = true;
        lastVisibleMode    = ctx.visibleModeIdx;
    }

    // 5) Escaneo de matriz (ligero)
    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW);
        delayMicroseconds(10);
        for (int j = 0; j < COLUMNAS; j++) {
            const byte color = pulsadorColor[i][j];
            bool currentPressed = (digitalRead(columnas[j]) == LOW);

            if (noInput) currentPressed = false;

            // Gate de 'active' salvo casos permitidos
            const int ledIdxGate = colorToLedIdx(color);
            if (ledIdxGate >= 0 && !isButtonEnabled(ledIdxGate)) {
                const bool communicatorBroadcast = (currentFile == "Comunicador") && ctx.isBcastCom;
                const bool allowRelay = (color == RELAY) && ctx.hasRelay;
                if (!(respMode || communicatorBroadcast || allowRelay)) currentPressed = false;
            }

            if (color != RELAY) {
                if (!lastState[i][j] && currentPressed)      pressTime[i][j] = millis();
                else if (lastState[i][j] && !currentPressed) pressTime[i][j] = 0;
            }

            if (color == RELAY) currentRelayState |= currentPressed;
            if (color == BLUE)  blueButtonState   |= currentPressed;

            // Eventos (disparo mínimo; sin cálculos pesados aquí)
            if (!lastState[i][j] && currentPressed) {
                if (respMode) {
                    processButtonEvent(i, j, BUTTON_PRESSED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
                                       PulsadoresHandler::getResponseTargetType(), getOwnNS());
                } else if (currentFile == "Fichas") {
                    if (color == RELAY)      relayButtonPressed = true;
                    else if (color == BLUE)  blueButtonPressed  = true;
                } else {
                    processButtonEvent(i, j, BUTTON_PRESSED, ctx.hasPulse, ctx.hasPassive, ctx.hasRelay,
                                       ctx.targetType, ctx.targetNS);
                }
            }
            else if (lastState[i][j] && !currentPressed) {
                if (respMode) {
                    processButtonEvent(i, j, BUTTON_RELEASED, /*hasPulse*/false, /*hasPassive*/false, /*hasRelay*/false,
                                       PulsadoresHandler::getResponseTargetType(), getOwnNS());
                } else if (currentFile == "Fichas") {
                    if (color == RELAY)      relayButtonPressed = false;
                    else if (color == BLUE)  blueButtonPressed  = false;
                } else {
                    processButtonEvent(i, j, BUTTON_RELEASED, ctx.hasPulse, ctx.hasPassive, ctx.hasRelay,
                                       ctx.targetType, ctx.targetNS);
                }
            }

            lastState[i][j] = currentPressed;
        }
        digitalWrite(filas[i], HIGH);
    }

    relayButtonPressed = currentRelayState;
    blueButtonPressed  = blueButtonState;

    // 6) Envío continuo de color para modos con PULSE (sigue igual, usando el target resuelto en ctx)
    if (!respMode && !inModesScreen && ctx.hasPulse && currentFile != "Fichas") {
        if (ctx.hasAdvanced) {
            int  count  = 0; byte c1 = BLACK, c2 = BLACK;
            for (int i = 0; i < FILAS; i++)
                for (int j = 0; j < COLUMNAS; j++)
                    if (pulsadorColor[i][j] != RELAY && pressTime[i][j] > 0) {
                        ++count; if (count==1) c1=pulsadorColor[i][j]; else if (count==2) c2=pulsadorColor[i][j];
                    }
            if (count < 2) {
                mixReady = true;
                if (count == 0) {
                    if (!blackSent) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, BLACK));
                        currentActiveColor = BLACK; blackSent = true;
                    }
                } else if (count == 1 && currentActiveColor != c1) {
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, c1));
                    currentActiveColor = c1; blackSent = false;
                }
            } else if (count == 2 && mixReady) {
                byte mixColor;
                if (colorHandler.color_mix_handler(c1, c2, &mixColor)) {
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, mixColor));
                    currentActiveColor = mixColor; blackSent=false; mixReady=false;
                } else {
                    if (!blackSent) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, BLACK));
                        currentActiveColor = BLACK; blackSent=true; mixReady=false;
                    }
                }
            }
        } else {
            unsigned long mt=0; byte newCol=BLACK; bool ok=false;
            for (int i = 0; i < FILAS; i++)
                for (int j = 0; j < COLUMNAS; j++)
                    if (pulsadorColor[i][j] != RELAY && pressTime[i][j] > mt) { mt=pressTime[i][j]; newCol=pulsadorColor[i][j]; ok=true; }
            if (ok && currentActiveColor != newCol) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, newCol));
                currentActiveColor=newCol; blackSent=false;
            } else if (!ok && !blackSent) {
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, ctx.targetType, ctx.targetNS, BLACK));
                currentActiveColor=BLACK; blackSent=true;
            }
        }
    }
}

void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           uint8_t targetType, const TARGETNS& targetNS)
{
    const byte   buttonColor = pulsadorColor[i][j];
    const String currentFile = elementFiles[currentIndex];

    const bool communicatorBroadcast = (currentFile == "Comunicador") &&
                                       (targetType == BROADCAST) && _isNSZero(targetNS);

    static bool    lastWasCom = false;
    static uint8_t lastLegacyColor = 0xFF;
    static bool    sameColorParity = false;

    // 0) Menú Cognitivo (prioritario)
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
    if (currentFile == "Fichas") return;

    // 1-bis) Ruta RESPUESTA (simple y rápida)
    if (event == BUTTON_PRESSED && PulsadoresHandler::isResponseRouteActive()) {
        const int ledIdx = colorToLedIdx(buttonColor);
        if (ledIdx < 0) return;
        if (!PulsadoresHandler::isButtonEnabled((uint8_t)ledIdx)) return;

        auto ledIdxToButtonId = [](uint8_t idx) -> uint8_t {
            static const uint8_t kMap[9] = { 5, 9, 4, 8, 3, 7, 2, 6, 1 };
            return (idx < 9) ? kMap[idx] : 0;
        };
        const uint8_t response = ledIdxToButtonId((uint8_t)ledIdx);
        if (response == 0) return;

        send_frame(frameMaker_SEND_RESPONSE(DEFAULT_BOTONERA, s_respTargetType, getOwnNS(), response));
        return;
    }

    // 2) COMUNICADOR: caso "solo relé" en target concreto
    if (currentFile == "Comunicador") {
        extern TARGETNS communicatorActiveNS;
        const bool comHasTarget = !_isNSZero(communicatorActiveNS);
        bool soloRele = false;

        if (comHasTarget) {
            uint8_t cfg[2] = {0,0}; bool ok=false; _getModeCfgCached(communicatorActiveNS, cfg, ok);
            if (ok) {
                const bool hasCol = getModeFlag(cfg, HAS_BASIC_COLOR) || getModeFlag(cfg, HAS_ADVANCED_COLOR);
                const bool hasRel = getModeFlag(cfg, HAS_RELAY);
                soloRele = (!hasCol && hasRel);
            }
        }

        if (soloRele) {
            if (buttonColor != BLUE && buttonColor != RELAY) return;
            if (buttonColor == BLUE && event == BUTTON_PRESSED && comHasTarget) {
                const bool cur  = RelayStateManager::get(communicatorActiveNS);
                const bool next = !cur;
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, communicatorActiveNS, next ? 0x01 : 0x00));
                RelayStateManager::set(communicatorActiveNS, next);
                return;
            }
        }
    }

    // // 3) AMBIENTES
    // if (currentFile == "Ambientes") {
    //     if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
    //         const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;
    //         send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
    //         delay(100);
    //         if (sendColorByte != BLACK) doitPlayer.play_file(AMBIENTS_BANK, sendColorByte + 1);
    //         else                         doitPlayer.stop_file();
    //         return;
    //     }
    // }

    // 3) AMBIENTES (broadcast patterns cuando está seleccionado)
    if (currentFile == "Ambientes") {
        if (event == BUTTON_PRESSED && isCurrentElementSelected()) {
            const uint8_t sendColorByte = (buttonColor == RELAY) ? BLACK : buttonColor;
            send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, sendColorByte));
            delay(100);

            if (sendColorByte != BLACK) {
                doitPlayer.play_file(AMBIENTS_BANK, sendColorByte + 1);
                ambienteActivo = true;     // ← desde ahora consideramos que hay ambiente “puesto”
            } else {
                doitPlayer.stop_file();
                ambienteActivo = false;    // ← pulsación de RELAY (negro) = ambiente apagado
            }
            return;
        }
    }


    // 4) Capacidades del target (ligero y cacheado).
    //    Para broadcast/comunicador-broadcast no bloqueamos por falta de flags.
    uint8_t mc[2] = {0,0}; bool mcOK = false;
    if (targetType == DEFAULT_DEVICE && !_isNSZero(targetNS)) {
        _getModeCfgCached(targetNS, mc, mcOK);
    } else {
        mcOK = true;
    }

    const bool hasBasic    = mcOK ? getModeFlag(mc, HAS_BASIC_COLOR)     : true; // permisivo en no-device
    const bool hasAdvanced = mcOK ? getModeFlag(mc, HAS_ADVANCED_COLOR)  : true;
    const bool hasPatterns = mcOK ? getModeFlag(mc, HAS_PATTERNS)        : false;

    const bool cfgHasRelay = mcOK ? getModeFlag(mc, HAS_RELAY)           : hasRelay; // si no sabemos, confía en el parámetro
    const bool cfgHasN1    = mcOK ? getModeFlag(mc, HAS_RELAY_N1)        : false;
    const bool cfgHasN2    = mcOK ? getModeFlag(mc, HAS_RELAY_N2)        : false;

    const bool isMultiRelay   = (cfgHasN1 || cfgHasN2);
    const bool isAromaterapia = (!cfgHasRelay && cfgHasN1 && cfgHasN2);

    // 5) MULTIRRELÉ (botones de color)
    int relayIndex = -1;
    if      (buttonColor == BLUE)   relayIndex = 0;
    else if (buttonColor == GREEN)  relayIndex = 1;
    else if (buttonColor == YELLOW) relayIndex = 2;
    else if (buttonColor == RED)    relayIndex = 3;

    if ((isMultiRelay || isAromaterapia) && (relayIndex >= 0)) {
        if (event == BUTTON_PRESSED) {
            uint8_t mask = (1u << relayIndex);
            if (isAromaterapia)   relay_state = (relay_state == mask) ? 0x00 : mask; // exclusivo
            else                  relay_state ^= mask;                               // toggle independiente
            // feedback de color + flags compuestos
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
            delay(20);
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, targetType, targetNS, relay_state));
        }
        return;
    }

    // 6) COMUNICADOR · RELÉ DE CICLO (mantengo tu lógica y timings)
    if (currentFile == "Comunicador" && buttonColor == RELAY) {
        if (event != BUTTON_PRESSED) return;
        if (digitalRead(ENC_BUTTON) == LOW) return;

        const unsigned CONSEC_DELAY_MS   = 100;
        const unsigned FIRST_ON_DELAY_MS = 3000;

        extern TARGETNS communicatorActiveNS;

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

        std::vector<TARGETNS> nsSPIFFS; nsSPIFFS.reserve(elementFiles.size());
        for (const String &f : elementFiles) {
            if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar") continue;
            TARGETNS ns = getNSFromFile(f);
            if (!_isNSZero(ns)) nsSPIFFS.push_back(ns);
        }
        std::sort(nsSPIFFS.begin(), nsSPIFFS.end(), cmpNS);

        if (relayStep < -1 || relayStep >= (int)nsSPIFFS.size()) relayStep = -1;
        if (!nsSPIFFS.empty() && !_isNSZero(communicatorActiveNS)) {
            for (size_t k = 0; k < nsSPIFFS.size(); ++k) if (eqNS(nsSPIFFS[k], communicatorActiveNS)) { relayStep = (int)k; break; }
        }

        bool firstCycleKick=false;
        uint8_t  blackType, whiteType;
        TARGETNS blackNS = NS_ZERO, whiteNS = NS_ZERO;

        if (nsSPIFFS.empty()) {
            blackType = BROADCAST; blackNS = NS_ZERO;
            whiteType = BROADCAST; whiteNS = NS_ZERO;
        } else {
            if (relayStep == -1) {
                blackType = BROADCAST;      blackNS = NS_ZERO;
                whiteType = DEFAULT_DEVICE; whiteNS = nsSPIFFS[0];
                relayStep = 0; firstCycleKick = true;
            } else if (relayStep < (int)nsSPIFFS.size()-1) {
                blackType = DEFAULT_DEVICE; blackNS = nsSPIFFS[relayStep];
                ++relayStep;
                whiteType = DEFAULT_DEVICE; whiteNS = nsSPIFFS[relayStep];
            } else {
                blackType = DEFAULT_DEVICE; blackNS = nsSPIFFS[relayStep];
                whiteType = BROADCAST;      whiteNS = NS_ZERO;
                relayStep = -1;
            }
        }

        if (firstCycleKick) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT));
            delay(FIRST_ON_DELAY_MS);
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD));
            delay(CONSEC_DELAY_MS);
            uint8_t cfgW[2]={0,0}; bool ok=false; _getModeCfgCached(whiteNS, cfgW, ok);
            if (ok && getModeFlag(cfgW, HAS_RELAY)) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01));
                delay(CONSEC_DELAY_MS);
            }
        } else if (whiteType == DEFAULT_DEVICE && blackType == DEFAULT_DEVICE) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, DEFAULT_DEVICE, blackNS, BLACKOUT));
            delay(CONSEC_DELAY_MS);
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD));
            delay(CONSEC_DELAY_MS);
            uint8_t cfgW[2]={0,0}; bool ok=false; _getModeCfgCached(whiteNS, cfgW, ok);
            if (ok && getModeFlag(cfgW, HAS_RELAY)) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01));
                delay(CONSEC_DELAY_MS);
            }
        } else if (whiteType == BROADCAST && blackType == DEFAULT_DEVICE) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, START_CMD));
            delay(CONSEC_DELAY_MS);
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, 0x01));
            delay(CONSEC_DELAY_MS);
        } else if (whiteType == DEFAULT_DEVICE && blackType == BROADCAST) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT));
            delay(FIRST_ON_DELAY_MS);
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, START_CMD));
            delay(CONSEC_DELAY_MS);
            uint8_t cfgW[2]={0,0}; bool ok=false; _getModeCfgCached(whiteNS, cfgW, ok);
            if (ok && getModeFlag(cfgW, HAS_RELAY)) {
                send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, DEFAULT_DEVICE, whiteNS, 0x01));
                delay(CONSEC_DELAY_MS);
            }
        } else { // broadcast→broadcast
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT));
            delay(FIRST_ON_DELAY_MS);
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, START_CMD));
            delay(CONSEC_DELAY_MS);
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, 0x01));
            delay(CONSEC_DELAY_MS);
        }

        ::communicatorActiveNS = (whiteType == DEFAULT_DEVICE) ? whiteNS : NS_ZERO;
        colorHandler.setCurrentFile("Comunicador");
        colorHandler.setPatternBotonera(currentModeIndex, ledManager);
        return;
    }

    // 7) RELÉ ÚNICO
    if (buttonColor == RELAY) {
        if (!cfgHasRelay || isMultiRelay || isAromaterapia) return;
        if (digitalRead(ENC_BUTTON) == LOW) return;
        if (targetType == DEFAULT_DEVICE && _isNSZero(targetNS)) return;

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

    // 8) MODO PASIVO → solo botón AZUL
    if (hasPassive && (buttonColor != BLUE)) return;

    // Validación: si target es dispositivo, NS debe ser válido
    if (targetType == DEFAULT_DEVICE && _isNSZero(targetNS)) return;

    // 9) PATRONES
    if (hasPatterns && event == BUTTON_PRESSED) {
        send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
        return;
    }

    // 10) ADVANCED NO PULSE (mezclas)
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
                lastBasicColor = BLACK; advancedMixed = false;
            } else {
                if (lastBasicColor == BLACK) {
                    lastBasicColor = buttonColor;
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));
                } else {
                    uint8_t mixColor;
                    if (colorHandler.color_mix_handler(lastBasicColor, buttonColor, &mixColor)) {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, mixColor));
                        lastBasicColor = mixColor; advancedMixed = true;
                    } else {
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, BLACK));
                        lastBasicColor = BLACK; advancedMixed = false;
                    }
                }
            }
        }
        return;
    }

    // 11) BÁSICO (no pulse)
    if (hasPulse) {
        // En pulse lo gestiona el escaneo continuo
        return;
    } else {
        const bool allowBasicSend = communicatorBroadcast || hasBasic || hasAdvanced;
        if (allowBasicSend && event == BUTTON_PRESSED && buttonColor != RELAY) {
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, targetType, targetNS, buttonColor));

            // Legacy SOLO en "Comunicador"
            if (currentFile == "Comunicador") {
                if (!lastWasCom) { lastWasCom = true; lastLegacyColor = 0xFF; sameColorParity = false; }
#ifdef LEGACY_COLOR_SUPPORT
                if (lastLegacyColor != buttonColor) { send_old_color(buttonColor); lastLegacyColor = buttonColor; sameColorParity = true; }
                else { if (sameColorParity) { send_old_color(BLACK); sameColorParity = false; } else { send_old_color(buttonColor); sameColorParity = true; } }
#endif
            } else {
                lastWasCom = false;
            }
        }
        return;
    }
}



