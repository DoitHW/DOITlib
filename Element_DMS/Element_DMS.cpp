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
}


ELEMENT_::~ELEMENT_() { 
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

void ELEMENT_::event_register(int eventNum, int eventVal) {
    unsigned long currentTime = millis();
    EVENT_TYPE eventType = static_cast<EVENT_TYPE>(eventNum);

    // Resetear el vector events cuando se recibe EV_START
    if (eventType == EV_START) {
        events.clear();
        lastStartTime = currentTime;
        lastModeChangeTime = 0;
        lastColorChangeTime = 0;
        lastMode = -1;
        lastColor = -1;
    }

    switch (eventType) {
        case EV_START:
            events.push_back(EVENT_T{eventType, 0, 0});
            break;

        case EV_END:
            if (lastStartTime > 0) {
                events.push_back(EVENT_T{eventType, 0, currentTime - lastStartTime});
                
                // Actualizar tiempos para eventos sin cierre
                for (auto it = events.rbegin(); it != events.rend(); ++it) {
                    if (it->eventNum == EV_COLOR_CHANGE && it->timestamp == 0) {
                        it->timestamp = currentTime - lastColorChangeTime;
                        break;
                    }
                    // Hacer lo mismo para MODE_CHANGE, FLAG_CHANGE, ID_CHANGE si es necesario
                }
            }
            break;

        case EV_MODE_CHANGE:
            if (lastMode != eventVal) {
                if (lastModeChangeTime > 0) {
                    for (auto it = events.rbegin(); it != events.rend(); ++it) {
                        if (it->eventNum == EV_MODE_CHANGE && it->timestamp == 0) {
                            it->timestamp = currentTime - lastModeChangeTime;
                            break;
                        }
                    }
                }
                events.push_back(EVENT_T{eventType, eventVal, 0});
                lastMode = eventVal;
                lastModeChangeTime = currentTime;
            }
            break;

        case EV_COLOR_CHANGE:
            // Solo contar el tiempo si el color no es negro (8)
            if (lastColor != eventVal) {
                if (lastColorChangeTime > 0 && lastColor != 8) { // Solo actualizar si el último color no es negro
                    for (auto it = events.rbegin(); it != events.rend(); ++it) {
                        if (it->eventNum == EV_COLOR_CHANGE && it->timestamp == 0) {
                            it->timestamp = currentTime - lastColorChangeTime;
                            break;
                        }
                    }
                }
                
                // Solo registrar el evento si no es negro
                if (eventVal != 8) { 
                    events.push_back(EVENT_T{eventType, eventVal, 0});
                }

                lastColor = eventVal;
                lastColorChangeTime = currentTime;
            }
            break;

        case EV_FLAG_CHANGE:
            events.push_back(EVENT_T{eventType, eventVal, 0});
            break;

        case EV_ID_CHANGE:
            events.push_back(EVENT_T{eventType, eventVal, 0});
            break;

        case EV_SECTOR_REQ:
            if (events.empty() || events.back().eventNum != EV_SECTOR_REQ) {
                events.push_back(EVENT_T{eventType, eventVal, 0});
            }
            break;
    }
}


void ELEMENT_::save_register() {
    const char* tempFileName = "/temp.txt";
    File oldFile = SPIFFS.open(ELEMENT_EVENT_REGISTER_FILE_PATH, FILE_READ);
    File newFile = SPIFFS.open(tempFileName, FILE_WRITE);

    if (!oldFile || !newFile) {
        Serial.println("Error al abrir archivos para escritura/reescritura");
        return;
    }

    // Variables para estadísticas globales
    unsigned long totalWorkTime = 0; // En segundos
    unsigned long totalLifeTime = 0;  // En minutos
    int totalCycles = 0;
    std::vector<String> cycles;

    // Leer estadísticas existentes
    if (oldFile.available()) {
        String line;
        while (oldFile.available()) {
            line = oldFile.readStringUntil('\n');
            if (line.startsWith("Tiempo de trabajo:")) {
                totalWorkTime = line.substring(line.indexOf(":") + 1).toInt();
            } else if (line.startsWith("Tiempo de vida:")) {
                totalLifeTime = line.substring(line.indexOf(":") + 1).toInt() * 60; // Convertir a segundos
            } else if (line.startsWith("Numero de ciclos:")) {
                totalCycles = line.substring(line.indexOf(":") + 1).toInt();
            }
        }
        oldFile.seek(0); // Volver al inicio del archivo
    }

    // Calcular el tiempo de trabajo del ciclo actual en segundos
    unsigned long currentCycleWorkTime = 0;
    for (const auto& event : events) {
        if (event.eventNum == EV_COLOR_CHANGE) {
            currentCycleWorkTime += event.timestamp; // Ya está en milisegundos, no se multiplica por 60
        }
    }

    // Actualizar estadísticas globales
    totalWorkTime += currentCycleWorkTime / 1000; // Convertir milisegundos a segundos
    totalLifeTime += totalWorkTime / 60;           // Convertir segundos a minutos
    totalCycles++;

    // Escribir la cabecera actualizada
    String name = get_string_from_info_DB(ELEM_NAME, 1);
    String serial = get_serial_from_file();
    newFile.println("Nombre: " + name);
    newFile.println("Numero de serie: " + serial);
    newFile.printf("Tiempo de trabajo: %lu segundos\n", totalWorkTime);
    newFile.printf("Tiempo de vida: %lu minutos\n", totalLifeTime);
    newFile.printf("Numero de ciclos: %d\n\n", totalCycles);

    // Leer ciclos existentes
    while (oldFile.available()) {
        String cycle = "";
        String line = oldFile.readStringUntil('\n');
        if (line.startsWith("--NUEVO CICLO")) {
            cycle += line + "\n";
            do {
                line = oldFile.readStringUntil('\n');
                cycle += line + "\n";
            } while (oldFile.available() && !line.startsWith("-- FIN DE CICLO --"));
            cycles.push_back(cycle);
        }
    }

    // Añadir el nuevo ciclo
    String newCycle = "--NUEVO CICLO " + String(totalCycles) + "--\n";
    for (const auto& event : events) {
        newCycle += "evento: " + get_word_from_eventNum(event.eventNum) + "\n";
        newCycle += "valor: " + String(event.eventVal) + "\n";
        newCycle += "duracion: " + String(event.timestamp / 1000) + " segundos\n\n"; // Convertir a segundos
    }
    newCycle += "-- FIN DE CICLO --\n\n";
    cycles.push_back(newCycle);

    // Eliminar ciclos antiguos si se excede el máximo
    while (cycles.size() > MAX_EVENTS) {
        cycles.erase(cycles.begin());
    }

    // Escribir los ciclos en el nuevo archivo
    for (const auto& cycle : cycles) {
        newFile.print(cycle);
    }

    oldFile.close();
    newFile.close();

    // Reemplazar el archivo antiguo con el nuevo
    SPIFFS.remove(ELEMENT_EVENT_REGISTER_FILE_PATH);
    SPIFFS.rename(tempFileName, ELEMENT_EVENT_REGISTER_FILE_PATH);

    Serial.println("Registro de eventos guardado y actualizado en SPIFFS");

    // Limpiar el vector de eventos para el próximo ciclo
    events.clear();
}



String ELEMENT_::get_word_from_eventNum(int eventNumber){

    String frase;
    switch(eventNumber){
        case EV_START:        frase=   "DISPOSITIVO INICIADO EN MODO BASICO"; break;
        case EV_END:          frase=   "APAGANDO DISPOSITIVO"; break;
        case EV_SECTOR_REQ:   frase=   "PETICION DE SECTOR";   break;
        case EV_MODE_CHANGE:  frase=   "CAMBIO DE MODO";       break;
        case EV_COLOR_CHANGE: frase=   "CAMBIO DE COLOR";      break;
        case EV_FLAG_CHANGE:  frase=   "CAMBIO DE FLAG";       break;
        case EV_ID_CHANGE:    frase=   "CAMBIO DE ID";       break;

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
