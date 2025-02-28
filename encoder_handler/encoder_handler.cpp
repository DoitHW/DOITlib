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
#include <info_elements_DMS/info_elements_DMS.h>

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

void handleEncoder() {
    // Si se debe ignorar el click residual, chequea si ya se soltó el botón.
    if (ignoreEncoderClick) {
        if (digitalRead(ENC_BUTTON) == HIGH) {
            ignoreEncoderClick = false;
        } else {
            return;
        }
    }
    
    if (ignoreInputs) return;
    int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue = newEncoderValue;

        // Navegar por elementos (cuando no se está en el menú de modos)
        if (!inModesScreen && elementFiles.size() > 1) {
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();
            String currentFile = elementFiles[currentIndex];

            // Si el elemento cambió, cargar su vector de estados alternativos desde el mapa
            static String lastElementFile = "";
            if (currentFile != lastElementFile) {
                if (elementAlternateStates.find(currentFile) != elementAlternateStates.end()) {
                    currentAlternateStates = elementAlternateStates[currentFile];
                } else {
                    currentAlternateStates.clear();
                }
                lastElementFile = currentFile;
            }
            
            int realModeIndex = 0;
            byte modeConfig[2] = {0};

            if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                realModeIndex = option->currentMode;
                memcpy(modeConfig, option->mode[realModeIndex].config, 2);
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

            adxl = getModeFlag(modeConfig, HAS_SENS_VAL_1);
            useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

#ifdef DEBUG
            Serial.println("🚀 Cambió de elemento, actualizando flags:");
            Serial.println("adxl status: " + String(adxl ? "true" : "false"));
            Serial.println("useMic status: " + String(useMic ? "true" : "false"));
#endif

            drawCurrentElement();  
        }
        // Navegar por modos cuando estamos en el menú de modos
        else if (inModesScreen && totalModes > 0) {
            currentModeIndex = (currentModeIndex + direction + totalModes) % totalModes;
            int realModeIndex = globalVisibleModesMap[currentModeIndex];
            if (realModeIndex >= 0) {
                String currentFile = elementFiles[currentIndex];
                colorHandler.setCurrentFile(currentFile);
                colorHandler.setPatternBotonera(realModeIndex, ledManager);
            }
            drawModesScreen();
        }
    }

    // Lectura del botón del encoder
    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        }
        // Caso 1: Desde la pantalla principal: Long press para entrar al menú de modos
        else if (!inModesScreen && millis() - buttonPressStart >= 2000 && !isLongPress) {
            Serial.println("DEBUG: Long press detectado en pantalla principal - entrando en menú de modos");
            isLongPress = true;
            modeScreenEnteredByLongPress = true;
            String currentFile = elementFiles[currentIndex];
            if (currentFile == "Apagar") {
                Serial.println("DEBUG: Long press detectado sobre 'Apagar' - se ignora");
                return;
            }
            int realModeIndex = 0;
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                realModeIndex = option->currentMode;
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    f.seek(OFFSET_CURRENTMODE, SeekSet);
                    realModeIndex = f.read();
                    f.close();
                }
            }
            // Determinar el índice visible en la lista de modos
            int tempCurrentModeIndex = 0;
            int foundVisibleIndex = -1;
            if (currentFile == "Ambientes" || currentFile == "Fichas") {
                INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
                for (int i = 0; i < 16; i++) {
                    if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                        if (i == realModeIndex) {
                            foundVisibleIndex = tempCurrentModeIndex;
                            break;
                        }
                        tempCurrentModeIndex++;
                    }
                }
            } else {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    for (int i = 0; i < 16; i++) {
                        char modeName[25] = {0};
                        char modeDesc[193] = {0};
                        byte modeConfig[2] = {0};
                        if (f.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet)) {
                            f.read((uint8_t*)modeName, 24);
                            f.read((uint8_t*)modeDesc, 192);
                            f.read(modeConfig, 2);
                            if (strlen(modeName) > 0 && checkMostSignificantBit(modeConfig)) {
                                if (i == realModeIndex) {
                                    foundVisibleIndex = tempCurrentModeIndex;
                                    break;
                                }
                                tempCurrentModeIndex++;
                            }
                        }
                    }
                    f.close();
                }
            }
            if (foundVisibleIndex >= 0) {
                currentModeIndex = foundVisibleIndex;
            } else {
                currentModeIndex = 0;
            }
            inModesScreen = true;
            drawModesScreen();
        }
        // Caso 2: Dentro del menú de modos: Long press para togglear estado alternativo
        else if (inModesScreen && millis() - buttonPressStart >= 2000 && !isLongPress) {
            Serial.println("DEBUG: Long press detectado en menú de modos - procesando toggle");
            isLongPress = true;
            String currentFile = elementFiles[currentIndex];
            // No se envía ningún comando en este bloque; solo se actualiza el estado y se guarda.
            if (currentAlternateStates.size() > (size_t)currentModeIndex) {
                currentAlternateStates[currentModeIndex] = !currentAlternateStates[currentModeIndex];
                // Actualizar el mapa para el elemento actual
                elementAlternateStates[currentFile] = currentAlternateStates;
                // Guardar el estado alternativo en SPIFFS para elementos que no son fijos
                if (currentFile != "Ambientes" && currentFile != "Fichas" && currentFile != "Apagar") {
                    fs::File f = SPIFFS.open(currentFile, "r+");
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
                if (currentAlternateStates[currentModeIndex]) {
                    Serial.println("DEBUG: Modo alternativo activado (solo toggle, comando diferido)");
                } else {
                    Serial.println("DEBUG: Modo alternativo desactivado (solo toggle, comando diferido)");
                }
            }
            drawModesScreen();
        }
    }
    // Cuando se suelta el botón
    else {
        if (!hiddenMenuActive && buttonPressStart > 0) {
            unsigned long pressDuration = millis() - buttonPressStart;
#ifdef DEBUG
            Serial.println("DEBUG: Botón soltado, duración: " + String(pressDuration) + " ms");
#endif
            // Solo procesar pulsaciones cortas (<2000 ms) para confirmar selección
            if (pressDuration < 2000) {
                String currentFile = elementFiles[currentIndex];
                if (currentFile == "Apagar") {
                    // Reiniciar la selección de todos los elementos
                    for (size_t i = 0; i < selectedStates.size(); i++) {
                        selectedStates[i] = false;
                    }
                    // Reinicializar los modos alternativos para TODOS los elementos
                    for (auto &entry : elementAlternateStates) {
                        entry.second = initializeAlternateStates(entry.first);
                    }
                    std::vector<byte> elementID;
                    elementID.push_back(0xFF);
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));
                    setAllElementsToBasicMode();

                    showMessageWithLoading("Apagando Sala...", 5000);

                    currentIndex = 0;
                    inModesScreen = false;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress = false;
                    return;
                }
                if (inModesScreen) {
                    // Al confirmar (pulsación corta) se envían los comandos a través de handleModeSelection,
                    // que gestionará el envío del comando de alternancia según el estado almacenado.
                    handleModeSelection(elementFiles[currentIndex]);
                } else {
                    toggleElementSelection(elementFiles[currentIndex]);
                }
            }
        }
        buttonPressStart = 0;
        isLongPress = false;
    }
}

void requestAndSyncElementMode() {
    String currentFile = elementFiles[currentIndex];

    // Obtener la ID del elemento actual
    byte elementID = BROADCAST;  // Valor predeterminado
    if (currentFile == "Ambientes" || currentFile == "Fichas" || currentFile == "Apagar") {
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        elementID = option->ID;  // ID para elementos fijos
    } else {
        // Leer la ID desde SPIFFS
       
        fs::File f = SPIFFS.open(currentFile, "r");
        if (f) {
            f.seek(OFFSET_ID, SeekSet);
            f.read(&elementID, 1);
            f.close();
        }
            // Enviar trama de petición de modo
    //send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, elementID, SPANISH_LANG, ELEM_CMODE_SECTOR));

    // Esperar respuesta del sector ELEM_CMODE_SECTOR
    // if (!element->esperar_respuesta(100)) {
    //     Serial.printf("No llegó respuesta del sector ELEM_CMODE_SECTOR para el elemento con ID %d\n", elementID);
    //     return;
    // }
    }



    // Si llega la respuesta, el modo será procesado en RX_main_handler
                                                                                                    #ifdef DEBUG
                                                                                                    Serial.printf("Respuesta del sector ELEM_CMODE_SECTOR recibida para el elemento con ID %d\n", elementID);    
                                                                                                    #endif
    
}

bool modeAlternateActive = false;
// Función handleModeSelection modificada
void handleModeSelection(const String& currentFile) {
    // Si se selecciona la opción de regresar (icono de flecha), salimos inmediatamente
    if (globalVisibleModesMap[currentModeIndex] == -2) {
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
  
    // Variables necesarias para procesar el modo seleccionado
    std::vector<byte> elementID;
    char elementName[25] = {0};
    String modeName;
    uint8_t modeConfig[2] = {0};

    // Obtener el índice real del modo a partir del índice visible
    int realModeIndex = globalVisibleModesMap[currentModeIndex];
    int modeNumber = currentModeIndex + 1; // Número del modo en la lista visible

    // Calcular el índice visible para el modo actual (necesario para saber qué posición en currentAlternateStates usar)
    int visibleModeIndex = -1;
    int count = 0;

    if (currentFile == "Ambientes" || currentFile == "Fichas") {
        // Procesar elementos fijos
        INFO_PACK_T* option = (currentFile == "Ambientes") ? &ambientesOption : &fichasOption;
        option->currentMode = realModeIndex;
        // Se asume que ya está el nombre completo (ej. "BASICO/BASICO LENTO")
        modeName = String((char*)option->mode[realModeIndex].name);
        memcpy(modeConfig, option->mode[realModeIndex].config, 2);
        elementID.push_back(0); // ID predeterminada

        // Calcular el índice visible para el modo actual
        for (int i = 0; i < 16; i++) {
            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                if (i == option->currentMode) {
                    visibleModeIndex = count;
                    break;
                }
                count++;
            }
        }
        Serial.println("DEBUG: [handleModeSelection] visibleModeIndex (fijos) = " + String(visibleModeIndex));
    } else {
        // Procesar elementos de SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write((uint8_t*)&realModeIndex, 1);

            f.seek(OFFSET_NAME, SeekSet);
            f.read((uint8_t*)elementName, 24);

            byte id = 0;
            f.seek(OFFSET_ID, SeekSet);
            f.read(&id, 1);
            elementID.push_back(id);

            int modoOffset = OFFSET_MODES + (SIZE_MODE * realModeIndex);
            f.seek(modoOffset, SeekSet);
            char modeNameBuf[25] = {0};
            f.read((uint8_t*)modeNameBuf, 24);
            modeName = String(modeNameBuf);

            f.seek(modoOffset + 24 + 192, SeekSet);
            f.read(modeConfig, 2);
            f.close();
        }
        
        // Calcular el índice visible para el modo actual en SPIFFS
        fs::File f2 = SPIFFS.open(currentFile, "r");
        if (f2) {
            for (int i = 0; i < 16; i++) {
                char modeBuf[25] = {0};
                byte tempConfig[2] = {0};
                f2.seek(OFFSET_MODES + i * SIZE_MODE, SeekSet);
                f2.read((uint8_t*)modeBuf, 24);
                f2.seek(OFFSET_MODES + i * SIZE_MODE + 216, SeekSet);
                f2.read(tempConfig, 2);
                if (strlen(modeBuf) > 0 && checkMostSignificantBit(tempConfig)) {
                    if (i == realModeIndex) {
                        visibleModeIndex = count;
                        break;
                    }
                    count++;
                }
            }
            f2.close();
        }
        Serial.println("DEBUG: [handleModeSelection] visibleModeIndex (SPIFFS) = " + String(visibleModeIndex));
    }

    // Extraer los flags usando getModeFlag()
    adxl = getModeFlag(modeConfig, HAS_SENS_VAL_1);
    useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

#ifdef DEBUG
    Serial.println("DEBUG: Mode Name (procesado): " + modeName);  
    Serial.println("DEBUG: adxl status: " + String(adxl ? "true" : "false"));
    Serial.println("DEBUG: useMic status: " + String(useMic ? "true" : "false"));    
#endif

    // Lógica de alternancia para long press:
    // En long press, solo se cambia el estado alternativo (y se guarda en SPIFFS) sin enviar comandos.
    if (getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE)) {
        if (isLongPress) {
            if (visibleModeIndex >= 0 && currentAlternateStates.size() > (size_t)visibleModeIndex) {
                currentAlternateStates[visibleModeIndex] = !currentAlternateStates[visibleModeIndex];
                
                // Guardar el estado alternativo en SPIFFS para elementos que no son fijos
                if (currentFile != "Ambientes" && currentFile != "Fichas" && currentFile != "Apagar") {
                    fs::File f = SPIFFS.open(currentFile, "r+");
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
#ifdef DEBUG
                Serial.println("DEBUG: Estado alternativo cambiado a: " + String(currentAlternateStates[visibleModeIndex] ? "ACTIVO" : "INACTIVO"));
#endif
            }
            isLongPress = false;
            drawModesScreen();
            return;
        }
    }

    // Confirmación normal (pulsación corta)
    // Se guarda si el elemento ya estaba seleccionado.
    bool wasAlreadySelected = selectedStates[currentIndex];
    if (!wasAlreadySelected) {
        selectedStates[currentIndex] = true;
#ifdef DEBUG
        Serial.println("DEBUG: Elemento seleccionado automáticamente.");
#endif
    }

    // En este punto, se envía el comando de alternancia (según el estado) siempre,
    // y se envían los demás comandos de confirmación.
    if (visibleModeIndex >= 0 && currentAlternateStates.size() > (size_t)visibleModeIndex) {
        if (getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE)) {
            if (currentAlternateStates[visibleModeIndex]) {
                modeAlternateActive = true;
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, ALTERNATE_MODE_ON));
                delay(300);
#ifdef DEBUG
                Serial.println("DEBUG: [Confirmación] Se envió ALTERNATE_MODE_ON");
#endif
            } else {
                modeAlternateActive = false;
                send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, ALTERNATE_MODE_OFF));
                delay(300);
#ifdef DEBUG
                Serial.println("DEBUG: [Confirmación] Se envió ALTERNATE_MODE_OFF");
#endif
            }
        }
    }

    // Solo enviar START_CMD si el elemento no estaba ya seleccionado
    if (!wasAlreadySelected) {
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, START_CMD));
        delay(300);
    }
    send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, elementID, realModeIndex));

    inModesScreen = false;
    drawCurrentElement();
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
            Serial.println("Error al leer la ID del archivo.");
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
        showMessageWithLoading("Apagando Sala...", 5000);
        currentIndex = 0;
        inModesScreen = false;
        drawCurrentElement();
        return;
    }
    
    // Para elementos de SPIFFS, se envía el comando de selección según el estado
    if (isElementFromSPIFFS) {
        byte command = selectedStates[currentIndex] ? START_CMD : BLACKOUT;
        Serial.printf("Enviando comando %s a la ID %d\n",
                      command == START_CMD ? "START_CMD" : "BLACKOUT",
                      elementID[0]);
        send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, command));

        if (command == BLACKOUT) {
            showMessageWithLoading("Apagando Elemento", 3000);
        }

        // Actualizar el modo del elemento a "básico" en SPIFFS
        fs::File f = SPIFFS.open(currentFile, "r+");
        if (f) {
            byte basicMode = DEFAULT_BASIC_MODE;  // modo 1
            f.seek(OFFSET_CURRENTMODE, SeekSet);
            f.write(&basicMode, 1);
            f.close();
            Serial.printf("Modo actualizado a básico (1) en SPIFFS para el elemento %s\n", currentFile.c_str());
        } else {
            Serial.println("Error al abrir el archivo para actualizar el modo.");
        }
    }
    
    // Redibujar el elemento actual para reflejar la selección/deselección
    drawCurrentElement();
}

void handleHiddenMenuNavigation(int &hiddenMenuSelection) {
    int32_t newEncoderValue = encoder.getCount();
    static bool encoderButtonPressed = false;
    static bool initialEntry = true;
    static bool menuJustOpened = true;  // Nueva variable para controlar la confirmación automática

    // Al entrar al menú oculto por primera vez, resalta la primera opción sin confirmarla
    if (initialEntry) {
        hiddenMenuSelection = 0;  // Preselección visual sin confirmar
        drawHiddenMenu(hiddenMenuSelection);
        initialEntry = false;
        menuJustOpened = true;  // Bloquea la confirmación inmediata
    }

    // Navegación por el menú con el encoder
    if (newEncoderValue != lastEncoderValue) {
        hiddenMenuSelection += (newEncoderValue > lastEncoderValue) ? 1 : -1;
        hiddenMenuSelection = constrain(hiddenMenuSelection, 0, 5); // Ahora hay 6 opciones (índices 0-5)
        lastEncoderValue = newEncoderValue;
        drawHiddenMenu(hiddenMenuSelection);
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
        case 0: {// Añadir elemento
            element->validar_elemento();
            hiddenMenuActive = false;
            break;
        }
        case 1: // Cambiar idioma
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Cambiando idioma...");
                                                                                            #endif
            // Lógica para cambiar idioma
            formatSPIFFS();
            loadElementsFromSPIFFS();
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 2: // Sonido
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Ajustando sonido...");
                                                                                            #endif
            // Lógica para ajustar sonido
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 3: // Brillo
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Ajustando brillo...");
                                                                                            #endif
            // Lógica para ajustar brillo
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 4: // Respuestas
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Configurando respuestas...");
                                                                                            #endif
            // Lógica para configurar respuestas
            drawCurrentElement();
            hiddenMenuActive = false;
            break;
        case 5: // Volver
                                                                                            #ifdef DEBUG
                                                                                            Serial.println("Volviendo al menú principal");
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

}


bool getModeFlag(const uint8_t modeConfig[2], MODE_CONFIGS flag) {
    // Construir el uint16_t interpretando data[0] como el MSByte y data[1] como el LSByte (big-endian)
    uint16_t config = (uint16_t(modeConfig[0]) << 8) | uint16_t(modeConfig[1]);
    // Extraer el bit (el enum indica el offset desde el bit 0, LSB)
    return (config >> flag) & 1;
}



void debugModeConfig(const uint8_t modeConfig[2]) {
    Serial.println("===== Estado de modeConfig =====");
    for (int i = HAS_BASIC_COLOR; i <= MODE_EXIST; i++) {
        MODE_CONFIGS flag = static_cast<MODE_CONFIGS>(i);
        bool isActive = getModeFlag(modeConfig, flag);
        switch (flag) {
            case HAS_BASIC_COLOR:   Serial.print("HAS_BASIC_COLOR"); break;
            case HAS_PULSE:         Serial.print("HAS_PULSE"); break;
            case HAS_ADVANCED_COLOR:Serial.print("HAS_ADVANCED_COLOR"); break;
            case HAS_RELAY_1:       Serial.print("HAS_RELAY_1"); break;
            case HAS_RELAY_2:       Serial.print("HAS_RELAY_2"); break;
            case HAS_RELAY_3:       Serial.print("HAS_RELAY_3"); break;
            case HAS_RELAY_4:       Serial.print("HAS_RELAY_4"); break;
            case HAS_SENS_VAL_1:    Serial.print("HAS_SENS_VAL_1"); break;
            case HAS_SENS_VAL_2:    Serial.print("HAS_SENS_VAL_2"); break;
            case SITUATED_HIGH:     Serial.print("SITUATED_HIGH"); break;
            case HAS_PASSIVE:       Serial.print("HAS_PASSIVE"); break;
            case HAS_BINARY_SENSORS:Serial.print("HAS_BINARY_SENSORS"); break;
            case HAS_BANK_FILE:     Serial.print("HAS_BANK_FILE"); break;
            case HAS_PATTERNS:      Serial.print("HAS_PATTERNS"); break;
            case HAS_ALTERNATIVE_MODE:Serial.print("HAS_ALTERNATIVE_MODE"); break;
            case MODE_EXIST:        Serial.print("MODE_EXIST"); break;
        }
        Serial.print(" = ");
        Serial.println(isActive ? "1" : "0");
    }
}



