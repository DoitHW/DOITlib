#include <botonera_DMS/botonera_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <defines_DMS/defines_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <vector>
#include <EEPROM.h>
#include <encoder_handler/encoder_handler.h>
#include <display_handler/display_handler.h>
#include <RelayManager_DMS/RelayStateManager.h>
#include <Translations_handler/translations.h>
#include <display_handler/display_handler.h>

#define MIN_VALID_ELEMENT_SIZE (OFFSET_ID + 1) // Se espera que el archivo tenga al menos OFFSET_ID+1 bytes


extern std::vector<uint8_t> printTargetID;
byte currentCognitiveCommand = COG_ACT_OFF;

BOTONERA_::BOTONERA_() : ELEMENT_() {
            set_type(TYPE_BOTONERA);
        }

void BOTONERA_::botonera_begin(){}

void BOTONERA_::printFrameInfo(LAST_ENTRY_FRAME_T LEF) {
    DEBUG__________ln("\n==== 📨 Trama Recibida 📨 ====");

    // Determinar origen
    String origenStr;
    if (LEF.origin == 0xDB) origenStr = "BOTONERA";
    else if (LEF.origin == 0xDC) origenStr = "CONSOLA";
    else if (LEF.origin == 0xFF) origenStr = "BROADCAST";
    else if (LEF.origin == 0xDF) origenStr = "HACKING BOX";
    else origenStr = (String)LEF.origin;

    DEBUG__________printf("📌 Origen:  %s (0x%02X)\n", origenStr.c_str(), LEF.origin);

    DEBUG__________("🎯 Targets: ");
    if (printTargetID.empty()) {
        DEBUG__________ln("Ninguno (posible broadcast)");
    } else {
        for (size_t i = 0; i < printTargetID.size(); i++) {
            DEBUG__________printf("0x%02X ", printTargetID[i]);
        }
        DEBUG__________ln();
    }

    // Determinar función
    String functionStr;
    switch (LEF.function) {
        case 0xA0: functionStr = "F_REQ_ELEM_SECTOR"; break;
        case 0xA1: functionStr = "F_REQ_ELEM_INFO"; break;
        case 0xA2: functionStr = "F_REQ_ELEM_STATE"; break;
        case 0xA3: functionStr = "F_REQ_ELEM_ICON"; break;
        case 0xB1: functionStr = "F_SET_ELEM_ID"; break;
        case 0xB2: functionStr = "F_SET_ELEM_MODE"; break;
        case 0xB3: functionStr = "F_SET_ELEM_DEAF"; break;
        case 0xC1: functionStr = "F_SEND_COLOR"; break;
        case 0xC2: functionStr = "F_SEND_RGB"; break;
        case 0xC3: functionStr = "F_SEND_BRIGHTNESS"; break;
        case 0xCA: functionStr = "F_SEND_SENSOR_VALUE"; break;
        case 0xCB: functionStr = "F_SEND_SENSOR_VALUE_2"; break;
        case 0xCC: functionStr = "F_SEND_FILE_NUM"; break;
        case 0xCD: functionStr = "F_SEND_PATTERN_NUM"; break;
        case 0xCE: functionStr = "F_SEND_FLAG_BYTE"; break;
        case 0xCF: functionStr = "F_SEND_COMMAND"; break;
        case 0xD0: functionStr = "F_RETURN_ELEM_SECTOR"; break;
        case 0xD1: functionStr = "F_RETURN_ELEM_INFO"; break;
        case 0xD2: functionStr = "F_RETURN_ELEM_STATE"; break;
        case 0xD3: functionStr = "F_RETURN_ELEM_ICON"; break;
        case 0xDE: functionStr = "F_RETURN_ELEM_ERROR"; break;
        default: functionStr = "FUNCIÓN DESCONOCIDA";
    }

    DEBUG__________printf("🛠️  Función: %s (0x%02X)\n", functionStr.c_str(), LEF.function);

    // Interpretación de datos
    DEBUG__________("📦 Data:    ");
    if (LEF.data.empty()) {
        DEBUG__________ln("No hay datos para esta función.");
    } else {
        if (LEF.function == 0xCA) {
            if (LEF.data.size() >= 12) {
                int minX = (LEF.data[0]  << 8) | LEF.data[1];
                int maxX = (LEF.data[2]  << 8) | LEF.data[3];
                int valX = (LEF.data[4]  << 8) | LEF.data[5];
                int minY = (LEF.data[6]  << 8) | LEF.data[7];
                int maxY = (LEF.data[8]  << 8) | LEF.data[9];
                int valY = (LEF.data[10] << 8) | LEF.data[11];
                DEBUG__________printf(
                    "Eje X → MIN=%d, MAX=%d, VAL=%d;  Eje Y → MIN=%d, MAX=%d, VAL=%d\n",
                    minX, maxX, valX, minY, maxY, valY
                );
            } else {
                DEBUG__________ln("⚠️ Trama SENSOR CA incompleta (esperados 12 bytes).");
            }
        }
        // --- Sensor único (3 valores) sigue con función 0xCB ---
        else if (LEF.function == 0xCB) {
            if (LEF.data.size() >= 6) {
                int minVal    = (LEF.data[0] << 8) | LEF.data[1];
                int maxVal    = (LEF.data[2] << 8) | LEF.data[3];
                int sensedVal = (LEF.data[4] << 8) | LEF.data[5];
                DEBUG__________printf(
                    "MIN=%d, MAX=%d, VAL=%d\n",
                    minVal, maxVal, sensedVal
                );
            } else {
                DEBUG__________ln("⚠️ Trama SENSOR CB incompleta (esperados 6 bytes).");
            }
        }
        else if (LEF.function == 0xC1) { // Color recibido
            String colorName;
            switch (LEF.data[0]) {
                case 0: colorName = "BLANCO"; break;
                case 1: colorName = "AMARILLO"; break;
                case 2: colorName = "NARANJA"; break;
                case 3: colorName = "ROJO"; break;
                case 4: colorName = "VIOLETA"; break;
                case 5: colorName = "AZUL"; break;
                case 6: colorName = "CELESTE"; break;
                case 7: colorName = "VERDE"; break;
                case 8: colorName = "NEGRO"; break;
                default: colorName = "COLOR DESCONOCIDO";
            }
            DEBUG__________printf("%s (0x%02X)\n", colorName.c_str(), LEF.data[0]);
        } 
        else if (LEF.function == 0xCE) { // Cambio en el relé
            DEBUG__________ln("Cambio de estado en el relé.");
        } 
        else if (LEF.function == 0xA0) { // Petición de sector
            String idioma = (LEF.data[0] == 1) ? "ES" : "OTRO";
            DEBUG__________printf("Idioma: %s, Sector: %d\n", idioma.c_str(), LEF.data[1]);
        } 
        else if (LEF.function == 0xCC) { // Petición de archivo
            DEBUG__________printf("Carpeta (Bank): %d, Archivo (File): %d\n", LEF.data[0], LEF.data[1]);
        }
        else if (LEF.function == F_SEND_COMMAND) {
        if (!LEF.data.empty()) {
            uint8_t cmd = LEF.data[0];
            String commandStr;
            switch (cmd) {
                case BLACKOUT:                          commandStr = "BLACKOUT"; break;
                case START_CMD:                         commandStr = "START_CMD"; break;
                case TEST_CMD:                          commandStr = "TEST_CMD"; break;
                case SEND_REG_RF_CMD:                   commandStr = "SEND_REG_RF_CMD"; break;
                case SEND_STATS_RF_CMD:                 commandStr = "SEND_STATS_RF_CMD"; break;
                case ERR_DBG_ON:                        commandStr = "ERR_DBG_ON"; break;
                case ERR_DBG_OFF:                       commandStr = "ERR_DBG_OFF"; break;
                case SET_ELEM_DEAF:                     commandStr = "SET_ELEM_DEAF"; break;
                case SET_ELEM_LONG_DEAF:                commandStr = "SET_ELEM_LONG_DEAF"; break;
                case MAGIC_TEST_CMD:                    commandStr = "MAGIC_TEST_CMD"; break;
                case MAGIC_TEST_2_CMD:                  commandStr = "MAGIC_TEST_2_CMD"; break;
                case ALTERNATE_MODE_ON:                 commandStr = "ALTERNATE_MODE_ON"; break;
                case ALTERNATE_MODE_OFF:                commandStr = "ALTERNATE_MODE_OFF"; break;
                case OTA_AP_ON:                         commandStr = "OTA_AP_ON"; break;
                case OTA_AP_OFF:                        commandStr = "OTA_AP_OFF"; break;
                case COG_ACT_ON:                        commandStr = "COG_ACT_ON"; break;
                case COG_ACT_OFF:                       commandStr = "COG_ACT_OFF"; break;
                case SHOW_ID_CMD:                       commandStr = "SHOW_ID_CMD"; break;
                case WIN_CMD:                           commandStr = "WIN_CMD"; break;
                case FAIL_CMD:                          commandStr = "FAIL_CMD"; break;
                case MOST_USED_MODE_RF_REQ:             commandStr = "MOST_USED_MODE_RF_REQ"; break;
                case MOST_USED_COLOR_RF_REQ:            commandStr = "MOST_USED_COLOR_RF_REQ"; break;
                case MOST_USED_AMBIENT_RF_REQ:          commandStr = "MOST_USED_AMBIENT_RF_REQ"; break;
                case LIFETIME_TOTAL_RF_REQ:             commandStr = "LIFETIME_TOTAL_RF_REQ"; break;
                case WORKTIME_TOTAL_RF_REQ:             commandStr = "WORKTIME_TOTAL_RF_REQ"; break;
                case CURRENT_SESSION_TIME_RF_REQ:       commandStr = "CURRENT_SESSION_TIME_RF_REQ"; break;
                case CURRENT_SESSION_FILENAME_RF_REQ:   commandStr = "CURRENT_SESSION_FILENAME_RF_REQ"; break;
                case EVENTS_LOG_RF_REQ:                 commandStr = "EVENTS_LOG_RF_REQ"; break;
                case LAST_EVENT_LOG_RF_REQ:             commandStr = "LAST_EVENT_LOG_RF_REQ"; break;
                case LIST_SESSIONS_RF_REQ:              commandStr = "LIST_SESSIONS_RF_REQ"; break;
                case CLEAR_SESSIONS_CMD:                commandStr = "CLEAR_SESSIONS_CMD"; break;
                case RESET_ALL_STATS_CMD:               commandStr = "RESET_ALL_STATS_CMD"; break;
                case SEND_LAST_ORIGIN_CMD:              commandStr = "SEND_LAST_ORIGIN_CMD"; break;
                case SEND_SESSION_LOG_RF_CMD:           commandStr = "SEND_SESSION_LOG_RF_CMD"; break;
                case FORMAT_LITTLEFS_CMD:               commandStr = "FORMAT_LITTLEFS_CMD"; break;
                case AVERAGE_SESSION_TIME_RF_REQ:       commandStr = "AVERAGE_SESSION_TIME_RF_REQ"; break;
                case MOST_SELECTED_MODE_RF_REQ:         commandStr = "MOST_SELECTED_MODE_RF_REQ"; break;
                case MOST_SELECTED_COLOR_RF_REQ:        commandStr = "MOST_SELECTED_COLOR_RF_REQ"; break;
                case MOST_SELECTED_AMBIENT_RF_REQ:      commandStr = "MOST_SELECTED_AMBIENT_RF_REQ"; break;
                case USAGE_GRAPH_RF_REQ:                commandStr = "USAGE_GRAPH_RF_REQ"; break;
                case LITTLEFS_MEM_STATS:                commandStr = "LITTLEFS_MEM_STATS"; break;
                case INTER_SESSION_TIMES:               commandStr = "INTER_SESSION_TIMES"; break;
                default:                                commandStr = "COMANDO DESCONOCIDO";
            }
            DEBUG__________printf("Comando: %s (0x%02X)\n", commandStr.c_str(), cmd);
        } 
        else {
            DEBUG__________ln("⚠️ No hay datos para el comando.");
        }
        }
        else {
            // Imprimir todos los datos si no hay interpretación específica
            for (size_t i = 0; i < LEF.data.size(); i++) {
                DEBUG__________printf("0x%02X ", LEF.data[i]);
            }
            DEBUG__________ln();
        }
    }

    DEBUG__________ln("=============================");
}

/**
 * @brief Despacha y atiende un frame RX de control para la botonera.
 *
 * Procesa el campo de función de @p LEF y ejecuta la acción correspondiente:
 * sincronización de sector, actualización de estado de relé, reproducción de
 * sonidos y gestión de comandos (p. ej., modo cognitivo y respuestas WIN/FAIL).
 *
 * @param LEF Estructura de frame recibido. Se espera que contenga:
 * - `function`: código de operación (p. ej. F_RETURN_ELEM_SECTOR, F_SEND_COMMAND, ...).
 * - `data[]`: payload; se usa `data[0]` y, según el caso, `data[1]`.
 * - `origin`: identificador de origen, reenviado a `sectorIn_handler`.
 *
 * @return void
 *
 * @pre `element` debe ser un puntero válido e inicializado.
 * @pre El contexto debe ser de tarea (no ISR) si se usa `uxTaskGetStackHighWaterMark`.
 * @pre Cuando `function == F_SEND_FILE_NUM` se requiere `LEF.data[0]` (banco) y `LEF.data[1]` (fichero).
 * @pre Cuando `function == F_SEND_COMMAND` se requiere `LEF.data[0]` (código de comando).
 *
 * @note Las dependencias externas (`RelayStateManager`, `doitPlayer`, `activateCognitiveMode`,
 * `deactivateCognitiveMode`, `printFrameInfo`, `sectorIn_handler`, etc.) deben estar disponibles.
 *
 * @warning Este manejador asume que `LEF.data` ofrece al menos 1–2 bytes según la función.
 * Validar tamaño aguas arriba para evitar accesos fuera de rango.
 *
 * @see activateCognitiveMode, deactivateCognitiveMode, RelayStateManager::set, doitPlayer.play_file
 */
void BOTONERA_::RX_main_handler(LAST_ENTRY_FRAME_T LEF) {
    // Validación básica del objeto dependiente.
    if (!element) {
    #ifdef DEBUG
        DEBUG__________ln("Error: 'element' no está inicializado.");
    #endif
        return;
    }

    // Trazas del frame entrante (función, tamaños, etc.).
    printFrameInfo(LEF);

    // (Solo en depuración) Medimos agua alta de la pila para diagnóstico.
    #ifdef DEBUG
    {
        UBaseType_t stackSizeBegin = uxTaskGetStackHighWaterMark(NULL);
        (void)stackSizeBegin; // Evitar warning si no se imprime.
        // DEBUG__________ln("Stack restante: " + String(stackSizeBegin));
    }
    #endif

    // ----------------------------
    // Constantes de protocolo/datos
    // ----------------------------
    constexpr uint8_t kFlagsBit0Mask    = 0x01u; // Bit 0: estado del relé remoto.
    constexpr uint8_t kIdx0             = 0u;    // Índice común para LEF.data[0]
    constexpr uint8_t kIdx1             = 1u;    // Índice común para LEF.data[1]
    constexpr uint8_t kLangStride       = 10u;   // Separación de bancos por idioma.
    constexpr uint8_t kWinFailVariants  = 4u;    // Nº de variantes WIN/FAIL por idioma.

    // -------------------------------------------------------
    // Despacho por tipo de función recibida en el frame (RX)
    // -------------------------------------------------------
    switch (LEF.function) {

        case F_RETURN_ELEM_SECTOR: {
            DEBUG__________ln("Ha llegado un F_RETURN_ELEM_SECTOR");
            // Reenviamos el payload al elemento y marcamos fin de espera.
            element->sectorIn_handler(LEF.data, LEF.origin);
            awaitingResponse = false;
            break;
        }

        case F_SET_ELEM_MODE: {
            // Reservado para futura ampliación. Mantener por compatibilidad.
            break;
        }

        case F_SEND_FLAG_BYTE: {
            // ① ID que origina el cambio de relé (fuente).
            const uint8_t sourceID = printTargetID[kIdx0];

            // ② Estado del bit 0 procedente del payload.
            const bool flags_bit0 = (LEF.data[kIdx0] & kFlagsBit0Mask) != 0u;

            // ③ Sincronizamos inmediatamente el mapa de relés.
            RelayStateManager::set(sourceID, flags_bit0);

            DEBUG__________ln("Estado del relé actualizado para ID " + String(sourceID) +
                              ": " + String(flags_bit0 ? "ON" : "OFF"));
            break;
        }

        case F_SEND_COLOR: {
            // Reservado para futura gestión de color. Sin efectos por ahora.
            break;
        }

        case F_SEND_PATTERN_NUM: {
            // Bank fijo de “ambientes” y el file viene en LEF.data[0]
            if (LEF.data.empty()) {
                #ifdef DEBUG
                DEBUG__________ln("F_SEND_PATTERN_NUM sin datos (esperado 1 byte de patrón).");
                #endif
                break;
            }

            const uint8_t pattern = LEF.data[0];

            #ifdef DEBUG
            DEBUG__________printf("F_SEND_PATTERN_NUM → bank=0x%02X, file=%u\n", AMBIENTS_BANK, pattern);
            #endif

            doitPlayer.play_file(AMBIENTS_BANK, pattern);
            break;
        }

        case F_SEND_FILE_NUM: {
            DEBUG__________ln("Recibido un play sound");
            // LEF.data[0] = banco, LEF.data[1] = fichero.
            doitPlayer.play_file(LEF.data[kIdx0], LEF.data[kIdx1]);
            break;
        }

        case F_SEND_COMMAND: {
            const uint8_t receivedCommand = LEF.data[kIdx0];
            currentCognitiveCommand = receivedCommand;

            DEBUG__________ln("Comando recibido: " + String(receivedCommand, HEX));

            if (receivedCommand == COG_ACT_ON) {
                DEBUG__________ln("Activando modo cognitivo...");
                activateCognitiveMode();

            } else if (receivedCommand == COG_ACT_OFF) {
                DEBUG__________ln("Desactivando modo cognitivo...");
                deactivateCognitiveMode();

            } else if (receivedCommand == WIN_CMD) {
                // Selección pseudoaleatoria de respuesta WIN según idioma.
                const uint8_t res  = static_cast<uint8_t>(rand() % kWinFailVariants);
                const uint8_t lang = static_cast<uint8_t>(currentLanguage);
                const uint8_t file = static_cast<uint8_t>(lang * kLangStride + res + 1u);
                doitPlayer.play_file(WIN_RESP_BANK, file);

            } else if (receivedCommand == FAIL_CMD) {
                // Selección pseudoaleatoria de respuesta FAIL según idioma.
                const uint8_t res  = static_cast<uint8_t>(rand() % kWinFailVariants);
                const uint8_t lang = static_cast<uint8_t>(currentLanguage);
                const uint8_t file = static_cast<uint8_t>(lang * kLangStride + res + 1u);
                doitPlayer.play_file(FAIL_RESP_BANK, file);
            }else if (receivedCommand == OTA_AP_ON) {
                DEBUG__________ln("Comando OTA_AP_ON recibido → Activando AP + OTA...");
                element->activarAP_OTA();     // Llamada a la función OTA
            }
            else if (receivedCommand == OTA_AP_OFF) {
                DEBUG__________ln("Comando OTA_AP_OFF recibido → Desactivando AP + OTA...");
                element->desactivarAP_OTA();
            }

            break;
        }

        default: {
        #ifdef DEBUG
            DEBUG__________ln("Se ha recibido una función desconocida.");
        #endif
            break;
        }
    }

    // (Solo en depuración) Lectura final del agua alta de la pila.
    #ifdef DEBUG
    {
        UBaseType_t stackSizeEnd = uxTaskGetStackHighWaterMark(NULL);
        (void)stackSizeEnd;
        // DEBUG__________ln("Stack restante al final: " + String(stackSizeEnd));
    }
    #endif
}

extern bool adxl;
extern bool useMic;

static inline bool isSpiffsPath(const String& s) {
    return s.length() > 0 && s[0] == '/';
}

static inline bool isRamElementId(byte id) {
    // Ajusta si tienes más IDs RAM. 0x00 lo usas como “broadcast de sala”; no lo tratamos como elemento único.
    return (id == DEFAULT_DICE) || (id == BROADCAST) || (id == DEFAULT_BOTONERA) || (id == DEFAULT_CONSOLE); // Dado, (Comunicador si alguna vez envía CMODE)
}

static inline bool isRamElementName(const String& name) {
    return (name == "Ambientes" || name == "Fichas" || name == "Apagar" || name == "Comunicador" || name == "Dado");
}

/**
 * @brief Procesa datos de sector recibidos para un elemento (botonera) y sincroniza estado.
 *
 * Despacha por tipo de sector (nombre, descripción, modo actual, icono, flags, etc.),
 * actualizando almacenamiento SPIFFS, variables internas y visualización (redibujado)
 * cuando corresponde.
 *
 * @param data Trama recibida. Formato: data[0] = código de sector; según sector, se
 *             requiere data[1] (p.ej. modo/flags). Longitud mínima: 1 byte.
 * @param targetin Identificador del elemento remoto (ID origen del frame).
 *
 * @return void
 *
 * @pre SPIFFS debe estar montado y accesible. Los offsets (OFFSET_*) deben ser válidos
 *      para la estructura de fichero de los elementos.
 * @pre Debe invocarse en contexto de tarea (no ISR) si posteriormente se redibuja UI.
 *
 * @note En ELEM_CMODE_SECTOR: data[1] = modo actual; se persiste en OFFSET_CURRENTMODE,
 *       se actualizan flags de sensores y patrón de color del elemento actual.
 * @note En ELEM_CURRENT_FLAGS_SECTOR: data[1] usa el bit 0 como estado ON/OFF del relé.
 *
 * @warning Este manejador accede a data[1] en algunos sectores. Si la trama no tiene
 *          al menos 2 bytes, se ignora el sector con traza de depuración.
 *
 * @see drawCurrentElement, RelayStateManager::set, getModeFlag, colorHandler.setPatternBotonera
 */
void BOTONERA_::sectorIn_handler(std::vector<byte> data, byte targetin) {
    // ----------------------------
    // Validación inicial de trama
    // ----------------------------
    if (data.size() < 1) {
        DEBUG__________ln("⚠️ Error: sectorIn_handler ha recibido una trama vacía.");
        return;
    }

    // ----------------------------
    // Constantes locales
    // ----------------------------
    constexpr byte kIdx0 = 0u;
    constexpr byte kIdx1 = 1u;
    constexpr byte kBit0Mask = 0x01u;

    // Offset interno dentro del modo para la configuración (reemplaza '216' mágico)
    constexpr size_t kOffsetConfigInMode = 216u;
    constexpr size_t kModeConfigBytes    = 2u;

    const byte sector = data[kIdx0];

    switch (sector) {

        case ELEM_NAME_SECTOR: {
            // TODO: Copiar data a partir de data[1] a INFO_PACK_T (nombre).
            // Requiere contrato de longitud y estructura del paquete.
            break;
        }

        case ELEM_DESC_SECTOR: {
            // TODO: Copiar data a partir de data[1] a INFO_PACK_T (descripción).
            // Requiere contrato de longitud y estructura del paquete.
            break;
        }

        case ELEM_CMODE_SECTOR: {
        if (data.size() < 2) {
            DEBUG__________ln("⚠️ ELEM_CMODE_SECTOR con longitud insuficiente (<2).");
            break;
        }

        const byte receivedMode = data[kIdx1];
        DEBUG__________ln("📢 ID: " + String(targetin) + " con MODO " + String(receivedMode));

        // ─────────────────────────────────────────────────────────
        // 0) RAM elements (no tocar SPIFFS)
        // ─────────────────────────────────────────────────────────
        if (isRamElementId(targetin)) {
            // Por ahora sólo Dado (0xDA) necesita reflejo de modo en RAM
            if (targetin == DEFAULT_DICE) {
                dadoOption.currentMode = receivedMode;

                // Marcar seleccionado/des-seleccionado si existe en el listado
                for (size_t i = 0; i < elementFiles.size(); ++i) {
                    if (elementFiles[i] == "Dado") {
                        if (i < selectedStates.size()) {
                            selectedStates[i] = (receivedMode != 0);
                        }
                        // Si es el elemento visible, redibuja
                        if ((int)i == currentIndex) {
                            drawCurrentElement();
                        }
                        break;
                    }
                }
            }

            // Actualizar color/patrón si el elemento visible es el target (y es RAM)
            if ((size_t)currentIndex < elementFiles.size() && isRamElementName(elementFiles[currentIndex])) {
                colorHandler.setCurrentFile(elementFiles[currentIndex]);
                colorHandler.setPatternBotonera(receivedMode, ledManager);
                drawCurrentElement();
            }

            break; // RAM: no seguimos con SPIFFS
        }

        // ─────────────────────────────────────────────────────────
        // 1) Elementos de SPIFFS
        // ─────────────────────────────────────────────────────────
        const String filePath = getCurrentFilePath(targetin);
        if (!isSpiffsPath(filePath)) {
            DEBUG__________ln("Error: No se encontró un archivo para el elemento ID " + String(targetin) + ".");
            break;
        }

        // 2) Leer/actualizar modo en SPIFFS
        fs::File file = SPIFFS.open(filePath, "r+");
        if (!file) {
            DEBUG__________ln("Error: No se pudo abrir el archivo en SPIFFS.");
            break;
        }

        byte storedMode = 0;
        if (!file.seek(OFFSET_CURRENTMODE, SeekSet)) {
            DEBUG__________ln("⚠️ No se pudo posicionar en OFFSET_CURRENTMODE.");
        } else {
            (void)file.read(&storedMode, 1);
        }

        if (storedMode != receivedMode) {
            DEBUG__________printf("Actualizando el modo en SPIFFS: %d -> %d\n", storedMode, receivedMode);
            if (file.seek(OFFSET_CURRENTMODE, SeekSet)) {
                (void)file.write(&receivedMode, 1);
            } else {
                DEBUG__________ln("⚠️ No se pudo reposicionar en OFFSET_CURRENTMODE para escribir.");
            }
        } else {
            DEBUG__________ln("El modo recibido coincide con el almacenado en SPIFFS.");
        }
        file.close();

        // 3) Redibujar pantalla
        drawCurrentElement();

        // 4) Actualizar estado de selección por ID (SÓLO SPIFFS)
        for (size_t i = 0; i < elementFiles.size(); ++i) {
            const String &path = elementFiles[i];
            if (!isSpiffsPath(path)) continue;  // ← ← ← EVITA abrir "Ambientes", etc.

            fs::File idFile = SPIFFS.open(path, "r");
            if (!idFile) continue;

            if (!idFile.seek(OFFSET_ID, SeekSet)) {
                idFile.close();
                continue;
            }

            byte idCheck = 0;
            (void)idFile.read(&idCheck, 1);
            idFile.close();

            if (idCheck == targetin) {
                if (i < selectedStates.size()) {
                    selectedStates[i] = (receivedMode != 0);
                    DEBUG__________printf("🔁 Estado de selección actualizado: %s => %s\n",
                                        path.c_str(),
                                        selectedStates[i] ? "Seleccionado" : "No seleccionado");
                } else {
                    DEBUG__________ln("⚠️ selectedStates desincronizado con elementFiles (índice fuera de rango).");
                }

                if ((int)i == currentIndex) {
                    drawCurrentElement();
                }
                break;
            }
        }

        // 5) Si el elemento visible es SPIFFS y coincide el ID, recargar flags/color
        if ((size_t)currentIndex < elementFiles.size()) {
            const String currentFile = elementFiles[currentIndex];
            if (isSpiffsPath(currentFile)) {
                fs::File f = SPIFFS.open(currentFile, "r");
                if (f) {
                    byte currentElementID = 0;
                    if (f.seek(OFFSET_ID, SeekSet)) {
                        (void)f.read(&currentElementID, 1);
                    }
                    f.close();

                    if (currentElementID == targetin) {
                        byte currentMode = 0;
                        byte modeConfig[kModeConfigBytes] = {0};

                        fs::File f2 = SPIFFS.open(currentFile, "r");
                        if (f2) {
                            if (f2.seek(OFFSET_CURRENTMODE, SeekSet)) {
                                (void)f2.read(&currentMode, 1);
                            }
                            const size_t cfgOffset =
                                (size_t)OFFSET_MODES +
                                (size_t)currentMode * (size_t)SIZE_MODE +
                                (size_t)kOffsetConfigInMode;
                            if (f2.seek(cfgOffset, SeekSet)) {
                                (void)f2.read(modeConfig, kModeConfigBytes);
                            } else {
                                DEBUG__________ln("⚠️ No se pudo posicionar en OFFSET_CONFIG del modo.");
                            }
                            f2.close();
                        }

                        adxl   = getModeFlag(modeConfig, HAS_SENS_VAL_1);
                        useMic = getModeFlag(modeConfig, HAS_SENS_VAL_2);

                        colorHandler.setCurrentFile(currentFile);
                        colorHandler.setPatternBotonera(currentMode, ledManager);

                        DEBUG__________ln("🔁 Configuración de modo actual actualizada tras recibir ELEM_CMODE_SECTOR.");
                    }
                }
            }
        } else {
            DEBUG__________ln("⚠️ currentIndex fuera de rango respecto a elementFiles.");
        }

        break;
    }


        case ELEM_ICON_ROW_63_SECTOR: {
            // Reservado: gestión de fila de iconos 63.
            break;
        }

        case ELEM_CURRENT_FLAGS_SECTOR: {
            // Necesitamos al menos 2 bytes: [sector, flags]
            if (data.size() < 2) {
                DEBUG__________ln("⚠️ ELEM_CURRENT_FLAGS_SECTOR con longitud insuficiente (<2).");
                break;
            }

            const byte sourceID = targetin;
            const bool flags_bit0 = (data[kIdx1] & kBit0Mask) != 0;

            // Actualizamos el estado local del relé
            RelayStateManager::set(sourceID, flags_bit0);

            DEBUG__________ln("Estado del relé actualizado para ID " + String(sourceID) + ": " +
                              String(flags_bit0 ? "ON" : "OFF"));
            break;
        }

        default: {
            // Sectores no manejados: intencionalmente ignorado.
            break;
        }
    }
}

/**
 * @brief Busca el fichero de SPIFFS asociado a un elemento por su ID.
 *
 * Recorre la lista `elementFiles`, abre cada fichero en modo lectura,
 * lee el byte en `OFFSET_ID` y devuelve el nombre del primero que coincide
 * con `elementID`. Si no encuentra coincidencias, devuelve una cadena vacía.
 *
 * @param elementID Identificador del elemento a localizar (0..255).
 * @return String Ruta/nombre de fichero correspondiente al ID, o cadena vacía si no se encontró.
 *
 * @pre SPIFFS debe estar montado y accesible.
 * @pre Cada entrada de `elementFiles` debe ser un fichero válido que contenga un byte de ID en `OFFSET_ID`.
 *
 * @note Si existen múltiples ficheros con el mismo ID, se devuelve el primero encontrado.
 * @warning El retorno puede ser cadena vacía; el llamador debe manejar este caso antes de abrir en modo escritura.
 */
String BOTONERA_::getCurrentFilePath(byte elementID) {
    // Constantes locales para evitar números mágicos.
    constexpr size_t kIdSize = 1u;   // Leemos exactamente 1 byte de ID.

    // Recorremos el catálogo de ficheros de elemento.
    for (const String& fileName : elementFiles) {

        fs::File file = SPIFFS.open(fileName, "r");
        if (!file) {
            // Fichero inexistente o inaccesible: pasamos al siguiente.
            continue;
        }

        // Posicionamos en el offset donde reside el ID del elemento.
        if (!file.seek(OFFSET_ID, SeekSet)) {
            file.close();
            continue;
        }

        // Leemos el byte de ID almacenado en el fichero.
        byte id = 0;
        const size_t readBytes = file.read(&id, kIdSize);
        file.close();

        if (readBytes != kIdSize) {
            // Lectura incompleta: ignoramos este fichero.
            continue;
        }

        // Si el ID coincide, devolvemos la ruta inmediatamente.
        if (id == elementID) {
            return fileName;
        }
    }

    // No se encontró ningún fichero con ese ID.
    DEBUG__________printf("Error: No se encontró un archivo para el elemento ID %d.\n", elementID);
    return String();  // Cadena vacía indica "no encontrado".
}


byte tempID;
// byte BOTONERA_::validar_serial() {
//     const int max_reintentos = 5;
    
//     iniciarEscaneoElemento("Buscando elementos...");
//     for (int intento = 0; intento < max_reintentos; intento++) {
//         // Mostrar algo en la interfaz de usuario, p.ej. "Escaneando..."
        
//         frameReceived = false;
//         // Petición de ELEM_SERIAL_SECTOR al DEFAULT_DEVICE
//         send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
//                                               DEFAULT_DEVICE,
//                                               SPANISH_LANG,
//                                               ELEM_SERIAL_SECTOR));

//         // unsigned long startTime = millis();

//         // // Espera hasta 2.5s a que frameReceived se ponga en true
//         // while (!frameReceived && (millis() - startTime < 2500)) {
//         //     delay(10);
//         // }
//         if (!esperar_respuesta(2500)) {
//             DEBUG__________ln("No llegó respuesta de ELEM_SERIAL_SECTOR");
//         }

//         // Si hubo respuesta (frameReceived = true), procesamos
//         if (frameReceived) {
//             frameReceived = false; // Reiniciamos el flag
//             LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

//             // Verificamos que la trama sea del sector correcto y tenga tamaño
//             if (LEF.data.size() >= 3 && LEF.data[0] == ELEM_SERIAL_SECTOR) {
//                 // Guardamos el serial recibido
//                 memcpy(lastSerial, &LEF.data[1], 5);

//                 // Decidimos si es un elemento existente o nuevo
//                 if (serialExistsInSPIFFS(lastSerial)) {
//                     // Elemento existente
//                     return 1;
//                 } else {
//                     // Elemento nuevo
//                     return 2;
//                 }
//             }
//         }

//         // Si llegamos aquí, este intento falló
//         DEBUG__________printf("⚠️ Intento %d/%d fallido\n", intento+1, max_reintentos);
//         delay(500);
//     }

//     // Si terminamos el bucle, no hubo respuesta válida
//     return 0;
// }

void BOTONERA_::procesar_datos_sector(LAST_ENTRY_FRAME_T &LEF, int sector, INFO_PACK_T* infoPack) {
    switch (sector) {
        case ELEM_NAME_SECTOR:
            memcpy(infoPack->name, &LEF.data[1], 
                   min(sizeof(infoPack->name), LEF.data.size() - 1));
            break;

        case ELEM_DESC_SECTOR:
            memcpy(infoPack->desc, &LEF.data[1], 
                   min(sizeof(infoPack->desc), LEF.data.size() - 1));
            break;
        
        case ELEM_LOCATION_SECTOR:
            infoPack->situacion = LEF.data[1];
            break;

        case ELEM_SERIAL_SECTOR:
            memcpy(infoPack->serialNum, &LEF.data[1], 
                   min(sizeof(infoPack->serialNum), LEF.data.size() - 1));
            break;

        case ELEM_ID_SECTOR:
            infoPack->ID = LEF.data[1];
            break;

        case ELEM_CMODE_SECTOR:
            infoPack->currentMode = LEF.data[1];
            break;

        default:
            // Procesar modos
            if (sector >= ELEM_MODE_0_NAME_SECTOR && sector <= ELEM_MODE_15_FLAG_SECTOR) {
                int modeIndex  = (sector - ELEM_MODE_0_NAME_SECTOR) / 3;
                int fieldIndex = (sector - ELEM_MODE_0_NAME_SECTOR) % 3;

                if (fieldIndex == 0) {
                    memcpy(infoPack->mode[modeIndex].name, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].name), LEF.data.size()-1));
                }
                else if (fieldIndex == 1) {
                    memcpy(infoPack->mode[modeIndex].desc, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].desc), LEF.data.size()-1));
                }
                else {
                    memcpy(infoPack->mode[modeIndex].config, &LEF.data[1],
                           min(sizeof(infoPack->mode[modeIndex].config), LEF.data.size()-1));
                }
            }
            // Procesar iconos
            else if (sector >= ELEM_ICON_ROW_0_SECTOR && sector <= ELEM_ICON_ROW_63_SECTOR) {
                int rowIndex = sector - ELEM_ICON_ROW_0_SECTOR;
                if (LEF.data.size() == 129) {
                    for (int col = 0; col < ICON_COLUMNS; ++col) {
                        uint8_t msb = LEF.data[2 * col + 1];
                        uint8_t lsb = LEF.data[2 * col + 2];
                        infoPack->icono[rowIndex][col] = (uint16_t(msb) << 8) | lsb;
                    }
                } else {
                    DEBUG__________printf("❌ Fila de icono incompleta: Sector %d\n", sector);
                }
            }
            break;
    }
}

void BOTONERA_::actualizar_elemento_existente() {

    fs::File root = SPIFFS.open("/");
    fs::File file = root.openNextFile();

    while(file) {
        byte existingSerial[5];
        file.seek(OFFSET_SERIAL);
        file.read(existingSerial, 5);

        // Si coincide con el "último serial" leído
        if(memcmp(existingSerial, lastSerial, 5) == 0) {
            // Actualizamos la ID en el archivo
            file.seek(OFFSET_ID);
            file.write(&lastAssignedID, 1);
            file.close();
            break;
        }
        file = root.openNextFile();
    }
    root.close();

    // Recargamos la lista en RAM
    loadElementsFromSPIFFS();
}

bool BOTONERA_::guardar_elemento(INFO_PACK_T* infoPack) {
    // Generamos un nombre de archivo único
    String uniqueFileName = generateUniqueFileName((char*)infoPack->name);
 
    fs::File file = SPIFFS.open(uniqueFileName, "w");
    if (!file) return false;

    bool success = true;
    success &= writeBytesChecked(file, infoPack->name,      sizeof(infoPack->name));
    success &= writeBytesChecked(file, infoPack->desc,      sizeof(infoPack->desc));
    success &= writeBytesChecked(file, infoPack->serialNum, sizeof(infoPack->serialNum));
    success &= writeBytesChecked(file, &infoPack->ID,       1);
    success &= writeBytesChecked(file, &infoPack->currentMode, 1);

    // Guardamos los 16 modos
    for (int i = 0; i < 16 && success; ++i) {
        success &= writeBytesChecked(file, infoPack->mode[i].name,   sizeof(infoPack->mode[i].name));
        success &= writeBytesChecked(file, infoPack->mode[i].desc,   sizeof(infoPack->mode[i].desc));
        success &= writeBytesChecked(file, infoPack->mode[i].config, sizeof(infoPack->mode[i].config));
    }

    // Guardamos icono (64 filas, N columnas * 2 bytes)
    for (int y = 0; y < ICON_ROWS && success; ++y) {
        success &= writeBytesChecked(file, (uint8_t*)infoPack->icono[y], ICON_COLUMNS * 2);
    }

    // Después del icono
    success &= writeBytesChecked(file, &infoPack->situacion, 1);


    file.close();

    if (success) {
        // Recargamos la lista en memoria
        loadElementsFromSPIFFS();
        return true;
    }

    // Si falló algo, borramos el archivo incompleto
    SPIFFS.remove(uniqueFileName);
    return false;
}

void BOTONERA_::reasignar_id_elemento(INFO_PACK_T* infoPack) {
    byte newID = getNextAvailableID();
    byte currentID = 0;

    if(infoPack) {
        // Caso 2: se nos ha pasado la info de un elemento nuevo
        if(infoPack->ID == 0xDD) {
            infoPack->ID = newID;
            DEBUG__________printf("🆔 Nueva ID asignada: %02X\n", newID);
        }
    } else {
        // Caso 1: reasignación en un elemento ya existente, sin un infoPack
    
        fs::File root = SPIFFS.open("/");
        fs::File file = root.openNextFile();

        while(file) {
            byte existingSerial[5];
            file.seek(OFFSET_SERIAL);
            file.read(existingSerial, 5);

            if(memcmp(existingSerial, element->lastSerial, 5) == 0) {
                file.seek(OFFSET_ID);
                file.read(&currentID, 1);

                if(currentID == 0xDD) {
                    file.seek(OFFSET_ID);
                    file.write(&newID, 1);
                    DEBUG__________printf("🆔 ID actualizada: %02X\n", newID);
                }
                file.close();
                break;
            }
            file = root.openNextFile();
        }
        root.close();
    }

    DEBUG__________ln("currentID: " + String(currentID));
    DEBUG__________ln("newID: "     + String(newID));

    // Se envía el frame para fijar la nueva ID
    send_frame(frameMaker_SET_ELEM_ID(DEFAULT_BOTONERA, DEFAULT_DEVICE, newID));
    delay(100);

    DEBUG__________ln("🆙🆙🆙🆙 ID reasignada");
}

byte BOTONERA_::getIdFromSPIFFS(byte *serial) {
    if (!SPIFFS.begin(true)) {
        return 0xFF;
    }
    
    fs::File root = SPIFFS.open("/");
    if (!root || !root.isDirectory()) {
        return 0xFF;
    }
    
    root.rewindDirectory();
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Solo considerar archivos válidos
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            // Verificar que el archivo tenga tamaño suficiente para leer el serial
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                file.seek(OFFSET_SERIAL);
                byte existingSerial[5];
                int bytesRead = file.read(existingSerial, 5);
                
                if (bytesRead == 5) {
                    if (memcmp(existingSerial, serial, 5) == 0) {
                        file.seek(OFFSET_ID);
                        byte existingID = 0;
                        bytesRead = file.read(&existingID, 1);
                        
                        if (bytesRead == 1) {
                            file.close();
                            root.close();
                            return existingID;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    root.close();
    return 0xFF; // No se encontró
}

bool BOTONERA_::confirmarCambioID(byte nuevaID) {
    // Petición de ELEM_ID_SECTOR al "nuevaID"
    frameReceived = false;
    DEBUG__________ln("Confirmamos cambio de id ####################");
    send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                          nuevaID,
                                          (byte) currentLanguage,
                                          ELEM_ID_SECTOR));

    if (!esperar_respuesta(2500)) {
        DEBUG__________ln("No llegó respuesta de ELEM_ID_SECTOR");
        return false;
    }

    DEBUG__________ln(" 🥲🥲🥲 frameReceived: " + String(frameReceived));
    
    

    // Procesamos la trama recibida
    LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

    // Verificamos que data[0] sea ELEM_ID_SECTOR y data[1] sea la nuevaID
    if (LEF.data.size() >= 2 &&
        LEF.data[0] == ELEM_ID_SECTOR &&
        LEF.data[1] == nuevaID) {
        return true; // Confirmación OK
    }

    return false; // Sector incorrecto o ID incorrecta
}

bool BOTONERA_::confirmarCambioIDConSerial(uint8_t nuevaID,
                                           const uint8_t serialEsperado[5],
                                           unsigned long timeoutPerAttempt,
                                           int retries)
{
    for (int intento = 0; intento < retries; ++intento) {
        frameReceived = false;

        // Pedimos el SERIAL a la NUEVA ID para verificar identidad
        send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA, nuevaID, (byte) currentLanguage, ELEM_SERIAL_SECTOR));

        if (esperar_respuesta(timeoutPerAttempt)) {
            const LAST_ENTRY_FRAME_T lef = extract_info_from_frameIn(uartBuffer);

            // Validar que nos responden el sector de SERIAL y que el payload tiene al menos 1 + 5 bytes
            if (lef.function == F_RETURN_ELEM_SECTOR &&
                lef.data.size() >= 6 &&
                lef.data[0] == ELEM_SERIAL_SECTOR)
            {
                const uint8_t* serialRx = &lef.data[1];
                if (memcmp(serialRx, serialEsperado, 5) == 0) {
                    return true; // ✅ La NUEVA ID pertenece al dispositivo esperado
                }
                // (Opcional) DEBUG: imprimir serialRx si no coincide
            }
        }

        delay(80); // pequeño backoff
    }

    return false; // ❌ No coincide serial o no hubo respuesta válida
}


bool BOTONERA_::esperar_respuesta(unsigned long timeout) {
    unsigned long startTime = millis();
    while (millis() - startTime < timeout) {
        if (frameReceived) return true; // Respuesta recibida
        delay(100);
    }
    return false; // Timeout
}

bool BOTONERA_::procesar_y_guardar_elemento_nuevo(byte targetID) {
    INFO_PACK_T* infoPack = new INFO_PACK_T;
    memset(infoPack, 0, sizeof(INFO_PACK_T));
    
    // Rellenamos la info base
    infoPack->ID = targetID;
    memcpy(infoPack->serialNum, lastSerial, 5);

    // Pedimos todos los sectores
    bool error = false;
    // Descargar sectores clave manualmente para evitar exclusión de LOCATION
    std::vector<int> sectores = {
        ELEM_NAME_SECTOR,
        ELEM_DESC_SECTOR,
        ELEM_LOCATION_SECTOR,
        ELEM_SERIAL_SECTOR,
        ELEM_ID_SECTOR,
        ELEM_CMODE_SECTOR
    };

    // Modos
    for (int i = 0; i < 16; i++) {
        sectores.push_back(ELEM_MODE_0_NAME_SECTOR + i * 3);
        sectores.push_back(ELEM_MODE_0_DESC_SECTOR + i * 3);
        sectores.push_back(ELEM_MODE_0_FLAG_SECTOR + i * 3);
    }

    // Icono
    for (int i = 0; i < 64; i++) {
        sectores.push_back(ELEM_ICON_ROW_0_SECTOR + i);
    }

    // Procesar todos
    for (int sector : sectores) {
        if (!procesar_sector(sector, infoPack, targetID)) {
            error = true;
            break;
        }
    }


    // Si falló alguno de los sectores, limpiamos y salimos
    if (error) {
        delete infoPack;
        return false;
    }

    // Si llegamos aquí, descargamos todo OK => lo guardamos en SPIFFS
    bool guardado = guardar_elemento(infoPack);
    print_info_pack(infoPack);
    delete infoPack;

    // guardado => true/false
    return guardado;
}

bool BOTONERA_::procesar_sector(int sector,
                                INFO_PACK_T* infoPack,
                                uint8_t targetID)
{
    const int max_reintentos = 10; // Tal como lo tenías

    for (int intento = 0; intento < max_reintentos; intento++) {
        // 1) Petición del sector
        frameReceived = false; // Asegúrate que esta variable es miembro de BOTONERA_ o accesible
        send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                              targetID,
                                              (byte) currentLanguage,
                                              sector));

        if (!esperar_respuesta(2000)) { // Usando tu timeout
            DEBUG__________printf("Procesar_sector: No hubo respuesta para ID 0x%02X, sector %d, intento %d/%d\n", targetID, sector, intento + 1, max_reintentos);
            delay(100); // Pequeña pausa antes de reintentar
            continue;
        }

        // Asumo que uartBuffer es miembro de BOTONERA_ o accesible
        LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
        frameReceived = false; // Resetear para la próxima posible recepción

        if (LEF.data.size() <= 1 || LEF.data[0] != sector) {
            DEBUG__________printf("Procesar_sector: Frame inválido o sector erróneo. ID:0x%02X, Sec Esperado:%d, Sec Recibido:%d, Intento:%d/%d\n",
                targetID, sector, (LEF.data.empty() ? -1 : LEF.data[0]), intento + 1, max_reintentos);
            delay(100); // Pequeña pausa
            continue;
        }

        // 2) Procesar datos del sector
        // Se asume que esta función llena infoPack->name si sector == ELEM_NAME_SECTOR
        // y que infoPack->name es un buffer de bytes (unsigned char) que contiene una cadena C (terminada en nulo).
        procesar_datos_sector(LEF, sector, infoPack);

        // 3) Si es el sector de nombre, lo mostramos:
        static const char* nombreAMostrar = nullptr;

        if (sector == ELEM_NAME_SECTOR) {
            const char* nombre = reinterpret_cast<const char*>(infoPack->name);
            nombreAMostrar = (nombre && *nombre) ? nombre : "[Elemento Sin Nombre]";
        }


        // 4) Actualizar sólo la barra de progreso (sin etiqueta explícita aquí):
        actualizarBarraProgreso2(sector,                   // Paso actual (el sector que se acaba de procesar)
                                ELEM_ICON_ROW_63_SECTOR,   // Pasos totales (último sector esperado)
                                nombreAMostrar);           // Con etiqueta, para sobrescribir el nombre

        return true; // Sector procesado exitosamente
    }

    // Si el bucle termina, todos los reintentos fallaron para este sector
    DEBUG__________printf("ERROR CRÍTICO: Fallaron todos los intentos (%d) para procesar sector %d del ID 0x%02X\n", max_reintentos, sector, targetID);
    return false;
}

void BOTONERA_::print_info_pack(const INFO_PACK_T *infoPack) {
    DEBUG__________ln("---- INFO DEL ELEMENTO ALMACENADO ----");
    
    DEBUG__________("Nombre: ");
    DEBUG__________ln((char*)infoPack->name);

    DEBUG__________("Descripción: ");
    DEBUG__________ln((char*)infoPack->desc);

    DEBUG__________("Número de Serie: ");
    DEBUG__________printf("0x%02X%02X%02X%02X%02X\n", infoPack->serialNum[0], infoPack->serialNum[1], infoPack->serialNum[2], infoPack->serialNum[3], infoPack->serialNum[4]);

    DEBUG__________("ID: 0x");
    DEBUG__________ln(infoPack->ID, HEX);

    DEBUG__________("Modo Actual: ");
    DEBUG__________ln(infoPack->currentMode);

    // Imprimir información de los modos
    for (int i = 0; i < 16; ++i) {
        DEBUG__________printf("Modo %d\n", i);
        DEBUG__________("  Nombre: ");
        DEBUG__________ln((char*)infoPack->mode[i].name);

        DEBUG__________("  Descripción: ");
        DEBUG__________ln((char*)infoPack->mode[i].desc);

        // Imprimir configuración en hexadecimal y binario
        DEBUG__________printf("  Configuración: 0x%02X%02X (Binario: ", infoPack->mode[i].config[0], infoPack->mode[i].config[1]);

        // Imprimir el primer byte en binario
        for (int bit = 7; bit >= 0; --bit) {
            DEBUG__________((infoPack->mode[i].config[0] >> bit) & 1);
        }

        DEBUG__________(" ");

        // Imprimir el segundo byte en binario
        for (int bit = 7; bit >= 0; --bit) {
            DEBUG__________((infoPack->mode[i].config[1] >> bit) & 1);
        }

        DEBUG__________ln(")");
    }


    // Imprimir información del icono (solo las primeras filas para no saturar el Serial)
    DEBUG__________ln("---- ICONO DEL ELEMENTO (16 bits) ----");
    for (int row = 0; row < 5; ++row) {
        DEBUG__________printf("Fila %02d: ", row);
        for (int col = 0; col < 64; ++col) {
            DEBUG__________printf("%04X ", infoPack->icono[row][col]);  // Mostrar en hexadecimal
        }
        DEBUG__________ln();
    }
    DEBUG__________ln("---- FIN DEL ICONO ----");

    DEBUG__________ln("---- FIN DEL INFO PACK ----");
}

bool BOTONERA_::serialExistsInSPIFFS(byte serialNum[5]) {
    if (!SPIFFS.begin(true)) {
        return false;
    }
  
    fs::File root = SPIFFS.open("/");
    if (!root) {
        return false;
    }
    if (!root.isDirectory()) {
        root.close();
        return false;
    }
    
    root.rewindDirectory();
    
    // Verificar primero cuántos archivos hay en total
    int totalFiles = 0;
    while (true) {
        fs::File tempFile = root.openNextFile();
        if (!tempFile) break;
        totalFiles++;
        tempFile.close();
    }
    
    root.rewindDirectory();
    
    INFO_PACK_T *tempPack = new (std::nothrow) INFO_PACK_T;
    if (!tempPack) {
        root.close();
        return false;
    }
    
    bool found = false;
    int filesChecked = 0;
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        filesChecked++;
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo (añadir '/' al inicio si no está presente)
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Solo procesar archivos reales de elementos
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                // Leer el ID primero para verificar
                file.seek(OFFSET_ID);
                byte id;
                int bytesRead = file.read(&id, 1);
                
                if (bytesRead == 1) {
                    // Ahora leer el serial
                    file.seek(OFFSET_SERIAL);
                    bytesRead = file.read(tempPack->serialNum, 5);
                    
                    if (bytesRead == 5) {
                        // Comparar con el serial buscado
                        if (memcmp(tempPack->serialNum, serialNum, 5) == 0) {
                            found = true;
                            file.close();
                            break;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    delete tempPack;
    root.close();
    
    return found;
}

void BOTONERA_::dibujarMarco(uint16_t color) {
    tft.drawRoundRect(2, 2, 124, 124, 10, color);
    tft.drawRoundRect(4, 4, 120, 120, 8, color);
}


#define MSG_ERROR_NO_RESPUESTA 0
#define MSG_ADVERTENCIA_ID_DESAJUSTE 1
#define MSG_EXITO_GENERICO 2
#define MSG_NUEVO_ELEMENTO_AGREGADO 3
#define MSG_ERROR_DESCONOCIDO 4

void BOTONERA_::mostrarMensajeTemporal(int respuesta, int dTime) {
    // 1) Limpiar el sprite (fondo negro) y preparar estilos
    uiSprite.fillSprite(TFT_BLACK);

    // Determinar color y mensajes
    uint32_t colorTexto;
    const char* mensajePrincipal;
    const char* mensajeSecundario;

    if (respuesta == 0) { // ERROR GENERAL
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Sin respuesta";
    } else if (respuesta == 1) { // ADVERTENCIA
        colorTexto = TFT_YELLOW;
        mensajePrincipal = "ADVERTENCIA";
        mensajeSecundario = "Verifique el estado del elemento";
    } else if (respuesta == 2) { // ÉXITO / NUEVO
        colorTexto = TFT_GREEN;
        mensajePrincipal  = getTranslation("SUCCESS");
        mensajeSecundario = getTranslation("ROOM_UPDATED");
    } else if (respuesta == 3) { // ERROR ESPECÍFICO
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Fallo en la operación";
    } else { // ERROR DESCONOCIDO
        colorTexto = TFT_RED;
        mensajePrincipal = "ERROR";
        mensajeSecundario = "Error desconocido";
    }

    // 2) Mensaje principal centrado
    uiSprite.setTextDatum(MC_DATUM);
    uiSprite.setTextFont(respuesta == 1 ? 2 : 4);   // Advertencia usa fuente más pequeña
    uiSprite.setTextColor(colorTexto, TFT_BLACK);    // Fondo negro para evitar “fantasmas”
    uiSprite.drawString(mensajePrincipal, 64, 30);

    // 3) Mensaje secundario (envuelto simple por longitud) centrado
    uiSprite.setTextFont(2);
    uiSprite.setTextColor(TFT_WHITE, TFT_BLACK);

    const int maxCharsPerLine = 18;
    const int lineHeight = 20;
    char buffer[maxCharsPerLine + 1];
    int textLength = strlen(mensajeSecundario);
    int yPos = 60;

    for (int i = 0; i < textLength; i += maxCharsPerLine) {
        strncpy(buffer, mensajeSecundario + i, maxCharsPerLine);
        buffer[maxCharsPerLine] = '\0';
        uiSprite.drawString(buffer, 64, yPos);
        yPos += lineHeight;
    }

    // 4) Icono correspondiente (centrado)
    const int iconCenterX = 64;
    const int iconCenterY = 90;
    const int iconSize    = 10;

    if (respuesta == 0 || respuesta == 3 || (respuesta != 1 && respuesta != 2)) {
        // ERROR (0, 3, default)
        uiSprite.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_RED);
        uiSprite.drawLine(iconCenterX - iconSize/2 + 2, iconCenterY - iconSize/2 + 2,
                          iconCenterX + iconSize/2 - 2, iconCenterY + iconSize/2 - 2, TFT_WHITE);
        uiSprite.drawLine(iconCenterX + iconSize/2 - 2, iconCenterY - iconSize/2 + 2,
                          iconCenterX - iconSize/2 + 2, iconCenterY + iconSize/2 - 2, TFT_WHITE);
    }
    else if (respuesta == 2) {
        // ÉXITO
        uiSprite.fillCircle(iconCenterX, iconCenterY, iconSize, TFT_GREEN);
        uiSprite.drawLine(iconCenterX - iconSize/2 + 2, iconCenterY,
                          iconCenterX - iconSize/2 + 4, iconCenterY + 2, TFT_WHITE);
        uiSprite.drawLine(iconCenterX - iconSize/2 + 4, iconCenterY + 2,
                          iconCenterX + iconSize/2 - 1, iconCenterY - 3, TFT_WHITE);
    }
    else if (respuesta == 1) {
        // ADVERTENCIA (triángulo con exclamación)
        const int triangleSize   = iconSize * 2;
        const int triangleHeight = (int)(triangleSize * 0.866f);

        const int x1 = iconCenterX;
        const int y1 = iconCenterY - (triangleHeight / 2);
        const int x2 = iconCenterX - (triangleSize / 2);
        const int y2 = iconCenterY + (triangleHeight / 2);
        const int x3 = iconCenterX + (triangleSize / 2);
        const int y3 = iconCenterY + (triangleHeight / 2);

        uiSprite.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_YELLOW);

        const int exclamationHeight = (int)(triangleHeight * 0.6f);
        const int barY = iconCenterY - (exclamationHeight / 2) + 1;
        uiSprite.fillRect(iconCenterX - 1, barY, 2, exclamationHeight - 3, TFT_BLACK);    // barra
        uiSprite.fillRect(iconCenterX - 1, barY + (exclamationHeight - 3) + 1, 2, 2, TFT_BLACK); // punto
    }

    // 5) Empujar el sprite a pantalla y mantenerlo visible
    uiSprite.pushSprite(0, 0);
    delay(dTime);
}

byte BOTONERA_::getNextAvailableID() {
    byte nextID = 0x01;
    
    if (!SPIFFS.begin(true)) {
        return 0xDD;
    }
    
    fs::File root = SPIFFS.open("/");
    if (!root) {
        return 0xDD;
    }
    if (!root.isDirectory()) {
        root.close();
        return 0xDD;
    }
    
    // Contar total de archivos
    root.rewindDirectory();
    int totalFiles = 0;
    while (true) {
        fs::File tempFile = root.openNextFile();
        if (!tempFile) break;
        
        totalFiles++;
        tempFile.close();
    }
    
    // Buscar archivos de elementos
    root.rewindDirectory();
    int elementCount = 0;
    
    while (true) {
        fs::File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        String fileName = String(file.name());
        
        // Normalizar el nombre del archivo (añadir '/' al inicio si no está presente)
        if (!fileName.startsWith("/")) {
            fileName = "/" + fileName;
        }
        
        // Verificar si es un archivo de elemento
        if (fileName.startsWith("/element_") && fileName.endsWith(".bin")) {
            elementCount++;
            
            if (file.size() >= MIN_VALID_ELEMENT_SIZE) {
                // Leer la ID
                file.seek(OFFSET_ID);
                byte existingID = 0;
                int bytesRead = file.read(&existingID, 1);
                
                if (bytesRead == 1) {
                    // Verificar el número de serie para confirmar que es un elemento válido
                    file.seek(OFFSET_SERIAL);
                    byte serial[5];
                    bytesRead = file.read(serial, 5);
                    
                    if (bytesRead == 5) {
                        // Actualizar nextID si es necesario
                        if (existingID >= nextID) {
                            nextID = existingID + 1;
                        }
                    }
                }
            }
        }
        
        file.close();
    }
    
    root.close();
    
    return nextID;
}

bool inCognitiveMenu = false;

void BOTONERA_::activateCognitiveMode() {
    inCognitiveMenu = true;
    drawCognitiveMenu();
    colorHandler.mapCognitiveLEDs(); // función que veremos abajo
}

void BOTONERA_::deactivateCognitiveMode() {
    inCognitiveMenu = false;
    drawCurrentElement(); // Volver a la pantalla principal
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool scanDD = false;

/**
 * @brief Pings a targetID for its serial number.
 * @param targetID The ID of the element to scan.
 * @param serial Output array (5 bytes) to store the received serial number.
 * @param timeoutPerAttempt Timeout in ms for each attempt.
 * @param retries Number of retries.
 * @return true if the element responded with a valid serial number, false otherwise.
 */

bool BOTONERA_::escanearID(byte targetID, byte serial[5], unsigned long timeoutPerAttempt, int retries) {
    DEBUG__________printf("escanearID: Solicitando serial a ID 0x%02X...\n", targetID);
    for (int i = 0; i < retries; ++i) {
        frameReceived = false;
            send_frame(frameMaker_REQ_ELEM_SECTOR(DEFAULT_BOTONERA,
                                                  targetID,
                                                  (byte) currentLanguage,
                                                  ELEM_SERIAL_SECTOR));
        

        if (esperar_respuesta(timeoutPerAttempt)) { // esperar_respuesta usa frameReceived
            LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);

            // Validar que la respuesta sea del sector correcto y tenga suficientes datos
            // Data[0] es el sector, Data[1] a Data[5] es el serial.
            if (LEF.function == F_RETURN_ELEM_SECTOR && LEF.data.size() >= (1 + 5) && LEF.data[0] == ELEM_SERIAL_SECTOR) {
                memcpy(serial, &LEF.data[1], 5);
                DEBUG__________printf("escanearID: ID 0x%02X respondió con SN: %02X%02X%02X%02X%02X\n",
                              targetID, serial[0], serial[1], serial[2], serial[3], serial[4]);
                return true;
            } else {
                DEBUG__________printf("escanearID: Respuesta inválida de ID 0x%02X (Fun:0x%02X, Sector:0x%02X, Size:%d). Intento %d/%d\n",
                              targetID, LEF.function, (LEF.data.size()>0 ? LEF.data[0] : 0xFF), LEF.data.size(), i + 1, retries);
            }
        } else {
            DEBUG__________printf("escanearID: Timeout esperando respuesta de ID 0x%02X. Intento %d/%d\n", targetID, i + 1, retries);
        }
        if (i < retries - 1) delay(50); // Pequeña pausa antes del siguiente reintento
    }
    return false; // No se obtuvo respuesta válida tras todos los reintentos
}

/**
 * @brief Finds an element file in SPIFFS by its serial number and updates its ID field.
 * @param serial The serial number (5 bytes) of the element to update.
 * @param nuevaID The new ID to write into the element's file.
 */
void BOTONERA_::actualizarIDenSPIFFS(const byte serial[5], byte nuevaID) {
    DEBUG__________printf("actualizarIDenSPIFFS: Buscando SN %02X%02X%02X%02X%02X para actualizar ID a 0x%02X\n",
                  serial[0], serial[1], serial[2], serial[3], serial[4], nuevaID);

    if (!SPIFFS.begin(true)) {
        DEBUG__________ln("actualizarIDenSPIFFS: Error al montar SPIFFS.");
        return;
    }

    // Iterar sobre los archivos conocidos por loadElementsFromSPIFFS
    // Esto es más eficiente que abrir y cerrar cada archivo del directorio raíz.
    for (const String& filePath : elementFiles) {
        fs::File file = SPIFFS.open(filePath, "r+"); // Abrir en modo lectura/escritura
        if (!file) {
            DEBUG__________printf("actualizarIDenSPIFFS: No se pudo abrir el archivo %s\n", filePath.c_str());
            continue;
        }

        if (file.size() < MIN_VALID_ELEMENT_SIZE) {
            file.close();
            continue;
        }

        byte existingSerial[5];
        file.seek(OFFSET_SERIAL);
        if (file.read(existingSerial, 5) == 5) {
            if (memcmp(existingSerial, serial, 5) == 0) {
                // Serial coincide, actualizamos la ID
                byte oldID;
                file.seek(OFFSET_ID);
                file.read(&oldID,1); // Leer la ID antigua por si acaso o para loguear

                file.seek(OFFSET_ID); // Volver a posicionar para escribir
                if (file.write(&nuevaID, 1) == 1) {
                    DEBUG__________printf("actualizarIDenSPIFFS: Éxito. ID para SN %02X... actualizada de 0x%02X a 0x%02X en archivo %s\n",
                                  serial[0], oldID, nuevaID, filePath.c_str());
                } else {
                    DEBUG__________printf("actualizarIDenSPIFFS: Error al escribir nueva ID en archivo %s\n", filePath.c_str());
                }
                file.close();
                // Una vez actualizado, se podría recargar elementFiles o la info del elemento específico si está en RAM.
                // loadElementsFromSPIFFS(); // Opcional: recargar todo si es necesario inmediatamente.
                return; // Encontrado y procesado (o intento de proceso)
            }
        }
        file.close();
    }
    DEBUG__________printf("actualizarIDenSPIFFS: No se encontró archivo en SPIFFS con SN %02X...\n", serial[0]);
}

/**
 * @brief Finds the first ID (1-32) not marked as true in the `ocupadas` array.
 * @param ocupadas Boolean array of 32 elements. ocupadas[i] is true if ID (i+1) is taken.
 * @return The first free ID (1-32), or 0xFF if no ID is free.
 */
byte BOTONERA_::buscarPrimerIDLibre(const bool ocupadas[32]) {
    for (int i = 0; i < 32; ++i) {
        if (!ocupadas[i]) {
            DEBUG__________printf("buscarPrimerIDLibre: ID libre encontrada: 0x%02X\n", (byte)(i + 1));
            return (byte)(i + 1); // IDs son 1-based
        }
    }
    DEBUG__________ln("buscarPrimerIDLibre: No hay IDs libres entre 1 y 32.");
    return 0xFF; // Ninguna ID libre encontrada
}


/**
 * @brief Downloads all sectors from a new element and saves it to SPIFFS.
 *        This is a modified version that takes the serial number as a parameter.
 * @param targetID The ID of the new element.
 * @param serialNumDelElemento The serial number (5 bytes) of the new element.
 * @return true if the element was successfully processed and saved, false otherwise.
 */
bool BOTONERA_::procesar_y_guardar_elemento_nuevo(byte targetID, const byte serialNumDelElemento[5]) {
    INFO_PACK_T* infoPack = new INFO_PACK_T;
    if (!infoPack) {
    DEBUG__________ln("procesar_y_guardar_elemento_nuevo: Error: Fallo al alocar memoria para INFO_PACK_T");
    return false;
    }
    memset(infoPack, 0, sizeof(INFO_PACK_T));

    // Rellenamos la info base
    infoPack->ID = targetID;
    memcpy(infoPack->serialNum, serialNumDelElemento, 5); // Usar el serial pasado como parámetro

    DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Preparando para descargar info de ID 0x%02X SN:%02X%02X%02X%02X%02X\n",
                targetID, serialNumDelElemento[0], serialNumDelElemento[1], serialNumDelElemento[2], serialNumDelElemento[3], serialNumDelElemento[4]);

    // Definición de los sectores a descargar
    std::vector<int> sectores_a_descargar = {
        ELEM_NAME_SECTOR, ELEM_DESC_SECTOR, ELEM_LOCATION_SECTOR,
        ELEM_CMODE_SECTOR
        // ELEM_SERIAL_SECTOR y ELEM_ID_SECTOR podrían pedirse de nuevo para doble confirmación,
        // pero ya tenemos el serial y hemos asignado (o estamos usando) targetID.
        // Si se añaden, hay que asegurarse que procesar_datos_sector no sobrescriba
        // infoPack->ID o infoPack->serialNum si ya están seteados correctamente.
        // Por ahora, los omitimos para eficiencia, asumiendo que la info que tenemos es correcta.
    };

    // Añadir sectores de los 16 modos
    for (int i = 0; i < 16; i++) {
        sectores_a_descargar.push_back(ELEM_MODE_0_NAME_SECTOR + i * 3);  // Nombre del modo i
        sectores_a_descargar.push_back(ELEM_MODE_0_DESC_SECTOR + i * 3);  // Descripción del modo i
        sectores_a_descargar.push_back(ELEM_MODE_0_FLAG_SECTOR + i * 3); // Config/Flags del modo i
    }

    // Añadir sectores del icono (64 filas)
    for (int i = 0; i < ICON_ROWS; i++) { // ICON_ROWS debería ser 64
        sectores_a_descargar.push_back(ELEM_ICON_ROW_0_SECTOR + i);
    }

    // Interfaz de Usuario: Iniciar progreso de descarga
    // iniciarEscaneoElemento("Descargando Info..."); // Mensaje general de descarga
    // actualizarBarraProgreso(0);

    bool error_descarga = false;
    for (size_t i = 0; i < sectores_a_descargar.size(); ++i) {
        int sector = sectores_a_descargar[i];
        DEBUG__________printf("Descargando sector 0x%02X para ID 0x%02X (%zu/%zu)\n", sector, targetID, i + 1, sectores_a_descargar.size());
        
        // Actualizar UI de progreso
        // float progreso = ((float)(i + 1) / sectores_a_descargar.size()) * 100.0;
        // actualizarBarraProgreso(progreso);

        if (!procesar_sector(sector, infoPack, targetID)) { // procesar_sector es una función existente
            DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Error al procesar sector 0x%02X para ID 0x%02X.\n", sector, targetID);
            error_descarga = true;
            break;
        }
    }

    if (error_descarga) {
        delete infoPack;
        DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Falló la descarga de uno o más sectores para ID 0x%02X.\n", targetID);
        // mostrarMensajeTemporal("Error Descarga", 2000);
        return false;
    }

    DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Todos los sectores descargados para ID 0x%02X. Guardando en SPIFFS...\n", targetID);
    // print_info_pack(infoPack); // Descomentar para depuración si es necesario

    bool guardado_exitoso = guardar_elemento(infoPack); // guardar_elemento es una función existente
    delete infoPack; // Liberar memoria del infoPack

    if (guardado_exitoso) {
        DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Elemento ID 0x%02X guardado exitosamente.\n", targetID);
        // `guardar_elemento` ya debería llamar a `loadElementsFromSPIFFS()` internamente si es necesario.
        // mostrarMensajeTemporal("Elemento Guardado", 2000);
    } else {
        DEBUG__________printf("procesar_y_guardar_elemento_nuevo: Error al guardar el elemento ID 0x%02X en SPIFFS.\n", targetID);
        // mostrarMensajeTemporal("Error Guardando", 2000);
    }

    return guardado_exitoso;
}

/**
 * @brief Checks if SPIFFS contains a file for an element that is registered with the given ID.
 * @param idToFind The ID to look for in SPIFFS element files.
 * @return true if an element file exists and is registered with idToFind, false otherwise.
 */
bool BOTONERA_::elementoAsignadoA_ID_enSPIFFS(byte idToFind) {
    if (!SPIFFS.begin(true)) {
        DEBUG__________ln("elementoAsignadoA_ID_enSPIFFS: Error al montar SPIFFS.");
        return false;
    }

    // Iterar sobre los archivos conocidos por loadElementsFromSPIFFS
    for (const String& filePath : elementFiles) {
        fs::File file = SPIFFS.open(filePath, "r");
        if (!file) {
            // DEBUG__________printf("elementoAsignadoA_ID_enSPIFFS: No se pudo abrir %s\n", filePath.c_str());
            continue;
        }

        if (file.size() < MIN_VALID_ELEMENT_SIZE) { // Asegúrate que MIN_VALID_ELEMENT_SIZE y OFFSET_ID estén definidos
            file.close();
            continue;
        }

        byte idEnArchivo;
        file.seek(OFFSET_ID); //OFFSET_ID debe estar definido
        if (file.read(&idEnArchivo, 1) == 1) {
            if (idEnArchivo == idToFind) {
                file.close();
                // DEBUG__________printf("elementoAsignadoA_ID_enSPIFFS: ID 0x%02X encontrada en %s\n", idToFind, filePath.c_str());
                return true;
            }
        }
        file.close();
    }
    // DEBUG__________printf("elementoAsignadoA_ID_enSPIFFS: ID 0x%02X no encontrada en ningún archivo de SPIFFS.\n", idToFind);
    return false;
}

void BOTONERA_::escanearSala() {
    formatSubMenuActive = false;
    hiddenMenuActive = false; 
inicio_escanear_sala_completo:
    DEBUG__________ln("=== 🚀 INICIO ESCANEO DE SALA (CON SPLIT ATTACH) 🚀 ===");
    scanDD = false;  // Para que escanearID (1-32) pida ELEM_SERIAL_SECTOR
    //iniciarEscaneoElemento("Escaneando Sala 1/32");

    
    //dibujarMarco(TFT_WHITE);

    loadElementsFromSPIFFS();

    bool listaIDsOcupadasScanActual[32];
    bool changeFlag;
    byte currentID;

    do {
        tft.fillScreen(TFT_BLACK);
        changeFlag = false;
        currentID = 1;
        memset(listaIDsOcupadasScanActual, false, sizeof(listaIDsOcupadasScanActual));

        DEBUG__________ln("--- Iniciando pasada de escaneo IDs 1-32 ---");

        while (currentID <= 32) {
            DEBUG__________printf("🔍 Escaneando ID: 0x%02X (%d/32)\n", currentID, currentID);

            // 1) Actualizar barra de progreso e indicar ID actual
            //char etiquetaID[16];
            //snprintf(etiquetaID, sizeof(etiquetaID), "ID %d/32", currentID);
            const char* textoBase = getTranslation("SEARCHING");
            //snprintf(etiquetaID, sizeof(etiquetaID), "%s %d/32", textoBase, currentID);

            //actualizarBarraProgreso(currentID, 32, etiquetaID);
            actualizarBarraProgreso2(currentID, 32, textoBase);

            // 2) Escanear
            byte serialRecibido[5];
            bool responde = escanearID(currentID, serialRecibido, 600, 2);

            if (!responde) {
                DEBUG__________printf("ID 0x%02X: No responde.\n", currentID);
                listaIDsOcupadasScanActual[currentID - 1] = false;
            } else {
                DEBUG__________printf(
                    "ID 0x%02X: Responde con SN: %02X%02X%02X%02X%02X\n",
                    currentID,
                    serialRecibido[0], serialRecibido[1],
                    serialRecibido[2], serialRecibido[3],
                    serialRecibido[4]
                );
                listaIDsOcupadasScanActual[currentID - 1] = true;

                bool snExisteEnSPIFFS_flag = serialExistsInSPIFFS(serialRecibido);
                if (!snExisteEnSPIFFS_flag) {
                    DEBUG__________printf(
                        "SN %02X... no encontrado en SPIFFS. Tratando como nuevo en ID 0x%02X.\n",
                        serialRecibido[0], currentID
                    );
                    bool elementoAgregadoConExito = false;
                    for (int intento = 0; intento < 3; ++intento) {
                        if (procesar_y_guardar_elemento_nuevo(currentID, serialRecibido)) {
                            elementoAgregadoConExito = true;
                            break;
                        }
                        DEBUG__________printf(
                            "Fallo intento %d/3 procesar_y_guardar para ID 0x%02X. Reintentando...\n",
                            intento + 1, currentID
                        );
                        delay(200);
                    }
                    if (elementoAgregadoConExito) {
                        DEBUG__________printf("Nuevo elemento en ID 0x%02X guardado.\n", currentID);
                        changeFlag = true;
                        DEBUG__________ln("Elemento nuevo agregado. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;
                    } else {
                        DEBUG__________printf(
                            "ERROR: Fallaron todos los intentos para guardar nuevo elemento en ID 0x%02X.\n",
                            currentID
                        );
                        listaIDsOcupadasScanActual[currentID - 1] = false;
                    }

                } else {
                    byte idSPIFFS = getIdFromSPIFFS(serialRecibido);
                    DEBUG__________printf(
                        "SN %02X... encontrado en SPIFFS. ID en SPIFFS: 0x%02X. ID actual respuesta: 0x%02X.\n",
                        serialRecibido[0], idSPIFFS, currentID
                    );
                    if (idSPIFFS == 0xFF) {
                        DEBUG__________printf(
                            "ERROR CRÍTICO: SN %02X... existe en SPIFFS pero ID guardada es inválida (0xFF).\n",
                            serialRecibido[0]
                        );
                        DEBUG__________printf(
                            "Actualizando SPIFFS para SN %02X... con ID actual 0x%02X.\n",
                            serialRecibido[0], currentID
                        );
                        actualizarIDenSPIFFS(serialRecibido, currentID);
                        changeFlag = true;
                        DEBUG__________ln("ID en SPIFFS (era 0xFF) actualizada. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;

                    } else if (idSPIFFS == currentID) {
                        DEBUG__________printf("ID 0x%02X: Correcta y confirmada con SPIFFS.\n", currentID);

                    } else {
                        DEBUG__________printf(
                            "ID 0x%02X: Desajuste. SPIFFS dice 0x%02X para SN %02X.... Actualizando SPIFFS a 0x%02X.\n",
                            currentID, idSPIFFS, serialRecibido[0], currentID
                        );
                        actualizarIDenSPIFFS(serialRecibido, currentID);
                        changeFlag = true;
                        DEBUG__________ln("ID en SPIFFS actualizada. Forzando reinicio de escaneo 1-32.");
                        goto reiniciar_barrido_1_32;
                    }
                }
            }

            currentID++;
        }

    reiniciar_barrido_1_32:;
        if (currentID <= 32 && changeFlag) {
            DEBUG__________printf(
                "Interrupción en ID 0x%02X para reiniciar escaneo 1-32 debido a changeFlag.\n",
                currentID
            );
        }
        DEBUG__________printf(
            "--- Pasada de escaneo IDs 1-32 completada. ChangeFlag: %s ---\n",
            changeFlag ? "TRUE" : "FALSE"
        );

    } while (changeFlag);

    DEBUG__________ln("=== Escaneo IDs 1-32 ESTABLE ===");
    //actualizarBarraProgreso(32, 32, "ID 32/32");
    int pasoActual = 32;
    const char* etiquetaID = getTranslation("SEARCHING");
    actualizarBarraProgreso2(pasoActual, 32, etiquetaID);
    delay(1000);

    // 🔶 PASO DE GESTIÓN DE 0xDD EN DOS FASES 🔶
    DEBUG__________ln("--- Iniciando gestión de dispositivos en ID 0xDD (DEFAULT_DEVICE) en dos fases ---");
    bool seProcesoUnDDYRequiereReinicio = false;
    std::vector<std::array<byte, 5>> serialesDDProcesadosGlobalmente;

    byte attachRequestTypes[] = { ELEM_FIRST_SPLIT_ATTACH_REQ, ELEM_LAST_SPLIT_ATTACH_REQ };
    const char* attachRequestNames[] = { "FIRST_SPLIT_ATTACH_REQ", "LAST_SPLIT_ATTACH_REQ" };

    unsigned long tiempoInicioEsperaDD = millis();
    unsigned long lastAnimFrameTime = millis();
    int frameCountAnim = 0;


    for (int fase = 0; fase < 2; ++fase) {
        DEBUG__________printf(
            "--- Fase %d de gestión 0xDD: Enviando %s ---\n",
            fase + 1, attachRequestNames[fase]
        );

        // Iniciar el modal visual desde frame 0
        int frameCountAnim = 0;
        drawLoadingModalFrame(getTranslation("UPDATING"), frameCountAnim);

        frameReceived = false;
        delay(100);

        // Envío de petición RF
        send_frame(frameMaker_REQ_ELEM_SECTOR(
            DEFAULT_BOTONERA, DEFAULT_DEVICE, (byte) currentLanguage, attachRequestTypes[fase]
        ));
        if (attachRequestTypes[fase] == ELEM_LAST_SPLIT_ATTACH_REQ) {
            delay(1000);
            send_frame(frameMaker_REQ_ELEM_SECTOR(
                DEFAULT_BOTONERA, DEFAULT_DEVICE, (byte) currentLanguage, attachRequestTypes[fase]
            ));
        }

        DEBUG__________printf(
            "Esperando respuestas de dispositivos 0xDD durante 60 segundos (Fase %d)...\n",
            fase + 1
        );
        tiempoInicioEsperaDD = millis();
        int ddResponsesProcessedThisPhase = 0;

        while (millis() - tiempoInicioEsperaDD < 61000) { //61000

            // Animación visual de puntos girando (no bloqueante)
            if (millis() - lastAnimFrameTime >= 33) {
                drawLoadingModalFrame(getTranslation("UPDATING"), frameCountAnim++);
                lastAnimFrameTime = millis();
            }

            if (esperar_respuesta(50)) {
                LAST_ENTRY_FRAME_T LEF = extract_info_from_frameIn(uartBuffer);
                frameReceived = false;

                if (LEF.origin == DEFAULT_DEVICE &&
                    LEF.function == F_RETURN_ELEM_SECTOR &&
                    LEF.data.size() >= 6 &&
                    (LEF.data[0] == ELEM_FIRST_SPLIT_ATTACH_REQ ||
                     LEF.data[0] == ELEM_LAST_SPLIT_ATTACH_REQ)
                ) {
                    byte serialRecibidoDD[5];
                    memcpy(serialRecibidoDD, &LEF.data[1], 5);
                    DEBUG__________printf(
                        "Fase %d: Respuesta de 0xDD con SN: %02X%02X%02X%02X%02X\n",
                        fase + 1,
                        serialRecibidoDD[0], serialRecibidoDD[1],
                        serialRecibidoDD[2], serialRecibidoDD[3],
                        serialRecibidoDD[4]
                    );
                    ddResponsesProcessedThisPhase++;

                    // Verificar duplicados globales
                    bool yaProcesadoGlobalmente = false;
                    for (const auto& s_arr : serialesDDProcesadosGlobalmente) {
                        if (memcmp(s_arr.data(), serialRecibidoDD, 5) == 0) {
                            yaProcesadoGlobalmente = true;
                            break;
                        }
                    }
                    if (yaProcesadoGlobalmente) {
                        DEBUG__________printf(
                            "SN %02X... de 0xDD ya fue procesado. Ignorando.\n",
                            serialRecibidoDD[0]
                        );
                        continue;
                    }

                    bool reasignacionExitosaEsteDD = false;

                    // Si ya existe en SPIFFS, reafirmar ID
                    if (serialExistsInSPIFFS(serialRecibidoDD)) {
                        byte idEnSpiffs = getIdFromSPIFFS(serialRecibidoDD);
                        if (idEnSpiffs >= 1 && idEnSpiffs <= 32) {
                            DEBUG__________printf(
                                "Fase %d: SN %02X... ya existe con ID 0x%02X. Reafirmando.\n",
                                fase + 1, serialRecibidoDD[0], idEnSpiffs
                            );
                            send_frame(frameMaker_SET_ELEM_ID(
                                DEFAULT_BOTONERA, DEFAULT_DEVICE, idEnSpiffs
                            ));
                            delay(500);
                            if (confirmarCambioIDConSerial(idEnSpiffs, serialRecibidoDD)){
                                DEBUG__________printf(
                                    "Fase %d: Confirmado: 0xDD restaurado a ID 0x%02X.\n",
                                    fase + 1, idEnSpiffs
                                );
                                listaIDsOcupadasScanActual[idEnSpiffs - 1] = true;
                                reasignacionExitosaEsteDD = true;
                            } else {
                                DEBUG__________ln("❌ Reafirmación fallida: la nueva ID no corresponde al serial esperado");
                            }
                        } else {
                            DEBUG__________printf(
                                "Fase %d: ID en SPIFFS no válida (0x%02X). Buscando libre.\n",
                                fase + 1, idEnSpiffs
                            );
                        }
                    }

                    // Si no se reafirmó, asignar primer ID libre
                    if (!reasignacionExitosaEsteDD) {
                        byte idLibre = buscarPrimerIDLibre(listaIDsOcupadasScanActual);
                        if (idLibre != 0xFF && idLibre != 0x00 && idLibre != DEFAULT_DEVICE) {
                            DEBUG__________printf(
                                "Fase %d: Asignando ID 0x%02X al 0xDD.\n",
                                fase + 1, idLibre
                            );
                            send_frame(frameMaker_SET_ELEM_ID(
                                DEFAULT_BOTONERA, DEFAULT_DEVICE, idLibre
                            ));
                            delay(250);
                            if (confirmarCambioIDConSerial(idLibre, serialRecibidoDD)) {
                                DEBUG__________printf(
                                    "Fase %d: Confirmado: 0xDD ahora en ID 0x%02X.\n",
                                    fase + 1, idLibre
                                );
                                listaIDsOcupadasScanActual[idLibre - 1] = true;
                                reasignacionExitosaEsteDD = true;
                            } else {
                                DEBUG__________printf(
                                    "Fase %d: ADVERTENCIA: No se pudo confirmar cambio.\n",
                                    fase + 1
                                );
                            }
                        } else {
                            DEBUG__________printf(
                                "Fase %d: ERROR: No hay ID libre válida.\n",
                                fase + 1
                            );
                        }
                    }

                    if (reasignacionExitosaEsteDD) {
                        seProcesoUnDDYRequiereReinicio = true;
                        std::array<byte, 5> s_arr_mem;
                        memcpy(s_arr_mem.data(), serialRecibidoDD, 5);
                        serialesDDProcesadosGlobalmente.push_back(s_arr_mem);
                    }
                }
            }
            delay(10);
        }  // while 60 s por fase

        DEBUG__________printf(
            "--- Fin de la ventana de 30 segundos para dispositivos 0xDD (Fase %d) ---\n",
            fase + 1
        );
        if (fase == 0 && !seProcesoUnDDYRequiereReinicio && ddResponsesProcessedThisPhase == 0) {
            DEBUG__________ln("Fase 1 no procesó ningún DD. Saltando Fase 2 opcionalmente.");
        }
    }  // for fases

    if (seProcesoUnDDYRequiereReinicio) {
        DEBUG__________ln("Se procesaron 0xDD que requieren reinicio. Reiniciando escaneo completo...");
        goto inicio_escanear_sala_completo;
    } else {
        DEBUG__________ln("No hubo 0xDD que requieran reinicio.");
    }

    loadElementsFromSPIFFS();
    mostrarMensajeTemporal(2, 3000);
    DEBUG__________ln("=== ✅ FIN ESCANEO DE SALA COMPLETO (SIN REINICIO POR 0xDD) ✅ ===");
    
}


// Función para mostrar texto multilínea ajustado al ancho máximo sin romper palabras

void BOTONERA_::iniciarEscaneoElemento(const char* mensajeInicial) {
    tft.fillScreen(TFT_BLACK);      // limpia TODO (solo aquí)
    //dibujarMarco(TFT_WHITE);        // marco fijo

    // muestra MULTILÍNEA (nombre + ID)
    mostrarTextoAjustado(tft,
                         mensajeInicial,
                         64,   // centro X
                         30,   // Y inicial
                         120); // ancho máximo de texto
    delay(100);
}

TFT_eSprite barraSprite = TFT_eSprite(&tft); 

void BOTONERA_::actualizarBarraProgreso(int pasoActual,
                                        int pasosTotales,
                                        const char* etiqueta)
{
    const int spriteX = 5;
    const int spriteY = 40;  // Y donde comienza tu área visual original
    const int anchoSprite = 118;
    const int altoSprite  = 60;

    const int xBarra    = 9; //14 orig
    const int yBarra    = 20; // relativo al sprite
    const int anchoMax  = 100;
    const int altoBarra = 10;
    const int padding   = 4;

    barraSprite.createSprite(anchoSprite, altoSprite);
    barraSprite.fillSprite(TFT_BLACK);
    //dibujarMarco(TFT_WHITE);

    barraSprite.setTextFont(2);
    barraSprite.setTextColor(TFT_WHITE, TFT_BLACK);

    // Etiqueta (arriba de la barra)
    if (etiqueta != nullptr) {
        barraSprite.setTextDatum(BC_DATUM);
        barraSprite.drawString(etiqueta, anchoSprite / 2, yBarra - padding);
    }

    // Fondo de la barra
    barraSprite.fillRoundRect(xBarra, yBarra, anchoMax, altoBarra, 5, TFT_DARKGREY);

    // Barra de progreso
    int pixProg = 0;
    if (pasosTotales > 0)
        pixProg = (int)(anchoMax * max(0, pasoActual - 1) / (float)pasosTotales);

    pixProg = max(0, min(pixProg, anchoMax));
    barraSprite.fillRoundRect(xBarra, yBarra, pixProg, altoBarra, 5, TFT_BLUE);

    // Porcentaje debajo
    char bufPct[8];
    int pct = 0;
    if (pasosTotales > 0)
        pct = (int)(max(0, pasoActual - 1) * 100.0f / pasosTotales);

    snprintf(bufPct, sizeof(bufPct), "%d%%", pct);
    barraSprite.setTextDatum(TC_DATUM);
    barraSprite.drawString(bufPct, xBarra + anchoMax / 2, yBarra + altoBarra + 12);

    // Mostrar sprite en la posición original
    barraSprite.pushSprite(spriteX, spriteY);

    barraSprite.deleteSprite();
}

void BOTONERA_::actualizarBarraProgreso2(int pasoActual,
                                         int pasosTotales,
                                         const char* etiqueta)
{
    // ───────────────── Estilos/lienzo (como showMessageWithProgress) ───────────────
    const int W = tft.width();    // 128
    const int H = tft.height();   // 128

    // Área de contenido (antes “card”); ya SIN marco
    const int cardW = 112;
    const int cardH = 96;
    const int cardX = (W - cardW) / 2; // 8
    const int cardY = (H - cardH) / 2; // 16

    // Cabecera
    const int lineHeight = 20;
    const int textX = cardX + 12;
    const int textY = cardY + 14;

    uiSprite.setTextDatum(TL_DATUM);
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextSize(1);

    // uiSprite.setTextDatum(TL_DATUM);
    // uiSprite.setFreeFont(nullptr);
    // uiSprite.setTextFont(1);  // 6×8 pixeles
    // uiSprite.setTextSize(1);


    // ───────────────── Geometría nueva: sin EQ, barra más ancha ───────────────────
    const int contentX = cardX + 12;
    const int contentW = cardW - 24;

    // Barra centrada verticalmente y MÁS ancha/“gruesa”
    const int barH = 14;                                // ↑ grosor (ajustable)
    const int barX = contentX;                          // ocupa todo el ancho útil
    const int barW = contentW;
    const int barY = cardY + (cardH/2) - (barH/2);      // centrada vertical

    // Porcentaje debajo y centrado respecto a la barra
    const int pctYOffset = 10;                          // separación (ajustable)
    const int pctY = barY + barH + pctYOffset;

    // ───────────────── Estado animación / suavizado ───────────────────────────────
    static bool     s_init=false;
    static uint32_t s_lastMs=0;
    static float    s_progSmooth=0.0f;
    static int      s_frameCount=0;

    if (!s_init) { s_init=true; s_lastMs=millis(); s_frameCount=0; }

    const uint32_t now = millis();
    float dt = (now - s_lastMs)/1000.0f; if (dt < 0) dt = 0;
    s_lastMs = now; s_frameCount++;

    float progTarget = 0.0f;
    if (pasosTotales > 0) {
        int pasosClamped = max(0, pasoActual - 1);
        pasosClamped = min(pasosClamped, pasosTotales);
        progTarget = (float)pasosClamped / (float)pasosTotales;
    }
    const float k = 8.0f;
    s_progSmooth += (progTarget - s_progSmooth) * min(1.0f, k*dt);
    s_progSmooth = constrain(s_progSmooth, 0.0f, 1.0f);
    const int progW = (int)(barW * s_progSmooth + 0.5f);

    // ───────────────── Dibujo ─────────────────────────────────────────────────────
    uiSprite.fillSprite(BACKGROUND_COLOR);

    // Cabecera (opcional)
    if (etiqueta) {
    const int headerMaxW = cardW - 16; // margen lateral de 12 px por cada lado

    uiSprite.setTextDatum(TL_DATUM);

    // 1) Fuente normal grande
    uiSprite.setFreeFont(&FreeSansBold9pt7b);
    uiSprite.setTextSize(1);

    int anchoTexto = uiSprite.textWidth(etiqueta);

    if (anchoTexto > headerMaxW) {
        // 2) Cambiar a fuente más pequeña si no cabe
        uiSprite.setFreeFont(nullptr); // salir de FreeFont
        uiSprite.setTextFont(2);       // fuente integrada 16 px alto
        uiSprite.setTextSize(1);

        // ⚠️ opcional: volver a medir para comprobar que ahora sí cabe
        // y si aún no cabe, usar Font 1 o recortar con "..."
    }

    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString(etiqueta, textX, textY);
    }
    // Separador sutil bajo cabecera (opcional, se puede retirar)
    const int sepY = textY + lineHeight + 2;
    if (sepY < barY - 14) uiSprite.drawFastHLine(cardX + 10, sepY, cardW - 20, TEXT_COLOR);

    // Pista y progreso (barber-pole)
    uiSprite.drawRoundRect(barX - 1, barY - 1, barW + 2, barH + 2, 4, TEXT_COLOR);
    if (progW > 0) {
        uiSprite.fillRoundRect(barX, barY, progW, barH, 4, TFT_CYAN);
        const int stripeStep = 6;
        const int offset = s_frameCount % stripeStep;
        for (int sx = -barH + offset; sx < progW; sx += stripeStep) {
            int x1 = barX + sx,       y1 = barY;
            int x2 = barX + sx + barH, y2 = barY + barH - 1;
            if (x2 > barX + progW) { int dx = x2 - (barX + progW); x2 -= dx; y2 -= dx; }
            if (x1 < barX)         { int dx = barX - x1;          x1 += dx; y1 += dx; }
            if (x1 <= x2) uiSprite.drawLine(x1, y1, x2, y2, BACKGROUND_COLOR);
        }
    }

    // Porcentaje centrado debajo
    char pctStr[8];
    int percent = (int)(s_progSmooth * 100.0f + 0.5f);
    snprintf(pctStr, sizeof(pctStr), "%d%%", percent);
    int pctW = uiSprite.textWidth(pctStr);
    int pctX = barX + (barW/2) - (pctW/2);
    uiSprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    uiSprite.drawString(pctStr, pctX, pctY);

    uiSprite.pushSprite(0, 0);
}
