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
std::vector<bool> selectedStates;
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


void encoder_init_func() {
    pinMode(ENC_BUTTON, INPUT_PULLUP);
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder.attachSingleEdge(ENC_B, ENC_A); //original encoder.attachSingleEdge(ENC_A, ENC_B);
    encoder.clearCount();
    encoder.setCount(0);
    encoder.setFilter(1023);
}

bool ignoreInputs = false;
bool ignoreEncoderClick = false;
bool systemLocked = false;


unsigned long lastFocusChangeTime = 0;
int lastQueriedElementIndex = -1;

unsigned long lastModeQueryTime = 0;
int pendingQueryIndex = -1;
uint8_t pendingQueryID = 0xFF;
bool awaitingResponse = false;



void handleEncoder() {
    // 1) Ignorar click residual
    if (ignoreEncoderClick) {
        if (digitalRead(ENC_BUTTON) == HIGH) {
            ignoreEncoderClick = false;
        } else {
            return;
        }
    }

    // 2) Si la pantalla está apagada, solo despertar con acción real
    if (!displayOn) {
        if ((encoder.getCount() != lastEncoderValue) || (digitalRead(ENC_BUTTON) == LOW)) {
            display_wakeup();
            encoderIgnoreUntil     = millis() + 500;  // Ignorar durante 500 ms
            lastDisplayInteraction = millis();
            lastEncoderValue       = encoder.getCount();
        }
        return;
    }

    // 3) Menú cognitivo
    if (inCognitiveMenu) {
        static bool clicked = false;
        if (digitalRead(ENC_BUTTON) == LOW) {
            if (!clicked) {
                inCognitiveMenu    = false;
                clicked            = true;
                ignoreEncoderClick = true;
                std::vector<byte> target = { DEFAULT_BOTONERA };
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, target, COG_ACT_OFF));
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

    // 7) Mientras esté bloqueado, solo marcamos tiempo para desbloquear al soltar
    bool lockedMain = isInMainMenu() && systemLocked;
    if (lockedMain && digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
        return;
    }

    // 8) Menú de idiomas (solo si no está bloqueado)
    if (languageMenuActive && !lockedMain) {
        int32_t newVal = encoder.getCount();
        if (newVal != lastEncoderValue) {
            lastDisplayInteraction = millis();
            int32_t dir           = (newVal > lastEncoderValue) ? 1 : -1;
            lastEncoderValue      = newVal;
            languageMenuSelection = (languageMenuSelection + dir + 8) % 8;
            drawLanguageMenu(languageMenuSelection);
        }
        if (digitalRead(ENC_BUTTON) == LOW) {
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
                case 7: currentLanguage = Language::IT;     break;
                default: currentLanguage = Language::X1;  break;
            }
            saveLanguageToSPIFFS(currentLanguage);
            languageMenuActive = false;
            buttonPressStart  = 0;
            drawCurrentElement();
        }
        return;
    }

    // 9) Navegación por giro (solo si no está bloqueado)
    int32_t newEncoderValue = encoder.getCount();
    if (!lockedMain && newEncoderValue != lastEncoderValue) {
        lastDisplayInteraction = millis();
        int32_t direction      = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue       = newEncoderValue;

        if (!inModesScreen && elementFiles.size() > 1) {
            // Cambio de elemento
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();
            lastFocusChangeTime = millis();     // Marca el tiempo del cambio de foco
            lastQueriedElementIndex = -1;       // Resetea: aún no se ha consultado este nuevo elemento

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
            int realModeIndex = 0;
            byte modeConfig[2] = {0};
            if (currentFile == "Ambientes" || currentFile == "Fichas" ||
                currentFile == "Comunicador" || currentFile == "Apagar")
            {
                INFO_PACK_T* opt = nullptr;
                if      (currentFile == "Ambientes")   opt = &ambientesOption;
                else if (currentFile == "Fichas")      opt = &fichasOption;
                else if (currentFile == "Comunicador") opt = &comunicadorOption;
                else                                   opt = &apagarSala;      // “Apagar”

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
            adxl   = getModeFlag(modeConfig, HAS_SENS_VAL_1);
            useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);
            drawCurrentElement();
            
            uint8_t id = getCurrentElementID();
            if (RelayStateManager::hasRelay(id)) {
                send_frame(frameMaker_REQ_ELEM_SECTOR(
                    DEFAULT_BOTONERA,
                    id,
                    SPANISH_LANG,
                    ELEM_CURRENT_FLAGS_SECTOR
                ));
            }
        }
        else if (inModesScreen && totalModes > 0) {
            // Cambio de modo
            int newIndex = currentModeIndex + direction;
            if (newIndex >= 0 && newIndex < totalModes) {
                currentModeIndex = newIndex;
                int realModeIndex = globalVisibleModesMap[currentModeIndex];
                if (realModeIndex >= 0) {
                    String file = elementFiles[currentIndex];
                    colorHandler.setCurrentFile(file);
                    colorHandler.setPatternBotonera(realModeIndex, ledManager);
                }
                drawModesScreen();
            }
        }
    }

    //9.5 ⏱️ Envío diferido de consulta de modo tras 100ms de foco
    if ((millis() - lastFocusChangeTime > 100) && lastQueriedElementIndex != currentIndex) {
        String currentFile = elementFiles[currentIndex];
        if (currentFile != "Comunicador" &&
            currentFile != "Fichas" &&
            currentFile != "Ambientes" &&
            currentFile != "Apagar") {

            fs::File f = SPIFFS.open(currentFile, "r");
            if (f) {
                f.seek(OFFSET_ID, SeekSet);
                f.read(&pendingQueryID, 1);
                f.close();

                send_frame(frameMaker_REQ_ELEM_SECTOR(
                    DEFAULT_BOTONERA,
                    pendingQueryID,
                    SPANISH_LANG,
                    ELEM_CMODE_SECTOR
                ));

                lastModeQueryTime = millis();
                pendingQueryIndex = currentIndex;
                awaitingResponse = true;
                lastQueriedElementIndex = currentIndex;

                frameReceived = false;
            }
        }
    }

    // ⏳ Timeout de 200ms para detectar si el elemento no respondió
    if (awaitingResponse && (millis() - lastModeQueryTime > 500)) {
        if (!frameReceived && pendingQueryIndex >= 0 && pendingQueryIndex < (int)elementFiles.size()) {
            selectedStates[pendingQueryIndex] = false;
            DEBUG__________printf("⚠️ Elemento %s no respondió en 200ms → marcado como NO SELECCIONADO\n",
                                elementFiles[pendingQueryIndex].c_str());
        }
        awaitingResponse = false;
    }

    // 10) Lectura del botón mantenido (solo si no está bloqueado)
    if (!lockedMain && digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        } else {
            // Pulsación larga: imprimir detalles al pasar 6000 ms
            if (isInMainMenu() && !isLongPress && (millis() - buttonPressStart >= 6000)) {
                printElementDetails();
                isLongPress = true;  // Para que no vuelva a reentrar
                return;              // Salimos inmediatamente, sin esperar a soltar
            }

            // Pulsación larga en menú de modos (2 s): alternar modo alternativo
            if (inModesScreen && !isLongPress && (millis() - buttonPressStart >= 2000)) {
                if (currentModeIndex > 0 && currentModeIndex < totalModes - 1) {
                    int adjustedIndex = currentModeIndex - 1;
                    String currFile = elementFiles[currentIndex];
                    uint8_t modeConfig[2] = {0};
                    bool canToggle = false;

                    // Obtener configuración del modo
                    if (currFile == "Ambientes" || currFile == "Fichas") {
                        INFO_PACK_T* option = (currFile == "Ambientes") ? &ambientesOption : &fichasOption;
                        int count = 0;
                        for (int i = 0; i < 16; i++) {
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
                    else if (currFile != "Apagar") {
                        fs::File f = SPIFFS.open(currFile, "r");
                        if (f) {
                            int count = 0;
                            for (int i = 0; i < 16; i++) {
                                char modeName[25] = {0};
                                byte tempConfig[2] = {0};
                                f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                                f.read((uint8_t*)modeName, 24);
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

                    // Alternar estado alternativo y persistir
                    if (canToggle &&
                        adjustedIndex >= 0 &&
                        currentAlternateStates.size() > (size_t)adjustedIndex) {
                        currentAlternateStates[adjustedIndex] = !currentAlternateStates[adjustedIndex];
                        elementAlternateStates[currFile] = currentAlternateStates;
                        if (currFile != "Ambientes" &&
                            currFile != "Fichas" &&
                            currFile != "Apagar") {
                            fs::File f = SPIFFS.open(currFile, "r+");
                            if (f) {
                                const int OFFSET_ALTERNATE_STATES = OFFSET_CURRENTMODE + 1;
                                f.seek(OFFSET_ALTERNATE_STATES, SeekSet);
                                byte states[16] = {0};
                                for (size_t i = 0; i < min(currentAlternateStates.size(), (size_t)16); i++) {
                                    states[i] = currentAlternateStates[i] ? 1 : 0;
                                }
                                f.write(states, 16);
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
    if (digitalRead(ENC_BUTTON) == HIGH) {
        // 11.0) Si estaba bloqueado y soltaste entre 1000 ms y 5000 ms → desbloquear
        if (lockedMain && buttonPressStart > 0) {
            unsigned long pressDuration = millis() - buttonPressStart;
            if (pressDuration >= 500 && pressDuration <= 5000) {
                systemLocked    = false;
                drawCurrentElement();
                buttonPressStart = 0;
                isLongPress      = false;
                return;
            }
        }

        // 11.1) Si sigue bloqueado (no entró en 11.0), ignorar suelta corta
        if (lockedMain) {
            buttonPressStart = 0;
            isLongPress      = false;
            return;
        }

        if (buttonPressStart > 0) {
            unsigned long pressDuration = millis() - buttonPressStart;
#ifdef DEBUG
            DEBUG__________ln("DEBUG: Duración suelta: " + String(pressDuration) + " ms");
#endif
            // 11.2) Si ya procesamos un long-press (por detalles o alternar modo), no re-procesar
            if (isLongPress && !systemLocked) {
                buttonPressStart = 0;
                isLongPress      = false;
                return;
            }

            // 11.3) Solo en menú principal y desbloqueado:
            //       1000–5000 ms → BLOQUEAR
            //       <500 ms → acción corta (Apagar o abrir modos)
            //       ≥6000 ms → detalles (por si no se llamó en sección 10)
            if (isInMainMenu()) {
                if (pressDuration >= 500 && pressDuration <= 5000) {
                    // — BLOQUEAR —
                    systemLocked = true;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
                else if (pressDuration < 500) {
                    // — “Short press” (<500 ms) — distinguir si es “Apagar” o “Abrir modos” —
                    String currentFile = elementFiles[currentIndex];
                    if (currentFile == "Apagar") {
                        // Apagar la sala (rutinario), idéntico a antes
                        for (size_t i = 0; i < selectedStates.size(); i++) {
                            selectedStates[i] = false;
                        }
                        std::vector<byte> id = { 0xFF };
                        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, id, BLACKOUT));
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
                    relayStep = -1;
                    idsSPIFFS.clear();
                    communicatorActiveID = 0xFF;

                    // Usamos el toggle del propio Comunicador como "intención" (encender/apagar en masa)
                    bool turningOn = !selectedStates[currentIndex];

                    auto isSelectableElement = [&](const String& name) {
                        return !(name == "Ambientes" || name == "Fichas" || name == "Apagar");
                        // añade aquí otros fijos que NO quieras seleccionar
                    };

                    // Aplica la selección solo a los elementos “reales”
                    for (size_t i = 0; i < elementFiles.size(); ++i) {
                        if (isSelectableElement(elementFiles[i])) {
                            selectedStates[i] = turningOn;   // true al “encender todos”, false al “apagar todos”
                        } else {
                            // Asegura que los especiales se quedan deseleccionados
                            selectedStates[i] = false;
                        }
                    }

                    if (turningOn) {
                        // START_CMD a todos (broadcast)
                        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, { 0xFF }, START_CMD));
                    } else {
                        // BLACKOUT a todos (broadcast) + feedback
                        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, { 0xFF }, BLACKOUT));
                        showMessageWithLoading(getTranslation("APAGANDO_ELEMENTOS"), 4000);
                    }

                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
                    else {
                        // No es “Apagar”, abrimos el submenú de modos
                        inModesScreen    = true;
                        currentModeIndex = 0;
                        drawModesScreen();
                        buttonPressStart = 0;
                        isLongPress      = false;
                        return;
                    }
                }
                else if (pressDuration >= 6000) {
                    // — Detalles (por si no se disparó en sección 10) —
                    printElementDetails();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
            }

            // 11.4) Resto de lógica: si no entramos en ninguna de las ramas anteriores,
            //       procesamos “Apagar” o “Seleccionar modo” en menú de modos.
            String currentFile = elementFiles[currentIndex];
            if (!inModesScreen) {
                if (currentFile == "Apagar") {
                    // Aquí realmente no llegaríamos, porque se habría procesado en la rama <500 ms
                    // (se garantizó un return). Pero por completitud:
                    for (size_t i = 0; i < selectedStates.size(); i++) {
                        selectedStates[i] = false;
                    }
                    std::vector<byte> id = { 0xFF };
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, id, BLACKOUT));
                    setAllElementsToBasicMode();
                    doitPlayer.stop_file();
                    showMessageWithLoading(getTranslation("APAGANDO_SALA"), 4000);
                    currentIndex     = 0;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress      = false;
                    return;
                }
                else if (pressDuration < 500) {
                    // Si no es “Apagar” y nos pilló aquí (corte de lógica), entramos a modos
                    if (currentFile != "Comunicador") {
                        inModesScreen    = true;
                        currentModeIndex = 0;
                        drawModesScreen();
                    }
                }
            } else {
                // Si ya estábamos en menú de modos y fue press corto <500 ms
                if (!isLongPress && pressDuration < 500) {
                    handleModeSelection(elementFiles[currentIndex]);
                }
            }
        }

        buttonPressStart = 0;
        isLongPress      = false;
    }
}


bool modeAlternateActive = false;
// Función handleModeSelection modificada
void handleModeSelection(const String& currentFile) {
    //  // —— BLOQUE DE “LIMPIEZA” DEL MODO SALIENTE ——
    // {
    //     uint8_t oldConfig[2] = {0};
    //     if ( getModeConfig(currentFile, currentModeIndex, oldConfig) ) {
    //         byte targetID = getCurrentElementID();
    //         // 1) Relay OFF si tocaba
    //         if ( getModeFlag(oldConfig, HAS_RELAY) ) {
    //             send_frame(frameMaker_SEND_FLAG_BYTE(
    //                 DEFAULT_BOTONERA,
    //                 std::vector<byte>{ targetID },
    //                 0x00
    //             ));
    //         }
    //         // 2) Sensor doble a cero si tocaba
    //         if ( getModeFlag(oldConfig, HAS_SENS_VAL_1) ) {
    //             SENSOR_DOUBLE_T zeroDouble = {};
    //             send_frame(frameMaker_SEND_SENSOR_VALUE(
    //                 DEFAULT_BOTONERA,
    //                 std::vector<byte>{ targetID },
    //                 zeroDouble
    //             ));
    //         }
    //         // 3) Sensor simple a cero si tocaba
    //         if ( getModeFlag(oldConfig, HAS_SENS_VAL_2) ) {
    //             SENSOR_VALUE_T zeroSingle = {};
    //             send_frame(frameMaker_SEND_SENSOR_VALUE_2(
    //                 DEFAULT_BOTONERA,
    //                 std::vector<byte>{ targetID },
    //                 zeroSingle
    //             ));
    //         }
    //         // 4) Color BLACK si estaba en BASIC_COLOR
    //         if ( getModeFlag(oldConfig, HAS_BASIC_COLOR) ) {
    //             Serial.println("COLOR SENDED: 1");
    //             send_frame(frameMaker_SEND_COLOR(
    //                 DEFAULT_BOTONERA,
    //                 std::vector<byte>{ targetID },
    //                 BLACK
    //             ));
    //         }
    //         // 5) Color BLACK si estaba en ADVANCED_COLOR
    //         if ( getModeFlag(oldConfig, HAS_ADVANCED_COLOR) ) {
    //             Serial.println("COLOR SENDED: 2");
    //             send_frame(frameMaker_SEND_COLOR(
    //                 DEFAULT_BOTONERA,
    //                 std::vector<byte>{ targetID },
    //                 BLACK
    //             ));
    //         }
    //     }
    // }
    // // —— FIN BLOQUE DE LIMPIEZA ——

    // Si se selecciona la opción "Regresar" (valor -2), salir del menú.
    if (globalVisibleModesMap[currentModeIndex] == -2) {
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
    
    // Si se selecciona la opción "Encender/Apagar" (índice 0, valor -3).
    if (currentModeIndex == 0) {
        bool wasSelected = selectedStates[currentIndex];
        selectedStates[currentIndex] = !wasSelected;
        if (currentFile == "Ambientes") {
            // 🔥 Nuevo comportamiento: enviar START_CMD por broadcast
            if (selectedStates[currentIndex]) {
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{0xFF}, START_CMD));
            } else {
                doitPlayer.stop_file();
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{0xFF}, BLACKOUT));
                for (size_t i = 0; i < selectedStates.size(); i++) {
                        selectedStates[i] = false;
                    }
                setAllElementsToBasicMode();
                doitPlayer.stop_file();
            }
        }
        else if (currentFile != "Fichas" && currentFile != "Apagar") {
            fs::File f = SPIFFS.open(currentFile, "r+");
            if (f) {
                if (selectedStates[currentIndex]) {
                    // Encender
                    send_frame(frameMaker_SEND_COMMAND(
                        DEFAULT_BOTONERA,
                        std::vector<byte>{getCurrentElementID()},
                        START_CMD
                    ));
                } else {
                    // Apagar
                    send_frame(frameMaker_SEND_COMMAND(
                        DEFAULT_BOTONERA,
                        std::vector<byte>{getCurrentElementID()},
                        BLACKOUT
                    ));

                    // 1) Restablecer modo básico en SPIFFS
                    {
                        fs::File f2 = SPIFFS.open(currentFile, "r+");
                        if (f2) {
                            byte basicMode = DEFAULT_BASIC_MODE;
                            f2.seek(OFFSET_CURRENTMODE, SeekSet);
                            f2.write(&basicMode, 1);

                            // 2) Limpiar los 16 bytes de estados alternativos justo después
                            const int OFFSET_ALTERNATE_STATES = OFFSET_CURRENTMODE + 1;
                            byte zeros[16] = {0};
                            f2.seek(OFFSET_ALTERNATE_STATES, SeekSet);
                            f2.write(zeros, sizeof(zeros));

                            f2.close();
                        }
                    }

                    // 3) Limpiar en RAM los flags alternativos de este elemento
                    elementAlternateStates[currentFile].assign(
                        elementAlternateStates[currentFile].size(), false
                    );

                    // 4) Mantener el resto de tu lógica
                    setAllElementsToBasicMode();
                    showMessageWithLoading(getTranslation("APAGANDO_ELEMENTO"), 2000);
                    selectedStates[currentIndex] = false;
                }
                f.close();
            }
        }
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
    
    // Para las opciones de modo (índices > 0, excepto "Regresar").
    int adjustedVisibleIndex = currentModeIndex - 1;
    String modeName;
    uint8_t modeConfig[2] = {0};
    int realModeIndex = 0;
    
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        int count = 0;
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                if (count == adjustedVisibleIndex) {
                    realModeIndex = i;
                    modeName = String((char*)option->mode[i].name);
                    memcpy(modeConfig, option->mode[i].config, 2);
                    break;
                }
                count++;
            }
        }
        option->currentMode = realModeIndex;
    } else if (currentFile != "Apagar") {
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            int count = 0;
            for (int i = 0; i < 16; i++) {
                char modeBuf[25] = {0};
                byte tempConfig[2] = {0};
                f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                f.read((uint8_t*)modeBuf, 24);
                f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                f.read(tempConfig, 2);
                if (strlen(modeBuf) > 0 && checkMostSignificantBit(tempConfig)) {
                    if (count == adjustedVisibleIndex) {
                        realModeIndex = i;
                        modeName = String(modeBuf);
                        memcpy(modeConfig, tempConfig, 2);
                        break;
                    }
                    count++;
                }
            }
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write((uint8_t*)&realModeIndex, 1);
            f.close();
        }
    }
    
    // Actualizar las variables adxl y useMic según la configuración del modo
    adxl = getModeFlag(modeConfig, HAS_SENS_VAL_1);
    useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);
#ifdef DEBUG
    DEBUG__________ln("🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻");
    DEBUG__________ln("DEBUG: Mode Name (procesado): " + modeName);
    DEBUG__________ln("DEBUG: adxl status: " + String(adxl ? "true" : "false"));
    DEBUG__________ln("DEBUG: useMic status: " + String(useMic ? "true" : "false"));
    DEBUG__________ln("🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻");
#endif
    
    // Confirmación normal (pulsación corta) para el modo seleccionado.
    bool wasAlreadySelected = selectedStates[currentIndex];
    if (!wasAlreadySelected) {
        selectedStates[currentIndex] = true;
    }
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{0}, START_CMD));
        delay(300);
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, std::vector<byte>{0}, realModeIndex));
        if (currentFile == "Fichas") {// Mapear el modo real seleccionado a un TOKEN_MODE_
        TOKEN_MODE_ tokenMode;
        switch (realModeIndex) {
            case 0: tokenMode  = TOKEN_BASIC_MODE;   bankSelectionActive = false; break;
            case 1: tokenMode  = TOKEN_PARTNER_MODE; bankSelectionActive = false; break;
            case 2: tokenMode  = TOKEN_GUESS_MODE;   bankSelectionActive = true;  break;
            default: tokenMode = TOKEN_BASIC_MODE;   bankSelectionActive = false; break;
        }

            // Llamar a la función set_mode de la instancia token
            token.set_mode(tokenMode);}
    } else if (currentFile != "Apagar") {
        byte modeConfigTemp[2] = {0};
        memcpy(modeConfigTemp, modeConfig, 2);  // Usar la configuración que ya tenemos
        if (!wasAlreadySelected) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, START_CMD));
            delay(300);
        }
        
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, realModeIndex));
        delay(300);
        if (getModeFlag(modeConfigTemp, HAS_ALTERNATIVE_MODE)) {
            
            if (currentAlternateStates.size() > (size_t)adjustedVisibleIndex && currentAlternateStates[adjustedVisibleIndex]) {
                modeAlternateActive = true;
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, ALTERNATE_MODE_ON));
                delay(300);
            } else {
                modeAlternateActive = false;
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, ALTERNATE_MODE_OFF));
                delay(300);
            }
        }
        
        
    }

    if (!bankSelectionActive) {
        inModesScreen = false;
        drawCurrentElement();
    } else {
        // Si estamos en modo ADIVINAR, permanecemos en este submenú.
        inModesScreen = false;
        // Por ejemplo, podrías llamar a la función que dibuja el menú de banks:
        // (suponiendo que bankList y selectedBanks sean variables globales o accesibles)
        drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset); // El índice inicial puede ser 0
    }
}

std::vector<bool> initializeAlternateStates(const String &currentFile) {
    std::vector<bool> states;
    // Para elementos fijos ("Ambientes" o "Fichas")
    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                // Iniciar en false = versión NO alterna (modo básico)
                states.push_back(false);
            }
        }
    }
    // Para elementos almacenados en SPIFFS (excepto "Apagar")
    else if (currentFile != "Apagar") {
        // Si queremos leer algún valor guardado, lo haríamos aquí; de lo contrario, se inicializa en false.
        for (int i = 0; i < 16; i++) {
            states.push_back(false);
        }
    }
    // Para "Apagar" se puede definir un vector simple
    else {
        states.push_back(false);
    }
    return states;
}

void toggleElementSelection(const String& currentFile) {
    // Alternar el estado de selección del elemento actual
    selectedStates[currentIndex] = !selectedStates[currentIndex];

    // Obtener la ID del elemento (si es de SPIFFS; si es fijo, usamos ID 0)
    std::vector<byte> elementID;
    bool isElementFromSPIFFS = !currentFile.startsWith("Ambientes") &&
                               !currentFile.startsWith("Fichas") &&
                               !currentFile.startsWith("Apagar");

    if (isElementFromSPIFFS) {
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            byte id = 0;
            f.seek(OFFSET_ID, SeekSet);
            f.read(&id, 1);
            elementID.push_back(id);
            f.close();
        } else {
            DEBUG__________ln("Error al leer la ID del archivo.");
            elementID.push_back(0);
        }
    } else {
        elementID.push_back(0);
    }

    // Si se deselecciona el elemento (estado false), reiniciamos el vector de modos para ese elemento
    if (!selectedStates[currentIndex]) {
        // Para elementos que NO sean "Apagar"
        if (!currentFile.startsWith("Apagar")) {
            // Reinicializar el vector de estados alternativos para el elemento actual
            std::vector<bool> newStates = initializeAlternateStates(currentFile);
            // Actualizar tanto el map global como el vector local
            elementAlternateStates[currentFile] = newStates;
            currentAlternateStates = newStates;
        }
    }

    // Si se presiona el botón "Apagar", reiniciamos los modos de TODOS los elementos
    if (currentFile == "Apagar") {
        // Reiniciar la selección de todos los elementos
        for (size_t i = 0; i < selectedStates.size(); i++) {
            selectedStates[i] = false;
        }
        // Reinicializar los estados alternativos para cada elemento en el map global
        for (auto &entry : elementAlternateStates) {
            entry.second = initializeAlternateStates(entry.first);
        }
        std::vector<byte> elementID;
        elementID.push_back(0xFF);
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));
        setAllElementsToBasicMode();
        showMessageWithLoading(getTranslation("APAGANDO_SALA"), 4000);
        currentIndex = 0;
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
    
    // Para elementos de SPIFFS, se envía el comando de selección según el estado
    if (isElementFromSPIFFS) {
        byte command = selectedStates[currentIndex] ? START_CMD : BLACKOUT;
        DEBUG__________printf("Enviando comando %s a la ID %d\n",
                      command == START_CMD ? "START_CMD" : "BLACKOUT",
                      elementID[0]);
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, command));

        if (command == BLACKOUT) {
            showMessageWithLoading(getTranslation("APAGANDO_ELEMENTO"), 2000);
        }

        // Actualizar el modo del elemento a "básico" en SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            byte basicMode = DEFAULT_BASIC_MODE;  // modo 1
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write(&basicMode, 1);
            f.close();
            DEBUG__________printf("Modo actualizado a básico (1) en SPIFFS para el elemento %s\n", currentFile.c_str());
        } else {
            DEBUG__________ln("Error al abrir el archivo para actualizar el modo.");
        }
    }
    
    // Redibujar el elemento actual para reflejar la selección/deselección
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
            drawCurrentElement();
        }
    }

    // Actualizar estado del botón para la próxima detección de flanco
    encoderPressed = currentEncoderState;
}


/**
 * @brief Gestiona la navegación y confirmación del menú de sonido.
 *
 * Desplaza la selección con el encoder (wrap circular sobre las opciones válidas)
 * y, ante una pulsación corta (< 1000 ms) del botón del encoder, aplica el ajuste
 * correspondiente (género de voz, respuesta negativa, volumen) o confirma/sale.
 */

const int soundOptions[] = {0, 1, 3, 4, 6, 7, 9}; // Índices seleccionables
const int numSoundOptions = sizeof(soundOptions) / sizeof(soundOptions[0]);
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
                    drawCurrentElement();
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

void handleHiddenMenuNavigation(int &hiddenMenuSelection) {
    int32_t newEncoderValue = encoder.getCount();
    static bool encoderButtonPressed = false;
    static bool initialEntry = true;
    static bool menuJustOpened = true;  // Nueva variable para controlar la confirmación automática

    // Al entrar al menú oculto por primera vez, resalta la primera opción sin confirmarla
    if (initialEntry) {
        hiddenMenuSelection = 0;  // Preselección visual sin confirmar
        //drawHiddenMenu(hiddenMenuSelection);
        initialEntry = false;
        menuJustOpened = true;  // Bloquea la confirmación inmediata
    }

    // Navegación por el menú con el encoder
    if (newEncoderValue != lastEncoderValue) {
        hiddenMenuSelection += (newEncoderValue > lastEncoderValue) ? 1 : -1;
        hiddenMenuSelection = constrain(hiddenMenuSelection, 0, 4); // Ahora hay 6 opciones (índices 0-5)
        lastEncoderValue = newEncoderValue;
        //drawHiddenMenu(hiddenMenuSelection);
    }

    // Confirmación con el botón del encoder
    if (digitalRead(ENC_BUTTON) == HIGH) {
        menuJustOpened = false;  // Solo ahora permite confirmaciones
        encoderButtonPressed = false;
    }

    if (digitalRead(ENC_BUTTON) == LOW && !encoderButtonPressed && !menuJustOpened) {
    encoderButtonPressed = true;
    ignoreEncoderClick = true;
    byte respuesta = 0;
    switch (hiddenMenuSelection) {
        case 0: // Cambiar idioma
        // Activar el submenú de idioma
        languageMenuActive = true;
        languageMenuSelection = 0;  // Inicialmente se selecciona la primera opción (ES)
        // Dibujar el submenú
        drawLanguageMenu(languageMenuSelection);
        hiddenMenuActive = false;
            break;
            
        case 1: // Sonido
        soundMenuActive = true;
        soundMenuSelection = 0;
        drawSoundMenu(soundMenuSelection);
        hiddenMenuActive = false;
                                                                                            #ifdef DEBUG
                                                                                            DEBUG__________ln("Cambiando Sonido...");
                                                                                            #endif
            break;
        case 2: // Brillo
                                                                                            #ifdef DEBUG
                                                                                            DEBUG__________ln("Ajustando brillo...");
                                                                                            #endif

        hiddenMenuActive = false;                // 🛑 Desactivar menú oculto
        brightnessMenuActive = true;             // ✅ Activar menú brillo
    
        currentBrightness = loadBrightnessFromSPIFFS();
        tempBrightness = currentBrightness;
        encoder.setCount(currentBrightness);
    
        // 🔄 Reiniciar estados
        lastEncoderCount = currentBrightness;
        encoderPressed = (digitalRead(ENC_BUTTON) == LOW);
        ignoreFirstRelease = true;
    
        drawBrightnessMenu();
            break;
        case 3: // Control
            hiddenMenuActive = false;
            formatSubMenuActive = true;
            formatMenuSelection = 0;
            buttonPressStart = 0;
            extern int formatMenuCurrentIndex;
            extern int32_t formatMenuLastValue;
            formatMenuCurrentIndex = 0;
            formatMenuLastValue = encoder.getCount();

            while (digitalRead(ENC_BUTTON) == LOW); // Espera a que se suelte el botón  
            drawFormatMenu(formatMenuSelection);
            break;
        case 4: // Volver
                                                                                            #ifdef DEBUG
                                                                                            DEBUG__________ln("Volviendo al menú principal");
                                                                                            #endif
            
            PulsadoresHandler::limpiarEstados();
            drawCurrentElement();
            initialEntry = false;
            hiddenMenuActive = false;
            break;
        default:
            break;
    }
}
    if (hiddenMenuActive) {
        drawHiddenMenu(hiddenMenuSelection);
    }
}


bool getModeFlag(const uint8_t modeConfig[2], MODE_CONFIGS flag) {
    // Construir el uint16_t interpretando data[0] como el MSByte y data[1] como el LSByte (big-endian)
    uint16_t config = (uint16_t(modeConfig[0]) << 8) | uint16_t(modeConfig[1]);
    // Extraer el bit (el enum indica el offset desde el bit 0, LSB)
    return (config >> flag) & 1;
}


void debugModeConfig(const uint8_t modeConfig[2]) {
    DEBUG__________ln("===== Estado de modeConfig =====");
    for (int i = HAS_BASIC_COLOR; i <= MODE_EXIST; i++) {
        MODE_CONFIGS flag = static_cast<MODE_CONFIGS>(i);
        bool isActive = getModeFlag(modeConfig, flag);
        switch (flag) {
            case HAS_BASIC_COLOR:   DEBUG__________("HAS_BASIC_COLOR"); break;
            case HAS_PULSE:         DEBUG__________("HAS_PULSE"); break;
            case HAS_ADVANCED_COLOR:DEBUG__________("HAS_ADVANCED_COLOR"); break;
            case HAS_RELAY:         DEBUG__________("HAS_RELAY"); break;
            case HAS_RELAY_N1:       DEBUG__________("HAS_RELAY_2"); break;
            case HAS_RELAY_N2:       DEBUG__________("HAS_RELAY_3"); break;
            case NOP_1:              DEBUG__________("HAS_RELAY_4"); break;
            case HAS_SENS_VAL_1:    DEBUG__________("HAS_SENS_VAL_1"); break;
            case HAS_SENS_VAL_2:    DEBUG__________("HAS_SENS_VAL_2"); break;
            case NOP_2:             DEBUG__________("ACCEPTS_X_Y_VAL"); break;
            case HAS_PASSIVE:       DEBUG__________("HAS_PASSIVE"); break;
            case HAS_BINARY_SENSORS:DEBUG__________("HAS_BINARY_SENSORS"); break;
            case HAS_BANK_FILE:     DEBUG__________("HAS_BANK_FILE"); break;
            case HAS_PATTERNS:      DEBUG__________("HAS_PATTERNS"); break;
            case HAS_ALTERNATIVE_MODE:DEBUG__________("HAS_ALTERNATIVE_MODE"); break;
            case MODE_EXIST:        DEBUG__________("MODE_EXIST"); break;
        }
        DEBUG__________(" = ");
        DEBUG__________ln(isActive ? "1" : "0");
    }
}

void handleBankSelectionMenu(std::vector<byte>& bankList, std::vector<bool>& selectedBanks) {
    // Calcular total de opciones (Confirmar + banks)
    int totalItems = bankList.size() + 1;

    // Leer el valor actual del encoder y actualizar bankMenuCurrentSelection
    int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        bankMenuCurrentSelection += direction;
        if (bankMenuCurrentSelection < 0) bankMenuCurrentSelection = totalItems - 1;
        if (bankMenuCurrentSelection >= totalItems) bankMenuCurrentSelection = 0;
        lastEncoderValue = newEncoderValue;

        // Ajustar window offset para que currentSelection sea visible
        if (bankMenuCurrentSelection < bankMenuWindowOffset) {
            bankMenuWindowOffset = bankMenuCurrentSelection;
        } else if (bankMenuCurrentSelection >= bankMenuWindowOffset + bankMenuVisibleItems) {
            bankMenuWindowOffset = bankMenuCurrentSelection - bankMenuVisibleItems + 1;
        }
        // Redibujar el menú con los parámetros actualizados
        drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset);
    }

    // Detectar pulsación del botón del encoder (con debounce)
    if (digitalRead(ENC_BUTTON) == LOW) {
        delay(200);
        // Si la opción actual es "Confirmar" (índice 0), finalizar el menú
        if (bankMenuCurrentSelection == 0) {
            // Aquí podrías imprimir la selección para depuración
            DEBUG__________ln("Bancos seleccionados:");
            for (size_t i = 0; i < selectedBanks.size(); i++) {
                if (selectedBanks[i]) {
                    DEBUG__________("0x");
                    DEBUG__________(bankList[i], HEX);
                    DEBUG__________(" ");
                }
            }
            DEBUG__________ln();
            // Reiniciar el encoder para el siguiente uso
            encoder.clearCount();
            lastEncoderValue = encoder.getCount();
            // Desactivar el menú de selección y regresar al menú principal
            bankSelectionActive = false;
            drawCurrentElement();
            return;
        } else {
            // Si no es Confirmar, alternar el estado del bank correspondiente
            int bankIndex = bankMenuCurrentSelection - 1;
            if (bankIndex >= 0 && bankIndex < (int)bankList.size()) {
                selectedBanks[bankIndex] = !selectedBanks[bankIndex];
            }
            // Redibujar el menú para reflejar el cambio
            drawBankSelectionMenu(bankList, selectedBanks, bankMenuCurrentSelection, bankMenuWindowOffset);
            // Esperar a que se suelte el botón para evitar múltiples toggles
            while (digitalRead(ENC_BUTTON) == LOW) {
                delay(10);
            }
            delay(200);
        }
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

void handleFormatMenu() {
    int32_t newEncoderValue = encoder.getCount();
    if (formatMenuLastValue == 0) formatMenuLastValue = newEncoderValue;  // Protección inicial

    // Usa variables globales
    int& currentIndex = formatMenuCurrentIndex;
    int32_t& lastValue = formatMenuLastValue;


    if (newEncoderValue != lastValue) {
    int dir = (newEncoderValue > lastValue) ? 1 : -1;
    lastValue = newEncoderValue;

    int proposedIndex = currentIndex + dir;
        if (proposedIndex >= 0 && proposedIndex < numFormatOptions) {
            currentIndex = proposedIndex;
            formatMenuSelection = formatOptions[currentIndex];
            //drawFormatMenu(formatMenuSelection);
        }
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        if (buttonPressStart > 0 && millis() - buttonPressStart < 1000) {
            switch (formatMenuSelection) {
                case 0: {// Escanear sala
                    element->escanearSala();
                    hiddenMenuActive = false;
                    formatSubMenuActive = false;
                    drawCurrentElement();
                    break;
                }
                case 1:  // Eliminar elemento
                    loadDeletableElements();
                    if (deletableElementFiles.size() > 0) {
                        DEBUG__________ln("[📂] Lista de elementos disponibles para eliminar:");
                        for (size_t i = 0; i < deletableElementFiles.size(); ++i) {
                            DEBUG__________printf(" - %s\n", deletableElementFiles[i].c_str());
                        }

                        deleteElementMenuActive = true;
                        deleteElementSelection = 0;
                        formatSubMenuActive = false;
                        drawDeleteElementMenu(deleteElementSelection);
                        forceDrawDeleteElementMenu = true;

                    } else {
                        DEBUG__________ln("No hay elementos para eliminar.");
                    }
                    break;

                case 2:  // Formatear SPIFFS
                    confirmRestoreMenuActive = true;
                    confirmRestoreSelection = 0;
                    formatSubMenuActive = false;
                    drawConfirmRestoreMenu(confirmRestoreSelection);
                    break;
                case 3:  // Mostrar ID
                    DEBUG__________ln("[🆔] Mostrando ID");
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, {BROADCAST}, SHOW_ID_CMD));
                    formatSubMenuActive = true;
                    showMessageWithLoading(getTranslation("SHOW_ID"), 3000);
                    // drawCurrentElement(); // volver al menú principal
                    break;
                
                case 4:  // Restaurar (formatear SPIFFS)
                    DEBUG__________ln("Restaurando elementos");
                    confirmRestoreMenuElementActive = true;
                    confirmRestoreElementSelection = 0;
                    formatSubMenuActive = false;
                    drawConfirmRestoreElementMenu(confirmRestoreElementSelection);
                break;

                case 5:  // Volver
                    formatSubMenuActive = false;
                    hiddenMenuActive = true;
                    drawHiddenMenu(0);
                    //currentIndex = 0;
                    formatMenuCurrentIndex = 0;
                    formatMenuLastValue = encoder.getCount();
                    break;
                
            }
            
            if (formatSubMenuActive && !confirmRestoreMenuActive && !deleteElementMenuActive){
                drawFormatMenu(formatMenuSelection);
            }
        }
        buttonPressStart = 0;
    }
    if (formatSubMenuActive && !confirmRestoreMenuActive && !deleteElementMenuActive && !confirmRestoreMenuElementActive) {
        // Pasa el ÍNDICE actual para que el resaltado y el scroll coincidan
        drawFormatMenu(currentIndex);
    }
}

void handleConfirmRestoreMenu() {
    static int lastSelection = 0;
    int32_t newValue = encoder.getCount();

    if (newValue != lastSelection) {
        int dir = (newValue > lastSelection) ? 1 : -1;
        lastSelection = newValue;
        confirmRestoreSelection = (confirmRestoreSelection + dir + 2) % 2;
        drawConfirmRestoreMenu(confirmRestoreSelection);
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) buttonPressStart = millis();
    } else {
        if (buttonPressStart > 0 && millis() - buttonPressStart < 1000) {
            if (confirmRestoreSelection == 0) {
                // Opción "Sí"
                DEBUG__________ln("[⚠️] Restaurando sala...");
                formatSPIFFS();
                loadElementsFromSPIFFS();
                confirmRestoreMenuActive = false;
                formatSubMenuActive = false;
                ESP.restart();  // Reinicia el sistema tras formatear
            } else {
                // Opción "No"
                confirmRestoreMenuActive = false;
                formatSubMenuActive = false;
                drawFormatMenu(formatMenuSelection);
            }
        }
        buttonPressStart = 0;
    }
}

void handleConfirmRestoreElementMenu() {
    static int lastSelection = 0;
    int32_t newValue = encoder.getCount();

    if (newValue != lastSelection) {
        int dir = (newValue > lastSelection) ? 1 : -1;
        lastSelection = newValue;
        confirmRestoreElementSelection = (confirmRestoreElementSelection + dir + 2) % 2;
        drawConfirmRestoreElementMenu(confirmRestoreElementSelection);
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) buttonPressStart = millis();
    } else {
        if (buttonPressStart > 0 && millis() - buttonPressStart < 1000) {
            if (confirmRestoreElementSelection == 0) {
                // Opción "Sí"
                DEBUG__________ln("[⚠️] Restaurando elementos...");
                send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, {BROADCAST}, DEFAULT_DEVICE));
                delay(600);
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, {BROADCAST}, SHOW_ID_CMD));
                delay(5000);
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, {BROADCAST}, BLACKOUT));
                delay(500);
        
            } 
            confirmRestoreMenuElementActive = false;
            formatSubMenuActive = true;
            formatMenuSelection = 0;
            drawFormatMenu(formatMenuSelection);
        }
        buttonPressStart = 0;
    }
}

bool deleteElementMenuActive = false;
int deleteElementSelection = 0;
std::vector<String> deletableElementFiles;
bool confirmDeleteActive = false;
int confirmSelection = 0;
String confirmedFileToDelete = "";

void handleDeleteElementMenu() {
    static int currentIndex = 0;
    int32_t newEncoderValue = encoder.getCount();
    static int32_t lastValue = newEncoderValue;

    if (newEncoderValue != lastValue) {
        int dir = (newEncoderValue > lastValue) ? 1 : -1;
        lastValue = newEncoderValue;

        currentIndex = (currentIndex + dir + deletableElementFiles.size()) % deletableElementFiles.size();
        deleteElementSelection = currentIndex;
        // ❌ no dibujar aquí
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) buttonPressStart = millis();
    } else {
        if (buttonPressStart > 0 && millis() - buttonPressStart < 1000) {
            String selected = deletableElementFiles[deleteElementSelection];

            if (selected == getTranslation("VOLVER")) {
                deleteElementMenuActive = false;
                formatSubMenuActive     = true;
                // El loop dibujará drawFormatMenu(...) en su bloque correspondiente
            } else {
                DEBUG__________printf("[❓] Confirmar eliminación de: %s\n", selected.c_str());
                confirmDeleteActive     = true;
                confirmSelection        = 0;
                confirmedFileToDelete   = selected;

                deleteElementMenuActive = false;
                confirmDeleteMenuActive = true;
                // ✅ NO dibujes aquí: el loop lo hará inmediatamente este mismo tick
            }
        }
        buttonPressStart = 0;
    }
}

bool confirmDeleteMenuActive = false;

void handleConfirmDelete() {
    static int32_t lastValue = encoder.getCount();
    int32_t newValue = encoder.getCount();
    if (newValue != lastValue) {
        int dir = (newValue > lastValue) ? 1 : -1;
        lastValue = newValue;

        confirmSelection = (confirmSelection + dir + 2) % 2;
        drawConfirmDelete(confirmedFileToDelete);
    }

    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
    } else {
        if (buttonPressStart > 0 && millis() - buttonPressStart < 1000) {
            if (confirmSelection == 0) {
                // ✅ Sí, eliminar
                String fullPath = "/element_" + confirmedFileToDelete + ".bin";
                if (SPIFFS.exists(fullPath)) {
                    SPIFFS.remove(fullPath);
                    DEBUG__________printf("[🗑] Eliminado: %s\n", fullPath.c_str());
                }
                formatSubMenuActive = false;
                confirmDeleteActive = false;
                confirmDeleteMenuActive = false;
                // Recargar elementos o reiniciar
                loadElementsFromSPIFFS();
                drawCurrentElement();  // Volver al menú principal
            } else {
                // ❌ Cancelar
                confirmDeleteActive = false;
                deleteElementMenuActive = true;
                confirmDeleteMenuActive = false;
                drawDeleteElementMenu(deleteElementSelection);
            }
        }
        buttonPressStart = 0;
    }
}

bool isInMainMenu() {
    return !inModesScreen &&
           !hiddenMenuActive &&
           !brightnessMenuActive &&
           !soundMenuActive &&
           !languageMenuActive &&
           !formatSubMenuActive &&
           !bankSelectionActive &&
           !deleteElementMenuActive &&
           !confirmDeleteMenuActive;
}

void printElementDetails() {
    String currentFile = elementFiles[currentIndex];
    String idStr;
    String serialStr;

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        idStr     = "N/A";
        serialStr = "N/A";
    }
    else if (currentFile == "Comunicador") {
        // Formatear ID (2 dígitos hexadecimales)
        char idBuf[3];
        sprintf(idBuf, "%02X", comunicadorOption.ID);
        idStr = idBuf;

        // Formatear Serial sin espacios
        serialStr = "";
        for (int i = 0; i < 5; i++) {
            char buf[3];
            sprintf(buf, "%02X", comunicadorOption.serialNum[i]);
            serialStr += buf;
        }
    }
    else if (currentFile == "Apagar") {
        idStr     = "N/A";
        serialStr = "N/A";
    }
    else {
        // Leer desde SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r");
        if (!f) {
            DEBUG__________ln("❌ No se pudo abrir el archivo para mostrar detalles.");
            return;
        }
        byte id;
        byte serial[5];
        f.seek(OFFSET_ID);
        f.read(&id, 1);
        f.seek(OFFSET_SERIAL);
        f.read(serial, 5);
        f.close();

        // ID
        char idBuf[3];
        sprintf(idBuf, "%02X", id);
        idStr = idBuf;

        // Serial sin espacios
        serialStr = "";
        for (int i = 0; i < 5; i++) {
            char buf[3];
            sprintf(buf, "%02X", serial[i]);
            serialStr += buf;
        }
    }

    // Por seguridad, pasar a mayúsculas (por si sprintf usa minúsculas)
    idStr.toUpperCase();
    serialStr.toUpperCase();

    // Debug opcional
    DEBUG__________ln("🔎 Detalles del elemento:");
    DEBUG__________("ID: ");     DEBUG__________ln(idStr);
    DEBUG__________("Serial: "); DEBUG__________ln(serialStr);

    // Mostrar en pantalla
    showElemInfo(10000, serialStr, idStr);
}

int getTotalModesForFile(const String &file) {
    if (file == "Ambientes" || file == "Fichas") {
        INFO_PACK_T* opt = (file == "Ambientes") ? &ambientesOption : &fichasOption;
        // Cuenta cuántos modos “visibles” hay en amb/fichas
        int count = 0;
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)opt->mode[i].name) > 0 &&
                checkMostSignificantBit(opt->mode[i].config)) {
                count++;
            }
        }
        return count;
    }
    if (file == "Apagar") {
        return 0;
    }
    // Para archivos SPIFFS:
    fs::File f = SPIFFS.open(file, "r");
    if (!f) return 0;
    int count = 0;
    char modeName[25];
    byte cfg[2];
    for (int i = 0; i < 16; i++) {
        f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
        f.read((uint8_t*)modeName, 24);
        modeName[24] = 0;
        f.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
        f.read(cfg, 2);
        if (strlen(modeName) > 0 && checkMostSignificantBit(cfg)) {
            count++;
        }
    }
    f.close();
    return count;
}


