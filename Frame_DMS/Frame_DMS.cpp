#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <column_DMS/column_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <icons_64x64_DMS/icons_64x64_DMS.h>
#include <vector>
#include <Arduino.h>
#include <WiFi.h>

std::vector<uint8_t> printTargetID;
extern LAST_ENTRY_FRAME_T            LEF;
extern byte globalID;
extern byte xManager;
volatile bool startFrameReceived=    false;
volatile bool frameInProgress=       false;
volatile bool partialFrameReceived=  false;
volatile bool frameReceived=         false;

std::vector<byte> uartBuffer;
bool BCframe= false;
volatile unsigned long last_received_time = 0;
int lastReceivedTime = 0;

volatile bool uartInterruptTriggered= false;

// LA FUNCION DIOS

void IRAM_ATTR onUartInterrupt() {
    // No Serial.println() or String inside ISR for efficiency!
    static enum {
        WAITING_START,
        RECEIVING_LENGTH_MSB,
        RECEIVING_LENGTH_LSB,
        RECEIVING_FRAME
    } receiveState = WAITING_START;

    static uint16_t expectedFrameLength = 0;
    static uint16_t totalFrameLength = 0;       // Longitud total (frameLength + 3)
    static uint16_t receivedBytes = 0;          // OJO con los statics, danger.
    static uint16_t calculatedChecksum = 0;
    static uint8_t receivedChecksum = 0;
    static unsigned long frameStartTime = 0; // Timestamp for timeout
    
    uint8_t bytesProcessed = 0;
    unsigned long currentTime = millis();
    
    
    while (Serial1.available()) {
        byte receivedByte = Serial1.read();
        bytesProcessed++; // Increment bytes processed count
        DEBUG__________((" 0x"+String(receivedByte, HEX)).c_str());
            //DEBUG__________(("\n\n0x" +String(receivedByte, HEX)).c_str())
            //DEBUG__________ln(("expectedFrameLength str = " +String(expectedFrameLength)).c_str())
            //DEBUG__________ln(("Recieve state: " +String(receiveState)).c_str())
            //DEBUG__________ln(("UartBuffer.size()" +String(uartBuffer.size())).c_str())
    
        lastReceivedTime = millis();
        if (receiveState == WAITING_START && receivedByte != NEW_START){
            
                //DEBUG__________ln("if (receiveState == WAITING_START && receivedByte != NEW_START)")
            
            continue;}
    
         // --- ADDED BUFFER SIZE CHECK ---
        if ((uartBuffer.size() > 0xFF) || (expectedFrameLength > 0xFF)) { // Check if buffer is already full or larger
            receiveState = WAITING_START;
            uartBuffer.clear();
            expectedFrameLength= 0;
            //DEBUG__________ln( " ISR Error: UART buffer MAX SIZE reached - Resetting\n\n\n\n\n\n\n\n\n");
            //DEBUG__________ln( " ISR Error: UART buffer MAX SIZE reached - Resetting\n\n\n\n\n\n\n\n\n");
            //DEBUG__________ln( " ISR Error: UART buffer MAX SIZE reached - Resetting\n\n\n\n\n\n\n\n\n");
            return; // Exit ISR to prevent buffer overflow
        }
        // --- END OF ADDED BUFFER SIZE CHECK ---
    
        switch (receiveState) {
            case WAITING_START:
                //DEBUG__________ln("fase waiting")
                if (receivedByte == NEW_START) {
                    uartBuffer.reserve(MAX_FRAME_LENGTH);
                    uartBuffer.clear();
                    uartBuffer.push_back(receivedByte);
    
                    calculatedChecksum = receivedByte;
                    receiveState = RECEIVING_LENGTH_MSB;
                    expectedFrameLength = 0;
                    receivedBytes = 1;
                    frameStartTime = currentTime; // Start timeout timer
                }
                break;
    
            case RECEIVING_LENGTH_MSB:
                //DEBUG__________ln("rec_L_MSB")
                uartBuffer.push_back(receivedByte);
                calculatedChecksum += receivedByte;
                while (calculatedChecksum > 0xFF) calculatedChecksum = (calculatedChecksum & 0xFF) + (calculatedChecksum >> 8);
                expectedFrameLength = receivedByte << 8;  // Guardamos el MSB
                receiveState = RECEIVING_LENGTH_LSB;
                break;
    
            case RECEIVING_LENGTH_LSB:
            //DEBUG__________ln("rec_L_LSB")
            uartBuffer.push_back(receivedByte);
            calculatedChecksum += receivedByte;
            while (calculatedChecksum > 0xFF) calculatedChecksum = (calculatedChecksum & 0xFF) + (calculatedChecksum >> 8);
            expectedFrameLength |= receivedByte;
            // Move the debug print HERE, after LSB processing:
            //DEBUG__________ln("expectedFrameLength = " +String(expectedFrameLength))
            totalFrameLength = expectedFrameLength + 3;
    
                if (totalFrameLength > MAX_FRAME_LENGTH || totalFrameLength < MIN_FRAME_LENGTH) {
                    receiveState = WAITING_START;
                    uartBuffer.clear();
                   
                        DEBUG__________ln("❌ Error: Longitud de trama inv\xE1lida - Resetting");
                  
                    return;
                }
    
                receiveState = RECEIVING_FRAME;
                receivedBytes = 3;  // Contamos NEW_START, LENGTH_MSB y LENGTH_LSB
                break;
    
            case RECEIVING_FRAME:
                //DEBUG__________("rec_FRAME");
                uartBuffer.push_back(receivedByte);
                receivedBytes++;
                if (receivedBytes != totalFrameLength - 1) calculatedChecksum += receivedByte;
                else receivedChecksum = receivedByte;
                while (calculatedChecksum > 0xFF) calculatedChecksum = (calculatedChecksum & 0xFF) + (calculatedChecksum >> 8);
    
                if (receivedBytes == totalFrameLength) {  // Último byte: NEW_END
                    if (receivedByte == NEW_END) {
                        if (calculatedChecksum == receivedChecksum) {
                            // Verificar destinatarios
                            uint8_t numTargets = uartBuffer[4];
                            bool isTarget = false; 
                            printTargetID.clear();
                            for (uint8_t i = 0; i < numTargets; i++) {
                                uint8_t targetID = uartBuffer[5 + i];
                                printTargetID.push_back(targetID);
                                if (targetID == globalID || targetID == BROADCAST || targetID == xManager)  {
                                    isTarget = true;
                                    if(targetID == BROADCAST) BCframe= true;
                                    break;
                                }
                            }
                            isTarget = true; //eliminar esta línea
                            if (isTarget) {
                                //DEBUG__________ln();
                                frameReceived = true; // Set flag for loop() to process
                                DEBUG__________ln("\nTrama recibida y válida 🟢🟢🟢");
                                //DEBUG__________ln(" ISR: Frame Received OK, targeted to device");
                                
                            } else {
                                
                                    DEBUG__________ln(" ISR: Frame Received OK, not targeted to device");
                               
                            }
                        } else {
                            
                                DEBUG__________ln(" ISR Error: Checksum invalid - Frame discarded");
                           
                        }
                    } else {
                      
                            DEBUG__________ln(" ISR Error: Invalid END byte - Frame discarded");
                       
                    }
                    receiveState = WAITING_START; // Reset state for next frame
                    return; // Exit ISR after frame processing or error
                } 
    
                if (uartBuffer.size() >= MAX_BUFFER_SIZE) {
    
                    DEBUG__________ln("Superado tamaño uartbuffer")
                    receiveState = WAITING_START; // Reset state on buffer overflow
                    uartBuffer.clear();
                    return; // Exit ISR on buffer overflow
                }
    
                break;
        }
    }
    
    if (Serial1.available() && bytesProcessed >= MAX_BYTES_PER_INTERRUPT) {
        partialFrameReceived = true; // Set flag for partial frame (not used in current loop() logic)
       
            DEBUG__________ln(" ISR: Partial frame received (more bytes available)");
        
    }
}


byte checksum_calc(const FRAME_T &framein) {
    uint16_t sum= 0;

    sum += framein.start;
    sum += framein.frameLengthMsb;
    sum += framein.frameLengthLsb;
    sum += framein.origin;
    sum += framein.numTargets;
    for (byte target : framein.target) sum += target;
    sum += framein.function;
    sum += framein.dataLengthMsb;
    sum += framein.dataLengthLsb;
    for (byte dato : framein.data) sum += dato;
    sum += NEW_END;
    while (sum > 0xFF) sum = (sum & 0xFF) + (sum >> 8);

    return (byte)sum;
}

void printFrameInfo(LAST_ENTRY_FRAME_T LEF) {
    DEBUG__________ln("\n==== 📨 Trama Recibida 📨 ====");

    // Determinar origen
    String origenStr;
    if (LEF.origin == 0xDB) origenStr = "BOTONERA";
    else if (LEF.origin == 0xDC) origenStr = "CONSOLA";
    else if (LEF.origin == 0xFF) origenStr = "BROADCAST";
    else origenStr = "DESCONOCIDO";

    DEBUG__________printf("📌 Origen: %s (0x%02X)\n", origenStr.c_str(), LEF.origin);

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
    DEBUG__________("📦 Data: ");
    if (LEF.data.empty()) {
        DEBUG__________ln("No hay datos para esta función.");
    } else {
        if (LEF.function == 0xCA || LEF.function == 0xCB) { // Sensores
            int minVal = (LEF.data[0] << 8) | LEF.data[1];
            int maxVal = (LEF.data[2] << 8) | LEF.data[3];
            int sensedVal = (LEF.data[4] << 8) | LEF.data[5];

            DEBUG__________printf("MIN = %d, MAX= %d, VAL= %d\n", minVal, maxVal, sensedVal);
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
        else if (LEF.function == F_SEND_COMMAND) { // Color recibido
            String cmdName;
            switch (LEF.data[0]) {
                case BLACKOUT:        cmdName = "DESACTIVAR"; break;
                case START_CMD:       cmdName = "ACTIVAR"; break;
                case TEST_CMD:        cmdName = "TEST"; break;
                case SEND_REG_RF_CMD: cmdName = "ENVIAR REG POR RF"; break;
                default: cmdName = "A SABER...";
            }
            DEBUG__________printf("%s (0x%02X)\n", cmdName.c_str(), LEF.data[0]);
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

LAST_ENTRY_FRAME_T extract_info_from_frameIn(const std::vector<uint8_t> &frame)
{
    LAST_ENTRY_FRAME_T result = {};

    if (frame.size() < 6)
        return result;

    uint8_t payloadOffset = frame[4];
    if (frame.size() < 5 + payloadOffset + 3) // Accederemos hasta headerOffset+2
        return result;

    size_t headerOffset = 5 + payloadOffset;
    result.origin = frame[3];
    result.function = frame[headerOffset];

    uint16_t dataLength = (frame[headerOffset + 1] << 8) | frame[headerOffset + 2];
    size_t dataStartIndex = headerOffset + 3;

    if (dataStartIndex + dataLength <= frame.size())
    {
        result.data.reserve(dataLength);
        result.data.insert(result.data.begin(),
                           frame.begin() + dataStartIndex,
                           frame.begin() + dataStartIndex + dataLength);
    }

    return result;
}

void send_frame(const FRAME_T &framein) {
    int i = 0;
    byte dTime = 5;
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln(" #### Trama enviada ####");
                                                                                                #endif
    Serial1.write(framein.start);
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] START = " + String(framein.start, HEX));
                                                                                                #endif
    Serial1.write(framein.frameLengthMsb);
    delay(dTime);
                                                                                                #ifdef DEBUG  
                                                                                                DEBUG__________ln("[" + String(++i) + "] Frame Length Msb = " + String(framein.frameLengthMsb, HEX)); 
                                                                                                #endif
    Serial1.write(framein.frameLengthLsb); 
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Frame Length Lsb = " + String(framein.frameLengthLsb, HEX)); 
                                                                                                #endif
    Serial1.write(framein.origin);          
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Origen = " + String(framein.origin, HEX)); 
                                                                                                #endif
    Serial1.write(framein.numTargets);      
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] numTargets = " + String(framein.numTargets, HEX)); 
                                                                                                #endif
    int dataIndex = 0;
    for (byte target : framein.target) {
        Serial1.write(target); 
        delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Target[" + String(dataIndex++) + "] = " + String(target, HEX)); 
                                                                                                #endif
    }
    Serial1.write(framein.function);        
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Function = " + String(framein.function, HEX)); 
                                                                                                #endif
    Serial1.write(framein.dataLengthMsb);   
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Data Length Msb = " + String(framein.dataLengthMsb, HEX)); 
                                                                                                #endif
    Serial1.write(framein.dataLengthLsb);   
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Data Length Lsb = " + String(framein.dataLengthLsb, HEX)); 
                                                                                                #endif
    dataIndex = 0;
    for (byte dato : framein.data) {
        Serial1.write(dato); 
        delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Data[" + String(dataIndex++) + "] = " + String(dato, HEX)); 
                                                                                                #endif
    }
    Serial1.write(framein.checksum);        
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] Checksum = " + String(framein.checksum, HEX)); 
                                                                                                #endif
    Serial1.write(framein.end);             
    delay(dTime);
                                                                                                #ifdef DEBUG
                                                                                                DEBUG__________ln("[" + String(++i) + "] End = " + String(framein.end, HEX)); 
                                                                                                DEBUG__________ln("======================================");
                                                                                                #endif
}

byte get_mapped_sensor_value(byte minMSB, byte minLSB, byte maxMSB, byte maxLSB, byte valMSB, byte valLSB) {

    uint16_t minVal = (minMSB << 8) | minLSB;
    uint16_t maxVal = (maxMSB << 8) | maxLSB;
    uint16_t currentVal = (valMSB << 8) | valLSB;

    uint16_t totalRange = maxVal - minVal;
    uint16_t valuePos = currentVal - minVal;
    uint16_t val = (valuePos * 255UL) / totalRange;

    return (val > 255) ? 255 : val;
}

byte get_brightness_from_sensorValue_simetric(LAST_ENTRY_FRAME_T LEFin) {
    byte minMSB = LEFin.data[0];
    byte minLSB = LEFin.data[1];
    byte maxMSB = LEFin.data[2];
    byte maxLSB = LEFin.data[3];
    byte valMSB = LEFin.data[4];
    byte valLSB = LEFin.data[5];
    uint16_t minVal = (minMSB << 8) | minLSB;
    uint16_t maxVal = (maxMSB << 8) | maxLSB;
    uint16_t currentVal = (valMSB << 8) | valLSB;

    uint16_t midPoint = (maxVal + minVal) / 2;
    uint16_t val;

    if (currentVal <= midPoint) {
        // De 0 a 1000 (midPoint), el brillo va de 255 a 0
        val = 255 - ((currentVal - minVal) * 255UL) / (midPoint - minVal);
    } else {
        // De 1000 (midPoint) a 2000, el brillo va de 0 a 255
        val = ((currentVal - midPoint) * 255UL) / (maxVal - midPoint);
    }

    return (byte)val;
}

byte get_brightness_from_sensorValue(LAST_ENTRY_FRAME_T LEFin) {

    byte minMSB= LEFin.data[0];
    byte minLSB= LEFin.data[1];
    byte maxMSB= LEFin.data[2];
    byte maxLSB= LEFin.data[3];
    byte valMSB= LEFin.data[4];
    byte valLSB= LEFin.data[5];
    uint16_t minVal = (minMSB << 8) | minLSB;
    uint16_t maxVal = (maxMSB << 8) | maxLSB;
    uint16_t currentVal = (valMSB << 8) | valLSB;

    uint16_t totalRange = maxVal - minVal;
    uint16_t valuePos = currentVal - minVal;
    uint16_t val = (valuePos * 255UL) / totalRange;

    return (val > 255) ? 255 : val;
}

float get_aux_var_01_from_sensorValue(LAST_ENTRY_FRAME_T LEFin) {
    byte minMSB = LEFin.data[0];
    byte minLSB = LEFin.data[1];
    byte maxMSB = LEFin.data[2];
    byte maxLSB = LEFin.data[3];
    byte valMSB = LEFin.data[4];
    byte valLSB = LEFin.data[5];

    uint16_t minVal = (minMSB << 8) | minLSB;
    uint16_t maxVal = (maxMSB << 8) | maxLSB;
    uint16_t currentVal = (valMSB << 8) | valLSB;

    // Asegurarnos de que no haya división por cero
    if (maxVal <= minVal) {
        return 0.0f; // Devuelve un valor por defecto si los límites son inválidos
    }

    uint16_t totalRange = maxVal - minVal;
    uint16_t valuePos = currentVal - minVal;

    // Escalar el valor al rango [0, 10]
    float val = (float(valuePos) / totalRange) * 10.0f;

    // Asegurar que el valor esté dentro del rango [0, 10]
    if (val < 0.0f) val = 0.0f;
    if (val > 10.0f) val = 10.0f;

    return val;
}


byte get_color_from_sensorValue(LAST_ENTRY_FRAME_T LEFin) {
    byte minMSB = LEFin.data[0];
    byte minLSB = LEFin.data[1];
    byte maxMSB = LEFin.data[2];
    byte maxLSB = LEFin.data[3];
    byte valMSB = LEFin.data[4];
    byte valLSB = LEFin.data[5];
    uint16_t minVal = (minMSB << 8) | minLSB;
    uint16_t maxVal = (maxMSB << 8) | maxLSB;
    uint16_t currentVal = (valMSB << 8) | valLSB;
    
    // if (currentVal < minVal) currentVal = minVal;
    // if (currentVal > maxVal) currentVal = maxVal;
    
    float proportion = (float)(currentVal - minVal) / (maxVal - minVal);
    uint16_t result = (uint16_t)(proportion * 19);
    if (result < 00) return 0;
    if (result > 19) return 19;
    
    return (byte)result;
}

#define SIZE 64

uint16_t unidimensional_table[SIZE * SIZE];

// void fillTable() {
//     for (int row = 0; row < SIZE; row++) {
//         for (int col = 0; col < SIZE; col++) {
//             unidimensional_table[row * SIZE + col] = row + 1;
//         }
//     }
// }


void  get_sector_data(byte *sector_data, byte lang, byte sector){
                                                                                            #ifdef DEBUG
                                                                                            DEBUG__________ln("Aqui hem entrat a get_sector_data.");
                                                                                            DEBUG__________ln(("GSD_Lang: " + String(lang)));
                                                                                            DEBUG__________ln(("GSD_Sector: " + String(sector)));
                                                                                            #endif
    memset(sector_data, 0, 192);

    if (sector >= ELEM_ICON_ROW_0_SECTOR && sector <= ELEM_ICON_ROW_63_SECTOR) {
        int startIndex = (sector - ELEM_ICON_ROW_0_SECTOR) * 64;
       
        for (int i = 0; i < 64; i++) {
            sector_data[i * 2] = elem_icon[startIndex + i] >> 8;     // MSB
            sector_data[i * 2 + 1] = elem_icon[startIndex + i] & 0xFF; // LSB
        }
    }
    
    String data;
    switch(sector){
        case ELEM_NAME_SECTOR:
            data = get_string_from_info_DB(ELEM_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;

        case ELEM_DESC_SECTOR:
            data = get_string_from_info_DB(ELEM_DESC, lang);
                                                                                                    #ifdef DEBUG
                                                                                                    DEBUG__________ln("Data: " + String(data));
                                                                                                    #endif
            data.toCharArray((char*)sector_data, 192);
            break;

        case ELEM_SERIAL_SECTOR: {
            String macNumbers = WiFi.macAddress();
            macNumbers = macNumbers.substring(9);
            macNumbers.replace(":", "");

            String serialPrefix = "";

            #ifdef SERIAL_BY_FILE
                File file = SPIFFS.open(ELEMENT_SERIALNUM_FILE_PATH, "r");
                if (file) {
                    serialPrefix = file.readString();
                    file.close();
                }
            #else
                // Convertir el valor hexadecimal a una cadena de 4 caracteres
                uint16_t serialHex = SERIAL_NUM;
                char buffer[5];
                snprintf(buffer, sizeof(buffer), "%04X", serialHex);
                serialPrefix = String(buffer);
            #endif

            while (serialPrefix.length() < 4) serialPrefix = "0" + serialPrefix;

            sector_data[0] = strtoul(serialPrefix.substring(0, 2).c_str(), nullptr, 16);
            sector_data[1] = strtoul(serialPrefix.substring(2, 4).c_str(), nullptr, 16);
            sector_data[2] = strtoul(macNumbers.substring(0, 2).c_str(), nullptr, 16);
            sector_data[3] = strtoul(macNumbers.substring(2, 4).c_str(), nullptr, 16);
            sector_data[4] = strtoul(macNumbers.substring(4, 6).c_str(), nullptr, 16);

            break;
        }




        case ELEM_ID_SECTOR:
            sector_data[0] = globalID;
            break;

        case ELEM_CMODE_SECTOR:
            sector_data[0] = element->get_currentMode();
            break;

        case ELEM_MODE_0_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_0_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      
        
        case ELEM_MODE_0_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_0_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_0_FLAG_SECTOR:   
            sector_data[0]= (get_config_flag_mode(0) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(0) & 0xFF;
            break;  

        case ELEM_MODE_1_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_1_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_1_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_1_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;      

        case ELEM_MODE_1_FLAG_SECTOR:  
            sector_data[0]= (get_config_flag_mode(1) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(1) & 0xFF;
            break;  

        case ELEM_MODE_2_NAME_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_2_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;                                                          

        case ELEM_MODE_2_DESC_SECTOR:    
            data = get_string_from_info_DB(ELEM_MODE_2_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_2_FLAG_SECTOR:   
            sector_data[0]= (get_config_flag_mode(2) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(2) & 0xFF;
            break;      

        case ELEM_MODE_3_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_3_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;

        case ELEM_MODE_3_DESC_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_3_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;      

        case ELEM_MODE_3_FLAG_SECTOR:   
            sector_data[0]= (get_config_flag_mode(3) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(3) & 0xFF;
            break;  

        case ELEM_MODE_4_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_4_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      

        case ELEM_MODE_4_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_4_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_4_FLAG_SECTOR:   
            sector_data[0]= (get_config_flag_mode(4) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(4) & 0xFF;
            break;

        case ELEM_MODE_5_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_5_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;

        case ELEM_MODE_5_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_5_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

            sector_data[0]= (get_config_flag_mode(5) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(5) & 0xFF;
            break;  

        case ELEM_MODE_6_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_6_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_6_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_6_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;

        case ELEM_MODE_6_FLAG_SECTOR:       
            sector_data[0]= (get_config_flag_mode(6) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(6) & 0xFF;
            break;

        case ELEM_MODE_7_NAME_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_7_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;

        case ELEM_MODE_7_DESC_SECTOR:   
            data = get_string_from_info_DB(ELEM_MODE_7_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;

        case ELEM_MODE_7_FLAG_SECTOR:       
            sector_data[0]= (get_config_flag_mode(7) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(7) & 0xFF;
            break;  

        case ELEM_MODE_8_NAME_SECTOR:               
            data = get_string_from_info_DB(ELEM_MODE_8_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      

        case ELEM_MODE_8_DESC_SECTOR:               
            data = get_string_from_info_DB(ELEM_MODE_8_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;      

        case ELEM_MODE_8_FLAG_SECTOR:        
            sector_data[0]= (get_config_flag_mode(8) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(8) & 0xFF;
            break;  

        case ELEM_MODE_9_NAME_SECTOR:        
            data = get_string_from_info_DB(ELEM_MODE_9_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      

        case ELEM_MODE_9_DESC_SECTOR:               
            data = get_string_from_info_DB(ELEM_MODE_9_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_9_FLAG_SECTOR:        
            sector_data[0]= (get_config_flag_mode(9) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(9) & 0xFF;
            break;

        case ELEM_MODE_10_NAME_SECTOR:               
            data = get_string_from_info_DB(ELEM_MODE_10_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_10_DESC_SECTOR:         
            data = get_string_from_info_DB(ELEM_MODE_10_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_10_FLAG_SECTOR:          
            sector_data[0]= (get_config_flag_mode(10) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(10) & 0xFF;
            break;      

        case ELEM_MODE_11_NAME_SECTOR:      
            data = get_string_from_info_DB(ELEM_MODE_11_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_11_DESC_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_11_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;      

        case ELEM_MODE_11_FLAG_SECTOR:      
            sector_data[0]= (get_config_flag_mode(11) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(11) & 0xFF;
            break;  

        case ELEM_MODE_12_NAME_SECTOR:    
            data = get_string_from_info_DB(ELEM_MODE_12_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      

        case ELEM_MODE_12_DESC_SECTOR:     
            data = get_string_from_info_DB(ELEM_MODE_12_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_12_FLAG_SECTOR:      
            sector_data[0]= (get_config_flag_mode(12) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(12) & 0xFF;
            break;

        case ELEM_MODE_13_NAME_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_13_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;      

        case ELEM_MODE_13_DESC_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_13_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_13_FLAG_SECTOR:       
            sector_data[0]= (get_config_flag_mode(13) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(13) & 0xFF;
            break;  

        case ELEM_MODE_14_NAME_SECTOR:               
            data = get_string_from_info_DB(ELEM_MODE_14_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_14_DESC_SECTOR:         
            data = get_string_from_info_DB(ELEM_MODE_14_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_14_FLAG_SECTOR:          
            sector_data[0]= (get_config_flag_mode(14) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(14) & 0xFF;
            break;      

        case ELEM_MODE_15_NAME_SECTOR:         
            data = get_string_from_info_DB(ELEM_MODE_15_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;  

        case ELEM_MODE_15_DESC_SECTOR:       
            data = get_string_from_info_DB(ELEM_MODE_15_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;  

        case ELEM_MODE_15_FLAG_SECTOR:      
            sector_data[0]= (get_config_flag_mode(15) >> 8) & 0xFF;
            sector_data[1]=  get_config_flag_mode(15) & 0xFF;
            break;

     case ELEM_MOST_USED_MODE_SECTOR:{
        sector_data[0]= (element->get_most_used_mode()) & 0xFF;
         break;
     }

    case ELEM_MOST_USED_COLOR_SECTOR:{
        sector_data[0]= (element->get_most_used_color()) & 0xFF;
            break;
    }
    case ELEM_MOST_USED_PATTERN_SECTOR:{
// pending
        break;
    }
    case ELEM_TOTAL_SESSION_TIME_SECTOR:{
        sector_data[0]= (element->get_total_session_time() >> 24) & 0xFF;
        sector_data[1]= (element->get_total_session_time() >> 16) & 0xFF;
        sector_data[2]= (element->get_total_session_time() >> 8) & 0xFF;
        sector_data[3]=  element->get_total_session_time() & 0xFF;
        break;
    }
    
    case ELEM_CURRENT_COLOR_SECTOR:
        sector_data[0]= 0x00;
        sector_data[1]= colorHandler.get_current_red(colorHandler.get_currentColor());
        sector_data[2]= colorHandler.get_current_green(colorHandler.get_currentColor());
        sector_data[3]= colorHandler.get_current_blue(colorHandler.get_currentColor());
        break;

    case ELEM_CURRENT_RED_SECTOR:
        sector_data[0]= colorHandler.get_current_red(colorHandler.get_currentColor());
        break;

    case ELEM_CURRENT_GREEN_SECTOR:
        sector_data[0]= colorHandler.get_current_green(colorHandler.get_currentColor());

        break;

    case ELEM_CURRENT_BLUE_SECTOR:
        sector_data[0]= colorHandler.get_current_blue(colorHandler.get_currentColor());
        break;

    case ELEM_CURRENT_BRIGHTNESS_SECTOR:
        sector_data[0] = colorHandler.get_currentBrightness();
        break;  

    case ELEM_CURRENT_PATTERN_SECTOR:
        sector_data[0] = element->activePattern;
        break;      
        
    case ELEM_CURRENT_FLAGS_SECTOR:
        sector_data[0]= element->get_flag();
        break;      

    // case ELEM_CURRENT_FILE_SECTOR:
    //         sector_data[0] = element->get_currentFile()        & 0xFF;
    //         sector_data[1] = (element->get_currentFile() >> 8) & 0xFF;
    //         break; 

    // case ELEM_CURRENT_XMANAGER_SECTOR:
    //         sector_data[0] = xManager;
    //         break;
    
    
        default: break;
    }
}







/*
////////////////////////////////////////////////////////////////////////////////////////////
                                           FRAME MAKERS
////////////////////////////////////////////////////////////////////////////////////////////
*/



FRAME_T frameMaker_REQ_ELEM_SECTOR(byte originin, byte targetin, byte idiomain, byte sectorin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_REQ_ELEM_SECTOR);
    uint16_t  frameLength = 0x08 + L_REQ_ELEM_SECTOR;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;      
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.function= F_REQ_ELEM_SECTOR;
    frame.dataLengthMsb = (L_REQ_ELEM_SECTOR >> 8) & 0xFF; 
    frame.dataLengthLsb = L_REQ_ELEM_SECTOR & 0xFF;   
    frame.data[0]= idiomain;
    frame.data[1]= sectorin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}



FRAME_T frameMaker_SET_ELEM_ID(byte originin, byte targetin, byte IDin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SET_ELEM_ID);
    uint16_t  frameLength = 0x08 + L_SET_ELEM_ID;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;        
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.function= F_SET_ELEM_ID;
    frame.dataLengthMsb = (L_SET_ELEM_ID >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SET_ELEM_ID & 0xFF; 
    frame.data[0]= IDin;  
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SET_ELEM_MODE(byte originin, std::vector<byte>targetin, byte modein){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SET_ELEM_MODE);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SET_ELEM_MODE;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;        
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SET_ELEM_MODE;
    frame.dataLengthMsb = (L_SET_ELEM_MODE >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SET_ELEM_MODE & 0xFF;   
    frame.data[0]= modein;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SET_ELEM_DEAF(byte originin, std::vector<byte>targetin, byte timein){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SET_ELEM_MODE);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SET_ELEM_DEAF;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;        
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SET_ELEM_DEAF;
    frame.dataLengthMsb = (L_SET_ELEM_DEAF >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SET_ELEM_DEAF & 0xFF;   
    frame.data[0]= timein;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;    
}
//////////////////////////////////////////////////////////////////////



FRAME_T frameMaker_SEND_COLOR(byte originin, std::vector<byte>targetin, byte colorin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_COLOR);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_COLOR;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_COLOR;
    frame.dataLengthMsb = (L_SEND_COLOR >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_COLOR & 0xFF;   
    frame.data[0]= colorin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SEND_RGB (byte originin, std::vector<byte>targetin, COLOR_T colorin){
    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_RGB);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_RGB;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_RGB;
    frame.dataLengthMsb = (L_SEND_RGB >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_RGB & 0xFF;
    frame.data[0]= colorin.red;
    frame.data[1]= colorin.green;
    frame.data[2]= colorin.blue;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SEND_SENSOR_VALUE(byte originin, std::vector<byte>targetin, SENSOR_VALUE_T sensorin){
    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_SENSOR_VALUE_1);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_SENSOR_VALUE_1;
    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_SENSOR_VALUE_1;
    frame.dataLengthMsb = (L_SEND_SENSOR_VALUE_1 >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_SENSOR_VALUE_1 & 0xFF;   
    frame.data[0]= sensorin.msb_min;
    frame.data[1]= sensorin.lsb_min;
    frame.data[2]= sensorin.msb_max;
    frame.data[3]= sensorin.lsb_max;
    frame.data[4]= sensorin.msb_val;
    frame.data[5]= sensorin.lsb_val;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;
    return frame;
}

FRAME_T frameMaker_SEND_SENSOR_VALUE_2(byte originin, std::vector<byte>targetin, SENSOR_VALUE_T sensorin){
    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_SENSOR_VALUE_2);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_SENSOR_VALUE_2;
    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_SENSOR_VALUE_2;
    frame.dataLengthMsb = (L_SEND_SENSOR_VALUE_2 >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_SENSOR_VALUE_2 & 0xFF;   
    frame.data[0]= sensorin.msb_min;
    frame.data[1]= sensorin.lsb_min;
    frame.data[2]= sensorin.msb_max;
    frame.data[3]= sensorin.lsb_max;
    frame.data[4]= sensorin.msb_val;
    frame.data[5]= sensorin.lsb_val;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;
    return frame;
}

FRAME_T frameMaker_SEND_FLAG_BYTE(byte originin, std::vector<byte>targetin, byte flagin){
    
    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_FLAG_BYTE);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_FLAG_BYTE;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_FLAG_BYTE;
    frame.dataLengthMsb = (L_SEND_FLAG_BYTE >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_FLAG_BYTE & 0xFF;   
    frame.data[0]= flagin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SEND_PATTERN_NUM(byte originin, std::vector<byte>targetin, byte patternin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_PATTERN_NUM);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_PATTERN_NUM;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_PATTERN_NUM;
    frame.dataLengthMsb = (L_SEND_PATTERN_NUM >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_PATTERN_NUM & 0xFF;   
    frame.data[0]= patternin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

FRAME_T frameMaker_SEND_FILE_NUM(byte originin, std::vector<byte>targetin, byte bankin, byte filein){
    
    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_FILE_NUM);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_FILE_NUM;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_FILE_NUM;
    frame.dataLengthMsb = (L_SEND_FILE_NUM >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_FILE_NUM & 0xFF;   
    frame.data[0]= bankin;
    frame.data[1]= filein;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}


FRAME_T frameMaker_SEND_COMMAND(byte originin, std::vector<byte>targetin, byte commandin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_COMMAND);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_COMMAND;
    frame.data.resize(L_SEND_COMMAND);

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_COMMAND;
    frame.dataLengthMsb = (L_SEND_COMMAND >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_COMMAND & 0xFF;   
    frame.data[0]= commandin;
    frame.function= F_SEND_COMMAND;
    frame.dataLengthMsb = (L_SEND_COMMAND >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_COMMAND & 0xFF;   
    frame.data[0]= commandin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////


FRAME_T frameMaker_RETURN_ELEM_SECTOR (byte originin, byte targetin, byte *sector_data, byte sectorin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    uint16_t  frameLength= 0;
    frame.start= NEW_START;
    frame.origin= originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.data.resize(0);
    frame.function= F_RETURN_ELEM_SECTOR;
                                                                                        #ifdef DEBUG
                                                                                            DEBUG__________ln("Sector a enviar: " + String(sectorin, HEX));
                                                                                        #endif
    frame.data.push_back(sectorin);
    switch (sectorin) {
            
             case ELEM_SERIAL_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_05 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_05 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_05 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_05; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

            case ELEM_ID_SECTOR:
            case ELEM_CMODE_SECTOR:
            case ELEM_MOST_USED_MODE_SECTOR:
            case ELEM_MOST_USED_COLOR_SECTOR:
            case ELEM_MOST_USED_PATTERN_SECTOR:
            case ELEM_CURRENT_RED_SECTOR:
            case ELEM_CURRENT_GREEN_SECTOR:
            case ELEM_CURRENT_BLUE_SECTOR:
            case ELEM_CURRENT_BRIGHTNESS_SECTOR:
            case ELEM_CURRENT_PATTERN_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_01 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_01 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_01 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_01; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;
// serial falta
            case ELEM_DESC_SECTOR:
                                                                                        #ifdef DEBUG
                                                                                        DEBUG__________ln("Se ha pedido una descripción del elemento. ");
                                                                                        #endif
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_192 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_192 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_192 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_192; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

            case ELEM_NAME_SECTOR:
            case ELEM_MODE_0_NAME_SECTOR:
            case ELEM_MODE_1_NAME_SECTOR:
            case ELEM_MODE_2_NAME_SECTOR:
            case ELEM_MODE_3_NAME_SECTOR:
            case ELEM_MODE_4_NAME_SECTOR:
            case ELEM_MODE_5_NAME_SECTOR:
            case ELEM_MODE_6_NAME_SECTOR:
            case ELEM_MODE_7_NAME_SECTOR:
            case ELEM_MODE_8_NAME_SECTOR:
            case ELEM_MODE_9_NAME_SECTOR:
            case ELEM_MODE_10_NAME_SECTOR:
            case ELEM_MODE_11_NAME_SECTOR:
            case ELEM_MODE_12_NAME_SECTOR:
            case ELEM_MODE_13_NAME_SECTOR:
            case ELEM_MODE_14_NAME_SECTOR:
            case ELEM_MODE_15_NAME_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_24 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_24 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_24 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_24; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

            case ELEM_MODE_0_DESC_SECTOR: 
            case ELEM_MODE_1_DESC_SECTOR:
            case ELEM_MODE_2_DESC_SECTOR: 
            case ELEM_MODE_3_DESC_SECTOR:
            case ELEM_MODE_4_DESC_SECTOR: 
            case ELEM_MODE_5_DESC_SECTOR:
            case ELEM_MODE_6_DESC_SECTOR: 
            case ELEM_MODE_7_DESC_SECTOR:
            case ELEM_MODE_8_DESC_SECTOR: 
            case ELEM_MODE_9_DESC_SECTOR:
            case ELEM_MODE_10_DESC_SECTOR: 
            case ELEM_MODE_11_DESC_SECTOR:
            case ELEM_MODE_12_DESC_SECTOR: 
            case ELEM_MODE_13_DESC_SECTOR:
            case ELEM_MODE_14_DESC_SECTOR: 
            case ELEM_MODE_15_DESC_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_192 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_192 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_192 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_192; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

           
            case ELEM_MODE_0_FLAG_SECTOR: 
            case ELEM_MODE_1_FLAG_SECTOR:
            case ELEM_MODE_2_FLAG_SECTOR: 
            case ELEM_MODE_3_FLAG_SECTOR:
            case ELEM_MODE_4_FLAG_SECTOR: 
            case ELEM_MODE_5_FLAG_SECTOR:
            case ELEM_MODE_6_FLAG_SECTOR: 
            case ELEM_MODE_7_FLAG_SECTOR:
            case ELEM_MODE_8_FLAG_SECTOR: 
            case ELEM_MODE_9_FLAG_SECTOR:
            case ELEM_MODE_10_FLAG_SECTOR: 
            case ELEM_MODE_11_FLAG_SECTOR:
            case ELEM_MODE_12_FLAG_SECTOR: 
            case ELEM_MODE_13_FLAG_SECTOR:
            case ELEM_MODE_14_FLAG_SECTOR: 
            case ELEM_MODE_15_FLAG_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_02 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_02 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_02 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_02; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

            case ELEM_ICON_ROW_0_SECTOR:
            case ELEM_ICON_ROW_1_SECTOR:
            case ELEM_ICON_ROW_2_SECTOR:
            case ELEM_ICON_ROW_3_SECTOR:
            case ELEM_ICON_ROW_4_SECTOR:
            case ELEM_ICON_ROW_5_SECTOR:
            case ELEM_ICON_ROW_6_SECTOR:
            case ELEM_ICON_ROW_7_SECTOR:
            case ELEM_ICON_ROW_8_SECTOR:
            case ELEM_ICON_ROW_9_SECTOR:
            case ELEM_ICON_ROW_10_SECTOR:
            case ELEM_ICON_ROW_11_SECTOR:
            case ELEM_ICON_ROW_12_SECTOR:
            case ELEM_ICON_ROW_13_SECTOR:
            case ELEM_ICON_ROW_14_SECTOR:
            case ELEM_ICON_ROW_15_SECTOR:
            case ELEM_ICON_ROW_16_SECTOR:
            case ELEM_ICON_ROW_17_SECTOR:   
            case ELEM_ICON_ROW_18_SECTOR:
            case ELEM_ICON_ROW_19_SECTOR:
            case ELEM_ICON_ROW_20_SECTOR:
            case ELEM_ICON_ROW_21_SECTOR:
            case ELEM_ICON_ROW_22_SECTOR:
            case ELEM_ICON_ROW_23_SECTOR:
            case ELEM_ICON_ROW_24_SECTOR:
            case ELEM_ICON_ROW_25_SECTOR:
            case ELEM_ICON_ROW_26_SECTOR:
            case ELEM_ICON_ROW_27_SECTOR:
            case ELEM_ICON_ROW_28_SECTOR:
            case ELEM_ICON_ROW_29_SECTOR:
            case ELEM_ICON_ROW_30_SECTOR:
            case ELEM_ICON_ROW_31_SECTOR:
            case ELEM_ICON_ROW_32_SECTOR:
            case ELEM_ICON_ROW_33_SECTOR:
            case ELEM_ICON_ROW_34_SECTOR:
            case ELEM_ICON_ROW_35_SECTOR:
            case ELEM_ICON_ROW_36_SECTOR:
            case ELEM_ICON_ROW_37_SECTOR:
            case ELEM_ICON_ROW_38_SECTOR:
            case ELEM_ICON_ROW_39_SECTOR:
            case ELEM_ICON_ROW_40_SECTOR:
            case ELEM_ICON_ROW_41_SECTOR:
            case ELEM_ICON_ROW_42_SECTOR:
            case ELEM_ICON_ROW_43_SECTOR:
            case ELEM_ICON_ROW_44_SECTOR:
            case ELEM_ICON_ROW_45_SECTOR:
            case ELEM_ICON_ROW_46_SECTOR:
            case ELEM_ICON_ROW_47_SECTOR:
            case ELEM_ICON_ROW_48_SECTOR:
            case ELEM_ICON_ROW_49_SECTOR:
            case ELEM_ICON_ROW_50_SECTOR:
            case ELEM_ICON_ROW_51_SECTOR:
            case ELEM_ICON_ROW_52_SECTOR:
            case ELEM_ICON_ROW_53_SECTOR:
            case ELEM_ICON_ROW_54_SECTOR:
            case ELEM_ICON_ROW_55_SECTOR:
            case ELEM_ICON_ROW_56_SECTOR:
            case ELEM_ICON_ROW_57_SECTOR:
            case ELEM_ICON_ROW_58_SECTOR:
            case ELEM_ICON_ROW_59_SECTOR:
            case ELEM_ICON_ROW_60_SECTOR:
            case ELEM_ICON_ROW_61_SECTOR:
            case ELEM_ICON_ROW_62_SECTOR:
            case ELEM_ICON_ROW_63_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_128 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_128 + 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_128 + 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_128; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

            case ELEM_TOTAL_SESSION_TIME_SECTOR:
            case ELEM_CURRENT_COLOR_SECTOR:
                frameLength = 0x08 + L_RETURN_ELEM_SECTOR_04 + 1;
                frame.frameLengthLsb = frameLength & 0xFF;     
                frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
                frame.dataLengthMsb = ((L_RETURN_ELEM_SECTOR_04+ 1) >> 8) & 0xFF; 
                frame.dataLengthLsb = (L_RETURN_ELEM_SECTOR_04+ 1) & 0xFF;
                for (int i = 0; i < L_RETURN_ELEM_SECTOR_04; i++) {
                    frame.data.push_back(sector_data[i]);
                }
                break;

        default:
        #ifdef DEBUG
         DEBUG__________ln("Sector no valido");
         #endif
                break;
    }
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;
    return frame;
}

FRAME_T frameMaker_SEND_RESPONSE (byte originin, std::vector<byte>targetin, byte response){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_RESPONSE);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_RESPONSE;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_RESPONSE;
    frame.dataLengthMsb = (L_SEND_RESPONSE>> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_RESPONSE & 0xFF;
    frame.data[0]= response;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}






































































//////////////////////////////////////////////////////////////////////////////////////

/*
FRAME_T frameMaker_RETURN_ELEM_INFO(byte originin, byte targetin, INFO_PACK_T* infoPack) {
    FRAME_T frame;
    uint16_t dataLength = L_RETURN_ELEM_INFO;
    uint16_t length = 0x08 + L_RETURN_ELEM_INFO;

    frame.start = NEW_START;
    frame.frameLengthMsb = (length >> 8) & 0xFF;
    frame.frameLengthLsb = length & 0xFF;
    frame.origin = originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.function = F_RETURN_ELEM_INFO;
    frame.dataLengthMsb = (dataLength >> 8) & 0xFF;
    frame.dataLengthLsb = dataLength & 0xFF;
    frame.data.reserve(sizeof(*infoPack));
    // Ahora usamos el operador -> para acceder a los miembros del puntero
    frame.data.insert(frame.data.end(),                          // NAME
                     infoPack->name,
                     infoPack->name + sizeof(infoPack->name));
    frame.data.insert(frame.data.end(),                         // DESC
                     infoPack->desc,
                     infoPack->desc + sizeof(infoPack->desc));
    frame.data.insert(frame.data.end(),                        // SERIALNUM
                     infoPack->serialNum,
                     infoPack->serialNum + sizeof(infoPack->serialNum));
    frame.data.push_back(infoPack->ID);                        // ID
    frame.data.push_back(infoPack->currentMode);               // CURRENTMODE
    for (int i = 0; i < 16; i++) {
        frame.data.insert(frame.data.end(),
                         infoPack->mode[i].name,
                         infoPack->mode[i].name + sizeof(infoPack->mode[i].name));
        frame.data.insert(frame.data.end(),
                         infoPack->mode[i].desc,
                         infoPack->mode[i].desc + sizeof(infoPack->mode[i].desc));
        frame.data.insert(frame.data.end(),
                         infoPack->mode[i].config,
                         infoPack->mode[i].config + 2);
    }
    const uint8_t* iconPtr = reinterpret_cast<const uint8_t*>(infoPack->icono);
    size_t iconSize = sizeof(infoPack->icono); // = 8192
    frame.data.insert(frame.data.end(), iconPtr, iconPtr + iconSize);
        
    frame.checksum = checksum_calc(frame);
    frame.end = NEW_END;

    return frame;
}

*/

























































