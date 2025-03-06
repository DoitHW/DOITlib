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

unsigned long lastDisplayInteraction = 0; // Última vez que se interactuó con la pantalla
bool displayOn = true;                    // Estado de la pantalla (encendida por defecto)
unsigned long encoderIgnoreUntil = 0; // Tiempo hasta el cual se ignoran las entradas del encoder


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
    
    // Si la pantalla está apagada, SOLO reactivar si hay una acción real (movimiento o pulsación)
    if (!displayOn) {
        if ((encoder.getCount() != lastEncoderValue) || (digitalRead(ENC_BUTTON) == LOW)) {
            display_wakeup();
            encoderIgnoreUntil = millis() + 500; // Ignorar entradas durante 500 ms
            lastDisplayInteraction = millis();
            // **Reiniciamos lastEncoderValue para que el giro realizado mientras la pantalla estaba apagada no se procese**
            lastEncoderValue = encoder.getCount();
        }
        return; // No procesamos ningún otro evento.
    }
    
    // Si estamos en el menú principal (drawCurrentElement) y el sistema está bloqueado,
    // se ignoran rotaciones y clicks simples; solo se permite la pulsación larga para desbloquear.
    if (!inModesScreen && systemLocked) {
        if (digitalRead(ENC_BUTTON) == LOW) {
            if (buttonPressStart == 0) {
                buttonPressStart = millis();
            } else if ((millis() - buttonPressStart >= 3000) && !isLongPress) {
                // Desbloquear el sistema
                systemLocked = false;
                isLongPress = true;
                lastEncoderValue = encoder.getCount();
                drawCurrentElement(); // Actualiza el display para quitar el icono de candado
            }
        } else { // Al soltar el botón
            buttonPressStart = 0;
            isLongPress = false;
        }
        return;
    }
    
    if (millis() < encoderIgnoreUntil) {
        lastDisplayInteraction = millis();
        return;
    }
    
    int32_t newEncoderValue = encoder.getCount();
    if (newEncoderValue != lastEncoderValue) {
        lastDisplayInteraction = millis();
        int32_t direction = (newEncoderValue > lastEncoderValue) ? 1 : -1;
        lastEncoderValue = newEncoderValue;
        
        // Navegar por elementos cuando NO estamos en el menú de modos.
        if (!inModesScreen && elementFiles.size() > 1) {
            currentIndex = (currentIndex + direction + elementFiles.size()) % elementFiles.size();
            String currentFile = elementFiles[currentIndex];
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
        // Navegar por modos cuando estamos en el menú de modos.
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
    
    // Lectura del botón del encoder.
    if (digitalRead(ENC_BUTTON) == LOW) {
        if (buttonPressStart == 0) {
            buttonPressStart = millis();
        } else {
            // En menú de modos, pulsación larga de 2s para alternar el nombre alternativo.
            if (inModesScreen && !isLongPress && (millis() - buttonPressStart >= 2000)) {
                if (currentModeIndex != 0 && currentModeIndex != totalModes - 1) {
                    int adjustedIndex = currentModeIndex - 1;
                    String currFile = elementFiles[currentIndex];
                    uint8_t modeConfig[2] = {0};
                    bool canToggle = false;
                    if (currFile == "Ambientes" || currFile == "Fichas") {
                        INFO_PACK_T* option = (currFile == "Ambientes") ? &ambientesOption : &fichasOption;
                        int count = 0;
                        for (int i = 0; i < 16; i++) {
                            if (strlen((char*)option->mode[i].name) > 0 && checkMostSignificantBit(option->mode[i].config)) {
                                if (count == adjustedIndex) {
                                    memcpy(modeConfig, option->mode[i].config, 2);
                                    break;
                                }
                                count++;
                            }
                        }
                        canToggle = getModeFlag(modeConfig, HAS_ALTERNATIVE_MODE);
                    } else if (currFile != "Apagar") {
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
                                if (strlen(modeName) > 0 && checkMostSignificantBit(tempConfig)) {
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
                    if (canToggle && adjustedIndex >= 0 && currentAlternateStates.size() > (size_t)adjustedIndex) {
                        currentAlternateStates[adjustedIndex] = !currentAlternateStates[adjustedIndex];
                        elementAlternateStates[currFile] = currentAlternateStates;
                        if (currFile != "Ambientes" && currFile != "Fichas" && currFile != "Apagar") {
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
            // En menú principal, si el sistema está desbloqueado, pulsación larga de 3s para bloquear.
            if (!inModesScreen && !systemLocked && (millis() - buttonPressStart >= 3000) && !isLongPress) {
                systemLocked = true;
                isLongPress = true;
                drawCurrentElement(); // Actualiza el display para mostrar el candado
            }
        }
    }
    else { // Al soltar el botón.
        if (buttonPressStart > 0) {
            unsigned long pressDuration = millis() - buttonPressStart;
#ifdef DEBUG
            Serial.println("DEBUG: Botón soltado, duración: " + String(pressDuration) + " ms");
#endif
            String currentFile = elementFiles[currentIndex];
            if (!inModesScreen) {
                // Si se presionó "Apagar"
                if (currentFile == "Apagar") {
                    for (size_t i = 0; i < selectedStates.size(); i++) {
                        selectedStates[i] = false;
                    }
                    std::vector<byte> elementID;
                    elementID.push_back(0xFF);
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, elementID, BLACKOUT));
                    setAllElementsToBasicMode();
                    showMessageWithLoading("Apagando Sala...", 5000);
                    currentIndex = 0;
                    drawCurrentElement();
                    buttonPressStart = 0;
                    isLongPress = false;
                    return;
                } else {
                    // Solo cambiar a pantalla de modos si la pulsación fue corta (<3000ms)
                    if (pressDuration < 3000) {
                        inModesScreen = true;
                        currentModeIndex = 0;
                        drawModesScreen();
                    }
                    // Si la pulsación fue larga (>=3000ms), ya se gestionó el bloqueo
                }
            }
            else {
                if (!isLongPress && pressDuration < 2000) {
                    handleModeSelection(elementFiles[currentIndex]);
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
        if (currentFile != "Ambientes" && currentFile != "Fichas" && currentFile != "Apagar") {
            fs::File f = SPIFFS.open(currentFile, "r+");
            if (f) {
                if (selectedStates[currentIndex]) {
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, START_CMD));
                } else {
                    send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, BLACKOUT));
                    fs::File f2 = SPIFFS.open(currentFile, "r+");
                    if (f2) {
                        byte basicMode = DEFAULT_BASIC_MODE;
                        f2.seek(OFFSET_CURRENTMODE, SeekSet);
                        f2.write(&basicMode, 1);
                        f2.close();
                    }
                    setAllElementsToBasicMode();
                    showMessageWithLoading("Apagando elemento...", 3000);
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
    Serial.println("🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻");
    Serial.println("DEBUG: Mode Name (procesado): " + modeName);
    Serial.println("DEBUG: adxl status: " + String(adxl ? "true" : "false"));
    Serial.println("DEBUG: useMic status: " + String(useMic ? "true" : "false"));
    Serial.println("🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻🌻");
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
    } else if (currentFile != "Apagar") {
        byte modeConfigTemp[2] = {0};
        memcpy(modeConfigTemp, modeConfig, 2);  // Usar la configuración que ya tenemos
        
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
        if (!wasAlreadySelected) {
            send_frame(frameMaker_SEND_COMMAND(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, START_CMD));
            delay(300);
        }
        send_frame(frameMaker_SET_ELEM_MODE(DEFAULT_BOTONERA, std::vector<byte>{getCurrentElementID()}, realModeIndex));
    }
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



