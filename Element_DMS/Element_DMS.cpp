#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Element_DMS/Element_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <icons_64x64_DMS/icons_64x64_DMS.h>
#include <WiFi.h>
#include <map>
#include <algorithm>

extern COLORHANDLER_ colorHandler;


void ELEMENT_::begin() {

    Serial.begin(115200);

    configurar_RF(RF_BAUD_RATE);
    delay(100);
    #ifdef PLAYER
        Serial2.begin(9600, SERIAL_8N1, 8, 9);
    #endif
    pinMode (0, INPUT_PULLUP);
    delay(100);
    Serial1.onReceive(onUartInterrupt);
    //digitalWrite(RF_CONFIG_PIN, HIGH);
    delay(100);
    if(!SPIFFS.begin()){
        #ifdef DEBUG
            DEBUG__________ln("Error al montar SPIFFS");
        #endif
    } else {
        #ifdef DEBUG
            //DEBUG__________ln("SPIFFS montado correctamente");
        #endif
    }
    delay(100);    
}

ELEMENT_::Statistics ELEMENT_::calculate_statistics() {
    Statistics stats;
    std::map<int, unsigned long> colorDurations;
    std::map<int, unsigned long> patternDurations;
    std::map<int, unsigned long> modeDurations;

    for (const auto& entry : eventLog) {
        if (entry.eventType == EV_END) {
            stats.totalSessionTime += entry.duration;
        } else if (entry.eventType == EV_COLOR_CHANGE && entry.duration > 0) {
            colorDurations[entry.value] += entry.duration;
        } else if (entry.eventType == EV_PATTERN_CHANGE && entry.duration > 0) {
            patternDurations[entry.value] += entry.duration;
        } else if (entry.eventType == EV_MODE_CHANGE && entry.duration > 0) {
            modeDurations[entry.value] += entry.duration;
        }
    }
    stats.accumulatedColorDurations = colorDurations;
    stats.accumulatedPatternDurations = patternDurations;
    stats.accumulatedModeDurations = modeDurations;

    return stats;
}

int ELEMENT_::find_max_duration_value(const std::map<int, unsigned long>& durationMap) {
    int maxValue = -1;
    unsigned long maxDuration = 0;
    for (const auto& pair : durationMap) {
        if (pair.second > maxDuration) {
            maxDuration = pair.second;
            maxValue = pair.first;
        }
    }
    return maxValue;
}


bool ELEMENT_::save_statistics(const Statistics& stats) {
    #ifdef DEBUG
        DEBUG__________ln("Guardando estadísticas en SPIFFS...");
    #endif
    File file = SPIFFS.open(ELEMENT_STATISTICS_FILE_PATH, FILE_WRITE);
    if (!file) {
        #ifdef DEBUG
            DEBUG__________ln("- Error al abrir el archivo de estadísticas para guardar");
        #endif
        return false;
    }

    file.println("total_session_time=" + String(stats.totalSessionTime));
    file.println("most_used_color=" + String(stats.mostUsedColor));
    file.println("most_used_pattern=" + String(stats.mostUsedPattern));
    file.println("most_used_mode=" + String(stats.mostUsedMode));
    file.println("total_sessions=" + String(stats.totalSessions)); // ADDED: Save total sessions

    file.close();
#ifdef DEBUG
    DEBUG__________ln("- Estadísticas guardadas en SPIFFS");
#endif
    return true;
}

void ELEMENT_::calculate_and_save_statistics() {
    Statistics currentSessionStats = calculate_statistics();
    Statistics accumulatedStats = load_statistics();

    accumulatedStats.totalSessionTime += currentSessionStats.totalSessionTime;

    for (const auto& pair : currentSessionStats.accumulatedColorDurations) {
        accumulatedStats.accumulatedColorDurations[pair.first] += pair.second;
    }
    for (const auto& pair : currentSessionStats.accumulatedPatternDurations) {
        accumulatedStats.accumulatedPatternDurations[pair.first] += pair.second;
    }
    for (const auto& pair : currentSessionStats.accumulatedModeDurations) {
        accumulatedStats.accumulatedModeDurations[pair.first] += pair.second;
    }
    accumulatedStats.mostUsedColor = find_max_duration_value(accumulatedStats.accumulatedColorDurations);
    accumulatedStats.mostUsedPattern = find_max_duration_value(accumulatedStats.accumulatedPatternDurations); // ADDED: Calculate most used pattern
    accumulatedStats.mostUsedMode = find_max_duration_value(accumulatedStats.accumulatedModeDurations);

    accumulatedStats.totalSessions++; // ADDED: Increment total sessions count

    save_statistics(accumulatedStats);
}

ELEMENT_::Statistics ELEMENT_::load_statistics() {
    Statistics stats;
    File file = SPIFFS.open(ELEMENT_STATISTICS_FILE_PATH, FILE_READ);
    if (!file || !file.available()) {
#ifdef DEBUG
        DEBUG__________ln("- Archivo de estadísticas no encontrado o vacío, usando valores por defecto.");
#endif
        return stats;
    }

    while (file.available()) {
        String line = file.readStringUntil('\n');
        line.trim();
        int separatorIndex = line.indexOf('=');
        if (separatorIndex > 0) {
            String key = line.substring(0, separatorIndex);
            String valueStr = line.substring(separatorIndex + 1);
            if (key == "total_session_time") {
                stats.totalSessionTime = valueStr.toInt();
            } else if (key == "most_used_color") {
                stats.mostUsedColor = valueStr.toInt();
            } else if (key == "most_used_pattern") {
                stats.mostUsedPattern = valueStr.toInt();
            } else if (key == "most_used_mode") {
                stats.mostUsedMode = valueStr.toInt();
            } else if (key == "total_sessions") {
                stats.totalSessions = valueStr.toInt(); // ADDED: Load total sessions
            }
        }
    }
    file.close();

    stats.accumulatedColorDurations = std::map<int, unsigned long>();
    stats.accumulatedPatternDurations = std::map<int, unsigned long>();
    stats.accumulatedModeDurations = std::map<int, unsigned long>();

#ifdef DEBUG
    DEBUG__________ln("- Estadísticas cargadas desde SPIFFS");
#endif
    return stats;
}

String ELEMENT_::get_statistic(String statisticName) {
    Statistics stats = load_statistics();
    if (statisticName == "total_session_time") {
        return String(stats.totalSessionTime);
    } else if (statisticName == "most_used_color") {
        return String(stats.mostUsedColor);
    } else if (statisticName == "most_used_pattern") {
        return String(stats.mostUsedPattern); // ADDED: Get most used pattern
    } else if (statisticName == "most_used_mode") {
        return String(stats.mostUsedMode);
    } else if (statisticName == "total_sessions") {
        return String(stats.totalSessions); // ADDED: Get total sessions
    }
     else {
        return ""; // Statistic not found
    }
}




bool ELEMENT_::save_register() {
    #ifdef DEBUG
        DEBUG__________ln("Guardando registro de eventos en SPIFFS (circular overwrite)...");
    #endif

    String newSessionData = "";
    if (!eventLog.empty()){
        newSessionData += "-----SESSION_START-----\n";
        for (const auto& entry : eventLog) {
            newSessionData += String(entry.eventType) + "," + String(entry.value) + "," + String(entry.timestamp) + "," + String(entry.duration) + "\n";
        }
        newSessionData += "-----SESSION_END-----\n";
    } else {
        #ifdef DEBUG
            DEBUG__________ln("- No hay eventos para guardar.");
        #endif
        return true; // No error, just nothing to save
    }

    size_t newSessionDataSize = newSessionData.length();
    size_t maxFileSize = MAX_EVENT_REGISTER_FILE_SIZE;
    String existingContent = "";
    size_t existingFileSize = 0;


    File readFile = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
    if(readFile){
        existingContent = readFile.readString();
        existingFileSize = readFile.size();
        readFile.close();
    }


    String fileContentToWrite = newSessionData; // Start with the new session data
    size_t availableSpace = maxFileSize - newSessionDataSize;

    if (existingFileSize > 0 && availableSpace > 0) {
        if (existingFileSize > availableSpace) {
            String contentToKeep = existingContent.substring(existingContent.length() - availableSpace);
            fileContentToWrite += contentToKeep; // Append the most recent part of the old content, using fileContentToWrite
             #ifdef DEBUG
                DEBUG__________("- Límite de tamaño alcanzado. Sobreescribiendo datos antiguos. Bytes mantenidos: ");
                DEBUG__________ln(contentToKeep.length());
            #endif
        } else {
             fileContentToWrite += existingContent; // Append all existing content if it fits
        }
    }


    File writeFile = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_WRITE);
    writeFile.print(fileContentToWrite); // <-- ¡Usa fileContentToWrite aquí!
    writeFile.close();


    #ifdef DEBUG
        DEBUG__________ln("- Registro de eventos guardado en SPIFFS (circular overwrite)");
    #endif
    eventLog.clear();
    return true;
}


String ELEMENT_::padStringRight(String text, int width) {
    String paddedText = text;
    if (paddedText.length() < width) {
        for (int i = 0; i < width - paddedText.length(); i++) {
            paddedText += " ";
        }
    }
    return paddedText;
}

void ELEMENT_::print_stats_file_RF(){
    Serial1.println("**********************************");
    Serial1.println("\n-- ESTADISTICAS DE ELEMENTO --\n");

    unsigned int numSessions= get_total_sessions();
    Serial1.println("Numero de sesiones: " +String(numSessions));

    String sessionsTime= format_millis_time(get_total_session_time());
    Serial1.println("Duracion total de sesiones: " + sessionsTime);

    byte mostMode= get_most_used_mode();
    Serial1.println("Modo mas usado: " +String(mostMode));

    byte mostColor= get_most_used_color();
    Serial1.println("Color mas usado: " +String(mostColor));

    Serial1.println("\n");
}

void ELEMENT_::print_event_register_file_RF() {
    #ifdef DEBUG
        Serial1.println("\n--- Contenido del archivo de registro de eventos en SPIFFS ---"); // Cambiado a Serial1.println
    #endif
    File file = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
    if(!file){
        #ifdef DEBUG
            Serial1.println("- Error al abrir el archivo de registro de eventos para lectura."); // Cambiado a Serial1.println
        #endif
        return;
    }

    if(!file.available()){
            Serial1.println("- El archivo de registro de eventos está vacío."); // Cambiado a Serial1.println
    } else {
            Serial1.print(padStringRight("Evento", 25)); // Header for Event Type - adjusted width
            Serial1.print(padStringRight("Valor", 10));  // Header for Value - adjusted width
            Serial1.print(padStringRight("Timestamp", 12)); // Header for Timestamp - adjusted width
            Serial1.println("Duración"); // Header for Duration - no padding, last column
        while(file.available()){
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.startsWith("-----SESSION_START-----") || line.startsWith("-----SESSION_END-----")) {
                    Serial1.println(line); // Imprimir delimitadores de sesión sin formato
            } else {
                // Procesar líneas de eventos
                int commaIndex1 = line.indexOf(',');
                int commaIndex2 = line.indexOf(',', commaIndex1 + 1);
                int commaIndex3 = line.indexOf(',', commaIndex2 + 1);

                if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1) {
                    String eventTypeStr = line.substring(0, commaIndex1);
                    String valueStr = line.substring(commaIndex1 + 1, commaIndex2);
                    String timestampStr = line.substring(commaIndex2 + 1, commaIndex3);
                    String durationStr = line.substring(commaIndex3 + 1);

                    EVENT_TYPE eventType = static_cast<EVENT_TYPE>(eventTypeStr.toInt()); // Convertir String a EVENT_TYPE
                    int value = valueStr.toInt();
                    unsigned long timestamp = timestampStr.toInt();
                    unsigned long duration = durationStr.toInt();
                        Serial1.print(padStringRight(get_event_type_name(eventType), 25)); // Event Type, padded to width 25
                        Serial1.print(padStringRight(String(value), 10)); // Value, padded to width 10
                        Serial1.print(padStringRight(format_millis_time(timestamp), 12)); // Timestamp, padded to width 12
                        if (duration != -1) {
                            Serial1.println(format_millis_time(duration)); // Duration, no padding, last column - println for newline
                        } else {
                            Serial1.println(""); // Newline for events without duration
                        }

                } else {

                        Serial1.println("Línea de evento con formato incorrecto: " + line); // Debug para líneas inesperadas

                }
            }
        }
    }

    file.close();
    Serial1.println("--- Fin del contenido del archivo de registro de eventos ---");

}


void ELEMENT_::print_event_register_file() {
    #ifdef DEBUG
        DEBUG__________ln("--- Contenido del archivo de registro de eventos en SPIFFS ---");
    #endif
    File file = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
    if(!file){
        #ifdef DEBUG
            DEBUG__________ln("- Error al abrir el archivo de registro de eventos para lectura.");
        #endif
        return;
    }

    if(!file.available()){
        #ifdef DEBUG
            DEBUG__________ln("- El archivo de registro de eventos está vacío.");
        #endif
    } else {
        while(file.available()){
            String line = file.readStringUntil('\n');
            line.trim();
            if (line.startsWith("-----SESSION_START-----") || line.startsWith("-----SESSION_END-----")) {
                #ifdef DEBUG
                    DEBUG__________ln(line); // Imprimir delimitadores de sesión sin formato
                #endif
            } else {
                // Procesar líneas de eventos
                int commaIndex1 = line.indexOf(',');
                int commaIndex2 = line.indexOf(',', commaIndex1 + 1);
                int commaIndex3 = line.indexOf(',', commaIndex2 + 1);

                if (commaIndex1 != -1 && commaIndex2 != -1 && commaIndex3 != -1) {
                    String eventTypeStr = line.substring(0, commaIndex1);
                    String valueStr = line.substring(commaIndex1 + 1, commaIndex2);
                    String timestampStr = line.substring(commaIndex2 + 1, commaIndex3);
                    String durationStr = line.substring(commaIndex3 + 1);

                    EVENT_TYPE eventType = static_cast<EVENT_TYPE>(eventTypeStr.toInt()); // Convertir String a EVENT_TYPE
                    int value = valueStr.toInt();
                    unsigned long timestamp = timestampStr.toInt();
                    unsigned long duration = durationStr.toInt();

                    #ifdef DEBUG

                        DEBUG__________(padStringRight(get_event_type_name(eventType), 25));
                        DEBUG__________(padStringRight(String(value), 10));
                        DEBUG__________(padStringRight(format_millis_time(timestamp), 12)); // Imprimir timestamp formateado
                        if (duration != -1) {
                            DEBUG__________ln(format_millis_time(duration)); // Imprimir duración formateada
                        }
                        DEBUG__________ln("");
                    #endif
                } else {
                    #ifdef DEBUG
                        DEBUG__________ln("Línea de evento con formato incorrecto: " + line); // Debug para líneas inesperadas
                    #endif
                }
            }
        }
    }
    file.close();
    #ifdef DEBUG
        DEBUG__________ln("--- Fin del contenido del archivo de registro de eventos ---");
    #endif
}

void ELEMENT_::print_statistics_file() {
    #ifdef DEBUG
        DEBUG__________ln("--- Contenido del archivo de estadísticas en SPIFFS ---");
    #endif
    File file = SPIFFS.open(ELEMENT_STATISTICS_FILE_PATH, FILE_READ);
    if(!file){
        #ifdef DEBUG
            DEBUG__________ln("- Error al abrir el archivo de estadísticas para lectura.");
        #endif
        return;
    }
    if(!file.available()){
        #ifdef DEBUG
            DEBUG__________ln("- El archivo de estadísticas está vacío.");
        #endif
    } else {
        while(file.available()){
            String line = file.readStringUntil('\n');
            #ifdef DEBUG
                DEBUG__________ln(line);
            #endif
        }
    }
    file.close();
    #ifdef DEBUG
        DEBUG__________ln("--- Fin del contenido del archivo de estadísticas ---");
    #endif
}

void ELEMENT_::event_register(EVENT_TYPE event, int value){
    unsigned long currentTimestamp = millis();

        if (event == EV_START) {
            if (!sessionActive) {
                sessionActive = true;
                sessionStartTime = currentTimestamp;
                log_event(event, value, currentTimestamp, 0); // Duration 0 para EV_START
                #ifdef DEBUG
                    DEBUG__________ln("Sesión iniciada");
                #endif
            } else {
                #ifdef DEBUG
                    DEBUG__________ln("Advertencia: EV_START recibido cuando la sesión ya estaba activa.");
                #endif
            }
        } else if (event == EV_END) {
            if (sessionActive) {
                sessionActive = false;
                unsigned long sessionDuration = currentTimestamp - sessionStartTime;
                log_event(event, value, currentTimestamp, sessionDuration);
                #ifdef DEBUG
                    DEBUG__________("Sesión finalizada. Duración: ");
                    DEBUG__________(sessionDuration / 1000.0); // en segundos
                    DEBUG__________ln(" segundos");
                #endif
                event_register(EV_MODE_CHANGE, DEFAULT_BASIC_MODE);
                event_register(EV_PATTERN_CHANGE, 1);  // ADDED: default pattern to pattern 1 on session end
                event_register(EV_COLOR_CHANGE, BLACK);
                calculate_and_save_statistics();
            } else {
                #ifdef DEBUG
                    DEBUG__________ln("Advertencia: EV_END recibido sin sesión activa.");
                #endif
            }
        } else if (event == EV_MODE_CHANGE || event == EV_COLOR_CHANGE || event == EV_PATTERN_CHANGE) {
            handle_timed_event(event, value, currentTimestamp);
        } else if (event == EV_FLAG_CHANGE || event == EV_ID_CHANGE) {
            log_event(event, value, currentTimestamp, -1); // Duration -1 para eventos sin tiempo
            #ifdef DEBUG
                DEBUG__________("Evento registrado: ");
                DEBUG__________(get_event_type_name(event));
                DEBUG__________(", Valor: ");
                DEBUG__________ln(value);
            #endif
        }
}

int ELEMENT_::get_most_used_color() {
    Statistics stats = load_statistics();
    return stats.mostUsedColor;
}

int ELEMENT_::get_most_used_mode() {
    Statistics stats = load_statistics();
    return stats.mostUsedMode;
}

int ELEMENT_::get_most_used_pattern() { // ADDED: getter for most used pattern
    Statistics stats = load_statistics();
    return stats.mostUsedPattern;
}

unsigned int ELEMENT_::get_total_sessions() { // ADDED: getter for total sessions
    Statistics stats = load_statistics();
    return stats.totalSessions;
}


unsigned long ELEMENT_::get_total_session_time() {
    Statistics stats = load_statistics();
    return stats.totalSessionTime;
}

String ELEMENT_::format_millis_time(unsigned long millisec) {
    time_t tiempo_segundos = millisec / 1000; // Convertir milisegundos a segundos
    int horas = (tiempo_segundos % 86400L) / 3600;
    int minutos = (tiempo_segundos % 3600) / 60;
    int segundos = tiempo_segundos % 60;
    String formattedTime = "";
    if (horas < 10) formattedTime += "0";
    formattedTime += String(horas) + ":";
    if (minutos < 10) formattedTime += "0";
    formattedTime += String(minutos) + ":";
    if (segundos < 10) formattedTime += "0";
    formattedTime += String(segundos);
    return formattedTime;
}


void ELEMENT_::handle_timed_event(EVENT_TYPE event, int value, unsigned long current_timestamp) {
    if (lastTimedEventValue != -1 && lastTimedEventType == event) {
        if (lastTimedEventValue != value) {
            unsigned long duration = current_timestamp - lastTimedEventStartTime; // Corrected: current_timestamp
            update_last_timed_event_duration(duration);
            log_event(event, value, current_timestamp, 0); // Corrected: current_timestamp
            lastTimedEventStartTime = current_timestamp;
            lastTimedEventValue = value;
            lastTimedEventType = event;
            #ifdef DEBUG
                DEBUG__________("Cambio de ");
                DEBUG__________(get_event_type_name(event));
                DEBUG__________(", Valor: ");
                DEBUG__________ln(value);
            #endif

        } else {
             // Mismo valor, no hacemos nada o podríamos registrar que se mantiene el mismo valor (opcional)
             // Para este ejemplo, no registramos eventos con el mismo valor consecutivo.
             #ifdef DEBUG
                 DEBUG__________("Mismo valor para ");
                 DEBUG__________(get_event_type_name(event));
                 DEBUG__________(", Valor: ");
                 DEBUG__________ln(value);
             #endif
        }
    } else {
        if (lastTimedEventValue != -1) {
            // Si había un evento temporizado previo de *otro* tipo, finalizamos su duración.
            unsigned long duration = current_timestamp - lastTimedEventStartTime; // Corrected: current_timestamp
            update_last_timed_event_duration(duration);
        }
        log_event(event, value, current_timestamp, 0); // Corrected: current_timestamp
        lastTimedEventStartTime = current_timestamp;
        lastTimedEventValue = value;
        lastTimedEventType = event;
        #ifdef DEBUG
            DEBUG__________("Inicio de ");
            DEBUG__________(get_event_type_name(event));
            DEBUG__________(", Valor: ");
            DEBUG__________ln(value);
        #endif
    }
}

void ELEMENT_::update_last_timed_event_duration(unsigned long duration) {
    if (!eventLog.empty()) {
        for (int i = eventLog.size() - 1; i >= 0; --i) {
            if (eventLog[i].eventType == lastTimedEventType && eventLog[i].duration == 0) { // Encuentra el último evento sin duración
                eventLog[i].duration = duration;
                #ifdef DEBUG
                    DEBUG__________("Duración de ");
                    DEBUG__________(get_event_type_name(lastTimedEventType));
                    DEBUG__________(" (valor ");
                    DEBUG__________(lastTimedEventValue);
                    DEBUG__________("): ");
                    DEBUG__________(duration / 1000.0); // en segundos
                    DEBUG__________ln(" segundos");
                #endif
                break; // Asumimos que solo hay un evento sin duración pendiente para el tipo
            }
        }
    }
    lastTimedEventValue = -1; // Reseteamos para el nuevo evento
}

void ELEMENT_::log_event(EVENT_TYPE event, int value, unsigned long timestamp, unsigned long duration) {
    EventLogEntry entry;
        entry.eventType = event;
        entry.value = value;
        entry.timestamp = timestamp;
        entry.duration = duration;
        eventLog.push_back(entry);
}

void ELEMENT_::print_event_log() {
    #ifdef DEBUG
    DEBUG__________ln("--- Registro de Eventos ---");
    for (const auto& entry : eventLog) {
        DEBUG__________("Evento: ");
        DEBUG__________(get_event_type_name(entry.eventType));
        DEBUG__________(", Valor: ");
        DEBUG__________(entry.value);
        DEBUG__________(", Timestamp: ");
        DEBUG__________(entry.timestamp);
        if (entry.duration != -1) {
            DEBUG__________(", Duración: ");
            DEBUG__________(entry.duration / 1000.0); // en segundos
            DEBUG__________(" segundos");
        }
        DEBUG__________ln("");
    }
    DEBUG__________ln("--- Fin del Registro ---");
    #endif
}

const char* ELEMENT_::get_event_type_name(EVENT_TYPE event) {
    switch (event) {
        case EV_START:           return "INICIO DE SESION";
        case EV_END:             return "FIN DE SESION";
        case EV_MODE_CHANGE:     return "CAMBIO DE MODO";
        case EV_COLOR_CHANGE:    return "CAMBIO DE COLOR";
        case EV_FLAG_CHANGE:     return "CAMBIO EN FLAGS";
        case EV_PATTERN_CHANGE:  return "CAMBIO DE PATRON";
        case EV_ID_CHANGE:       return "CAMBIO DE ID";
        default:                 return "UNKNOWN";
    }
}

String ELEMENT_::get_mode_name(byte numMode) {

    constexpr byte modeStringIds[] = { 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31, 33};
    if (numMode < sizeof(modeStringIds) / sizeof(modeStringIds[0])) return get_string_from_info_DB(modeStringIds[numMode], 1);
    else return "";  
}




String ELEMENT_::get_serial_from_file(){
    String serial= "";
    
    File file = SPIFFS.open(ELEMENT_SERIALNUM_FILE_PATH, "r");
    if (!file) {
                                                                                        #ifdef DEBUG
                                                                                        DEBUG__________ln("Error al abrir el archivo");
                                                                                        #endif
        return serial;
    }
    serial = file.readStringUntil('\n');
    file.close();
    serial.trim();
    
    return serial;
}


byte ELEMENT_::get_ID_from_file(){
   
    File file = SPIFFS.open(ELEMENT_ID_FILE_PATH, "r");
    if (!file) {
                                                                                    #ifdef DEBUG
                                                                                        DEBUG__________ln("Error al abrir el archivo");
                                                                                    #endif
        return -1;
    }
    int value = file.parseInt();
    file.close();
    return (byte)value;
}

void ELEMENT_::set_ID_to_file(byte IDin){
    
    File file = SPIFFS.open(ELEMENT_ID_FILE_PATH, "w");
    if (!file) {
                                                                                #ifdef DEBUG
                                                                                    DEBUG__________ln("Error al abrir el archivo");
                                                                                #endif
        return;
    }
    file.println(IDin);
                                                                                #ifdef DEBUG
                                                                                DEBUG__________ln("ID guardado en FILE ID: " + String(IDin));
                                                                                #endif
    file.close();
}


void ELEMENT_::set_ID(byte IDin){
    ID= IDin;
    globalID= IDin;
    set_ID_to_file(IDin);

}

byte ELEMENT_::get_ID(){
    return ID;
}





byte ELEMENT_::get_currentMode(){
   
    return currentMode;
}


void ELEMENT_::set_manager(byte managerin){

    exclusiveIDmanager= managerin;
                                                                #ifdef DEBUG
                                                                  DEBUG__________ln("configurado manager exclusivo: " +String(exclusiveIDmanager));
                                                                #endif
}

byte ELEMENT_::get_manager(){
    return exclusiveIDmanager;
}


void ELEMENT_::set_mode(uint8_t mode) {
    currentMode = mode;
                                                                            #ifdef DEBUG
                                                                        DEBUG__________ln("current: " +String(currentMode));
                                                                        DEBUG__________ln("mode " +String(mode));
                                                                        #endif
}

void ELEMENT_::set_type(byte typein){
    type= typein;
}

byte ELEMENT_::get_type(){
    return type;
}
void ELEMENT_::set_flag(byte flagNum, bool state) {
    if(flagNum < 8) {
        if(state) flag |= (1 << flagNum);  
        else      flag &= ~(1 << flagNum);   // Desactivar  
    }
}

byte ELEMENT_::get_flag(){
    return flag;
}

void ELEMENT_::set_active_pattern(byte patt){

    colorHandler.set_activePattern(patt);
}

void ELEMENT_::configurar_RF(int baudRate) {
    pinMode(RF_CONFIG_PIN, OUTPUT);
    digitalWrite(RF_CONFIG_PIN, LOW);  // Entrar en modo configuración
    delay(50);

    // Intentar comunicarse con ambas velocidades por seguridad
    Serial1.begin(115200, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    byte comandoReset[] = {0xAA, 0xFA, 0xF0};  // Comando para resetear a valores de fábrica
    Serial1.write(comandoReset, sizeof(comandoReset));
    delay(200); // Dar tiempo al módulo para reiniciarse

    // Verificar si responde a 115200
    bool resetConfirmado = false;
    if (Serial1.available()) {
        #ifdef DEBUG
        DEBUG__________ln("Respuesta a 115200 detectada");
        #endif
        resetConfirmado = true;
    } else {
        // Si no responde, intentar con 9600
        Serial1.end();
        Serial1.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
        Serial1.write(comandoReset, sizeof(comandoReset));
        delay(200);
        if (Serial1.available()) {
            #ifdef DEBUG
                //DEBUG__________ln("Respuesta a 9600 detectada");
            #endif
            resetConfirmado = true;
        }
    }

    if (!resetConfirmado) {
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("Error: No se detectó respuesta del módulo RF");
                                                                                                #endif
        digitalWrite(RF_CONFIG_PIN, HIGH);
        return;
    }

    // Configurar comandos según la velocidad deseada
    byte comandoUART[7];
    byte comandoWirelessDataRate[7];

    if (baudRate == 115200) {
                                                                                                #ifdef DEBUG
                                                                                                //DEBUG__________ln("Config UART y Wireless a 115200");
                                                                                                #endif
        comandoUART[0] = 0xAA; comandoUART[1] = 0xFA; comandoUART[2] = 0x1E;
        comandoUART[3] = 0x00; comandoUART[4] = 0x01; comandoUART[5] = 0xC2; comandoUART[6] = 0x00;

        comandoWirelessDataRate[0] = 0xAA; comandoWirelessDataRate[1] = 0xFA; comandoWirelessDataRate[2] = 0xC3;
        comandoWirelessDataRate[3] = 0x00; comandoWirelessDataRate[4] = 0x01; comandoWirelessDataRate[5] = 0xC2; comandoWirelessDataRate[6] = 0x00;
    } else {
                                                                                                #ifdef DEBUG
                                                                                                //DEBUG__________ln("Config UART y Wireless a 9600");
                                                                                                #endif
        comandoUART[0] = 0xAA; comandoUART[1] = 0xFA; comandoUART[2] = 0x1E;
        comandoUART[3] = 0x00; comandoUART[4] = 0x00; comandoUART[5] = 0x25; comandoUART[6] = 0x80;

        comandoWirelessDataRate[0] = 0xAA; comandoWirelessDataRate[1] = 0xFA; comandoWirelessDataRate[2] = 0xC3;
        comandoWirelessDataRate[3] = 0x00; comandoWirelessDataRate[4] = 0x00; comandoWirelessDataRate[5] = 0x25; comandoWirelessDataRate[6] = 0x80;
    }

    // Configurar UART y velocidad inalámbrica
    Serial1.write(comandoUART, sizeof(comandoUART));
    delay(200);
    Serial1.write(comandoWirelessDataRate, sizeof(comandoWirelessDataRate));
    delay(200);

    // Confirmar configuración
    byte comandoLeerConfiguracion[] = {0xAA, 0xFA, 0xE1};
    Serial1.write(comandoLeerConfiguracion, sizeof(comandoLeerConfiguracion));
    delay(200);
                                                                                                        #ifdef DEBUG
                                                                                                        //DEBUG__________ln(" =[Desglosando configuración recibida]=");
                                                                                                        #endif

    // Filtrar valores no relevantes
    while (Serial1.available()) {
        if (Serial1.peek() == 0x4F) {  // ASCII 'O'
            Serial1.read(); // Ignorar 'O'
            if (Serial1.peek() == 0x4B) {  // ASCII 'K'
                Serial1.read(); // Ignorar 'K'
                Serial1.read(); // Ignorar '\r'
                Serial1.read(); // Ignorar '\n'
            }
        } else {
            break;
        }
    }

    if (Serial1.available() >= 13) {  // Verificar que haya suficientes datos para desglosar
        byte frecuencia[4];
        byte velocidad[4];
        byte anchoBanda[2];
        byte desviacionFrecuencia;
        byte potencia;

        for (int i = 0; i < 4; i++) frecuencia[i] = Serial1.read();
        for (int i = 0; i < 4; i++) velocidad[i] = Serial1.read();
        for (int i = 0; i < 2; i++) anchoBanda[i] = Serial1.read();
        desviacionFrecuencia = Serial1.read();
        potencia = Serial1.read();

        uint32_t freq = (frecuencia[0] << 24) | (frecuencia[1] << 16) | (frecuencia[2] << 8) | frecuencia[3];
        uint32_t baudrate = (velocidad[0] << 24) | (velocidad[1] << 16) | (velocidad[2] << 8) | velocidad[3];
        uint16_t bw = (anchoBanda[0] << 8) | anchoBanda[1];
        #ifdef DEBUG
        // DEBUG__________("📡 Frecuencia: ");
        // DEBUG__________(freq);
        // DEBUG__________ln(" Hz");

        // DEBUG__________("⚡ Velocidad inalámbrica: ");
        // DEBUG__________(baudrate);
        // DEBUG__________ln(" bps");

        // DEBUG__________("📶 Ancho de banda: ");
        // DEBUG__________(bw);
        // DEBUG__________ln(" kHz");

        // DEBUG__________("🎛️  Desviación de frecuencia: ");
        // DEBUG__________(desviacionFrecuencia);
        // DEBUG__________ln(" kHz");

        // DEBUG__________("🔋 Potencia de transmisión: ");
        // DEBUG__________(potencia);
        // DEBUG__________ln(" dBm");
        // DEBUG__________ln();
        #endif
    } else {
        #ifdef DEBUG
           // DEBUG__________ln("Error: Datos insuficientes para interpretar la configuración.");
        #endif
    }

    // Salir del modo configuración
    digitalWrite(RF_CONFIG_PIN, HIGH);
    delay(200);

    // Reconfigurar la velocidad UART en el ESP32
    Serial1.end();
    if (baudRate == 115200) {
        #ifdef DEBUG
            //DEBUG__________ln("Config velocidad UART en ESP32 a 115200");
        #endif
        Serial1.begin(115200, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    } else {
        #ifdef DEBUG
            //DEBUG__________ln("Config velocidad UART en ESP32 a 9600");
        #endif
        Serial1.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    }
        #ifdef DEBUG
        //DEBUG__________ln("Configuración completa y módulo reiniciado correctamente.");
        #endif
    delay(10);
}