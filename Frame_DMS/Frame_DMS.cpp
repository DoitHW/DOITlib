#include <defines_DMS/defines_DMS.h>
#include <Frame_DMS/Frame_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <column_DMS/column_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <vector>
#include <Arduino.h>


extern LAST_ENTRY_FRAME_T            LEF;
extern byte globalID;
extern byte xManager;
volatile bool startFrameReceived=    false;
volatile bool frameInProgress=       false;
volatile bool partialFrameReceived=  false;
volatile bool frameReceived=         false;

std::vector<byte> uartBuffer;

volatile unsigned long last_received_time = 0;
int lastReceivedTime = 0;

volatile bool uartInterruptTriggered= false;

// LA FUNCION DIOS

void IRAM_ATTR onUartInterrupt() {
    unsigned long startTime = millis();
    static enum {
        WAITING_START,
        RECEIVING_LENGTH_MSB,
        RECEIVING_LENGTH_LSB,
        RECEIVING_FRAME
    } receiveState = WAITING_START;

    static uint16_t expectedFrameLength = 0;    // Longitud del frame (sin NEW_START y LENGTH bytes)
    static uint16_t totalFrameLength = 0;       // Longitud total (frameLength + 3)
    static uint16_t receivedBytes = 0;          // OJO con los statics, danger.
    static uint16_t calculatedChecksum = 0;
    static uint8_t receivedChecksum = 0;
    

    uint8_t bytesProcessed = 0;
    static int count = 0;
    
    while (Serial1.available()) {
        byte receivedByte = Serial1.read();
        Serial.print(receivedByte, HEX);
        Serial.print(" ");
        count ++;
        bytesProcessed++;
        lastReceivedTime = millis();
        if (receiveState == WAITING_START && receivedByte != NEW_START) continue;

        switch (receiveState) {
            case WAITING_START:
                if (receivedByte == NEW_START) {
                    
                    uartBuffer.reserve(MAX_FRAME_LENGTH);
                    uartBuffer.clear();
                    uartBuffer.push_back(receivedByte);

                    calculatedChecksum = receivedByte;
                    receiveState = RECEIVING_LENGTH_MSB;
                    expectedFrameLength = 0;
                    receivedBytes = 1;
                }
                break;

            case RECEIVING_LENGTH_MSB:
                uartBuffer.push_back(receivedByte);
                calculatedChecksum += receivedByte;
                while (calculatedChecksum > 0xFF) calculatedChecksum = (calculatedChecksum & 0xFF) + (calculatedChecksum >> 8);
                expectedFrameLength = receivedByte << 8;  // Guardamos el MSB
                receiveState = RECEIVING_LENGTH_LSB;
                break;

            case RECEIVING_LENGTH_LSB:
                uartBuffer.push_back(receivedByte);
                calculatedChecksum += receivedByte;
                while (calculatedChecksum > 0xFF) calculatedChecksum = (calculatedChecksum & 0xFF) + (calculatedChecksum >> 8);
                expectedFrameLength |= receivedByte;  
                totalFrameLength = expectedFrameLength + 3;  

                if (totalFrameLength > MAX_FRAME_LENGTH || totalFrameLength < MIN_FRAME_LENGTH) {
                    
                    #ifdef DEBUG
                        Serial.println("❌ Error: Longitud de trama inv\xE1lida");
                    #endif
                    receiveState = WAITING_START;
                    uartBuffer.clear();
                    return;
                }

                receiveState = RECEIVING_FRAME;
                receivedBytes = 3;  // Contamos NEW_START, LENGTH_MSB y LENGTH_LSB
                break;

            case RECEIVING_FRAME:
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
                            
                            for (uint8_t i = 0; i < numTargets; i++) {
                                uint8_t targetID = uartBuffer[5 + i];
                                if (targetID == globalID || targetID == BROADCAST || targetID == xManager)  {
                                    isTarget = true;
                                    break;
                                }
                            }

                            if (isTarget) {
                                frameReceived = true;
                                                                #ifdef DEBUG
                                                                    Serial.println("✅ Trama recibida correctamente y dirigida al dispositivo");
                                                                #endif
                            } else {
                                                                #ifdef DEBUG
                                                                    Serial.println("❌ Trama recibida correctamente pero no dirigida al dispositivo");
                                                                #endif
                            }
                        } else {
                            
                                                                #ifdef DEBUG
                                                                    Serial.println("❌ Error: Checksum inv\xE1lido");
                                                                #endif
                        }
                    } else {
                    
                                                                #ifdef DEBUG
                                                                    Serial.println("❌ Error: Byte de fin incorrecto");
                                                                #endif
                    }
            
                    Serial.println("Total de bytes recibidos: " + String(count));
                    count = 0;
                    receiveState = WAITING_START;
                    return;
                } 

                if (uartBuffer.size() >= MAX_BUFFER_SIZE) {
                                                                #ifdef DEBUG
                                                                    Serial.println("Error: Desbordamiento de buffer UART");
                                                                #endif
                    receiveState = WAITING_START;
                    uartBuffer.clear();
                    return;
                }

                break;
        }
    }

    if (Serial1.available() && bytesProcessed >= MAX_BYTES_PER_INTERRUPT) {
        partialFrameReceived = true;
                                                            #ifdef DEBUG
                                                                Serial.println("Trama parcial recibida");
                                                            #endif
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


LAST_ENTRY_FRAME_T extract_info_from_frameIn(std::vector<byte> vectorin) {
    LAST_ENTRY_FRAME_T LEF;
    LEF.origin = vectorin[3]; 
    LEF.function = vectorin[5 + vectorin[4]];
    int dataLength = (vectorin[5 + vectorin[4] + 1] << 8) | vectorin[5 + vectorin[4] + 2];
    for (int i = 0; i < dataLength; i++)  LEF.data.push_back(vectorin[5 + vectorin[4] + 3 + i]);

    return LEF;
}


void send_frame(const FRAME_T &framein) {
    int i = 0;
    byte dTime = 5;

    Serial.println(" #### Trama enviada ####");
    Serial1.write(framein.start);           Serial.println("[" + String(++i) + "] START = " + String(framein.start, HEX)); delay(dTime);
    Serial1.write(framein.frameLengthMsb);  Serial.println("[" + String(++i) + "] Frame Length Msb = " + String(framein.frameLengthMsb, HEX)); delay(dTime);
    Serial1.write(framein.frameLengthLsb);  Serial.println("[" + String(++i) + "] Frame Length Lsb = " + String(framein.frameLengthLsb, HEX)); delay(dTime);
    Serial1.write(framein.origin);          Serial.println("[" + String(++i) + "] Origen = " + String(framein.origin, HEX)); delay(dTime);
    Serial1.write(framein.numTargets);      Serial.println("[" + String(++i) + "] numTargets = " + String(framein.numTargets, HEX)); delay(dTime);
    int dataIndex = 0;
    for (byte target : framein.target) {
        Serial1.write(target); Serial.println("[" + String(++i) + "] Target[" + String(dataIndex++) + "] = " + String(target, HEX)); delay(dTime);
    }
    Serial1.write(framein.function);        Serial.println("[" + String(++i) + "] Function = " + String(framein.function, HEX)); delay(dTime);
    Serial1.write(framein.dataLengthMsb);   Serial.println("[" + String(++i) + "] Data Length Msb = " + String(framein.dataLengthMsb, HEX)); delay(dTime);
    Serial1.write(framein.dataLengthLsb);   Serial.println("[" + String(++i) + "] Data Length Lsb = " + String(framein.dataLengthLsb, HEX)); delay(dTime);
    dataIndex = 0;
    for (byte dato : framein.data) {
        Serial1.write(dato); Serial.println("[" + String(++i) + "] Data[" + String(dataIndex++) + "] = " + String(dato, HEX)); delay(dTime);
    }
    Serial1.write(framein.checksum);        Serial.println("[" + String(++i) + "] Checksum = " + String(framein.checksum, HEX)); delay(dTime);
    Serial1.write(framein.end);             Serial.println("[" + String(++i) + "] End = " + String(framein.end, HEX)); delay(dTime);
    Serial.println("======================================");

    //Serial.println("Trama enviada, bytes: " + String(i));
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




void  get_sector_data(byte *sector_data, byte lang, byte sector){

    String data;
    switch(sector){
        case ELEM_NAME_SECTOR:
            data = get_string_from_info_DB(ELEM_NAME, lang);
            data.toCharArray((char*)sector_data, 24);
            break;

        case ELEM_DESC_SECTOR:
            data = get_string_from_info_DB(ELEM_DESC, lang);
            data.toCharArray((char*)sector_data, 192);
            break;
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


FRAME_T frameMaker_REQ_ELEM_INFO(byte originin, byte targetin, byte idiomain, byte sectorin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_REQ_ELEM_INFO);
    uint16_t  frameLength = 0x08 + L_REQ_ELEM_INFO;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;      
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.function= F_REQ_ELEM_INFO;
    frame.dataLengthMsb = (L_REQ_ELEM_INFO >> 8) & 0xFF; 
    frame.dataLengthLsb = L_REQ_ELEM_INFO & 0xFF;   
    frame.data[0]= idiomain;
    frame.data[1]= sectorin;
    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}


FRAME_T frameMaker_REQ_ELEM_STATE(byte originin, byte targetin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_REQ_ELEM_INFO);
    uint16_t  frameLength = 0x08 + L_REQ_ELEM_STATE;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;        
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = 0x01;
    frame.target.push_back(targetin);
    frame.function= F_REQ_ELEM_STATE;
    frame.dataLengthMsb = (L_REQ_ELEM_STATE >> 8) & 0xFF; 
    frame.dataLengthLsb = L_REQ_ELEM_STATE & 0xFF;   
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

FRAME_T frameMaker_SET_ELEM_MODE(byte originin, std::vector<byte>targetin, byte modein)
{
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

FRAME_T frameMaker_SEND_SENSOR_VALUE(byte originin, std::vector<byte>targetin, SENSOR_VALUE_T sensorin){
    
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


FRAME_T frameMaker_SEND_TEST(byte originin, std::vector<byte>targetin, byte testin){

    FRAME_T frame;
    memset(&frame, 0, sizeof(FRAME_T));
    frame.data.resize(L_SEND_TEST);
    uint16_t  frameLength = 0x07 + targetin.size() + L_SEND_TEST;

    frame.start= NEW_START;
    frame.frameLengthLsb = frameLength & 0xFF;     
    frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
    frame.origin= originin;
    frame.numTargets = targetin.size();
    frame.target= targetin;
    frame.function= F_SEND_TEST;
    frame.dataLengthMsb = (L_SEND_TEST >> 8) & 0xFF; 
    frame.dataLengthLsb = L_SEND_TEST & 0xFF;   
    frame.data[0]= testin;
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

    switch(sectorin){
        case ELEM_NAME_SECTOR:
            frame.data.resize(L_RETURN_ELEM_SECTOR_24 +1);
            frameLength = 0x08 + L_RETURN_ELEM_SECTOR_24 +1;
            frame.frameLengthLsb = frameLength & 0xFF;     
            frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
            frame.function= F_RETURN_ELEM_SECTOR;
            frame.dataLengthMsb = (L_RETURN_ELEM_SECTOR_24 +1 >> 8) & 0xFF; 
            frame.dataLengthLsb = L_RETURN_ELEM_SECTOR_24 +1 & 0xFF;
            frame.data.push_back(sectorin);   
            for (int i = 0; i < L_RETURN_ELEM_SECTOR_24; i++) frame.data.push_back(sector_data[i]);
            break;

        case ELEM_DESC_SECTOR:
            frame.data.resize(L_RETURN_ELEM_SECTOR_192 +1);
            frameLength = 0x08 + L_RETURN_ELEM_SECTOR_192 +1;
            frame.frameLengthLsb = frameLength & 0xFF;     
            frame.frameLengthMsb = (frameLength >> 8) & 0xFF; 
            frame.function= F_RETURN_ELEM_SECTOR;
            frame.dataLengthMsb = (L_RETURN_ELEM_SECTOR_192 +1 >> 8) & 0xFF; 
            frame.dataLengthLsb = L_RETURN_ELEM_SECTOR_192 +1 & 0xFF;
            frame.data.push_back(sectorin);   
            for (int i = 0; i < L_RETURN_ELEM_SECTOR_192; i++) frame.data.push_back(sector_data[i]);
            break;
    }

    frame.checksum= checksum_calc(frame);
    frame.end= NEW_END;

    return frame;
}



FRAME_T frameMaker_RETURN_ELEM_STATE(byte originin, byte targetin, INFO_STATE_T infoState){

    FRAME_T frame;
    uint16_t length = 0x06 + 0x01 + L_RETURN_ELEM_STATE;
    uint16_t dataLength = L_RETURN_ELEM_STATE;

    frame.start= NEW_START;
    frame.frameLengthMsb= (length >> 8) & 0xFF;
    frame.frameLengthLsb= length & 0xFF;
    frame.origin= originin; 
    frame.numTargets= 0x01;
    frame.target.push_back(targetin);
    frame.function= F_RETURN_ELEM_STATE;
    frame.dataLengthMsb = (dataLength >> 8) & 0xFF;
    frame.dataLengthLsb = dataLength & 0xFF;
    frame.data.push_back(infoState.exclusiveOrigins);
    frame.data.push_back(infoState.currentMode);
    frame.data.push_back(infoState.settedFlags);
    frame.data.push_back(infoState.currentRed);
    frame.data.push_back(infoState.currentGreen);
    frame.data.push_back(infoState.currentBlue);
    frame.data.push_back(infoState.serialNum[0]);
    frame.data.push_back(infoState.serialNum[1]);
    frame.data.push_back(infoState.lifeTime[0]);
    frame.data.push_back(infoState.lifeTime[1]);
    frame.data.push_back(infoState.lifeTime[2]);
    frame.data.push_back(infoState.lifeTime[3]);
    frame.data.push_back(infoState.workingTime[0]);
    frame.data.push_back(infoState.workingTime[1]);
    frame.data.push_back(infoState.workingTime[2]);
    frame.data.push_back(infoState.workingTime[3]);
    frame.checksum= checksum_calc(frame);  // OJO
    frame.end= NEW_END;

    return frame;

}






































































//////////////////////////////////////////////////////////////////////////////////////


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



























































