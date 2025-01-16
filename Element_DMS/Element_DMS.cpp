#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Element_DMS/Element_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <icons_64x64_DMS/icons_64x64_DMS.h>




ELEMENT_::ELEMENT_(){

    lastEventTime = new unsigned long[MAX_EVENTS];
    lastEventValue = new int[MAX_EVENTS];
    
    for(int i = 0; i < MAX_EVENTS; i++) {
        lastEventTime[i] = 0;
        lastEventValue[i] = -1;
    }
}


ELEMENT_::~ELEMENT_() {
    
    delete[] lastEventTime;
    delete[] lastEventValue;
}


void ELEMENT_::begin() {
                                                    #ifdef DEBUG
                                                        Serial.begin(115200);
                                                        Serial.println("ATENCION!!!!!!!!!!!!!!");
                                                    #endif
ELEMENT_::configurar_RF(RF_BAUD_RATE);
delay(100);
#ifdef PLAYER
    Serial2.begin(9600, SERIAL_8N1, 8, 9);
#endif
delay(100);
Serial1.onReceive(onUartInterrupt);
agregar_evento(EV_START, 0);
delay(100);
if (!SPIFFS.begin(true)) {
                                            #ifdef DEBUG
                                                Serial.println("Error al montar SPIFFS.");
                                            #endif
    return;
}
delay(100);

// if (!SPIFFS.exists(ELEMENT_WORKTIME_FILE_PATH))  ELEMENT_::set_workTime(0);
// if (!SPIFFS.exists(ELEMENT_LIFETIME_FILE_PATH))  ELEMENT_::set_lifeTime(0);
// if (!SPIFFS.exists(ELEMENT_SERIALNUM_FILE_PATH)) 
// if (!SPIFFS.exists(ELEMENT_ID_FILE_PATH))       

}

String ELEMENT_::get_word_from_eventNum(int eventNumber){

    String frase;
    switch(eventNumber){
        case 0: frase=   "DISPOSITIVO INICIADO EN MODO BASICO"; break;
        case 1: frase=   "APAGANDO DISPOSITIVO"; break;
        case 2: frase=   "PETICION DE SECTOR";   break;
        case 3: frase=   "CAMBIO DE MODO";       break;
        case 4: frase=   "CAMBIO DE COLOR";      break;
        default: frase=   "ERROR"; break;
    }
    return frase;
}
void   ELEMENT_::print_event_register(){

    File file = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
    if (!file) {
        Serial.println("Error al abrir el archivo para lectura");
        return;
    }
    Serial.println("Contenido del archivo:");
    while (file.available()) {
        Serial.println(file.readStringUntil('\n'));
    }
    file.close();
}




void ELEMENT_::event_register_update(int eventNumber, int eventValue) {
    static int currentLine = 0;  // Mantener el índice global de línea
    static bool isFileInitialized = false;
    // Inicializar SPIFFS si no está iniciado

    unsigned long currentTime = millis() / 1000;
    unsigned long elapsedTime = currentTime - lastEventTime[eventNumber];

    // Abrir el archivo en modo lectura y escritura
    File file = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_APPEND);
    if (!file) {
        Serial.println("Error al abrir el archivo para escritura");
        return;
    }

    // Si es la primera vez, contar las líneas para saber dónde empezar
    if (!isFileInitialized) {
        File tempFile = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
        if (tempFile) {
            int lineCount = 0;
            while (tempFile.available()) {
                tempFile.readStringUntil('\n');
                lineCount++;
            }
            currentLine = lineCount % MAX_REG_EVENTS;
            tempFile.close();
        }
        isFileInitialized = true;
    }

    // Crear un archivo temporal para sobrescribir la línea deseada
    File tempFile = SPIFFS.open("/temp_log.txt", FILE_WRITE);
    File readFile = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);

    int lineCounter = 0;
    while (readFile.available()) {
        String line = readFile.readStringUntil('\n');
        tempFile.println(line);
        lineCounter++;
    }

    // Si el evento es el mismo pero con valor diferente, actualizar la línea anterior
    if (lastEventValue[eventNumber] != -1 && lastEventValue[eventNumber] != eventValue) {
        tempFile.printf("%s -> %d   durante %lu segundos\n", get_word_from_eventNum(eventNumber).c_str(), lastEventValue[eventNumber], elapsedTime);
    }

    // Registrar el nuevo evento
    tempFile.printf("%s -> %d comenzando ahora\n", get_word_from_eventNum(eventNumber).c_str(), eventValue);

    lastEventTime[eventNumber] = currentTime;
    lastEventValue[eventNumber] = eventValue;

    readFile.close();
    tempFile.close();

    // Reemplazar el archivo original
    SPIFFS.remove(ELEMENT_EVENT_REGISTER_FILE_PATH);
    SPIFFS.rename("/temp_log.txt", ELEMENT_EVENT_REGISTER_FILE_PATH);

    Serial.println("Evento registrado correctamente");
}




void ELEMENT_::work_time_handler(byte colorin) {
    if (colorin != 8) {
        if (!workTimerRunning) {  // Usar workTimerRunning en lugar de stopwatchRunning
            start_working_time();
        } else {
            #ifdef DEBUG
                Serial.println("El cronómetro ya está activo.");
            #endif
        }
    } else {
        if (workTimerRunning) {  // Usar workTimerRunning en lugar de stopwatchRunning
            stopAndSave_working_time();
        } else {
            #ifdef DEBUG
                Serial.println("El cronómetro ya está detenido.");
            #endif
        }
    }
}

String ELEMENT_::get_serial_from_file(){
    String serial= "";
    File file = SPIFFS.open(ELEMENT_SERIALNUM_FILE_PATH, "r");
    if (!file) {
                                                                                        #ifdef DEBUG
                                                                                        Serial.println("Error al abrir el archivo");
                                                                                        #endif
        return serial;
    }
    serial = file.readStringUntil('\n');
    file.close();
    serial.trim();
    
    return serial;
}


void ELEMENT_::set_lifeTime(unsigned long lifeTime){
    File file = SPIFFS.open(ELEMENT_LIFETIME_FILE_PATH, "w");
    if (!file) {
                                                                                        #ifdef DEBUG
                                                                                            Serial.println("Error al abrir el archivo lifetime");
                                                                                        #endif
        return;
    }
    file.print(lifeTime);
    file.close();
}

unsigned long ELEMENT_::get_lifeTime(){

    File file = SPIFFS.open(ELEMENT_LIFETIME_FILE_PATH, "r");
    if (!file) {
                                                                                        #ifdef DEBUG
                                                                                            Serial.println("Error al abrir el archivo lifetime");
                                                                                        #endif
        return -1;
    }
    int value = file.parseInt();
    file.close();
    return value;
}

void ELEMENT_::set_workTime(int workTime){
    File file = SPIFFS.open(ELEMENT_WORKTIME_FILE_PATH, "w");
    if (!file) {
                                                                                            #ifdef DEBUG
                                                                                                Serial.println("Error al abrir el archivo worktime");
                                                                                            #endif
        return;
    }
    file.print(workTime);
    file.close();
}

int ELEMENT_::get_workTime(){
    
    File file = SPIFFS.open(ELEMENT_WORKTIME_FILE_PATH, "r");
    if (!file) {
                                                                            #ifdef DEBUG
                                                                                Serial.println("Error al abrir el archivo worktime");
                                                                            #endif
        return -1;
    }
    int value = file.parseInt();
    file.close();
    return value;
}

byte ELEMENT_::get_ID_from_file(){
    File file = SPIFFS.open(ELEMENT_ID_FILE_PATH, "r");
    if (!file) {
                                                                                    #ifdef DEBUG
                                                                                        Serial.println("Error al abrir el archivo");
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
                                                                                    Serial.println("Error al abrir el archivo");
                                                                                #endif
        return;
    }
    file.println(IDin);
                                                                                #ifdef DEBUG
                                                                                Serial.println("ID guardado: " + String(IDin));
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



void ELEMENT_::start_working_time() {
    if (!workTimerRunning) {
        workTimeStart = millis();
        workTimerRunning = true;
    }
}


void ELEMENT_::stopAndSave_working_time() {
     if (workTimerRunning) {
        unsigned long elapsedTime = (millis() - workTimeStart) / 1000; // convertir a segundos
        int currentWorkTime = ELEMENT_::get_workTime();
        ELEMENT_::set_workTime(currentWorkTime + elapsedTime);
        workTimerRunning = false;
                                                                                            #ifdef DEBUG
                                                                                            Serial.print("⏳ WorkTime actualizado: ");
                                                                                            Serial.print(ELEMENT_::get_workTime());
                                                                                            Serial.println(" segundos.");
                                                                                            #endif
    }

}

void ELEMENT_::lifeTime_update() {
        if (millis() - lastLifeTimeUpdate >= LIFETIME_UPDATE_INTERVAL) {   
            //ELEMENT_::print_event_register(); 
        lastLifeTimeUpdate = millis();
        unsigned long currentLifeTime = ELEMENT_::get_lifeTime();
        ELEMENT_::set_lifeTime(currentLifeTime + 1);
                                                                                            #ifdef DEBUG
                                                                                            Serial.print("⏳ LifeTime incrementado: ");
                                                                                            Serial.print(ELEMENT_::get_lifeTime());
                                                                                            Serial.println(" minutos.");
                                                                                            #endif  
    }
}




byte ELEMENT_::get_currentMode(){
    return currentMode;
}


void ELEMENT_::set_manager(byte managerin){

    exclusiveIDmanager= managerin;
                                                                #ifdef DEBUG
                                                                  Serial.println("configurado manager exclusivo: " +String(exclusiveIDmanager));
                                                                #endif
}

byte ELEMENT_::get_manager(){
    return exclusiveIDmanager;
}


void ELEMENT_::set_mode(uint8_t mode) {
    currentMode = mode;
                                                                            #ifdef DEBUG
                                                                        Serial.println("current: " +String(currentMode));
                                                                        Serial.println("mode " +String(mode));
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

void ELEMENT_::agregar_evento(byte eventType, int eventData) {
    unsigned long currentTime = millis();
    int duration = 0;
    static byte lastEventType = 255; // Inicializado con un valor que no es un evento válido

    switch (eventType) {
        case EV_START:
            lastStartTime = currentTime;
            break;
            
        case EV_END:
            if (lastStartTime > 0) {
                duration = currentTime - lastStartTime;
                lastStartTime = 0;
            }
            break;

        case EV_MODE_CHANGE:
            if (lastMode != eventData) {
                if (lastModeChangeTime > 0) {
                    duration = currentTime - lastModeChangeTime;
                }
                lastModeChangeTime = currentTime;
                lastMode = eventData;
            } else {
                return; // No agregar si el modo no ha cambiado realmente
            }
            break;

        case EV_COLOR_CHANGE:
            if (lastColor != eventData) {
                if (lastColorChangeTime > 0) {
                    duration = currentTime - lastColorChangeTime;
                }
                lastColorChangeTime = currentTime;
                lastColor = eventData;
            } else {
                return; // No agregar si el color no ha cambiado realmente
            }
            break;

        case EV_SECTOR_REQ:
            if (lastEventType == EV_SECTOR_REQ) {
                return; // No agregar si el último evento también fue EV_SECTOR_REQ
            }
            break;

        default: 
            return; // No agregar eventos desconocidos
    }

    EVENT_REGISTER_T newEvent = {static_cast<byte>(eventType), eventData, duration};
    eventVector.push_back(newEvent);
    lastEventType = eventType;
}


void ELEMENT_::save_event_register() {

    File file = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_APPEND);
    if (!file) {
        Serial.println("Failed to open file for appending");
        return;
    }
    for (const auto& event : eventVector) {
        String eventDescription = get_word_from_eventNum(event.type);
        file.printf("%s  ->  %d -> %lu\n", eventDescription.c_str(), event.value, event.duration);
    }
    file.close();
    eventVector.clear();
}

void ELEMENT_::configurar_RF(int baudRate) {
    pinMode(RF_CONFIG_PIN, OUTPUT);
    digitalWrite(RF_CONFIG_PIN, LOW);  // Entrar en modo configuración
    delay(500);

    // Intentar comunicarse con ambas velocidades por seguridad
    Serial1.begin(115200, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    byte comandoReset[] = {0xAA, 0xFA, 0xF0};  // Comando para resetear a valores de fábrica
    Serial1.write(comandoReset, sizeof(comandoReset));
    delay(1000); // Dar tiempo al módulo para reiniciarse

    // Verificar si responde a 115200
    bool resetConfirmado = false;
    if (Serial1.available()) {
        Serial.println("Respuesta a 115200 detectada");
        resetConfirmado = true;
    } else {
        // Si no responde, intentar con 9600
        Serial1.end();
        Serial1.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
        Serial1.write(comandoReset, sizeof(comandoReset));
        delay(1000);
        if (Serial1.available()) {
            Serial.println("Respuesta a 9600 detectada");
            resetConfirmado = true;
        }
    }

    if (!resetConfirmado) {
        Serial.println("Error: No se detectó respuesta del módulo RF");
        digitalWrite(RF_CONFIG_PIN, HIGH);
        return;
    }

    // Configurar comandos según la velocidad deseada
    byte comandoUART[7];
    byte comandoWirelessDataRate[7];

    if (baudRate == 115200) {
        Serial.println("Config UART y Wireless a 115200");
        comandoUART[0] = 0xAA; comandoUART[1] = 0xFA; comandoUART[2] = 0x1E;
        comandoUART[3] = 0x00; comandoUART[4] = 0x01; comandoUART[5] = 0xC2; comandoUART[6] = 0x00;

        comandoWirelessDataRate[0] = 0xAA; comandoWirelessDataRate[1] = 0xFA; comandoWirelessDataRate[2] = 0xC3;
        comandoWirelessDataRate[3] = 0x00; comandoWirelessDataRate[4] = 0x01; comandoWirelessDataRate[5] = 0xC2; comandoWirelessDataRate[6] = 0x00;
    } else {
        Serial.println("Config UART y Wireless a 9600");
        comandoUART[0] = 0xAA; comandoUART[1] = 0xFA; comandoUART[2] = 0x1E;
        comandoUART[3] = 0x00; comandoUART[4] = 0x00; comandoUART[5] = 0x25; comandoUART[6] = 0x80;

        comandoWirelessDataRate[0] = 0xAA; comandoWirelessDataRate[1] = 0xFA; comandoWirelessDataRate[2] = 0xC3;
        comandoWirelessDataRate[3] = 0x00; comandoWirelessDataRate[4] = 0x00; comandoWirelessDataRate[5] = 0x25; comandoWirelessDataRate[6] = 0x80;
    }

    // Configurar UART y velocidad inalámbrica
    Serial1.write(comandoUART, sizeof(comandoUART));
    delay(500);
    Serial1.write(comandoWirelessDataRate, sizeof(comandoWirelessDataRate));
    delay(500);

    // Confirmar configuración
    byte comandoLeerConfiguracion[] = {0xAA, 0xFA, 0xE1};
    Serial1.write(comandoLeerConfiguracion, sizeof(comandoLeerConfiguracion));
    delay(500);

    Serial.println(" =[Desglosando configuración recibida]=");

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

        Serial.print("📡 Frecuencia: ");
        Serial.print(freq);
        Serial.println(" Hz");

        Serial.print("⚡ Velocidad inalámbrica: ");
        Serial.print(baudrate);
        Serial.println(" bps");

        Serial.print("📶 Ancho de banda: ");
        Serial.print(bw);
        Serial.println(" kHz");

        Serial.print("🎛️ Desviación de frecuencia: ");
        Serial.print(desviacionFrecuencia);
        Serial.println(" kHz");

        Serial.print("🔋 Potencia de transmisión: ");
        Serial.print(potencia);
        Serial.println(" dBm");
        Serial.println();

    } else {
        Serial.println("Error: Datos insuficientes para interpretar la configuración.");
    }

    // Salir del modo configuración
    digitalWrite(RF_CONFIG_PIN, HIGH);
    delay(500);

    // Reconfigurar la velocidad UART en el ESP32
    Serial1.end();
    if (baudRate == 115200) {
        Serial.println("Config velocidad UART en ESP32 a 115200");
        Serial1.begin(115200, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    } else {
        Serial.println("Config velocidad UART en ESP32 a 9600");
        Serial1.begin(9600, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    }

    Serial.println("Configuración completa y módulo reiniciado correctamente.");
    delay(200);
}









































































// bool firstBoot = preferences.getBool("firstBoot", true); // Leer la bandera, por defecto 'true'
    // if (firstBoot) {
    //     ELEMENT_::set_ID_to_file(DEFAULT_DEVICE); // Solo se ejecutará en la primera carga del firmware
    //     preferences.putBool("firstBoot", false);  // Marcar como ya inicializado
    //     Serial.println("Archivo creado por primera vez.");
    // } else {
    //     Serial.println("El archivo ya fue creado anteriormente.");
    // }

    // preferences.end();

    //if (esp_reset_reason() == ESP_RST_POWERON) ELEMENT_::set_ID_to_file(DEFAULT_DEVICE);


    // RTC_DATA_ATTR static uint32_t bootCount = 0;
    // if(bootCount == 0) ELEMENT_::set_ID_to_file(DEFAULT_DEVICE);
    // bootCount++;
    // Serial.println("#########################    Boot number: " + String(bootCount));


    // byte IDtocheck= ELEMENT_::get_ID_from_file();
    // if(IDtocheck != DEFAULT_DEVICE) ELEMENT_::set_ID_protected(false);                                  
    // bool prot=   ELEMENT_::get_ID_protected();
    // if(prot == false) ELEMENT_::set_ID_to_file(DEFAULT_DEVICE);