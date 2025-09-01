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
    std::vector<byte> target;
    String currentFile = elementFiles[currentIndex];
    

    if (inCognitiveMenu) {
        // Solo responder a pulsadores válidos para actividades cognitivas
        static bool lastState[FILAS][COLUMNAS] = { { false } };
        //static unsigned long pressTime[FILAS][COLUMNAS] = { { 0 } };
    
        for (int i = 0; i < FILAS; i++) {
            digitalWrite(filas[i], LOW);
            delayMicroseconds(10);
            for (int j = 0; j < COLUMNAS; j++) {
                byte color = pulsadorColor[i][j];
                if (color != BLUE && color != GREEN && color != YELLOW && color != RED && color != RELAY)
                    continue;
    
                bool currentPressed = (digitalRead(columnas[j]) == LOW);
                target = { DEFAULT_CONSOLE };
                if (!lastState[i][j] && currentPressed) {
        
                    processButtonEvent(i, j, BUTTON_PRESSED, true, false, true, target);
                }
                if (lastState[i][j] && !currentPressed) {
                    processButtonEvent(i, j, BUTTON_RELEASED, true, false, true, target);
                }
    
                lastState[i][j] = currentPressed;
            }
            digitalWrite(filas[i], HIGH);
        }
        return;  // IMPORTANTE: salir de la función para no ejecutar la lógica normal
    }

    if (currentFile == "Apagar") return;  // ← Ignora todas las pulsaciones si es "Apagar"
    
    // Ignorar pulsaciones si es un elemento dinámico no seleccionado
    if (currentFile != "Ambientes" && currentFile != "Fichas" && !selectedStates[currentIndex]) return;

    bool isFichas = (currentFile == "Fichas");

    

    if (!isFichas){

        /*──── “Ambientes” → broadcast SOLO si el elemento está encendido ────*/
        if (currentFile == "Ambientes"){
            bool ambientesSeleccionado = false;
            for (size_t i = 0; i < elementFiles.size(); ++i){
                if (elementFiles[i] == "Ambientes" && selectedStates[i]){
                    ambientesSeleccionado = true;
                    break;
                }
            }
            if (ambientesSeleccionado){
                target.push_back(BROADCAST);
            }
        }

        /*──── “Comunicador” → SIEMPRE broadcast (modo Básico) ────*/
        else if (currentFile == "Comunicador"){
            target.push_back(communicatorActiveID); // nunca va a SPIFFS
        }

        /*──── CUALQUIER OTRO ELEMENTO ────*/
        else{
            byte elementID = BROADCAST;

            if (currentFile == "Apagar"){
                elementID = apagarSala.ID; // ID fija en RAM
            }
            else{
                /* Elementos almacenados en SPIFFS */
                String path = currentFile.startsWith("/") ? currentFile : "/" + currentFile;
                fs::File f = SPIFFS.open(path, "r");
                if (f){
                    f.seek(OFFSET_ID, SeekSet);
                    f.read(&elementID, 1);
                    f.close();
                }
                else{
                #ifdef DEBUG
                DEBUG__________printf("❌ Error al leer la ID del archivo %s\n", currentFile.c_str());
                #endif
                }
            }
            target.push_back(elementID);
        }
    }

    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig)) {
        DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }

    bool hasPulse        = getModeFlag(modeConfig, HAS_PULSE);
    bool hasPassive      = getModeFlag(modeConfig, HAS_PASSIVE);
    bool hasRelay        = getModeFlag(modeConfig, HAS_RELAY);
    bool hasRelayN1      = getModeFlag(modeConfig, HAS_RELAY_N1);
    bool hasRelayN2      = getModeFlag(modeConfig, HAS_RELAY_N2);
    bool hasAdvanced     = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);

    int relayCount = (hasRelayN1 << 1) | hasRelayN2;
    relayCount = relayCount == 0 && hasRelay ? 1 : relayCount + (hasRelay ? 1 : 0);
    //bool isMultiRelay = relayCount > 1;
    //bool isAromaterapia = (!hasRelay && hasRelayN1 && hasRelayN2);    

    static bool lastState[FILAS][COLUMNAS] = { { false } };
    static unsigned long pressTime[FILAS][COLUMNAS] = { { 0 } };
    static byte currentActiveColor = BLACK;
    static bool blackSent = false;
    static bool mixReady = true;

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
        mixReady = true;
        lastModeIndex = currentModeIndex;
    }

    for (int i = 0; i < FILAS; i++) {
        digitalWrite(filas[i], LOW);
        delayMicroseconds(10);
        for (int j = 0; j < COLUMNAS; j++) {
            bool currentPressed = (digitalRead(columnas[j]) == LOW);
            byte color = pulsadorColor[i][j];

            if (color != RELAY) {
                if (!lastState[i][j] && currentPressed) {
                    pressTime[i][j] = millis();
                } else if (lastState[i][j] && !currentPressed) {
                    pressTime[i][j] = 0;
                }
            }

            if (color == RELAY) currentRelayState |= currentPressed;
            if (color == BLUE) blueButtonState |= currentPressed;

            if (!lastState[i][j] && currentPressed) {
                if (isFichas) {
                    if (color == RELAY) relayButtonPressed = true;
                    else if (color == BLUE) blueButtonPressed = true;
                } else {
                    processButtonEvent(i, j, BUTTON_PRESSED, hasPulse, hasPassive, hasRelay, target);
                }
            }
            if (lastState[i][j] && !currentPressed) {
                if (isFichas) {
                    if (color == RELAY) relayButtonPressed = false;
                    else if (color == BLUE) blueButtonPressed = false;
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

    if (!inModesScreen && hasPulse && !isFichas) {
        if (hasAdvanced) {
            int count = 0;
            byte color1 = BLACK, color2 = BLACK;
            for (int i = 0; i < FILAS; i++) {
                for (int j = 0; j < COLUMNAS; j++) {
                    if (pulsadorColor[i][j] == RELAY) continue;
                    if (pressTime[i][j] > 0) {
                        count++;
                        if (count == 1) color1 = pulsadorColor[i][j];
                        else if (count == 2) color2 = pulsadorColor[i][j];
                    }
                }
            }
            if (count < 2) {
                mixReady = true;
                if (count == 0) {
                    if (!blackSent) {
                        
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                        currentActiveColor = BLACK;
                        blackSent = true;
                    }
                } else if (count == 1 && currentActiveColor != color1) {
                 
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, color1));
                    currentActiveColor = color1;
                    blackSent = false;
                }
            } else if (count == 2 && mixReady) {
                byte mixColor;
                if (colorHandler.color_mix_handler(color1, color2, &mixColor)) {
              
                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, mixColor));
                    currentActiveColor = mixColor;
                    blackSent = false;
                    mixReady = false;
                } else {
                    if (!blackSent) {
             
                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                        currentActiveColor = BLACK;
                        blackSent = true;
                        mixReady = false;
                    }
                }
            }
        } else {
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

std::vector<uint8_t> idsSPIFFS;   // IDs ordenadas               (solo Comunicador)
int  relayStep = -1;              // -1 = BROADCAST encendido
uint8_t communicatorActiveID = 0xFF;   // 0xFF = broadcast

void PulsadoresHandler::processButtonEvent(int i, int j, ButtonEventType event,
                                           bool hasPulse, bool hasPassive, bool hasRelay,
                                           std::vector<byte> &target)
{
    byte buttonColor = pulsadorColor[i][j];

    String currentFile = elementFiles[currentIndex];

    if (inCognitiveMenu) {
        if (event == BUTTON_PRESSED) {
            if (buttonColor != RELAY) {
         
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
    #ifdef DEBUG
                DEBUG__________printf("[COG] COLOR: PRESIÓN → %d\n", buttonColor);
    #endif
            }
            // RELAY se maneja solo en RELEASE
        } 
        else if (event == BUTTON_RELEASED && buttonColor == RELAY) {
            relay_state = 1; // Siempre enviar 1, no toggle
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
    #ifdef DEBUG
            DEBUG__________ln("[COG] RELAY: LIBERACIÓN → ENVÍA 1");
    #endif
        }
        return; // 🚨 SALIR para no ejecutar lógica estándar
    }
    
    
    
    if (currentFile == "Fichas")
        return;

    /*────────  COMUNICADOR · elemento sólo-relé  — filtra botones  ───────*/
    if (currentFile == "Comunicador" && communicatorActiveID != BROADCAST) {

        uint8_t cfg[2] = {0};
        bool soloRele = false;
        if (RelayStateManager::getModeConfigForID(communicatorActiveID, cfg)) {
            bool hasCol = getModeFlag(cfg, HAS_BASIC_COLOR) ||
                        getModeFlag(cfg, HAS_ADVANCED_COLOR);
            bool hasRel = getModeFlag(cfg, HAS_RELAY);
            soloRele    = (!hasCol && hasRel);           // sólo-relé
        }

        /* ——— Caso “solo-relé” ——— */
        if (soloRele) {

            /* Ignorar todo salvo AZUL y RELAY */
            if (buttonColor != BLUE && buttonColor != RELAY) return;

            /* --- 1 · Botón AZUL → toggle ON/OFF --- */
            if (buttonColor == BLUE) {
                if (event == BUTTON_PRESSED) {
                    bool cur  = RelayStateManager::get(communicatorActiveID);
                    bool next = !cur;
                    send_frame(frameMaker_SEND_FLAG_BYTE(
                        DEFAULT_BOTONERA, { communicatorActiveID },
                        next ? 0x01 : 0x00));
                    RelayStateManager::set(communicatorActiveID, next);
                }
                return;                                   // no más lógica para AZUL
            }
        }
    }

    // ---------------------------- AMBIENTES ----------------------------
    if (currentFile == "Ambientes")
    {
        if (event == BUTTON_PRESSED && isCurrentElementSelected())
        {
            byte sendColor = (buttonColor == RELAY) ? BLACK : buttonColor;
            send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, target, sendColor));
            delay(100);
            if (sendColor != BLACK)
                doitPlayer.play_file(8, sendColor + 1);
            else
                doitPlayer.stop_file();
            return;
        }
    }

    // ---------------------------- CARGAR CONFIGURACIÓN ----------------------------
    byte modeConfig[2] = {0};
    if (!getModeConfig(currentFile, currentModeIndex, modeConfig))
    {
        DEBUG__________ln("⚠️ No se pudo obtener la configuración del modo actual.");
        return;
    }

    bool hasBasic    = getModeFlag(modeConfig, HAS_BASIC_COLOR);
    bool hasAdvanced = getModeFlag(modeConfig, HAS_ADVANCED_COLOR);
    bool hasPatterns = getModeFlag(modeConfig, HAS_PATTERNS);
    bool hasRelayN1 = getModeFlag(modeConfig, HAS_RELAY_N1);
    bool hasRelayN2 = getModeFlag(modeConfig, HAS_RELAY_N2);
    bool isMultiRelay = hasRelayN1 || hasRelayN2;
    bool isAromaterapia = (!hasRelay && hasRelayN1 && hasRelayN2);
    
    if ((isAromaterapia || isMultiRelay) && buttonColor != BLUE && buttonColor != GREEN && buttonColor != YELLOW && buttonColor != RED) return;

    
    int relayIndex = -1;
    if (buttonColor == BLUE)
        relayIndex = 0;
    else if (buttonColor == GREEN)
        relayIndex = 1;
    else if (buttonColor == YELLOW)
        relayIndex = 2;
    else if (buttonColor == RED)
        relayIndex = 3;

    // ---------------------------- MULTIRRELÉ (botones de color) ----------------------------
    if ((isMultiRelay || isAromaterapia) && relayIndex >= 0 && relayIndex <= 3)
    {
        if (event == BUTTON_PRESSED)
        {
            byte mask = (1 << relayIndex);

            if (isAromaterapia) {
                if (relay_state == mask) {
                    // Si el relé ya estaba activo, apágalo
                    relay_state = 0x00;
                } else {
                    // Activa solo este relé (exclusivo)
                    relay_state = mask;
                }
            } else {
                // Toggle independiente
                relay_state ^= mask;
            }
     
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor)); // Color asociado al botón
            delay(20);
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));

#ifdef DEBUG
            DEBUG__________printf("[RELAY %s] Botón color activado - Index: %d → Estado: 0x%02X\n",
                          isAromaterapia ? "AROMATERAPIA" : "MULTI",
                          relayIndex, relay_state);
#endif
        }
        return;
    }

// ----------------------- COMUNICADOR · RELÉ DE “CICLO” -----------------------


if (currentFile == "Comunicador" && buttonColor == RELAY)
{
    if (event != BUTTON_PRESSED) return;                 // solo actuamos en PRESSED
    if (digitalRead(ENC_BUTTON) == LOW)  return;         // no interferir con encoder

    /* 1 ─ Construir la lista de IDs (una sola vez) */
    if (idsSPIFFS.empty()) {
        for (const String &f : elementFiles) {
            if (f == "Ambientes" || f == "Fichas" ||
                f == "Comunicador" || f == "Apagar") continue;

            fs::File fi = SPIFFS.open(f.startsWith("/") ? f : "/" + f, "r");
            if (fi) {
                uint8_t id;
                fi.seek(OFFSET_ID, SeekSet);
                fi.read(&id, 1);
                fi.close();
                idsSPIFFS.push_back(id);
            }
        }
        std::sort(idsSPIFFS.begin(), idsSPIFFS.end());
    }

    /* ---------- helper: envía COLOR o FLAG según capacidades del ID ---------- */
    auto sendColorOrRelay = [&](uint8_t id, bool on)
    {
        if (id == BROADCAST) {

            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, { id }, on ? WHITE : BLACK));
            return;
        }

        /* localizar elemento con esa ID */
        String fileFound = "";
        bool isStatic = false;
        for (const String &f : elementFiles) {
            uint8_t fid;
            if (f == "Ambientes" || f == "Fichas" || f == "Comunicador" || f == "Apagar") {
                INFO_PACK_T* opt =
                      (f == "Ambientes")   ? &ambientesOption   :
                      (f == "Fichas")      ? &fichasOption      :
                      (f == "Comunicador") ? &comunicadorOption :
                                             &apagarSala;
                fid      = opt->ID;
                isStatic = true;
            } else {
                fs::File tf = SPIFFS.open(f.startsWith("/") ? f : "/" + f, "r");
                if (!tf) continue;
                tf.seek(OFFSET_ID, SeekSet);
                tf.read(&fid, 1);
                tf.close();
            }
            if (fid == id) { fileFound = f; break; }
        }

        if (fileFound == "") {

            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, { id }, on ? WHITE : BLACK));
            return;
        }

        /* leer modeConfig actual */
        uint8_t mCfg[2] = {0};
        if (isStatic) {
            INFO_PACK_T* opt =
                  (fileFound == "Ambientes")   ? &ambientesOption   :
                  (fileFound == "Fichas")      ? &fichasOption      :
                  (fileFound == "Comunicador") ? &comunicadorOption :
                                                 &apagarSala;
            memcpy(mCfg, opt->mode[opt->currentMode].config, 2);
        } else {
            fs::File tf = SPIFFS.open(fileFound, "r");
            if (tf) {
                uint8_t cur;
                tf.seek(OFFSET_CURRENTMODE, SeekSet);
                tf.read(&cur, 1);
                tf.seek(OFFSET_MODES + cur * SIZE_MODE + 216, SeekSet);
                tf.read(mCfg, 2);
                tf.close();
            }
        }

        bool hasColor  = getModeFlag(mCfg, HAS_BASIC_COLOR) || getModeFlag(mCfg, HAS_ADVANCED_COLOR);
        bool hasRelayF = getModeFlag(mCfg, HAS_RELAY);

        if (hasColor) {

            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, { id }, on ? WHITE : BLACK));
        } else if (hasRelayF) {
            send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, { id }, on ? 0x01 : 0x00));
        } else {

            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, { id }, on ? WHITE : BLACK));
        }
    };

    /* 2 ─ Determinar ID negro / blanco */
    uint8_t idBlack, idWhite;
    if (idsSPIFFS.empty()) {                // sin elementos → broadcast ↔ broadcast
        idBlack = idWhite = BROADCAST;
    } else {
        if (relayStep == -1) {              // primer toque
            idBlack  = BROADCAST;
            idWhite  = idsSPIFFS[0];
            relayStep = 0;
        }
        else if (relayStep < (int)idsSPIFFS.size() - 1) {
            idBlack  = idsSPIFFS[relayStep];
            ++relayStep;
            idWhite  = idsSPIFFS[relayStep];
        }
        else {                              // último ID → vuelta a broadcast
            idBlack  = idsSPIFFS[relayStep];
            idWhite  = BROADCAST;
            relayStep = -1;
        }
    }

    /* 3 ─ Enviar frames con lógica COLOR / RELAY */
    sendColorOrRelay(idBlack, false);   // OFF / Negro
    delay(30);
    sendColorOrRelay(idWhite, true);    // ON  / Blanco

    communicatorActiveID = idWhite;     // botones de color apuntan aquí

    colorHandler.setCurrentFile("Comunicador");
    colorHandler.setPatternBotonera(currentModeIndex, ledManager);

#ifdef DEBUG
    DEBUG__________printf("[COMUNICADOR-RELAY] BLACK→0x%02X  WHITE→0x%02X  paso=%d\n",
                          idBlack, idWhite, relayStep);
#endif
    return;    // no continuar con lógica estándar
}

    // ---------------------------- RELÉ ÚNICO (botón RELAY) ----------------------------
if (buttonColor == RELAY) {
    // Sólo relé simple, no multirrelé ni aromaterapia
    if (!hasRelay || isMultiRelay || isAromaterapia) 
        return;
    // Evita interferir con el encoder
    if (digitalRead(ENC_BUTTON) == LOW) 
        return;

    uint8_t id = target.empty() ? BROADCAST : target[0];

    if (event == BUTTON_PRESSED) {
        if (hasPulse) {
            // MODO PULSE: al presionar → encender (1)
            send_frame(frameMaker_SEND_FLAG_BYTE(
                DEFAULT_BOTONERA,
                { id },
                0x01
            ));
            // Actualizamos el gestor a ON
            RelayStateManager::set(id, true);
        } else {
            // MODO BÁSICO: toggle contra el estado guardado
            bool cur  = RelayStateManager::get(id);
            bool next = !cur;
            send_frame(frameMaker_SEND_FLAG_BYTE(
                DEFAULT_BOTONERA,
                { id },
                next ? 0x01 : 0x00
            ));
            RelayStateManager::set(id, next);
        }

    #ifdef DEBUG
        DEBUG__________printf("RELAY %s: PRESIÓN → Elemento %d, estado %d→%d\n",
            hasPulse ? "PULSE" : "BÁSICO",
            id, hasPulse ? 0 : RelayStateManager::get(id),
            hasPulse ? 1 : RelayStateManager::get(id)
        );
    #endif

    }
    else if (event == BUTTON_RELEASED && hasPulse) {
        // MODO PULSE: al soltar → apagar (0)
        send_frame(frameMaker_SEND_FLAG_BYTE(
            DEFAULT_BOTONERA,
            { id },
            0x00
        ));
        // Actualizamos el gestor a OFF
        RelayStateManager::set(id, false);

    #ifdef DEBUG
        DEBUG__________printf("RELAY PULSE: LIBERACIÓN → Elemento %d, estado 1→0\n", id);
    #endif
    }
    return;
}


//     // ---------------------------- RELÉ ÚNICO (botón RELAY) ----------------------------
//     if (buttonColor == RELAY)
//     {
//         if (!hasRelay || isMultiRelay || isAromaterapia)
//             return;
//         if (digitalRead(ENC_BUTTON) == LOW)
//             return;

//         if (event == BUTTON_PRESSED)
//         {
//             relayButtonPressed = true;
//             if (hasPulse)
//             {
//                 relay_state = 1;
//                 send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
// #ifdef DEBUG
//                 DEBUG__________ln("RELAY PULSE: PRESIÓN, enviando relay_state = TRUE");
// #endif
//             }
//             else
//             {
//                 if (currentFile != "Ambientes" && currentFile != "Fichas" && currentFile != "Apagar")
//                 {
//                     relay_state = !relay_state;
//                     send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
// #ifdef DEBUG
//                     DEBUG__________printf("RELAY BÁSICO: PRESIÓN, toggle relay_state a %d\n", relay_state);
// #endif
//                 }
//             }
//         }
//         else if (event == BUTTON_RELEASED && hasPulse)
//         {
//             relayButtonPressed = false;
//             relay_state = 0;
//             send_frame(frameMaker_SEND_FLAG_BYTE(DEFAULT_BOTONERA, target, relay_state));
// #ifdef DEBUG
//             DEBUG__________ln("RELAY PULSE: LIBERACIÓN, enviando relay_state = FALSE");
// #endif
//         }
//         return;
//     }

    // ---------------------------- MODO PASIVO: SOLO AZUL ----------------------------
    if (hasPassive && (buttonColor != BLUE))
        return;
    if (target.empty())
        return;

    // ---------------------------- PATRONES ----------------------------
    if (hasPatterns && event == BUTTON_PRESSED)
    {
        send_frame(frameMaker_SEND_PATTERN_NUM(DEFAULT_BOTONERA, target, buttonColor));
        return;
    }

    // ---------------------------- ADVANCED NO PULSE ----------------------------
    if (hasAdvanced && !hasPulse)
    {
        static int lastModeForAdvanced = -1;
        static bool advancedMixed = false;

        if (currentModeIndex != lastModeForAdvanced)
        {
            lastBasicColor = BLACK;
            advancedMixed = false;
            lastModeForAdvanced = currentModeIndex;
#ifdef DEBUG
            DEBUG__________ln("ADVANCED NON-PULSE: Cambio de modo detectado. Reiniciando estado avanzado.");
#endif
        }

        if (event == BUTTON_PRESSED)
        {
            if (advancedMixed)
            {
   
                send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                lastBasicColor = BLACK;
                advancedMixed = false;
#ifdef DEBUG
                DEBUG__________ln("ADVANCED NON-PULSE: Nueva pulsación tras mezcla, enviando Negro.");
#endif
            }
            else
            {
                if (lastBasicColor == BLACK)
                {
                    lastBasicColor = buttonColor;

                    send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
#ifdef DEBUG
                    DEBUG__________printf("ADVANCED NON-PULSE: Primer botón, enviando color %d\n", buttonColor);
#endif
                }
                else
                {
                    byte mixColor;
                    if (colorHandler.color_mix_handler(lastBasicColor, buttonColor, &mixColor))
                    {

                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, mixColor));
                        lastBasicColor = mixColor;
                        advancedMixed = true;
#ifdef DEBUG
                        DEBUG__________printf("ADVANCED NON-PULSE: Enviando color mezcla %d\n", mixColor);
#endif
                    }
                    else
                    {

                        send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, BLACK));
                        lastBasicColor = BLACK;
                        advancedMixed = false;
#ifdef DEBUG
                        DEBUG__________ln("ADVANCED NON-PULSE: Combinación no definida, enviando Negro.");
#endif
                    }
                }
            }
        }
        return;
    }

    // ---------------------------- MODO BÁSICO ----------------------------
    if (hasPulse)
    {
#ifdef DEBUG
        DEBUG__________ln("COLOR: Evento en modo PULSE, envío diferido al escaneo.");
#endif
        return;
    }
    else
    {
       
        if ((hasBasic || hasAdvanced) && event == BUTTON_PRESSED)
        {
       
            send_frame(frameMaker_SEND_COLOR(DEFAULT_BOTONERA, target, buttonColor));
#ifdef DEBUG
            DEBUG__________printf("COLOR BÁSICO: PRESIÓN, enviando color %d\n", buttonColor);
#endif
        }
        return;
    }
}
