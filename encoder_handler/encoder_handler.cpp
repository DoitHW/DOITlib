#include <encoder_handler/encoder_handler.h>
#include <Frame_DMS/Frame_DMS.h>
#include <SPIFFS_handler/SPIFFS_handler.h>
#include <display_handler/display_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>
#include <Colors_DMS/Color_DMS.h>
#include <botonera_DMS/botonera_DMS.h>
#include <DynamicLEDManager_DMS/DynamicLEDManager_DMS.h>
#include <ADXL345_handler/ADXL345_handler.h>
#include <microphone_DMS/microphone_DMS.h>
#include <Translations_handler/translations.h>
#include <token_DMS/token_DMS.h>
#include <RelayManager_DMS/RelayStateManager.h>

#define MODE_BACK -2
// Variables globales para el manejo del encoder
ESP32Encoder encoder;
int32_t lastEncoderValue = 0;
int currentIndex = 0;
unsigned long buttonPressStart = 0; // Marca el inicio de la pulsación larga
bool isLongPress = false;          // Bandera para la pulsación larga
bool inModesScreen = false;  
int currentModeIndex = 0;  // Índice del modo actual dentro del menú MODOS
int totalModes = 0;    
unsigned long buttonReleaseTime = 0;  // Track when button is released
bool modeScreenEnteredByLongPress = false;  // Flag to track how modes screen was entered
bool longPressDetected = false;
std::vector<String> elementFiles;
std::vector<char> selectedStates;
int globalVisibleModesMap[17] = {0};  // Definición e inicialización 

unsigned long lastDisplayInteraction = 0; // Última vez que se interactuó con la pantalla
bool displayOn = true;                    // Estado de la pantalla (encendida por defecto)
unsigned long encoderIgnoreUntil = 0; // Tiempo hasta el cual se ignoran las entradas del encoder

// Variables para el submenú de selección de idioma
bool languageMenuActive = false;
int languageMenuSelection = 0;  // Índice de la opción seleccionada (0 a 5)
extern TOKEN_ token;
bool bankSelectionActive = false;
std::vector<byte> bankList;
std::vector<bool> selectedBanks;

int bankMenuCurrentSelection = 0;   // 0: Confirmar, 1..n: banks
int bankMenuWindowOffset = 0;       // Índice del primer elemento visible en la ventana
int bankMenuVisibleItems = 4;

bool soundMenuActive = false;
int soundMenuSelection = 0;

byte selectedVoiceGender = 0; // 0 = Mujer, 1 = Hombre
bool negativeResponse = true;
byte selectedVolume = 0; // 0 = Normal, 1 = Atenuado

bool formatSubMenuActive = false;
int formatMenuSelection = 0;

/**
 * @brief Inicializa el encoder rotativo y su pulsador asociado.
 * 
 * Configura el pin del botón del encoder con resistencia pull-up interna,
 * habilita resistencias internas débiles para las señales del encoder en el ESP32
 * y realiza la configuración inicial de la librería `ESP32Encoder`.
 * 
 * @pre Las constantes `ENC_BUTTON`, `ENC_A` y `ENC_B` deben estar definidas con pines válidos.
 * @pre La instancia global `encoder` debe estar declarada y accesible.
 * @note Se utiliza `attachSingleEdge()` con orden de pines invertido respecto al original.
 * @warning El orden de pines en `attachSingleEdge(ENC_B, ENC_A)` puede invertir el sentido de conteo.
 */

void encoder_init_func() noexcept
{
    // Constantes para configuración del filtro del encoder
    constexpr uint16_t kEncoderFilterValue = 1023; // Máximo filtrado por hardware

    // Configuración del pin del pulsador del encoder
    pinMode(ENC_BUTTON, INPUT_PULLUP);

    // Habilitar resistencias internas débiles para señales de encoder en el ESP32
    ESP32Encoder::useInternalWeakPullResistors = UP;

    // Adjuntar el encoder en modo Single Edge (captura un flanco por paso)
    // Nota: El orden ENC_B, ENC_A invertirá el sentido respecto al orden habitual
    encoder.attachSingleEdge(ENC_B, ENC_A);

    // Inicializar contador del encoder
    encoder.clearCount();
    encoder.setCount(0);

    // Aplicar filtrado por hardware para reducir rebotes
    encoder.setFilter(kEncoderFilterValue);
}


bool ignoreInputs = false;
bool ignoreEncoderClick = false;
bool systemLocked = false;


unsigned long lastFocusChangeTime = 0;
int lastQueriedElementIndex = -1;

unsigned long lastModeQueryTime = 0;
int pendingQueryIndex = -1;
TARGETNS pendingQueryNS= {0,0,0,0,0}; 
bool awaitingResponse = false;

static inline bool isSpiffsPath(const String& s) {
    return s.length() > 0 && s[0] == '/';
}

// encoder_handler.cpp (ámbito de archivo)
static inline bool isSpecialFile(const String& name) noexcept {
    return (name == "Ambientes" || name == "Fichas" ||
            name == "Comunicador" || name == "Apagar" || name == "Dado");
}


/**
 * @brief Gestiona el encoder rotativo y su pulsador para navegación de menús y acciones.
 *
 * Orquesta el despertar de la pantalla, bloqueo/desbloqueo del sistema, navegación por
 * elementos y modos, selección de idioma, y consultas diferidas a elementos. Incluye
 * manejo de pulsación corta/larga y tiempos de espera/ignorados.
 *
 * @return void
 *
 * @pre
 *  - `encoder_init_func()` ya ejecutada y `ESP32Encoder` operativo.
 *  - Pines `ENC_BUTTON`, señales del encoder y SPIFFS correctamente inicializados.
 *  - Variables/estados globales válidos: `encoder`, `lastEncoderValue`, `displayOn`,
 *    `inCognitiveMenu`, `bankSelectionActive`, `inModesScreen`, `elementFiles`,
 *    `selectedStates`, `currentIndex`, `totalModes`, `globalVisibleModesMap`, etc.
 *
 * @note
 *  - Pulsador activo en nivel bajo (`LOW` = pulsado).
 *  - Tiempos usados (ms): ignorar tras despertar (500), pulsación corta (<500),
 *    alternativo en modos (≥2000), detalles (≥6000), ventana de bloqueo (500–5000),
 *    tiempo de consulta diferida (100), timeout de respuesta (500).
 *
 * @warning Función pensada para invocarse de forma periódica en el bucle principal.
 * @see drawCurrentElement(), drawModesScreen(), handleBankSelectionMenu()
 */
void handleEncoder() noexcept
{
    // ---------------------------
    // Constantes de temporización
    // ---------------------------
    constexpr unsigned long kIgnoreAfterWakeMs      = 500UL;
    constexpr unsigned long kShortPressMaxMs        = 500UL;
    constexpr unsigned long kAltModeLongPressMs     = 2000UL;
    constexpr unsigned long kDetailsLongPressMs     = 6000UL;
    constexpr unsigned long kUnlockMinMs            = 500UL;
    constexpr unsigned long kUnlockMaxMs            = 5000UL;
    constexpr unsigned long kFocusQueryDelayMs      = 200UL;
    constexpr unsigned long kResponseTimeoutMs      = 500UL;
    constexpr int           kNumLanguages           = 8;
    constexpr int           kMaxModesPerFile        = 16;
    constexpr int           kModeNameLen            = 24;

    // Target Types y NS "cero"
    constexpr uint8_t TARGET_TYPE_BOTONERA = DEFAULT_BOTONERA; // 0xDB
    constexpr uint8_t TARGET_TYPE_DEVICE   = DEFAULT_DEVICE;   // 0xDD
    constexpr uint8_t TARGET_TYPE_BCAST    = BROADCAST;        // 0xFF
    const TARGETNS ZERO_NS{0,0,0,0,0};

#ifdef DEBUG
    if (elementFiles.size() == 0) return;
    if (currentIndex >= (int)elementFiles.size()) {
        DEBUG__________printf("⚠️ currentIndex fuera de rango (%d), reajustando a 0\n", currentIndex);
    }
#endif
    if (elementFiles.size() == 0) return;
    if ((size_t)currentIndex >= elementFiles.size()) {
        currentIndex = 0;
    }

    auto isButtonPressed = []() -> bool { return digitalRead(ENC_BUTTON) == LOW; };
    
    
    // === Helper: aplicado inmediato de CMODE en UI + SPIFFS ===
    auto applyModeImmediate = [&](const String& path, uint8_t cmode) {
        // 1) Persistir CMODE (SPIFFS o RAM)
        if (isSpiffsPath(path)) {
            fs::File f = SPIFFS.open(path, "r+");
            if (f) {
                if (f.seek(OFFSET_CURRENTMODE, SeekSet)) {
                    (void)f.write(&cmode, 1);
                }
                f.close();
            }
        } else {
            if      (path == "Ambientes")   ambientesOption.currentMode   = cmode;
            else if (path == "Fichas")      fichasOption.currentMode      = cmode;
            else if (path == "Apagar")      apagarSala.currentMode        = cmode;
            else if (path == "Comunicador") comunicadorOption.currentMode = cmode;
            else if (path == "Dado")        dadoOption.currentMode        = cmode;
        }

        // 2) Marcar seleccionado si cmode != 0
        if ((size_t)currentIndex < selectedStates.size()) {
            selectedStates[currentIndex] = (cmode != 0);
        }

        // 3) Leer flags del modo y aplicarlos (solo si tienes archivo)
        byte modeConfig[2] = {0};
        if (isSpiffsPath(path)) {
            fs::File f2 = SPIFFS.open(path, "r");
            if (f2) {
                const size_t cfgOffset =
                    (size_t)OFFSET_MODES +
                    (size_t)cmode * (size_t)SIZE_MODE +
                    (size_t)216; // kOffsetConfigInMode (mismo que usas más arriba)
                if (f2.seek(cfgOffset, SeekSet)) {
                    (void)f2.read(modeConfig, 2);
                }
                f2.close();
            }
        }
        adxl   = getModeFlag(modeConfig, HAS_SENS_VAL_1);
        useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

        // 4) Aplicar patrón y refrescar UI YA
        colorHandler.setCurrentFile(path);
        colorHandler.setPatternBotonera(cmode, ledManager);
        drawCurrentElement();
    };

    // 1) Ignorar click residual (hasta soltar)
    if (ignoreEncoderClick) {
        if (!isButtonPressed()) {
            ignoreEncoderClick = false;
        } else {
            return;
        }
    }

    // 2) Si la pantalla está apagada, solo despertar con acción real
    if (!displayOn) {
        if ((encoder.getCount() != lastEncoderValue) || isButtonPressed()) {
            display_wakeup();
            encoderIgnoreUntil     = millis() + kIgnoreAfterWakeMs;
            lastDisplayInteraction = millis();
            lastEncoderValue       = encoder.getCount();
        }
        return;
    }

    // 3) Menú cognitivo
    if (inCognitiveMenu) {
        static bool clicked = false;
        if (isButtonPressed()) {
            if (!clicked) {
                inCognitiveMenu    = false;
                clicked            = true;
                ignoreEncoderClick = true;
                // Enviar comando COG_ACT_OFF a la propia BOTONERA (destino tipo 0xDB, NS=00000)
                send_frame(frameMaker_SEND_COMMAND(
                    DEFAULT_BOTONERA, TARGET_TYPE_BOTONERA, ZERO_NS, COG_ACT_OFF
                ));
                drawCurrentElement();
            }
        } else {
            clicked = false;
        }
        return;
    }

    // 4) Salidas tempranas en otros menús
    if (confirmRestoreMenuActive) return;
    if (deleteElementMenuActive ) return;
    if (formatSubMenuActive    ) return;
    if (soundMenuActive        ) return;
    if (brightnessMenuActive   ) return;
    if (ignoreInputs           ) return;
    if (confirmEnableDadoActive) { handleConfirmEnableDadoMenu(); return; }
    if (extraElementsMenuActive) { handleExtraElementsMenu(); return; }

    // 5) Menú selección de bancos
    if (bankSelectionActive) {
        handleBankSelectionMenu(bankList, selectedBanks);
        return;
    }

    // 6) Ignorar entradas tras despertar pantalla
    if (millis() < encoderIgnoreUntil) {
        lastDisplayInteraction = millis();
        return;
    }

    // 7) Mientras esté bloqueado, solo marcar tiempo para desbloquear al soltar
    bool lockedMain = isInMainMenu() && systemLocked;
    if (lockedMain && isButtonPressed()) {
        if (buttonPressStart == 0) buttonPressStart = millis();
        return;
    }

    // 8) Menú de idiomas (solo si no está bloqueado)
    if (languageMenuActive && !lockedMain) {
        const int32_t newCount = encoder.getCount();
        if (newCount != lastEncoderValue) {
            lastDisplayInteraction = millis();
            const int32_t dir      = (newCount > lastEncoderValue) ? 1 : -1;
            lastEncoderValue       = newCount;
            languageMenuSelection  = (languageMenuSelection + dir + kNumLanguages) % kNumLanguages;
            drawLanguageMenu(languageMenuSelection);
        }
        if (isButtonPressed()) {
            if (buttonPressStart == 0) buttonPressStart = millis();
        } else if (buttonPressStart > 0) {
            switch (languageMenuSelection) {
                case 0: currentLanguage = Language::ES;    break;
                case 1: currentLanguage = Language::ES_MX; break;
                case 2: currentLanguage = Language::CA;    break;
                case 3: currentLanguage = Language::EU;    break;
                case 4: currentLanguage = Language::FR;    break;
                case 5: currentLanguage = Language::DE;    break;
                case 6: currentLanguage = Language::EN;    break;
                case 7: currentLanguage = Language::IT;    break;
                default: currentLanguage = Language::X1;   break;
            }
            saveLanguageToSPIFFS(currentLanguage);
            languageMenuActive = false;
            buttonPressStart   = 0;
            drawCurrentElement();
        }
        return;
    }

    // 9) Navegación por giro (solo si no está bloqueado)
    const int32_t newEncoderValue = encoder.getCount();
    if (!lockedMain && newEncoderValue != lastEncoderValue) {
        lastDisplayInteraction = millis();
        const int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue        = newEncoderValue;

        if (!inModesScreen && elementFiles.size() > 1) {
            PulsadoresHandler::clearResponseRoute();
            // Cambio de elemento
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();
            lastFocusChangeTime      = millis();
            lastQueriedElementIndex  = -1;

            String currentFile = elementFiles[currentIndex];
            static String lastElementFile = "";
            if (currentFile != lastElementFile) {
                if (elementAlternateStates.count(currentFile)) {
                    currentAlternateStates = elementAlternateStates[currentFile];
                } else {
                    currentAlternateStates.clear();
                }
                lastElementFile = currentFile;
            }

            // Extraer configuración del modo actual para flags (igual que antes)
            int  realModeIndex = 0;
            byte modeConfig[2] = {0};

            if (isSpecialFile(currentFile))
            {
                INFO_PACK_T* opt = nullptr;
                if      (currentFile == "Ambientes")   opt = &ambientesOption;
                else if (currentFile == "Fichas")      opt = &fichasOption;
                else if (currentFile == "Comunicador") opt = &comunicadorOption;
                else if (currentFile == "Apagar")      opt = &apagarSala;
                else if (currentFile == "Dado")        opt = &dadoOption;

                realModeIndex = opt->currentMode;
                memcpy(modeConfig, opt->mode[realModeIndex].config, 2);
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    realModeIndex = f.read();
                    f.seek(OFFSET_MODES + realModeIndex * SIZE_MODE + 216, SeekSet);
                    f.read(modeConfig, 2);
                    f.close();
                }
            }

            // Flags de sensores
            adxl   = getModeFlag(modeConfig, HAS_SENS_VAL_1);
            useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

            if (isSpecialFile(currentFile)) {
                // Para RAM: Ambientes, Fichas, Comunicador, Apagar, Dado
                colorHandler.setCurrentFile(currentFile);
                colorHandler.setPatternBotonera(realModeIndex, ledManager);
            }

            // Redibujo del elemento actual
            drawCurrentElement();

            // Consulta de flags del elemento si tiene relé (vía NS)
            const TARGETNS ns = getCurrentElementNS();
            if (RelayStateManager::hasRelay(ns)) {
                send_frame(frameMaker_REQ_ELEM_SECTOR(
                    DEFAULT_BOTONERA,
                    TARGET_TYPE_DEVICE,
                    ns,
                    (byte) currentLanguage,
                    ELEM_CURRENT_FLAGS_SECTOR
                ));
            }
        }
        else if (inModesScreen && totalModes > 0) {
            // Cambio de modo
            const int newIndex = currentModeIndex + direction;
            if (newIndex >= 0 && newIndex < totalModes) {
                currentModeIndex = newIndex;
                const int realModeIndex = globalVisibleModesMap[currentModeIndex];
                if (realModeIndex >= 0) {
                    String file = elementFiles[currentIndex];
                    colorHandler.setCurrentFile(file);
                    colorHandler.setPatternBotonera(realModeIndex, ledManager);
                }
                drawModesScreen();
            }
        }
    }

    // 9.5) Envío diferido de consulta de MODO tras 100 ms de foco (vía NS)
    if ((millis() - lastFocusChangeTime > kFocusQueryDelayMs) &&
        lastQueriedElementIndex != currentIndex)
    {
        String currentFile = elementFiles[currentIndex];

        TARGETNS nsToQuery{0,0,0,0,0};
        bool     shouldQuery = false;

        if (!isSpecialFile(currentFile)) {
            nsToQuery   = getCurrentElementNS();   // SPIFFS
            shouldQuery = true;
        } else if (currentFile == "Dado") {
            Serial1.write(0xDA);
            delay(300UL);
            nsToQuery   = getCurrentElementNS();   // Dado en RAM
            shouldQuery = true;
        }

        if (shouldQuery) {
            pendingQueryNS = nsToQuery;   // ← el NS al que preguntas el CMODE
            awaitingResponse = true;

            send_frame(frameMaker_REQ_ELEM_SECTOR(
                DEFAULT_BOTONERA,
                TARGET_TYPE_DEVICE,
                nsToQuery,
                (byte) currentLanguage,
                ELEM_CMODE_SECTOR
            ));

            lastModeQueryTime       = millis();
            pendingQueryIndex       = currentIndex;
            awaitingResponse        = true;
            lastQueriedElementIndex = currentIndex;
            frameReceived           = false;

            // (Cambio de variable global a NS)
            pendingQueryNS = nsToQuery; // <<< ver cambios de cabecera más abajo
        }
    }

    // === Timeout de respuesta de CMODE (500 ms) ===
    if (awaitingResponse && (millis() - lastModeQueryTime > kResponseTimeoutMs)) {
        if (pendingQueryIndex >= 0 && pendingQueryIndex < (int)elementFiles.size()) {
            const String &path = elementFiles[pendingQueryIndex];

            // 1) Forzar NO SELECCIONADO en la lista visible
            if ((size_t)pendingQueryIndex < selectedStates.size()) {
                selectedStates[pendingQueryIndex] = false;
            }

            // 2) Persistir CMODE = 0
            if (isSpiffsPath(path)) {
                fs::File f = SPIFFS.open(path, "r+");
                if (f) {
                    if (f.seek(OFFSET_CURRENTMODE, SeekSet)) {
                        const byte zero = 0;
                        (void)f.write(&zero, 1);
                    } else {
                        DEBUG__________ln("⚠️ Timeout: no se pudo posicionar en OFFSET_CURRENTMODE para escribir 0.");
                    }
                    f.close();
                } else {
                    DEBUG__________printf("⚠️ Timeout: no se pudo abrir %s para forzar CMODE=0.\n", path.c_str());
                }
            } else {
                // Elementos en RAM: reflejar CMODE=0 en su struct
                if      (path == "Ambientes")   ambientesOption.currentMode   = 0;
                else if (path == "Fichas")      fichasOption.currentMode      = 0;
                else if (path == "Apagar")      apagarSala.currentMode        = 0;
                else if (path == "Comunicador") comunicadorOption.currentMode = 0;
                else if (path == "Dado")        dadoOption.currentMode        = 0;
            }

            // 3) Si es el elemento enfocado, apagar patrón y refrescar YA
            if (pendingQueryIndex == currentIndex) {
                // CMODE=0 => pattern apagado; flags de sensores a 0 por coherencia
                adxl   = false;
                useMic = false;

                colorHandler.setCurrentFile(path);
                colorHandler.setPatternBotonera(0, ledManager); // apaga LEDs (Mode 0 - 0x0000)
                drawCurrentElement();                           // quita el resaltado/etiqueta de modo
            }

            DEBUG__________printf("⚠️ Elemento %s no respondió en %lu ms → NO SELECCIONADO\n",
                                path.c_str(), (unsigned long)kResponseTimeoutMs);
        }

        // 4) Cerrar la espera (no tocar frameReceived: lo gobierna onUartReceive)
        awaitingResponse = false;
    }


    // 10) Lectura del botón mantenido (solo si no está bloqueado)
    if (!lockedMain && isButtonPressed()) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        } else {
            const unsigned long held = millis() - buttonPressStart;

            // Pulsación larga: imprimir detalles al pasar 6000 ms
            if (isInMainMenu() && !isLongPress && held >= kDetailsLongPressMs) {
                printElementDetails();
                isLongPress = true;
                return;
            }

            // Pulsación larga en modos (2 s): alternar modo alternativo
            if (inModesScreen && !isLongPress && held >= kAltModeLongPressMs) {
                if (currentModeIndex > 0 && currentModeIndex < totalModes - 1) {
                    const int adjustedIndex = currentModeIndex - 1;
                    String currFile = elementFiles[currentIndex];
                    uint8_t modeConfig[2] = {0};
                    bool canToggle = false;

                    if (currFile == "Ambientes" || currFile == "Fichas") {
                        INFO_PACK_T* option = (currFile == "Ambientes") ? &ambientesOption : &fichasOption;
                        int count = 0;
                        for (int i = 0; i < kMaxModesPerFile; i++) {
                            if (strlen((char*)option->mode[i].name) > 0 &&
                                checkMostSignificantBit(option->mode[i].config)) {
                                if (count == adjustedIndex) {
                                    memcpy(modeConfig, option->mode[i].config, 2);
                                    break;
                                }
                                count++;
                            }
                        }
                        canToggle = getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE);
                    }
                    else if (!isSpecialFile(currFile)) {
                        fs::File f = SPIFFS.open(currFile, "r");
                        if (f) {
                            int count = 0;
                            for (int i = 0; i < kMaxModesPerFile; i++) {
                                char modeName[kModeNameLen + 1] = {0};
                                byte tempConfig[2] = {0};
                                f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                                f.read((uint8_t*)modeName, kModeNameLen);
                                f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                                f.read(tempConfig, 2);
                                if (strlen(modeName) > 0 &&
                                    checkMostSignificantBit(tempConfig)) {
                                    if (count == adjustedIndex) {
                                        memcpy(modeConfig, tempConfig, 2);
                                        break;
                                    }
                                    count++;
                                }
                            }
                            f.close();
                            canToggle = getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE);
                        }
                    }

                    if (canToggle &&
                        adjustedIndex >= 0 &&
                        currentAlternateStates.size() > (size_t)adjustedIndex)
                    {
                        currentAlternateStates[adjustedIndex] = !currentAlternateStates[adjustedIndex];
                        elementAlternateStates[currFile] = currentAlternateStates;

                        if (!isSpecialFile(currFile) && currFile != "Comunicador") {
                            fs::File f = SPIFFS.open(currFile, "r+");
                            if (f) {
                                const int OFFSET_ALTERNATE_STATES = OFFSET_CURRENTMODE + 1;
                                f.seek(OFFSET_ALTERNATE_STATES, SeekSet);
                                byte states[kMaxModesPerFile] = {0};
                                for (size_t i = 0; i < min(currentAlternateStates.size(), (size_t)kMaxModesPerFile); i++) {
                                    states[i] = currentAlternateStates[i] ? 1 : 0;
                                }
                                f.write(states, kMaxModesPerFile);
                                f.close();
                            }
                        }
                        drawModesScreen();
                        isLongPress = true;
                    }
                }
            }
        }
        return;
    }

    // 11) Al soltar el botón (siempre)
    if (!isButtonPressed()) {
        // 11.0) Desbloqueo (500–5000 ms)
        if (lockedMain && buttonPressStart > 0) {
            const unsigned long pressDuration = millis() - buttonPressStart;
            if (pressDuration >= kUnlockMinMs && pressDuration <= kUnlockMaxMs) {
                systemLocked     = false;
                drawCurrentElement();
                buttonPressStart = 0;
                isLongPress      = false;
                return;
            }
        }

        // 11.1) Si sigue bloqueado, limpiar y salir
        if (lockedMain) {
            buttonPressStart = 0;
            isLongPress      = false;
            return;
        }

        if (buttonPressStart > 0) {
            const unsigned long pressDuration = millis() - buttonPressStart;
#ifdef DEBUG
            DEBUG__________ln("DEBUG: Duración suelta: " + String(pressDuration) + " ms");
#endif
            if (isLongPress && !systemLocked) {
                buttonPressStart = 0;
                isLongPress      = false;
                return;
            }

            if (isInMainMenu()) {
                if (pressDuration >= kUnlockMinMs && pressDuration <= kUnlockMaxMs) {
                    systemLocked     = true;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
                else if (pressDuration < kShortPressMaxMs) {
                    String currentFile = elementFiles[currentIndex];

                    if (currentFile == "Apagar") {
                        // BLACKOUT para todos: targetType=BROADCAST, NS=00000
                        for (size_t i = 0; i < selectedStates.size(); i++) selectedStates[i] = false;
                        if (isDadoEnabled()){
                            Serial1.write(0xDA);
                            delay(300UL);
                        }
                        send_frame(frameMaker_SEND_COMMAND(
                            DEFAULT_BOTONERA, TARGET_TYPE_BCAST, ZERO_NS, BLACKOUT
                        ));
                        setAllElementsToBasicMode();
                        doitPlayer.stop_file();
                        showMessageWithLoading(getTranslation("APAGANDO_SALA"), 4000);
                        currentIndex     = 0;
                        drawCurrentElement();
                        buttonPressStart = 0;
                        isLongPress      = false;
                        return;
                    }
                    else if (currentFile == "Comunicador") {
                        // Resetea el ciclo del relé y el foco del comunicador
                        relayStep = -1;
                        idsSPIFFS.clear();

                        // Ahora el comunicador apunta a BROADCAST por defecto
                        uint8_t communicatorTargetType = BROADCAST;   // 0xFF
                        communicatorActiveNS   = NS_ZERO;     // {0,0,0,0,0}

                        // Intención: encender si no estaba seleccionado; apagar si lo estaba
                        const bool turningOn = (currentIndex < (int)selectedStates.size())
                                            ? !static_cast<bool>(selectedStates[currentIndex])
                                            : true;

                        auto isSelectableElement = [&](const String& name) {
                            // Igual que antes: no aplicar a Ambientes / Fichas / Apagar
                            return !(name == "Ambientes" || name == "Fichas" || name == "Apagar");
                        };

                        // Marca selección masiva en la UI
                        for (size_t i = 0; i < elementFiles.size(); ++i) {
                            selectedStates[i] = isSelectableElement(elementFiles[i]) ? (turningOn ? 1 : 0) : 0;
                        }

                        // Enviar START/BLACKOUT en broadcast con el nuevo formato (targetType + NS_ZERO)
                        if (turningOn) {
                            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, START_CMD));
                        } else {
                            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, NS_ZERO, BLACKOUT));
                            showMessageWithLoading(getTranslation("APAGANDO_ELEMENTOS"), 4000);
                        }

                        drawCurrentElement();
                        buttonPressStart = 0;
                        isLongPress      = false;
                        return;
                    }
                    else {
                        // Abrir submenú de modos
                        inModesScreen    = true;
                        currentModeIndex = 0;
                        drawModesScreen();
                        buttonPressStart = 0;
                        isLongPress      = false;
                        return;
                    }
                }
                else if (pressDuration >= kDetailsLongPressMs) {
                    printElementDetails();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
            }

            // 11.4) Lógica residual
            String currentFile = elementFiles[currentIndex];
            if (!inModesScreen) {
                if (currentFile == "Apagar") {
                    // Ruta de seguridad
                    for (size_t i = 0; i < selectedStates.size(); i++) selectedStates[i] = false;
                    send_frame(frameMaker_SEND_COMMAND(
                        DEFAULT_BOTONERA, TARGET_TYPE_BCAST, ZERO_NS, BLACKOUT
                    ));
                    setAllElementsToBasicMode();
                    doitPlayer.stop_file();
                    showMessageWithLoading(getTranslation("APAGANDO_SALA"), 4000);
                    currentIndex     = 0;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
                else if (pressDuration < kShortPressMaxMs) {
                    if (currentFile != "Comunicador") {
                        inModesScreen    = true;
                        currentModeIndex = 0;
                        drawModesScreen();
                    }
                }
            } else {
                if (!isLongPress && pressDuration < kShortPressMaxMs) {
                            // === Aplicado inmediato del modo seleccionado ===
                const String path = elementFiles[currentIndex];
                int realMode = -1;
                if (currentModeIndex >= 0 && currentModeIndex < 17) {
                    realMode = globalVisibleModesMap[currentModeIndex]; // modo real (0..15)
                }
                if (realMode >= 0) {
                    applyModeImmediate(path, (uint8_t)realMode);
                }

                // Enviar al dispositivo (mantén tu lógica actual)
                handleModeSelection(elementFiles[currentIndex]);
                }
            }
        }

        // Reset de estado (siempre al soltar)
        buttonPressStart = 0;
        isLongPress      = false;
    }
}

bool modeAlternateActive = false;

// Función handleModeSelection modificada
/**
 * @brief Gestiona la selección de modo del elemento actual (o conmutación ON/OFF).
 *
 * Aplica el modo elegido para el elemento apuntado por `currentIndex`, incluyendo:
 * - Salir con la opción *Regresar* (valor -2 en `globalVisibleModesMap`).
 * - Conmutar encendido/apagado con el índice visible 0.
 * - Cargar/guardar el modo real en SPIFFS (OFFSET_CURRENTMODE) y actualizar flags.
 * - Enviar comandos a la botonera (START/BLACKOUT/SET_MODE/ALTERNATE).
 * - Tratamientos especiales para "Ambientes" (broadcast) y "Fichas" (mapeo de tokens).
 *
 * @param currentFile Nombre del archivo del elemento en SPIFFS. Valores especiales:
 *        "Ambientes", "Fichas" y "Apagar" tienen tratamiento específico.
 * @return void
 *
 * @pre
 *  - SPIFFS montado; offsets válidos: `OFFSET_MODES`, `SIZE_MODE`, `OFFSET_CURRENTMODE`.
 *  - Estructuras globales coherentes: `elementFiles`, `selectedStates`, `globalVisibleModesMap`.
 *  - `currentIndex` e `currentModeIndex` dentro de rango de sus contenedores.
 *  - `send_frame(...)`, `getCurrentElementID()`, `frameMaker_*` y dependencias disponibles.
 *
 * @note
 *  - Índice visible 0 = Encender/Apagar; *Regresar* se detecta con valor -2 en `globalVisibleModesMap`.
 *  - Delays entre comandos: 300 ms para asegurar secuenciación de órdenes al bus.
 *  - Se actualizan flags `adxl` y `useMic` según la configuración del modo seleccionado.
 *
 * @warning
 *  - Llama a `delay(300)` en varios puntos (bloqueante).
 *  - Escrituras en SPIFFS (modo r+) pueden afectar a la vida útil si se invoca con mucha frecuencia.
 *  - Función pensada para ejecutarse desde el bucle principal (no ISR).
 */
void handleModeSelection(const String& currentFile) noexcept
{
    constexpr int  kMaxModesPerFile    = 16;
    constexpr int  kModeNameLen        = 32;     // Ahora name ocupa 32
    constexpr int  kModeConfigOffset   = 216;
    constexpr unsigned long kInterCmdDelayMs = 300UL;

    if (elementFiles.empty()) return;
    if ((size_t)currentIndex >= elementFiles.size()) return;

    auto hasSelectedIndex = [&]() -> bool {
        return (size_t)currentIndex < selectedStates.size();
    };

    // 1) Opción "Regresar"
    bool isRegresar = false;
    if ((size_t)currentModeIndex < sizeof(globalVisibleModesMap)/sizeof(globalVisibleModesMap[0])) {
        isRegresar = (globalVisibleModesMap[currentModeIndex] == -2);
    }
    if (isRegresar) {
        inModesScreen = false;
        drawCurrentElement();
        return;
    }

    // 2) Opción índice visible 0: Encender / Apagar
    if (currentModeIndex == 0) {
        if (!hasSelectedIndex()) {
            inModesScreen = false;
            drawCurrentElement();
            return;
        }

        const bool wasSelected = selectedStates[currentIndex];
        selectedStates[currentIndex] = !wasSelected;

        if (currentFile == "Ambientes") {
            if (selectedStates[currentIndex]) {
                // Broadcast START
                TARGETNS bcast = {0,0,0,0,0};
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, START_CMD));
            } else {
                // Broadcast BLACKOUT
                TARGETNS bcast = {0,0,0,0,0};
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, BLACKOUT));
                for (auto &s : selectedStates) s = false;
                setAllElementsToBasicMode();
                doitPlayer.stop_file();
            }
        }
        else if (currentFile == "Dado") {
            const uint8_t basicModeIndex = 1;
            if (isDadoEnabled()) {
                Serial1.write(0xDA);
                delay(kInterCmdDelayMs);
            }
            TARGETNS dadoNS = {0};
            memcpy(&dadoNS, dadoOption.serialNum, 5);

            if (selectedStates[currentIndex]) {
                dadoOption.currentMode = basicModeIndex;
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, dadoNS, START_CMD));
                delay(kInterCmdDelayMs);
                send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, 0xDD, dadoNS, basicModeIndex));
            } else {
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, dadoNS, BLACKOUT));
                dadoOption.currentMode = basicModeIndex;
            }
            inModesScreen = false;
            drawCurrentElement();
            return;
        }
        else if (currentFile != "Fichas" && currentFile != "Apagar") {
            fs::File f = SPIFFS.open(currentFile, "r+");
            if (f) {
                TARGETNS elemNS = getCurrentElementNS();

                if (selectedStates[currentIndex]) {
                    // ENCENDER
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, START_CMD));

                    // === Abrir espera de CMODE tras enviar START_CMD ===
                    if (!isSpecialFile(currentFile) && !awaitingResponse) {                    // Evita RAM (Ambientes/Fichas/Apagar/Comunicador/Dado)
               
                        pendingQueryIndex = currentIndex;                // recuerda qué elemento esperamos
                        pendingQueryNS    = elemNS;                          // a quién esperamos CMODE
                        lastModeQueryTime = millis();                    // arranca cronómetro
                        awaitingResponse  = true;                        // habilita timeout de 500 ms (ya implementado)
                        // (opcional) frameReceived = false;             // si quieres arrancar "limpio"
                    }

                    byte basicMode = DEFAULT_BASIC_MODE; // normalmente 1
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    f.write(&basicMode, 1);

                    // === APLICADO INMEDIATO (ENCENDER → patrón del modo básico) ===
                    // Leemos los 2 bytes de config del modo básico para ajustar flags y mapear LEDs ya
                    {
                        byte cfg[2] = {0};
                        const size_t cfgOffset =
                            (size_t)OFFSET_MODES +
                            (size_t)basicMode * (size_t)SIZE_MODE +
                            (size_t)kModeConfigOffset; // 216

                        if (f.seek(cfgOffset, SeekSet)) {
                            (void)f.read(cfg, 2);  // usamos el mismo 'f' abierto en r+
                        }
                        adxl   = getModeFlag(cfg, HAS_SENS_VAL_1);
                        useMic = getModeFlag(cfg, HAS_SENS_VAL_2);
                    }

                    colorHandler.setCurrentFile(currentFile);
                    colorHandler.setPatternBotonera(basicMode, ledManager);
                    // ==============================================================
                } else {
                    // APAGAR
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, BLACKOUT));

                    byte basicMode = DEFAULT_BASIC_MODE; // tu lógica actual persiste básico aquí
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    f.write(&basicMode, 1);

                    const int OFFSET_ALTERNATE_STATES = OFFSET_CURRENTMODE + 1;
                    byte zeros[kMaxModesPerFile] = {0};
                    f.seek(OFFSET_ALTERNATE_STATES, SeekSet);
                    f.write(zeros, sizeof(zeros));
                    elementAlternateStates[currentFile].assign(
                        elementAlternateStates[currentFile].size(), false
                    );
                    setAllElementsToBasicMode();
                    showMessageWithLoading(getTranslation("APAGANDO_ELEMENTO"), 2000);
                    selectedStates[currentIndex] = false;

                    // === APLICADO INMEDIATO (APAGAR → patrón 0) ===
                    adxl   = false;
                    useMic = false;
                    colorHandler.setCurrentFile(currentFile);
                    colorHandler.setPatternBotonera(0, ledManager);
                    // ===============================================
                }

                f.close();
            }

        }

        inModesScreen = false;
        drawCurrentElement();
        return;
    }

    // 3) Selección de modo
    const int adjustedVisibleIndex = currentModeIndex - 1;
    String  modeName;
    uint8_t modeConfig[2] = {0};
    int     realModeIndex = 0;

    if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Dado") {
        INFO_PACK_T* option =
            (currentFile == "Ambientes") ? &ambientesOption :
            (currentFile == "Fichas")    ? &fichasOption    :
                                           &dadoOption;
        int count = 0;
        for (int i = 0; i < kMaxModesPerFile; i++) {
            if (strlen((char*)option->mode[i].name) > 0 &&
                checkMostSignificantBit(option->mode[i].config)) {
                if (count == adjustedVisibleIndex) {
                    realModeIndex = i;
                    modeName = String((char*)option->mode[i].name);
                    memcpy(modeConfig, option->mode[i].config, 2);
                    break;
                }
                ++count;
            }
        }
        option->currentMode = realModeIndex;
    }
    else if (currentFile != "Apagar") {
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            int count = 0;
            for (int i = 0; i < kMaxModesPerFile; i++) {
                char buf[kModeNameLen+1] = {0};
                byte tempCfg[2] = {0};
                f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                f.read((uint8_t*)buf, kModeNameLen);
                f.seek(OFFSET_MODES + i * SIZE_MODE + kModeConfigOffset, SeekSet);
                f.read(tempCfg, 2);
                if (strlen(buf) > 0 && checkMostSignificantBit(tempCfg)) {
                    if (count == adjustedVisibleIndex) {
                        realModeIndex = i;
                        modeName = String(buf);
                        memcpy(modeConfig, tempCfg, 2);
                        break;
                    }
                    ++count;
                }
            }
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write((uint8_t*)&realModeIndex, 1);
            f.close();
        }
    }

    adxl   = getModeFlag(modeConfig, HAS_SENS_VAL_1);
    useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

    bool wasAlreadySelected = hasSelectedIndex() ? selectedStates[currentIndex] : false;
    if (!wasAlreadySelected && hasSelectedIndex()) {
        selectedStates[currentIndex] = true;
    }

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        TARGETNS bcast = {0,0,0,0,0};
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, START_CMD));
        delay(kInterCmdDelayMs);
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, 0xFF, bcast, realModeIndex));
        if (currentFile == "Fichas") {
            TOKEN_MODE_ tmode = (realModeIndex == 1) ? TOKEN_PARTNER_MODE :
                                (realModeIndex == 2) ? TOKEN_GUESS_MODE   :
                                                       TOKEN_BASIC_MODE;
            token.set_mode(tmode);
            bankSelectionActive = (realModeIndex == 2);
        }
    }
    else if (currentFile == "Dado") {
        TARGETNS dadoNS = {0};
        memcpy(&dadoNS, dadoOption.serialNum, 5);
        if (!wasAlreadySelected && isDadoEnabled()) {
            Serial1.write(0xDA);
            delay(kInterCmdDelayMs);
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, dadoNS, START_CMD));
            delay(kInterCmdDelayMs);
        }
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, 0xDD, dadoNS, realModeIndex));
    }
    else if (currentFile != "Apagar") {
        TARGETNS elemNS = getCurrentElementNS();
        if (!wasAlreadySelected) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, START_CMD));

            if (!isSpecialFile(currentFile) && !awaitingResponse) {
                pendingQueryIndex = currentIndex;
                pendingQueryNS    = elemNS;
                lastModeQueryTime = millis();
                awaitingResponse  = true;
            }

            delay(kInterCmdDelayMs);
        }
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, 0xDD, elemNS, realModeIndex));
        if (getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE) &&
            currentAlternateStates.size() > (size_t)adjustedVisibleIndex)
        {
            if (currentAlternateStates[adjustedVisibleIndex]) {
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, ALTERNATE_MODE_ON));
            } else {
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, ALTERNATE_MODE_OFF));
            }
        }
    }

    if (!bankSelectionActive) {
        inModesScreen = false;
        drawCurrentElement();
    } else {
        inModesScreen = false;
        drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset);
    }
}

/**
 * @brief Inicializa el vector de estados alternativos para el elemento indicado.
 *
 * Para "Ambientes" y "Fichas" crea un vector con tantas entradas como modos visibles
 * (nombre no vacío y bit más significativo de la config activo), inicializadas a `false`.
 * Para elementos cargados desde SPIFFS (cualquier otro distinto de "Apagar"), devuelve
 * 16 entradas a `false`. Para "Apagar" devuelve un vector de tamaño 1 a `false`.
 *
 * @param currentFile Nombre lógico del elemento (p. ej. "Ambientes", "Fichas", fichero SPIFFS o "Apagar").
 * @return std::vector<bool> Vector de flags de “modo alternativo”, alineado con los modos visibles.
 *
 * @pre Estructuras globales `ambientesOption`/`fichasOption` inicializadas si se usan.
 * @note El tamaño del vector para "Ambientes"/"Fichas" depende del número de modos visibles.
 * @warning Se asume que `option->mode[i].name` está correctamente terminado en '\0'.
 */
std::vector<bool> initializeAlternateStates(const String &currentFile) noexcept
{
    constexpr int kMaxModesPerFile = 16;

    // --- RAM: Ambientes / Fichas / Comunicador / Dado ---
    if (currentFile == "Ambientes"   ||
        currentFile == "Fichas"      ||
        currentFile == "Comunicador" ||
        currentFile == "Dado")
    {
        INFO_PACK_T* opt =
            (currentFile == "Ambientes")   ? &ambientesOption   :
            (currentFile == "Fichas")      ? &fichasOption      :
            (currentFile == "Comunicador") ? &comunicadorOption :
                                             &dadoOption; // Dado

        std::vector<bool> v;
        for (int i = 0; i < kMaxModesPerFile; ++i) {
            const char* name = reinterpret_cast<const char*>(opt->mode[i].name);
            if (name[0] != '\0' && checkMostSignificantBit(opt->mode[i].config)) {
                // si el modo soporta alternativo podrías inicializar a false igual
                // (si luego quieres persistir, lo manejas en RAM)
                v.push_back(false);
            }
        }
        if (v.empty()) v.push_back(false); // seguridad
        return v;
    }

    // --- Apagar ---
    if (currentFile == "Apagar") {
        return std::vector<bool>(1, false);
    }

    // --- SPIFFS: calcular visibles leyendo el archivo ---
    std::vector<bool> v;
    fs::File f = SPIFFS.open(currentFile, "r");
    if (!f) {
        #ifdef DEBUG
        DEBUG__________printf("❌ Error abriendo el archivo: %s\n", currentFile.c_str());
        #endif
        return v; // vacío, el resto del flujo debe tolerar lista vacía
    }

    for (int i = 0; i < kMaxModesPerFile; ++i) {
        char modeName[25] = {0};
        byte cfg[2]       = {0};
        f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
        f.read((uint8_t*)modeName, 24);
        f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
        f.read(cfg, 2);
        if (modeName[0] != '\0' && checkMostSignificantBit(cfg)) {
            // si el modo soporta alternativo, inícialo a false igualmente
            v.push_back(false);
        }
    }
    f.close();

    if (v.empty()) v.push_back(false); // seguridad
    return v;
}

/**
 * @brief Alterna la selección del elemento actual y envía los comandos asociados.
 *
 * Cambia el estado `selectedStates[currentIndex]` y:
 * - Para ficheros de SPIFFS: lee su ID (OFFSET_ID), envía START/BLACKOUT y
 *   fuerza el modo básico (OFFSET_CURRENTMODE = DEFAULT_BASIC_MODE).
 * - Para "Apagar": deselecciona todos, reinicia estados alternativos globales,
 *   envía BLACKOUT en broadcast (0xFF) y vuelve al índice 0.
 * - Para "Ambientes"/"Fichas": solo alterna selección local (sin I/O).
 *
 * Si el elemento queda deseleccionado (false) y no es "Apagar", reinicia su
 * vector de estados alternativos en RAM mediante `initializeAlternateStates()`.
 *
 * @param currentFile Nombre lógico del elemento (fichero en SPIFFS o
 *        etiquetas especiales: "Ambientes", "Fichas", "Apagar").
 * @return void
 *
 * @pre
 *  - `currentIndex` dentro de rango de `selectedStates` y `elementFiles`.
 *  - SPIFFS montado si `currentFile` pertenece al FS.
 *  - `initializeAlternateStates()`, `elementAlternateStates`, `currentAlternateStates`
 *    y `send_frame()/frameMaker_*` disponibles.
 *
 * @note ID broadcast 0xFF para "Apagar". Para elementos de SPIFFS, ID leído en `OFFSET_ID`.
 * @warning Realiza I/O bloqueante sobre SPIFFS y envíos de tramas; no invocar desde ISR.
 */
void toggleElementSelection(const String& currentFile) noexcept
{
    // -------------------------
    // Utilidad
    // -------------------------
    const auto isElementFromSPIFFS = [&](const String& name) -> bool {
        return !name.startsWith("Ambientes") &&
               !name.startsWith("Fichas")    &&
               !name.startsWith("Apagar");
    };

    // -------------------------
    // Validaciones defensivas
    // -------------------------
    if ((size_t)currentIndex >= selectedStates.size()) {
    #ifdef DEBUG
            DEBUG__________printf("⚠️ toggleElementSelection: currentIndex fuera de rango (%d)\n", currentIndex);
    #endif
        return;
    }

    // -------------------------
    // 1) Alternar selección local
    // -------------------------
    selectedStates[currentIndex] = !selectedStates[currentIndex];

    // -------------------------
    // 2) Si se deselecciona y no es "Apagar", reiniciar alternativos
    // -------------------------
    if (!selectedStates[currentIndex] && currentFile != "Apagar") {
        std::vector<bool> newStates = initializeAlternateStates(currentFile);
        elementAlternateStates[currentFile] = newStates;
        currentAlternateStates = newStates;
    }

    // ----------------------------------------
    // 3) Caso especial: botón "Apagar" (global)
    // ----------------------------------------
    if (currentFile == "Apagar") {
        for (size_t i = 0; i < selectedStates.size(); ++i) {
            selectedStates[i] = false;
        }
        for (auto &entry : elementAlternateStates) {
            entry.second = initializeAlternateStates(entry.first);
        }

        // Broadcast BLACKOUT
        TARGETNS bcast = {0,0,0,0,0};
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, BLACKOUT));

        setAllElementsToBasicMode();
        showMessageWithLoading(getTranslation("APAGANDO_SALA"), 4000);

        currentIndex   = 0;
        inModesScreen  = false;
        drawCurrentElement();
        return;
    }

    // ------------------------------------------------
    // 4) Elementos de SPIFFS: envío START/BLACKOUT + FS
    // ------------------------------------------------
    if (isElementFromSPIFFS(currentFile)) {
        const uint8_t command = selectedStates[currentIndex] ? START_CMD : BLACKOUT;

        TARGETNS elemNS{};
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            // Leer serialNum[5] desde el fichero en lugar de ID
            f.seek(OFFSET_SERIALNUM, SeekSet);
            f.read(reinterpret_cast<uint8_t*>(&elemNS), sizeof(TARGETNS));
            f.close();

            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xDD, elemNS, command));

        #ifdef DEBUG
                    DEBUG__________printf("Enviando comando %s al elemento NS %02X%02X%02X%02X%02X\n",
                                        command == START_CMD ? "START_CMD" : "BLACKOUT",
                                        elemNS.mac01, elemNS.mac02, elemNS.mac03,
                                        elemNS.mac04, elemNS.mac05);
        #endif
            if (command == BLACKOUT) {
                showMessageWithLoading(getTranslation("APAGANDO_ELEMENTO"), 2000);
            }

            // Forzar modo básico en fichero
            f = SPIFFS.open(currentFile, "r+");
            if (f) {
                byte basicMode = DEFAULT_BASIC_MODE;
                f.seek(OFFSET_CURRENTMODE, SeekSet);
                f.write(&basicMode, 1);
                f.close();
            }
        } else {
            DEBUG__________ln("Error al abrir archivo SPIFFS para NS.");
        }
    }

    // -------------------------
    // 5) Redibujar UI
    // -------------------------
    drawCurrentElement();
}

// Variables globales para brillo
int lastEncoderCount = 0;
bool encoderPressed = false;
bool ignoreFirstRelease = true;

// Índice seleccionado en el menú (0 = Normal, 1 = Atenuado)
int brightnessMenuIndex = 0;

/**
 * @brief Gestiona la navegación y selección del menú de brillo.
 * Lee el encoder rotativo para mover el cursor (con wrap 0↔1), detecta la
 * liberación del botón para confirmar y aplica el nivel de brillo seleccionado.
 * Redibuja el menú al cambiar la posición del encoder y, al confirmar,
 * persiste el brillo, lo aplica a FastLED y sale al elemento principal.
 */
void handleBrightnessMenu()
{
    // ───────── Constantes y configuración ─────────
    constexpr int kMinIndex = 0;
    constexpr int kMaxIndex = 1;                 // Menú de 2 opciones: índices 0 y 1
    constexpr uint8_t kBrightnessNormal = 255;   // Valor absoluto para brillo normal
    constexpr uint8_t kBrightnessDim    = 50;    // Valor absoluto para brillo atenuado
    constexpr bool kButtonActiveLow     = true;  // Botón del encoder activo en LOW

    // ───────── Navegación con encoder (wrap 0↔1) ─────────
    const int currentCount = encoder.getCount();
    if (currentCount != lastEncoderCount) {
        const int delta = currentCount - lastEncoderCount;
        lastEncoderCount = currentCount;

        // Mantener semántica original: cualquier delta>0 => +1; delta<0 => -1
        if (delta > 0) {
            ++brightnessMenuIndex;
        } else {
            --brightnessMenuIndex;
        }

        // Wrap dentro del rango [kMinIndex..kMaxIndex]
        if (brightnessMenuIndex < kMinIndex) brightnessMenuIndex = kMaxIndex;
        if (brightnessMenuIndex > kMaxIndex) brightnessMenuIndex = kMinIndex;

        // Redibuja el menú tras moverse el cursor
        drawBrightnessMenu();
    }

    // ───────── Confirmación por botón (flanco de suelta) ─────────
    const bool rawButtonRead = (digitalRead(ENC_BUTTON) == LOW);
    const bool currentEncoderState = kButtonActiveLow ? rawButtonRead : !rawButtonRead;

    // Flanco de suelta: antes estaba presionado y ahora no lo está
    if (encoderPressed && !currentEncoderState) {
        if (ignoreFirstRelease) {
            // Se consume la primera suelta para evitar una confirmación espuria
            ignoreFirstRelease = false;
        } else {
            // Selección del valor absoluto de brillo según la opción actual
            const uint8_t valueToApply =
                (brightnessMenuIndex == BRIGHTNESS_NORMAL) ? kBrightnessNormal : kBrightnessDim;

            // 1) Actualizar variable global (0..255)
            currentBrightness = valueToApply;

            // 2) Guardar en SPIFFS (persistencia)
            saveBrightnessToSPIFFS(currentBrightness);

            // 3) Aplicar a FastLED y refrescar
            FastLED.setBrightness(currentBrightness);
            FastLED.show();

            // 4) Salir del menú y volver a la pantalla principal
            brightnessMenuActive = false;
            ignoreEncoderClick = true;
            drawCurrentElement();
        }
    }

    // Actualizar estado del botón para la próxima detección de flanco
    encoderPressed = currentEncoderState;
}

const int soundOptions[] = {0, 1, 3, 4, 6, 7, 9}; // Índices seleccionables
const int numSoundOptions = sizeof(soundOptions) / sizeof(soundOptions[0]);

/**
 * @brief Gestiona la navegación y confirmación del menú de sonido.
 *
 * Desplaza la selección con el encoder (wrap circular sobre las opciones válidas)
 * y, ante una pulsación corta (< 1000 ms) del botón del encoder, aplica el ajuste
 * correspondiente (género de voz, respuesta negativa, volumen) o confirma/sale.
 */
void handleSoundMenu()
{
    // ──────────────── Constantes de comportamiento ────────────────
    constexpr unsigned long kShortPressMs = 1000UL; // Umbral de pulsación corta
    constexpr bool kButtonActiveLow = true;         // Botón activo en LOW

    // Valores de volumen asociados a las opciones (mantener semántica)
    constexpr int kVolNormal  = 26;
    constexpr int kVolAtenuado = 20;

    // ──────────────── Lectura del encoder y navegación ─────────────
    static int currentIndex = 0; // índice dentro de soundOptions[]
    const int32_t newEncoderValue = encoder.getCount();
    static int32_t lastValue = newEncoderValue; // se inicializa la primera vez

    if (newEncoderValue != lastValue) {
        // Cualquier incremento => +1, decremento => -1
        const int dir = (newEncoderValue > lastValue) ? 1 : -1;
        lastValue = newEncoderValue;

        // Avance circular en el vector de opciones válidas
        currentIndex = (currentIndex + dir + numSoundOptions) % numSoundOptions;

        // Actualizar selección efectiva (índice lógico del menú) y redibujar
        soundMenuSelection = soundOptions[currentIndex];
        drawSoundMenu(soundMenuSelection);
    }

    // ──────────────── Gestión de pulsación del botón ───────────────
    const bool rawRead = (digitalRead(ENC_BUTTON) == LOW);
    const bool buttonPressed = kButtonActiveLow ? rawRead : !rawRead;

    if (buttonPressed) {
        // Inicio de pulsación
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        // Botón liberado: si hubo pulsación corta, ejecutar acción
        if (buttonPressStart > 0 && (millis() - buttonPressStart) < kShortPressMs) {
            const int sel = soundMenuSelection;
            switch (sel) {
                case 0:  selectedVoiceGender = 0; token.genre = 0;                  break; // Voz: mujer
                case 1:  selectedVoiceGender = 1; token.genre = 1;                  break; // Voz: hombre
                case 3: negativeResponse = true;                                    break; // Respuesta negativa: activar
                case 4: negativeResponse = false;                                   break; // Respuesta negativa: desactivar
                case 6: selectedVolume = 0; doitPlayer.player.volume(kVolNormal);   break; // Volumen: normal
                case 7: selectedVolume = 1; doitPlayer.player.volume(kVolAtenuado); break; // Volumen: atenuado
                case 9: // Confirmar y salir
                    saveSoundSettingsToSPIFFS();
                    soundMenuActive = false;
                    ignoreEncoderClick = true;
                    //drawCurrentElement();
                    DEBUG__________ln("✅ Ajustes de sonido confirmados:");
                    DEBUG__________printf(" - Tipo de voz: %s\n", (selectedVoiceGender == 0) ? "Mujer" : "Hombre");
                    DEBUG__________printf(" - Respuesta negativa: %s\n", negativeResponse ? "Con" : "Sin");
                    DEBUG__________printf(" - Volumen: %s\n", (selectedVolume == 0) ? "Normal" : "Atenuado");
                    break;

                default:
                    // Opción no accionable: no hacer nada (se mantiene el estado)
                    break;
            }

            // Refrescar para mostrar el nuevo estado tras la acción
            drawSoundMenu(soundMenuSelection);
        }

        // Reset de la marca de tiempo al soltar
        buttonPressStart = 0;
    }
    if (soundMenuActive) {
        drawSoundMenu(soundMenuSelection);  // ticker y scroll integrados
    }
}

/**
 * @brief Navega y confirma opciones del menú oculto usando el encoder y su pulsador.
 *
 * Gestiona la preselección al entrar, el desplazamiento por las opciones con el giro
 * del encoder y la confirmación con el pulsador. Activa submenús (idioma, sonido,
 * brillo, formateo/“control”) o vuelve al menú principal.
 *
 * @param[out] hiddenMenuSelection Índice de opción seleccionada visualmente (rango 0..4).
 * @return void
 */
void handleHiddenMenuNavigation(int &hiddenMenuSelection) noexcept
{
    // ----------------------------
    // Constantes y utilidades
    // ----------------------------
    constexpr int kHiddenMenuMaxIndex = 4; // Opciones 0..4 (5 opciones reales)

    auto isButtonPressed = []() -> bool { return digitalRead(ENC_BUTTON) == LOW; };
    auto isButtonReleased = []() -> bool { return digitalRead(ENC_BUTTON) == HIGH; };

    // Estado interno persistente entre llamadas
    static bool encoderButtonPressed = false;
    static bool initialEntry         = true;
    static bool menuJustOpened       = true; // Bloquea la confirmación inmediata al abrir

    // Lectura del contador del encoder
    const int32_t newEncoderValue = encoder.getCount();

    static bool prevHiddenMenuActive = false;
    if (hiddenMenuActive && !prevHiddenMenuActive) {
        // Ajustes acaba de abrirse → tratarlo como entrada inicial “limpia”
        initialEntry   = true;
        menuJustOpened = true;
        encoderButtonPressed = false;
    }
    prevHiddenMenuActive = hiddenMenuActive;

    // ---------------------------------
    // 1) Entrada inicial al menú oculto
    // ---------------------------------
    if (initialEntry) {
        hiddenMenuSelection = 0;   // Preselección visual sin confirmar
        initialEntry        = false;
        menuJustOpened      = true; // Evita confirmar si el botón ya estaba pulsado
        // drawHiddenMenu(hiddenMenuSelection); // (opcional, mantenido como en el original)
    }

    // ---------------------------------
    // 2) Navegación por giro del encoder
    // ---------------------------------
    if (newEncoderValue != lastEncoderValue) {
        hiddenMenuSelection += (newEncoderValue > lastEncoderValue) ? 1 : -1;
        hiddenMenuSelection  = constrain(hiddenMenuSelection, 0, kHiddenMenuMaxIndex);
        lastEncoderValue     = newEncoderValue;
        // drawHiddenMenu(hiddenMenuSelection); // (se redibuja al final si sigue activo)
    }

    // ---------------------------------
    // 3) Gestión del pulsador del encoder
    //    - Se permite confirmar sólo tras detectar una suelta
    // ---------------------------------
    if (isButtonReleased()) {
        menuJustOpened       = false; // A partir de ahora se permite confirmar
        encoderButtonPressed = false;
    }

    // Pulsación nueva y menú ya “armado” para confirmar
    if (isButtonPressed() && !encoderButtonPressed && !menuJustOpened) {
        encoderButtonPressed = true;
        ignoreEncoderClick   = true; // Evita relecturas residuales al salir de submenús

        switch (hiddenMenuSelection) {
            case 0: { // Idioma
                languageMenuActive   = true;
                languageMenuSelection = 0;
                drawLanguageMenu(languageMenuSelection);
                hiddenMenuActive     = false;
            } break;

            case 1: { // Sonido
                soundMenuActive    = true;
                soundMenuSelection = 0;
                drawSoundMenu(soundMenuSelection);
                hiddenMenuActive   = false;
                #ifdef DEBUG
                                DEBUG__________ln("Cambiando Sonido...");
                #endif
            } break;

            case 2: { // Brillo
                #ifdef DEBUG
                                DEBUG__________ln("Ajustando brillo...");
                #endif
                hiddenMenuActive    = false;  // Desactiva menú oculto
                brightnessMenuActive = true;  // Activa menú de brillo

                currentBrightness = loadBrightnessFromSPIFFS();
                tempBrightness    = currentBrightness;
                encoder.setCount(currentBrightness);

                // Reinicio de estados auxiliares del submenú de brillo
                lastEncoderCount   = currentBrightness;
                encoderPressed     = isButtonPressed();
                ignoreFirstRelease = true;

                drawBrightnessMenu();
            } break;

            case 3: { // Control / Formateo
                hiddenMenuActive   = false;
                formatSubMenuActive = true;
                formatMenuSelection = 0;
                buttonPressStart    = 0;

                // Variables externas utilizadas por el submenú
                extern int   formatMenuCurrentIndex;
                extern int32_t formatMenuLastValue;
                formatMenuCurrentIndex = 0;
                formatMenuLastValue    = encoder.getCount();

                // Espera activa hasta soltar (igual que en el original; bloqueante)
                while (isButtonPressed()) { /* busy wait */ }

                drawFormatMenu(formatMenuSelection);
            } break;

            case 4: { // Volver
                // Consumir pulsación en curso antes de salir
                while (digitalRead(ENC_BUTTON) == LOW) { /* busy wait breve */ }

                // Evitar que la suelta siguiente dispare acciones en la pantalla principal
                ignoreEncoderClick = true;

                // Reiniciar estado interno para la próxima entrada a Ajustes
                initialEntry   = true;
                menuJustOpened = true;
                encoderButtonPressed = false;

                PulsadoresHandler::limpiarEstados();
                hiddenMenuActive = false;
                drawCurrentElement();
            } break;

            default:
                // Índice fuera de las opciones contempladas (defensivo)
                break;
        }
    }

    // ---------------------------------
    // 4) Redibujado si el menú sigue activo
    // ---------------------------------
    if (hiddenMenuActive) {
        drawHiddenMenu(hiddenMenuSelection);
    }
}

/**
 * @brief Lee un flag (bit) de la configuración de un modo.
 *
 * Interpreta `modeConfig[0]` como el byte más significativo (MSB) y
 * `modeConfig[1]` como el menos significativo (LSB), formando un `uint16_t`
 * en formato big-endian. El enum `flag` indica el desplazamiento del bit
 * desde el LSB (bit 0).
 *
 * @param modeConfig Array de 2 bytes (MSB, LSB) que codifican la configuración.
 * @param flag Enumeración `MODE_CONFIGS` que indica el índice de bit a leer (0..15).
 * @return true si el bit indicado está a 1; false si está a 0 o parámetros inválidos.
 *
 * @pre `modeConfig` debe apuntar a al menos 2 bytes válidos.
 * @warning No valida si `flag` está fuera de 0..15; en tal caso el resultado es indefinido.
 */
bool getModeFlag(const uint8_t modeConfig[2], MODE_CONFIGS flag) noexcept
{
    // Validación defensiva: evitar puntero nulo
    if (!modeConfig) {
        #ifdef DEBUG
                DEBUG__________ln("⚠️ getModeFlag: modeConfig es nulo");
        #endif
        return false;
    }

    // Construir valor de 16 bits en big-endian:
    // modeConfig[0] = MSB, modeConfig[1] = LSB
    const uint16_t config = (static_cast<uint16_t>(modeConfig[0]) << 8) |
                             static_cast<uint16_t>(modeConfig[1]);

    // Extraer el bit especificado por 'flag'
    return ((config >> static_cast<uint8_t>(flag)) & 0x1u) != 0u;
}

/**
 * @brief Vuelca por puerto serie el estado de cada flag de `modeConfig`.
 *
 * Interpreta `modeConfig` como valor de 16 bits en big-endian (MSB = modeConfig[0],
 * LSB = modeConfig[1]) y recorre los flags definidos por el enum `MODE_CONFIGS`
 * desde `HAS_BASIC_COLOR` hasta `MODE_EXIST`, imprimiendo si cada bit está activo.
 *
 * @param modeConfig Array de 2 bytes (MSB, LSB) con la configuración del modo.
 * @return void
 *
 * @pre `modeConfig` debe apuntar a 2 bytes válidos.
 * @note Imprime además el valor bruto (MSB/LSB y 0xFFFF).
 * @warning Función de depuración; realiza E/S por serie. No llamar desde ISR.
 * @see getModeFlag()
 */
void debugModeConfig(const uint8_t modeConfig[2]) noexcept
{
    // Validación defensiva de puntero
    if (!modeConfig) {
        #ifdef DEBUG
                DEBUG__________ln("===== Estado de modeConfig =====");
                DEBUG__________ln("⚠️ modeConfig es nulo");
        #endif
        return;
    }

    // Construcción del valor de 16 bits en big-endian
    const uint16_t cfg =
        (static_cast<uint16_t>(modeConfig[0]) << 8) |
         static_cast<uint16_t>(modeConfig[1]);

    #ifdef DEBUG
        DEBUG__________ln("===== Estado de modeConfig =====");
        DEBUG__________printf("RAW: MSB=0x%02X LSB=0x%02X (0x%04X)\n",
                            modeConfig[0], modeConfig[1], cfg);
    #endif

    // Rango del enum a iterar (asumimos contiguo entre ambos extremos)
    constexpr int kStart = static_cast<int>(HAS_BASIC_COLOR);
    constexpr int kEnd   = static_cast<int>(MODE_EXIST);

    for (int i = kStart; i <= kEnd; ++i) {
        const MODE_CONFIGS flag = static_cast<MODE_CONFIGS>(i);
        const bool isActive     = getModeFlag(modeConfig, flag);

        // Etiqueta legible por flag (mantiene nombres del enum; evita etiquetas inconsistentes)
        switch (flag) {
            case HAS_BASIC_COLOR:       DEBUG__________("HAS_BASIC_COLOR"); break;
            case HAS_PULSE:             DEBUG__________("HAS_PULSE"); break;
            case HAS_ADVANCED_COLOR:    DEBUG__________("HAS_ADVANCED_COLOR"); break;
            case HAS_RELAY:             DEBUG__________("HAS_RELAY"); break;
            case HAS_RELAY_N1:          DEBUG__________("HAS_RELAY_N1"); break;
            case HAS_RELAY_N2:          DEBUG__________("HAS_RELAY_N2"); break;
            case NOP_1:                 DEBUG__________("NOP_1"); break;
            case HAS_SENS_VAL_1:        DEBUG__________("HAS_SENS_VAL_1"); break;
            case HAS_SENS_VAL_2:        DEBUG__________("HAS_SENS_VAL_2"); break;
            case NOP_2:                 DEBUG__________("NOP_2"); break;
            case HAS_PASSIVE:           DEBUG__________("HAS_PASSIVE"); break;
            case HAS_BINARY_SENSORS:    DEBUG__________("HAS_BINARY_SENSORS"); break;
            case HAS_BANK_FILE:         DEBUG__________("HAS_BANK_FILE"); break;
            case HAS_PATTERNS:          DEBUG__________("HAS_PATTERNS"); break;
            case HAS_ALTERNATIVE_MODE:  DEBUG__________("HAS_ALTERNATIVE_MODE"); break;
            case MODE_EXIST:            DEBUG__________("MODE_EXIST"); break;
            default:                    DEBUG__________("UNKNOWN_FLAG"); break;
        }
        DEBUG__________(" = ");
        DEBUG__________ln(isActive ? "1" : "0");
    }
}

/**
 * @brief Gestiona el menú de selección de bancos con el encoder y su pulsador.
 *
 * Permite navegar por la lista (opción 0 = "Confirmar" + N bancos), alternar la selección
 * de cada banco y confirmar para salir. Actualiza la ventana visible del menú y redibuja
 * la UI según cambios de foco o selección.
 *
 * @param[in,out] bankList       Vector con los IDs de banco (tamaño N).
 * @param[in,out] selectedBanks  Vector de flags de selección por banco (tamaño N).
 * @return void
 *
 * @pre
 *  - `encoder` inicializado; `lastEncoderValue` contiene la última cuenta válida.
 *  - `ENC_BUTTON` configurado (LOW = pulsado).
 *  - `bankMenuCurrentSelection`, `bankMenuWindowOffset`, `bankMenuVisibleItems` son globales válidos.
 *  - `drawBankSelectionMenu(...)`, `drawCurrentElement()`, `bankSelectionActive` disponibles.
 *
 * @note Índice 0 del menú corresponde a "Confirmar". Los bancos empiezan en el índice 1.
 * @warning Incluye esperas bloqueantes (`delay`) y bucle de espera al soltar botón; no invocar desde ISR.
 */
void handleBankSelectionMenu(std::vector<byte>& bankList, std::vector<bool>& selectedBanks) noexcept
{
    // -------------------------
    // Constantes (evita "números mágicos")
    // -------------------------
    constexpr unsigned long kDebouncePressMs = 200UL; // anti-rebote al pulsar
    constexpr unsigned long kWaitReleaseMs   = 10UL;  // espera entre lecturas al soltar

    // -------------------------
    // Utilidades y defensivas
    // -------------------------
    auto isButtonPressed  = []() -> bool { return digitalRead(ENC_BUTTON) == LOW;  };
    //auto isButtonReleased = []() -> bool { return digitalRead(ENC_BUTTON) == HIGH; };

    // Total de items del menú: 1 ("Confirmar") + bancos
    const int totalItems = static_cast<int>(bankList.size()) + 1;
    if (totalItems <= 0) {
    #ifdef DEBUG
            DEBUG__________ln("⚠️ handleBankSelectionMenu: sin items en el menú.");
    #endif
        return;
    }

    // -------------------------
    // Navegación con el encoder
    // -------------------------
    const int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        const int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;

        // Avance circular por los items
        bankMenuCurrentSelection += direction;
        if (bankMenuCurrentSelection < 0)                          bankMenuCurrentSelection = totalItems - 1;
        if (bankMenuCurrentSelection >= totalItems)                bankMenuCurrentSelection = 0;

        lastEncoderValue = newEncoderValue;

        // Mantener la selección dentro de la ventana visible
        if (bankMenuCurrentSelection < bankMenuWindowOffset) {
            bankMenuWindowOffset = bankMenuCurrentSelection;
        } else if (bankMenuCurrentSelection >= bankMenuWindowOffset + bankMenuVisibleItems) {
            bankMenuWindowOffset = bankMenuCurrentSelection - bankMenuVisibleItems + 1;
        }

        // Clamp defensivo del offset de ventana (por si totalItems < bankMenuVisibleItems)
        if (bankMenuWindowOffset < 0) bankMenuWindowOffset = 0;
        const int maxWindowStart = (totalItems > bankMenuVisibleItems) ? (totalItems - bankMenuVisibleItems) : 0;
        if (bankMenuWindowOffset > maxWindowStart) bankMenuWindowOffset = maxWindowStart;

        // Redibujar con los parámetros actualizados
        drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset);
    }

    // -------------------------
    // Confirmación / toggle con botón
    // -------------------------
    if (isButtonPressed()) {
        delay(kDebouncePressMs); // Debounce simple

        // Opción "Confirmar" (índice 0): salir del menú
        if (bankMenuCurrentSelection == 0) {
#ifdef DEBUG
            DEBUG__________ln("Bancos seleccionados:");
            const size_t pairs = (selectedBanks.size() < bankList.size())
                                 ? selectedBanks.size() : bankList.size();
            for (size_t i = 0; i < pairs; ++i) {
                if (selectedBanks[i]) {
                    DEBUG__________("0x");
                    DEBUG__________(bankList[i], HEX);
                    DEBUG__________(" ");
                }
            }
            DEBUG__________ln();
#endif
            // Reiniciar encoder para siguiente uso
            encoder.clearCount();
            lastEncoderValue = encoder.getCount();

            // Cerrar menú y volver
            bankSelectionActive = false;
            ignoreEncoderClick = true;
            drawCurrentElement();
            return;
        }

        // Toggle de un banco (índices 1..N → bancos 0..N-1)
        const int bankIndex = bankMenuCurrentSelection - 1;
        if (bankIndex >= 0 && bankIndex < static_cast<int>(bankList.size()) &&
            bankIndex < static_cast<int>(selectedBanks.size()))
        {
            selectedBanks[bankIndex] = !selectedBanks[bankIndex];
        }

        // Redibujar para reflejar el cambio
        drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset);

        // Esperar a la suelta para evitar toggles múltiples por una sola pulsación
        while (isButtonPressed()) {
            delay(kWaitReleaseMs);
        }
        delay(kDebouncePressMs); // debounce tras soltar
    }
}

const int numFormatOptions = 6;
const int formatOptions[numFormatOptions] = {0, 1, 2, 3, 4, 5};
bool confirmRestoreMenuActive = false;
int confirmRestoreSelection = 0;  // 0 = Sí, 1 = No
bool confirmRestoreMenuElementActive = false;
int confirmRestoreElementSelection = 0;  // 0 = Sí, 1 = No

int formatMenuCurrentIndex = 0;
int32_t formatMenuLastValue = 0;

/**
 * @brief Gestiona el submenú de formateo/control usando el encoder y su pulsador.
 *
 * Mueve la selección con el giro del encoder y, en pulsación corta (<1000 ms),
 * ejecuta la acción asociada: escanear sala, eliminar elemento, formatear SPIFFS,
 * mostrar ID, restaurar elementos, o volver.
 *
 * @return void
 *
 * @pre
 *  - `encoder` inicializado; `ENC_BUTTON` configurado (LOW = pulsado).
 *  - Globales válidas: `formatMenuCurrentIndex`, `formatMenuLastValue`,
 *    `formatMenuSelection`, `numFormatOptions`, `formatOptions[]`,
 *    flags `formatSubMenuActive`, `hiddenMenuActive`, y funciones de dibujo/envío.
 *  - `element` expone `escanearSala()`. `SPIFFS` montado si se formatea/restaura.
 *
 * @note Se usa pulsación corta <1000 ms para confirmar; no hay pulsación larga aquí.
 * @warning Contiene esperas bloqueantes (e.g., al mostrar mensajes/menús externos).
 *          No invocar desde ISR.
 */
void handleFormatMenu()
{
    constexpr unsigned long kShortPressMaxMs = 1000UL;

    int&     currentIndex = formatMenuCurrentIndex;
    int32_t& lastValue    = formatMenuLastValue;

    const int32_t newEncoderValue = encoder.getCount();

    static bool firstEntry = true;
    if (firstEntry) {
        lastValue  = newEncoderValue;
        firstEntry = false;
    } else if (lastValue == 0 && newEncoderValue != 0) {
        lastValue = newEncoderValue;
    }

    if (newEncoderValue != lastValue) {
        const int dir = (newEncoderValue > lastValue) ? 1 : -1;
        lastValue     = newEncoderValue;

        const int proposedIndex = currentIndex + dir;
        if (proposedIndex >= 0 && proposedIndex < numFormatOptions) {
            currentIndex        = proposedIndex;
            if (currentIndex >= 0 && currentIndex < numFormatOptions) {
                formatMenuSelection = formatOptions[currentIndex];
            }
        }
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
        return;
    }

    if (buttonPressStart > 0 && (millis() - buttonPressStart) < kShortPressMaxMs) {
        switch (formatMenuSelection) {
            case 0: { // Escanear sala
                formatSubMenuActive = false;
                hiddenMenuActive    = false;
                element->escanearSala();
                drawCurrentElement();
            } break;

            case 1: { // Eliminar elemento
                loadDeletableElements();
                if (!deletableElementFiles.empty()) {
#ifdef DEBUG
                    DEBUG__________ln("[📂] Lista de elementos disponibles para eliminar:");
                    for (size_t i = 0; i < deletableElementFiles.size(); ++i) {
                        DEBUG__________printf(" - %s\n", deletableElementFiles[i].c_str());
                    }
#endif
                    deleteElementMenuActive = true;
                    deleteElementSelection  = 0;
                    formatSubMenuActive     = false;
                    drawDeleteElementMenu(deleteElementSelection);
                    forceDrawDeleteElementMenu = true;
                } else {
#ifdef DEBUG
                    DEBUG__________ln("No hay elementos para eliminar.");
#endif
                }
            } break;

            case 2: { // Formatear SPIFFS
                confirmRestoreMenuActive = true;
                confirmRestoreSelection  = 0;
                formatSubMenuActive      = false;
                drawConfirmRestoreMenu(confirmRestoreSelection);
            } break;

//             case 3: { // Mostrar NS (antes ID)
// #ifdef DEBUG
//                 DEBUG__________ln("[🆔] Mostrando Serial Numbers");
// #endif
//                 TARGETNS bcast = {0,0,0,0,0};
//                 send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, BROADCAST, bcast, SHOW_ID_CMD));
//                 formatSubMenuActive = true;
//                 showMessageWithLoading(getTranslation("SHOW_ID"), 3000);
//             } break;

            case 3: { // Restaurar elementos
#ifdef DEBUG
                DEBUG__________ln("Restaurando elementos");
#endif
                confirmRestoreMenuElementActive = true;
                confirmRestoreElementSelection  = 0;
                formatSubMenuActive             = false;
                drawConfirmRestoreElementMenu(confirmRestoreElementSelection);
            } break;

            case 4: { // Elementos Adicionales
#ifdef DEBUG
                DEBUG__________ln("[➕] Elementos Adicionales");
#endif
                extraElementsMenuActive   = true;
                extraElementsMenuSelection= 0;
                formatSubMenuActive       = false;
            } break;

            case 5: { // Volver
                ignoreEncoderClick = true;
                formatSubMenuActive = false;
                hiddenMenuActive    = true;
                drawHiddenMenu(0);
            } break;

            default:
                break;
        }
    }

    buttonPressStart = 0;
}

/**
 * @brief Gestiona el menú de confirmación de restauración (Sí/No) con el encoder.
 *
 * Permite alternar entre dos opciones con el giro del encoder y confirma con pulsación
 * corta (<1000 ms). Si se confirma "Sí", formatea SPIFFS, recarga elementos y reinicia
 * el sistema. Si se elige "No", vuelve al submenú de formato.
 *
 * @return void
 *
 * @pre `encoder` inicializado; `ENC_BUTTON` configurado (LOW=pulsado).
 * @pre Globales válidas: `confirmRestoreSelection`, `confirmRestoreMenuActive`,
 *      `formatSubMenuActive`, `formatMenuSelection`.
 * @pre Funciones/recursos: `drawConfirmRestoreMenu()`, `formatSPIFFS()`,
 *      `loadElementsFromSPIFFS()`, `drawFormatMenu()`, `uiSprite`, `ESP.restart()`.
 *
 * @note El índice de selección es binario (0=Sí, 1=No). El recorrido es circular.
 * @warning Incluye reinicio del sistema en la opción "Sí".
 * @see drawConfirmRestoreMenu(), formatSPIFFS(), loadElementsFromSPIFFS(), drawFormatMenu()
 */
void handleConfirmRestoreMenu() noexcept
{
    // -------------------------
    // Constantes
    // -------------------------
    constexpr unsigned long kShortPressMaxMs = 1000UL;

    // -------------------------
    // Seguimiento del encoder (usa la cuenta absoluta como referencia)
    // -------------------------
    static int32_t lastEncoderCount = 0;             // Estado local persistente
    const  int32_t newCount         = encoder.getCount();

    if (newCount != lastEncoderCount) {
        const int dir = (newCount > lastEncoderCount) ? 1 : -1;
        lastEncoderCount = newCount;

        // Dos opciones: 0 (Sí) y 1 (No), recorrido circular
        confirmRestoreSelection = (confirmRestoreSelection + dir + 2) % 2;

        // Redibujo con la nueva selección
        drawConfirmRestoreMenu(confirmRestoreSelection);
    }

    // -------------------------
    // Gestión del pulsador (pulsación corta para confirmar)
    // -------------------------
    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        // Al soltar, comprobar si ha sido una pulsación corta
        if (buttonPressStart > 0 && (millis() - buttonPressStart) < kShortPressMaxMs) {
            if (confirmRestoreSelection == 0) {
                // --- Opción "Sí" ---
                #ifdef DEBUG
                                DEBUG__________ln("[⚠️] Restaurando sala...");
                #endif
                formatSPIFFS();
                loadElementsFromSPIFFS();

                // Cerrar menús y limpiar pantalla antes del reinicio
                confirmRestoreMenuActive = false;
                formatSubMenuActive      = false;
                uiSprite.fillSprite(BACKGROUND_COLOR);
                uiSprite.pushSprite(0, 0);

                ESP.restart(); // Reinicio del sistema
            } else {
                // --- Opción "No" ---
                confirmRestoreMenuActive = false;
                formatSubMenuActive      = false;
                drawFormatMenu(formatMenuSelection);
            }
        }
        // Reset del temporizador de pulsación (siempre al soltar)
        buttonPressStart = 0;
    }
}

/**
 * @brief Gestiona el menú de confirmación para restaurar elementos mediante encoder y pulsación corta.
 *
 * Lee el contador del encoder para mover la selección (2 opciones, navegación circular)
 * y, al detectar una pulsación corta (< 1000 ms) del botón del encoder, ejecuta la acción
 * asociada: si la opción seleccionada es "Sí", envía una secuencia de tramas para restaurar
 * elementos; después sale del menú de confirmación y abre el submenú de formato.
 *
 * @return void
 * @pre
 *  - `encoder` inicializado y operativo.
 *  - `ENC_BUTTON` configurado como entrada (activo a nivel bajo).
 *  - Variables/funciones globales válidas: `confirmRestoreElementSelection`,
 *    `drawConfirmRestoreElementMenu(int)`, `confirmRestoreMenuElementActive`,
 *    `formatSubMenuActive`, `formatMenuSelection`, `drawFormatMenu(int)`,
 *    `buttonPressStart`, `send_frame(...)`, `frameMaker_*`, `DEFAULT_BOTONERA`,
 *    `{BROADCAST}`, `DEFAULT_DEVICE`, `SHOW_ID_CMD`, `BLACKOUT`.
 *  - No llamar desde ISR (usa `delay()` y llamadas potencialmente bloqueantes).
 * @note
 *  - Pulsación corta: < 1000 ms. Botón activo en LOW.
 *  - Número de opciones del menú: 2 (índices 0..1).
 *  - La secuencia de restauración incluye retardos bloqueantes (600 ms, 5000 ms, 500 ms).
 * @warning
 *  - Función bloqueante durante la secuencia de restauración.
 *  - No implementa *debounce*; depende del hardware/lectura estable.
 * @see drawConfirmRestoreElementMenu, drawFormatMenu, send_frame
 */
void handleConfirmRestoreElementMenu() {
    // --- Constantes (evitan números mágicos) ---
    constexpr uint8_t  kNumOptions              = 2;      // Opciones del menú (0..1)
    constexpr uint32_t kShortPressThresholdMs   = 1000U;  // Umbral de pulsación corta
    constexpr uint32_t kDelaySetElemMs          = 600U;   // Retardo tras SET_ELEM_ID
    constexpr uint32_t kDelayShowIdMs           = 5000U;  // Retardo mostrando IDs
    constexpr uint32_t kDelayBlackoutMs         = 500U;   // Retardo tras BLACKOUT

    // --- Seguimiento del encoder ---
    // Valor anterior del contador del encoder para detectar cambio y dirección.
    static int32_t lastSelection = 0;

    const int32_t newCount = encoder.getCount();
    if (newCount != lastSelection) {
        // Determina la dirección de giro (ignora la magnitud del salto -> coherente con original).
        const int dir = (newCount > lastSelection) ? 1 : -1;
        lastSelection = newCount;

        // Actualiza selección en anillo [0, kNumOptions-1].
        confirmRestoreElementSelection =
            (confirmRestoreElementSelection + dir + kNumOptions) % kNumOptions;

        // Redibuja el menú con la nueva selección.
        drawConfirmRestoreElementMenu(confirmRestoreElementSelection);
    }

    // --- Gestión del botón del encoder (activo en LOW) ---
    const int btnState = digitalRead(ENC_BUTTON);

    if (btnState == LOW) {
        // Inicio de pulsación: registra el instante solo una vez.
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        // Botón liberado: evalúa la duración de la pulsación.
        if (buttonPressStart > 0) {
            const uint32_t now     = millis();
            const uint32_t elapsed = now - static_cast<uint32_t>(buttonPressStart);

            if (elapsed < kShortPressThresholdMs) {
                // --- Pulsación corta ---
                if (confirmRestoreElementSelection == 0) {
                    // Opción "Sí": ejecutar secuencia de restauración (bloqueante por diseño).
                    DEBUG__________ln("[⚠️] Restaurando elementos...");
                    showMessageWithLoading("Restaurando", 2000);
                    
                    TARGETNS bcast = {0,0,0,0,0};
                    send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, 0xFF, bcast, DEFAULT_DEVICE));
                    delay(kDelaySetElemMs);
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, SHOW_ID_CMD));
                    delay(kDelayShowIdMs);
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, 0xFF, bcast, BLACKOUT));
                    delay(kDelayBlackoutMs);
                }

                // Tras confirmar (sí/no), cerrar este menú y abrir el de formato.
                confirmRestoreMenuElementActive = false;
                formatSubMenuActive = true;
                formatMenuSelection = 0;
                drawFormatMenu(formatMenuSelection);
            }
        }
        // Reset del estado de pulsación al liberar el botón.
        buttonPressStart = 0;
    }
}

bool deleteElementMenuActive = false;
int deleteElementSelection = 0;
std::vector<String> deletableElementFiles;
bool confirmDeleteActive = false;
int confirmSelection = 0;
String confirmedFileToDelete = "";

/**
 * @brief Gestiona el menú de borrado de elementos con encoder y pulsación corta.
 *
 * Actualiza la selección (navegación circular) a partir del contador del encoder y,
 * al detectar una pulsación corta (< 1000 ms) del botón del encoder, decide si:
 * - Vuelve al submenú de formato (si el elemento seleccionado es "VOLVER"), o
 * - Abre el menú de confirmación de borrado para el elemento seleccionado.
 *
 * No realiza dibujado; el loop principal es quien redibuja en el tick correspondiente.
 *
 * @return void
 * @pre
 *  - `encoder` inicializado y operativo.
 *  - `ENC_BUTTON` configurado como entrada con nivel activo en LOW.
 *  - `deletableElementFiles` contiene los nombres de elementos (incluyendo la opción "VOLVER").
 *  - Variables globales válidas: `deleteElementSelection`, `deleteElementMenuActive`,
 *    `formatSubMenuActive`, `confirmDeleteActive`, `confirmDeleteMenuActive`,
 *    `confirmSelection`, `confirmedFileToDelete`, `buttonPressStart`.
 *  - Funciones externas disponibles: `getTranslation(const char*)`.
 *  - No llamar desde ISR (usa temporización con `millis()` y accede a estructuras globales).
 * @note
 *  - Pulsación corta: < 1000 ms (botón activo en LOW).
 *  - Navegación circular sobre `deletableElementFiles`.
 *  - No hay debounce por software; se asume hardware o filtrado externo adecuado.
 * @warning
 *  - Si `deletableElementFiles` está vacío, la pulsación se ignora (no hay elemento válido).
 * @see getTranslation
 */
void handleDeleteElementMenu() {
    // --- Constantes (evitan números mágicos) ---
    constexpr uint32_t kShortPressThresholdMs = 1000U; // Umbral de pulsación corta

    // --- Estado del encoder ---
    static int currentIndex = 0; // Índice actual dentro de la lista
    const int32_t newEncoderValue = encoder.getCount();
    // Inicialización dinámica en la primera llamada para evitar salto inicial
    static int32_t lastValue = newEncoderValue;

    const size_t listSize = deletableElementFiles.size();

    // Detecta cambio de encoder (solo dirección, no magnitud del salto)
    if (newEncoderValue != lastValue) {
        const int dir = (newEncoderValue > lastValue) ? 1 : -1;
        lastValue = newEncoderValue; // Actualiza siempre, aun sin lista, para seguir el estado real

        if (listSize > 0) {
            const int n = static_cast<int>(listSize);
            // Navegación circular: 0..n-1
            currentIndex = (currentIndex + dir + n) % n;
            deleteElementSelection = currentIndex;
            // ❌ no dibujar aquí
        }
    }

    // --- Gestión del botón del encoder (activo en LOW) ---
    if (digitalRead(ENC_BUTTON) == LOW) {
        // Inicio de pulsación (marca el tiempo una sola vez)
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        // Botón liberado: evaluar duración si había pulsación en curso
        if (buttonPressStart > 0) {
            const uint32_t now     = millis();
            const uint32_t elapsed = now - static_cast<uint32_t>(buttonPressStart);

            if (elapsed < kShortPressThresholdMs) {
                // --- Pulsación corta ---
                if (listSize > 0 && static_cast<size_t>(deleteElementSelection) < listSize) {
                    const String selected = deletableElementFiles[deleteElementSelection];

                    if (selected == getTranslation("VOLVER")) {
                        // Salir a submenú de formato
                        deleteElementMenuActive = false;
                        formatSubMenuActive     = true;
                        // El loop dibujará drawFormatMenu(...) en su bloque correspondiente
                    } else {
                        // Pasar a menú de confirmación de borrado
                        DEBUG__________printf("[❓] Confirmar eliminación de: %s\n", selected.c_str());
                        confirmDeleteActive     = true;
                        confirmSelection        = 0;
                        confirmedFileToDelete   = selected;

                        deleteElementMenuActive = false;
                        confirmDeleteMenuActive = true;
                        // ✅ NO dibujes aquí: el loop lo hará inmediatamente este mismo tick
                    }
                }
                // Si la lista está vacía o el índice es inválido, se ignora la pulsación (defensivo).
            }
        }
        // Reset del estado de pulsación al liberar el botón
        buttonPressStart = 0;
    }
}

bool confirmDeleteMenuActive = false;

/**
 * @brief Gestiona la pantalla de confirmación de borrado mediante el encoder y el pulsador.
 * @return void
 * @warning El borrado es permanente si el archivo existe en SPIFFS.
 */
void handleConfirmDelete() {
    //--- Constantes de comportamiento/UX (evitan "números mágicos")
    constexpr int      kConfirmOptions = 2;        // 0 = Sí, 1 = No
    constexpr uint32_t kShortPressMs   = 1000u;    // Umbral de pulsación corta

    //--- Estado del encoder (persistente entre invocaciones)
    static int32_t lastValue = encoder.getCount();

    //--- Lecturas actuales
    const int32_t newValue = encoder.getCount();
    const uint32_t nowMs   = millis();

    //--- Navegación con el encoder: alterna entre Sí/No
    if (newValue != lastValue) {
        const int dir = (newValue > lastValue) ? 1 : -1; // +1 derecha / -1 izquierda
        lastValue = newValue;

        // Mantener confirmSelection en [0..kConfirmOptions-1]
        confirmSelection = (confirmSelection + dir + kConfirmOptions) % kConfirmOptions;

        // Refrescar UI de confirmación
        drawConfirmDelete(confirmedFileToDelete);
    }

    //--- Gestión de pulsación del botón del encoder
    if (digitalRead(ENC_BUTTON) == LOW) {
        // Botón presionado: capturar instante de inicio si no estaba ya en curso
        if (buttonPressStart == 0) {
            buttonPressStart = nowMs;
        }
    } else {
        // Botón liberado: si hubo pulsación y fue corta, ejecutar acción
        if (buttonPressStart > 0 && static_cast<uint32_t>(nowMs - buttonPressStart) < kShortPressMs) {
            if (confirmSelection == 0) {
                // ✅ Sí, eliminar
                const String fullPath = String("/element_") + confirmedFileToDelete + ".bin";

                if (SPIFFS.exists(fullPath)) {
                    // Intentar borrar; log de éxito (se mantiene la macro existente)
                    if (SPIFFS.remove(fullPath)) {
                        DEBUG__________printf("[🗑] Eliminado: %s\n", fullPath.c_str());
                    } else {
                        // Nota: no cambiamos el comportamiento; solo se omite log en fallo.
                        // Puede añadirse log adicional si existe infraestructura.
                    }
                }

                // Salir del flujo de confirmación y volver al principal
                formatSubMenuActive     = false;
                confirmDeleteActive     = false;
                confirmDeleteMenuActive = false;

                // Recargar elementos y volver al menú principal
                loadElementsFromSPIFFS();
                drawCurrentElement();
            } else {
                // ❌ Cancelar: volver al menú de borrado
                confirmDeleteActive     = false;
                deleteElementMenuActive = true;
                confirmDeleteMenuActive = false;

                drawDeleteElementMenu(deleteElementSelection);
            }
        }

        // Reset del estado de pulsación (evita re-disparos)
        buttonPressStart = 0;
    }
}

/**
 * @brief Indica si el sistema se encuentra actualmente en el menú principal.
 * @return true si ningún submenú o pantalla secundaria está activo; false en caso contrario.
 * @note Evalúa múltiples banderas internas que indican si otros menús están activos.
 */
bool isInMainMenu() {
    // Pantalla principal = NO hay ningún submenú/diálogo activo
    return !inModesScreen &&
           !hiddenMenuActive &&
           !brightnessMenuActive &&
           !soundMenuActive &&
           !languageMenuActive &&
           !formatSubMenuActive &&
           !bankSelectionActive &&
           !deleteElementMenuActive &&
           !confirmDeleteMenuActive &&
           !confirmRestoreMenuActive &&
           !confirmRestoreMenuElementActive &&
           !inCognitiveMenu &&
           !extraElementsMenuActive &&
           !confirmEnableDadoActive;
}


/**
 * @brief Muestra en pantalla los detalles (ID y Serial) del elemento seleccionado.
 *
 * Obtiene el ID (1 byte) y el número de serie (5 bytes) y los formatea en
 * hexadecimal sin separadores y en mayúsculas. Para entradas especiales
 * ("Ambientes", "Fichas", "Apagar") se muestra "N/A". Para "Comunicador" se
 * usan los campos de `comunicadorOption`. En el resto de casos se leen los
 * datos desde SPIFFS usando los offsets configurados.
 *
 * @return void
 * @pre SPIFFS debe estar montado; `elementFiles[currentIndex]` debe ser válido.
 * @pre `OFFSET_ID` y `OFFSET_SERIAL` deben apuntar a posiciones válidas del archivo.
 * @pre `showElemInfo(uint32_t, const String&, const String&)` debe estar disponible.
 * @note El ID se muestra con 2 dígitos hexadecimales; el Serial con 10 (5 bytes).
 * @warning Si falla la apertura, seek o lectura de archivo, la función retorna sin mostrar en pantalla.
 * @see showElemInfo
 */

void printElementDetails() {
    // -------------------------------
    // Constantes y utilidades locales
    // -------------------------------
    constexpr uint32_t kDisplayTimeoutMs = 10000U; // Evita número mágico en showElemInfo
    constexpr size_t   kSerialLen        = 5U;     // Longitud en bytes del serial
    constexpr size_t   kHexByteStrLen    = 3U;     // "FF" + '\0'
    constexpr char     kNA[]             = "N/A";

    // Lambda para convertir un byte a dos dígitos hex (mayúsculas)
    auto byteToHex = [](uint8_t v, char out[kHexByteStrLen]) noexcept {
        // %02X ya genera mayúsculas; se mantiene toUpperCase posterior por robustez.
        (void)snprintf(out, kHexByteStrLen, "%02X", static_cast<unsigned>(v));
    };

    // Lambda para crear un String con los bytes en hex sin separadores
    auto hexOfArray = [&](const uint8_t* data, size_t len) -> String {
        String s;
        s.reserve(len * 2U); // 2 chars por byte
        char buf[kHexByteStrLen] = {};
        for (size_t i = 0; i < len; ++i) {
            byteToHex(data[i], buf);
            s += buf;
        }
        return s;
    };

    // -------------------------------
    // Selección del origen de datos
    // -------------------------------
    const String currentFile = elementFiles[currentIndex];
    String idStr;
    String serialStr;

    if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
        // Entradas especiales sin ID/Serial
        idStr     = kNA;
        serialStr = kNA;
    }
    else if (currentFile == "Comunicador") {
        // Formateo desde estructura en memoria
        {
            char idBuf[kHexByteStrLen] = {};
            byteToHex(static_cast<uint8_t>(comunicadorOption.ID), idBuf);
            idStr = idBuf;
        }

        // Serial de 5 bytes (sin separadores)
        serialStr = hexOfArray(reinterpret_cast<const uint8_t*>(comunicadorOption.serialNum), kSerialLen);
    }
    else {
        // -------------------------------
        // Lectura desde SPIFFS con checks
        // -------------------------------
        fs::File f = SPIFFS.open(currentFile, "r");
        if (!f) {
            DEBUG__________ln("❌ No se pudo abrir el archivo para mostrar detalles.");
            return; // Mantiene semántica original en caso de fallo de apertura
        }

        uint8_t id      = 0U;
        uint8_t serial[kSerialLen] = {0U};

        // Validaciones defensivas de seek/read
        bool ok = true;
        ok = ok && f.seek(OFFSET_ID);
        if (ok) {
            size_t n = f.read(&id, 1U);
            ok = ok && (n == 1U);
        }
        ok = ok && f.seek(OFFSET_SERIAL);
        if (ok) {
            size_t n = f.read(serial, kSerialLen);
            ok = ok && (n == kSerialLen);
        }
        f.close();

        if (!ok) {
            DEBUG__________ln("❌ Error de lectura/seek en archivo de elemento.");
            return; // Evita mostrar datos corruptos
        }

        // Formateo ID (2 dígitos hex)
        {
            char idBuf[kHexByteStrLen] = {};
            byteToHex(id, idBuf);
            idStr = idBuf;
        }

        // Formateo serial (5 bytes → 10 dígitos hex sin espacios)
        serialStr = hexOfArray(serial, kSerialLen);
    }

    // Normalización por seguridad (debería ser innecesario con %02X, pero no cuesta)
    idStr.toUpperCase();
    serialStr.toUpperCase();

    // Debug opcional
    DEBUG__________ln("🔎 Detalles del elemento:");
    DEBUG__________("ID: ");     DEBUG__________ln(idStr);
    DEBUG__________("Serial: "); DEBUG__________ln(serialStr);

    // Mostrar en pantalla
    showElemInfo(kDisplayTimeoutMs, serialStr, idStr);
}

/**
 * @brief Calcula cuántos modos "visibles" existen para un archivo o categoría.
 *
 * Para "Ambientes" y "Fichas" cuenta los modos del paquete en RAM cuyo nombre no está vacío
 * y cuyo bit más significativo en el campo de configuración está activo.
 * Para "Apagar" y "Comunicador" retorna 0.
 * Para otros archivos, lee 16 entradas desde SPIFFS: nombre (24 bytes) y configuración (2 bytes),
 * y cuenta las que cumplan las mismas condiciones.
 *
 * @param file Nombre lógico de la categoría ("Ambientes", "Fichas", "Apagar", "Comunicador")
 *             o ruta de archivo en SPIFFS.
 * @return Número de modos visibles en el rango [0..16].
 * @pre SPIFFS debe estar montado si `file` no es "Ambientes", "Fichas", "Apagar" o "Comunicador".
 * @note Disposición en archivo: nombre (24 bytes) en `OFFSET_MODES + i*SIZE_MODE`, y config (2 bytes)
 *       en `OFFSET_MODES + i*SIZE_MODE + 216`. Se asume orden little-endian para `config`.
 * @warning Si un `seek`/`read` falla al leer una entrada desde SPIFFS, esa entrada se ignora.
 * @see checkMostSignificantBit
 */
int getTotalModesForFile(const String &file) {
    // -------------------------------
    // Constantes para legibilidad
    // -------------------------------
    constexpr int    kMaxModes             = 16;    // Total de slots
    constexpr size_t kModeNameLen          = 24U;   // Bytes de nombre por modo
    constexpr size_t kCfgBytes             = 2U;    // Tamaño del campo config en archivo
    constexpr size_t kCfgOffsetWithinMode  = 216U;  // Offset del campo config dentro de cada modo

    // -------------------------------
    // Casos en RAM: Ambientes / Fichas
    // -------------------------------
    if (file == "Ambientes" || file == "Fichas") {
        INFO_PACK_T* opt = (file == "Ambientes") ? &ambientesOption : &fichasOption;

        int count = 0;
        for (int i = 0; i < kMaxModes; ++i) {
            const char* name = reinterpret_cast<const char*>(opt->mode[i].name);
            // Nombre no vacío y MSB de config activo => visible
            if (name != nullptr &&
                ::strlen(name) > 0 &&
                checkMostSignificantBit(opt->mode[i].config)) {
                ++count;
            }
        }
        return count;
    }

    // -------------------------------
    // Casos sin modos visibles
    // -------------------------------
    if (file == "Apagar" || file == "Comunicador") {
        return 0;
    }

    // -------------------------------
    // Lectura desde SPIFFS
    // -------------------------------
    fs::File f = SPIFFS.open(file, "r");
    if (!f) {
        return 0; // Comportamiento original
    }

    int  count = 0;
    char modeName[kModeNameLen + 1] = {}; // +1 para terminador
    byte cfgRaw[kCfgBytes] = {0, 0};

    for (int i = 0; i < kMaxModes; ++i) {
        // Offset base de la entrada i
        const size_t base = static_cast<size_t>(OFFSET_MODES) +
                            static_cast<size_t>(i) * static_cast<size_t>(SIZE_MODE);

        // --- Leer nombre (24 bytes) ---
        if (!f.seek(base, SeekSet)) {
            continue; // Entrada inválida: se ignora
        }
        size_t n = f.read(reinterpret_cast<uint8_t*>(modeName), kModeNameLen);
        if (n != kModeNameLen) {
            continue; // Lectura incompleta: se ignora
        }
        modeName[kModeNameLen] = '\0'; // Asegurar terminación

        // --- Leer config (2 bytes tal cual) ---
        if (!f.seek(base + kCfgOffsetWithinMode, SeekSet)) {
            continue;
        }
        n = f.read(cfgRaw, kCfgBytes);
        if (n != kCfgBytes) {
            continue;
        }

        // --- Visibilidad: nombre no vacío + MSB activo ---
        if (::strlen(modeName) > 0 && checkMostSignificantBit(cfgRaw)) {
            ++count;
        }
    }

    f.close();
    return count;
}

void handleExtraElementsMenu() {
    static int32_t lastVal = encoder.getCount();
    int32_t newVal = encoder.getCount();
    if (newVal != lastVal) {
        int dir = (newVal > lastVal) ? 1 : -1;
        lastVal = newVal;
        extraElementsMenuSelection = (extraElementsMenuSelection + dir + 2) % 2;
        drawExtraElementsMenu(extraElementsMenuSelection);
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) buttonPressStart = millis();
        return;
    }
    if (buttonPressStart > 0) {
        unsigned long press = millis() - buttonPressStart;
        buttonPressStart = 0;
        if (press < 500) {
            if (extraElementsMenuSelection == 0) {
                confirmEnableDadoActive = true;
                confirmEnableDadoSelection = 0;
            } else {
                extraElementsMenuActive = false;
            }
        }
    }
}

void handleConfirmEnableDadoMenu(){
    static int32_t lastVal = encoder.getCount();
    int32_t newVal = encoder.getCount();
    if (newVal != lastVal){
        int dir = (newVal > lastVal) ? 1 : -1;
        lastVal = newVal;
        confirmEnableDadoSelection = (confirmEnableDadoSelection + dir + 2) % 2;
        drawConfirmEnableDadoMenu(confirmEnableDadoSelection);
    }

    if (digitalRead(ENC_BUTTON) == LOW){
        if (buttonPressStart == 0)
            buttonPressStart = millis();
        return;
    }
    if (buttonPressStart > 0){
        unsigned long press = millis() - buttonPressStart;
        buttonPressStart = 0;
        if (press < 500){
            if (confirmEnableDadoSelection == 0){
                // Sí → toggle persistente y recargar lista
                bool enabled = isDadoEnabled();
                setDadoEnabled(!enabled);
                loadElementsFromSPIFFS();

                if (!enabled){
                    for (size_t i = 0; i < elementFiles.size(); ++i){
                        if (elementFiles[i] == "Dado"){
                            currentIndex = (int)i;
                            break;
                        }
                    }
                }
            }

            // En ambos casos (Sí o No) cerramos y volvemos al menú principal
            confirmEnableDadoActive = false;
            extraElementsMenuActive = false;
            //drawCurrentElement();
        }
    }
}
