#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <Element_DMS/Element_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Frame_DMS/Frame_DMS.h>




ELEMENT_::ELEMENT_(uint16_t serialNumber){
    
    serialNum[0] = (serialNumber >> 8) & 0xFF; 
    serialNum[1] = serialNumber & 0xFF;


}



void ELEMENT_::begin() {
    Serial.begin(115200);
    Serial.println("ATENCION!!!!!!!!!!!!!!!!!!!");

    Serial1.begin(RF_BAUD_RATE, SERIAL_8N1, RF_RX_PIN, RF_TX_PIN);
    Serial1.onReceive(onUartInterrupt);
    pinMode(RF_CONFIG_PIN, OUTPUT);
    digitalWrite(RF_CONFIG_PIN, HIGH);
    delay(500);

    // Montar SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("Error al montar SPIFFS. Reiniciando.");
        return;
    }

    // Verificar existencia del archivo de configuración
    if (SPIFFS.exists(ELEMENT_CONFIG_FILE_PATH)) {
        File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r+");
        if (!configFile) {
            Serial.println("Error al abrir el archivo de configuración existente.");
            return;
        }

        // Leer el protectedID_ para verificar inicialización
        byte protectedID_ = configFile.readStringUntil('\n').toInt();

        if (protectedID_ == 0) {
            Serial.println("Archivo no inicializado. Estableciendo valores predeterminados.");
            configFile.seek(0); // Volver al inicio del archivo
            configFile.println("1"); // Marcar protectedID_ como 1

            // Manejo de configuración por defecto o personalizada
            if (DEFAULT_CONFIG) {
                configFile.println(DEFAULT_DEVICE); // Escribir configuración por defecto
            } else if (CUSTOM_ID_INIC) {
                configFile.println(CUSTOM_ID_NUM); // Escribir configuración personalizada
            }

            // Escribir valores iniciales para tiempo de inicio y trabajo
            configFile.println("0"); // onStartTime
            configFile.println("0"); // workedTime
            configFile.flush();
        } else {
            Serial.println("Archivo ya inicializado.");
        }

        // Leer los valores de configuración
        ID = configFile.readStringUntil('\n').toInt();
        onStartTime = configFile.readStringUntil('\n').toInt();
        workedTime = configFile.readStringUntil('\n').toInt();

        // Depuración de los valores leídos
        Serial.println("ID en ROM: " + String(ID));
        Serial.println("Tiempo de vida acumulado: " + String(onStartTime));
        Serial.println("Tiempo de trabajo acumulado: " + String(workedTime));

        configFile.close();
    } else {
        // Crear un archivo nuevo si no existe
        Serial.println("Archivo no encontrado. Creando nuevo archivo.");
        File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "w");
        if (configFile) {
            configFile.println("1"); // protectedID_
            if (DEFAULT_CONFIG) {
                configFile.println(DEFAULT_DEVICE); // Escribir configuración por defecto
            } else if (CUSTOM_ID_INIC) {
                configFile.println(CUSTOM_ID_NUM); // Escribir configuración personalizada
            }
            configFile.println("0"); // onStartTime
            configFile.println("0"); // workedTime
            configFile.flush();
            configFile.close();
        } else {
            Serial.println("Error al crear el archivo de configuración.");
        }
    }

    #ifdef DEBUG
        Serial.println("Configuración inicial completada.");
    #endif
}



void ELEMENT_::reset_config_file() {
    // Verificar si el archivo de configuración existe
    if (SPIFFS.exists(ELEMENT_CONFIG_FILE_PATH)) {
        // Intentar eliminar el archivo
        if (SPIFFS.remove(ELEMENT_CONFIG_FILE_PATH)) {
            Serial.println("Archivo de configuración eliminado correctamente.");
        } else {
            Serial.println("Error: No se pudo eliminar el archivo de configuración.");
        }
    } else {
        Serial.println("El archivo de configuración no existe, no es necesario eliminarlo.");
    }
}



ELEMENT_::~ELEMENT_() {
    // Implementación del destructor
}

void ELEMENT_::start_working_time() {
    if (!stopwatchRunning && canStartStopwatch) {
        stopwatchStartTime = millis(); // Guardar el tiempo de inicio
        stopwatchRunning = true;       // Marcar el cronómetro como activo
        canStartStopwatch = false;     // Bloquear hasta que se detenga el cronómetro
                                                #ifdef DEBUG
                                                    Serial.println("Cronómetro iniciado.");
                                                #endif
    } else if (!canStartStopwatch) {
                                                #ifdef DEBUG
                                                    Serial.println("Error: No se puede iniciar el cronómetro sin detenerlo previamente.");
                                                #endif
    }
}



void ELEMENT_::stopAndSave_working_time() {
    if (stopwatchRunning) {
        unsigned long elapsedMillis = millis() - stopwatchStartTime; // Calcular el tiempo transcurrido
        stopwatchRunning = false;       // Detener el cronómetro
        canStartStopwatch = true;       // Permitir reiniciar el cronómetro

        unsigned long elapsedSeconds = elapsedMillis / 1000; // Convertir a segundos

        File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r+");
        if (!configFile) {
            Serial.println("Error al abrir el archivo de configuración.");
            return;
        }

        // Leer las líneas existentes
        String line1 = configFile.readStringUntil('\n'); // Primera línea
        String line2 = configFile.readStringUntil('\n'); // Segunda línea
        String line3 = configFile.readStringUntil('\n'); // Tercera línea
        String line4 = configFile.readStringUntil('\n'); // Cuarta línea

        // Validar las líneas leídas
        Serial.println("Línea 1: " + line1);
        Serial.println("Línea 2: " + line2);
        Serial.println("Línea 3: " + line3);
        Serial.println("Línea 4 (workedTime): " + line4);

        // Actualizar el tiempo acumulado en la cuarta línea
        unsigned long updatedWorkedTime = line4.toInt() + elapsedSeconds;
        line4 = String(updatedWorkedTime);

        // Sobrescribir el archivo con los valores actualizados
        configFile.seek(0);
        configFile.println(line1);
        configFile.println(line2);
        configFile.println(line3);
        configFile.println(line4);
        configFile.flush();
        configFile.close();

        Serial.println("Cronómetro detenido. Tiempo acumulado actualizado: " + String(updatedWorkedTime) + " segundos.");
    } else {
        Serial.println("Error: No hay cronómetro activo para detener.");
    }

}
void ELEMENT_::set_default_ID(){
    File archivoID = SPIFFS.open("/element_ID.txt", "r+");
    if (!archivoID) {
        archivoID = SPIFFS.open("/element_ID.txt", "w");
        if (archivoID) {
            archivoID.print(DEFAULT_DEVICE);       
                                                                    #ifdef DEBUG
                                                                        Serial.println("ID configurada DEFAULT en SPIFFS.");
                                                                    #endif
        }
    }   
    archivoID.close();
}

void ELEMENT_::set_custom_ID(){
    File archivoID = SPIFFS.open("/element_ID.txt", "r+");
    if (!archivoID) {
        archivoID = SPIFFS.open("/element_ID.txt", "w");
        if (archivoID) {
            archivoID.print(CUSTOM_ID_NUM);  // ln?
            archivoID.close();       
                                                                    #ifdef DEBUG
                                                                        Serial.println("ID configurada DEFAULT en SPIFFS.");
                                                                    #endif
        }
    }   
    archivoID.close();
}


void ELEMENT_::set_sinceStart_time(uint64_t timein){
    onStartTime= timein;
}

uint64_t ELEMENT_::get_sinceStart_time(){
    return onStartTime;
}

void ELEMENT_::lifeTime_update() {
    static unsigned long lastCheckTime = 0;   // Para actualizar el tiempo acumulado
    static unsigned long lastPrintTime = 0;  // Para imprimir cada 10 segundos
    unsigned long currentMillis = millis();

    // Control de impresión cada 10 segundos
    if (currentMillis - lastPrintTime >= 30000) {
        // Abrir archivo para leer el valor actual de minutos transcurridos
        File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r");
        if (configFile) {
            configFile.readStringUntil('\n'); // Saltar protectedID_
            configFile.readStringUntil('\n'); // Saltar ID
            String onStartTimeStr = configFile.readStringUntil('\n'); // Leer tiempo acumulado
            int onStartTime = onStartTimeStr.toInt();
            Serial.println("Minutos transcurridos: " + String(onStartTime));
            configFile.close();
        } else {
            Serial.println("Error: No se pudo abrir el archivo para leer los minutos transcurridos.");
        }

        lastPrintTime = currentMillis;
    }

    // Control de actualización cada minuto
    if (currentMillis - lastCheckTime < 60000) { // Actualizar solo cada minuto
        return;
    }
    lastCheckTime = currentMillis;

    // Abrir archivo para actualizar los valores
    File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r+");
    if (!configFile) {
        Serial.println("Error crítico: No se puede abrir el archivo de configuración");
        return;
    }

    // Leer las líneas existentes
    configFile.readStringUntil('\n'); // Saltar protectedID_
    configFile.readStringUntil('\n'); // Saltar ID
    String onStartTimeStr = configFile.readStringUntil('\n'); // Leer tiempo acumulado
    String workedTimeStr = configFile.readStringUntil('\n'); // Leer tiempo trabajado

    int onStartTime = onStartTimeStr.toInt();
    onStartTime++; // Incrementar en 1 minuto

    // Sobrescribir el archivo con los valores actualizados
    configFile.seek(0); // Volver al inicio
    configFile.println("1"); // protectedID_
    configFile.println(String(ID)); // ID (asumiendo que ID ya está definido en tu clase)
    configFile.println(String(onStartTime)); // Tiempo acumulado actualizado
    configFile.println(workedTimeStr); // Tiempo de trabajo acumulado sin cambios

    configFile.close();

    Serial.println("Valor actualizado de minutos transcurridos: " + String(onStartTime));
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

void ELEMENT_::set_ID_protected(){
    File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "w");
    configFile.println(1);
    configFile.close();
}

uint8_t ELEMENT_::get_ID() {
    return ID;
}

void ELEMENT_::set_ID(uint8_t deviceID) {
    ID = deviceID;
    File configFile = SPIFFS.open(ELEMENT_CONFIG_FILE_PATH, "r+");
    configFile.readStringUntil('\n');
    configFile.println(ID);
    configFile.close();
    delay(1);
   
                                                                #ifdef DEBUG
                                                                  Serial.print("ID cambiada a "+ String(ID));
                                                                #endif
}

byte ELEMENT_::get_serialNum(byte ml){
   byte serial;
   if(ml = MSB)      serial= serialNum[0];
   else if(ml = LSB) serial= serialNum[1];
   return serial;
}

void ELEMENT_::set_mode(uint8_t mode) {
    currentMode = mode;
}

void ELEMENT_::set_type(byte typein){

    type= typein;
}

byte ELEMENT_::get_type(){
    return type;
}

INFO_STATE_T ELEMENT_::get_state_pack(){

    INFO_STATE_T info;
    
    return info;
}

INFO_PACK_T ELEMENT_::get_info_pack(byte typein, byte languajein){

    INFO_PACK_T info;
    String texto;
    byte msb;
    byte lsb;
    byte i= 0;
    uint16_t aux;

    // ELEM
    texto= get_string_from_info_DB(typein, ELEM_NAME, languajein);
    texto.toCharArray((char*)info.name, sizeof(info.name));
    texto= get_string_from_info_DB(typein, ELEM_DESC, languajein);
    texto.toCharArray((char*)info.desc, sizeof(info.desc));

    // MODO 0
    texto= get_string_from_info_DB(typein, ELEM_MODE_0_NAME, languajein);
    texto.toCharArray((char*)info.mode[i].name, sizeof(info.mode[i].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_0_DESC, languajein);
    texto.toCharArray((char*)info.mode[i].desc, sizeof(info.mode[i].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;       


    // MODO 1
    texto= get_string_from_info_DB(typein, ELEM_MODE_1_NAME, languajein);
    texto.toCharArray((char*)info.mode[1].name, sizeof(info.mode[1].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_1_DESC, languajein);
    texto.toCharArray((char*)info.mode[1].desc, sizeof(info.mode[1].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 2
    texto= get_string_from_info_DB(typein, ELEM_MODE_2_NAME, languajein);
    texto.toCharArray((char*)info.mode[2].name, sizeof(info.mode[2].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_2_DESC, languajein);
    texto.toCharArray((char*)info.mode[2].desc, sizeof(info.mode[2].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 3
    texto= get_string_from_info_DB(typein, ELEM_MODE_3_NAME, languajein);
    texto.toCharArray((char*)info.mode[3].name, sizeof(info.mode[3].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_3_DESC, languajein);
    texto.toCharArray((char*)info.mode[3].desc, sizeof(info.mode[3].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 4
    texto= get_string_from_info_DB(typein, ELEM_MODE_4_NAME, languajein);
    texto.toCharArray((char*)info.mode[4].name, sizeof(info.mode[4].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_4_DESC, languajein);
    texto.toCharArray((char*)info.mode[4].desc, sizeof(info.mode[4].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 5      
    texto= get_string_from_info_DB(typein, ELEM_MODE_5_NAME, languajein);
    texto.toCharArray((char*)info.mode[5].name, sizeof(info.mode[5].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_5_DESC, languajein);
    texto.toCharArray((char*)info.mode[5].desc, sizeof(info.mode[5].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 6
    texto= get_string_from_info_DB(typein, ELEM_MODE_6_NAME, languajein);
    texto.toCharArray((char*)info.mode[6].name, sizeof(info.mode[6].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_6_DESC, languajein);
    texto.toCharArray((char*)info.mode[6].desc, sizeof(info.mode[6].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 7
    texto= get_string_from_info_DB(typein, ELEM_MODE_7_NAME, languajein);
    texto.toCharArray((char*)info.mode[7].name, sizeof(info.mode[7].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_7_DESC, languajein);
    texto.toCharArray((char*)info.mode[7].desc, sizeof(info.mode[7].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 8      
    texto= get_string_from_info_DB(typein, ELEM_MODE_8_NAME, languajein);
    texto.toCharArray((char*)info.mode[8].name, sizeof(info.mode[8].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_8_DESC, languajein);
    texto.toCharArray((char*)info.mode[8].desc, sizeof(info.mode[8].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 9
    texto= get_string_from_info_DB(typein, ELEM_MODE_9_NAME, languajein);
    texto.toCharArray((char*)info.mode[9].name, sizeof(info.mode[9].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_9_DESC, languajein);
    texto.toCharArray((char*)info.mode[9].desc, sizeof(info.mode[9].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 10
    texto= get_string_from_info_DB(typein, ELEM_MODE_10_NAME, languajein);
    texto.toCharArray((char*)info.mode[10].name, sizeof(info.mode[10].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_10_DESC, languajein);
    texto.toCharArray((char*)info.mode[10].desc, sizeof(info.mode[10].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 11
    texto= get_string_from_info_DB(typein, ELEM_MODE_11_NAME, languajein);
    texto.toCharArray((char*)info.mode[11].name, sizeof(info.mode[11].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_11_DESC, languajein);
    texto.toCharArray((char*)info.mode[11].desc, sizeof(info.mode[11].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 12
    texto= get_string_from_info_DB(typein, ELEM_MODE_12_NAME, languajein);
    texto.toCharArray((char*)info.mode[12].name, sizeof(info.mode[12].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_12_DESC, languajein);
    texto.toCharArray((char*)info.mode[12].desc, sizeof(info.mode[12].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 13
    texto= get_string_from_info_DB(typein, ELEM_MODE_13_NAME, languajein);
    texto.toCharArray((char*)info.mode[13].name, sizeof(info.mode[13].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_13_DESC, languajein);
    texto.toCharArray((char*)info.mode[13].desc, sizeof(info.mode[13].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 14       
    texto= get_string_from_info_DB(typein, ELEM_MODE_14_NAME, languajein);
    texto.toCharArray((char*)info.mode[14].name, sizeof(info.mode[14].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_14_DESC, languajein);
    texto.toCharArray((char*)info.mode[14].desc, sizeof(info.mode[14].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      

    // MODO 15
    texto= get_string_from_info_DB(typein, ELEM_MODE_15_NAME, languajein);
    texto.toCharArray((char*)info.mode[15].name, sizeof(info.mode[15].name));
    texto= get_string_from_info_DB(typein, ELEM_MODE_15_DESC, languajein);
    texto.toCharArray((char*)info.mode[15].desc, sizeof(info.mode[15].desc));
    aux= get_config_flag_mode(typein, i);   
    aux = get_config_flag_mode(typein, i);
    msb = (aux >> 8) & 0xFF;
    lsb = aux & 0xFF; 
    info.mode[i].config[0]= lsb;
    info.mode[i].config[1]= msb;
    i++;      


return info;
}