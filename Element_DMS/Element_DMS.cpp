#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Element_DMS/Element_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Colors_DMS/Color_DMS.h>
#include <icons_64x64_DMS/icons_64x64_DMS.h>




ELEMENT_::ELEMENT_(){}

    void ELEMENT_::begin() {
                                                        #ifdef DEBUG
                                                            Serial.begin(115200);
                                                            Serial.println("ATENCION!!!!!!!!!!!!!!!!!!!");
                                                        #endif
    Serial1.begin(RF_BAUD_RATE, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    pinMode(RF_CONFIG_PIN, OUTPUT);
    digitalWrite(RF_CONFIG_PIN, HIGH);

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


ELEMENT_::~ELEMENT_() {
    
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