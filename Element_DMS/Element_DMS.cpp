#include <defines_DMS/defines_DMS.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <Element_DMS/Element_DMS.h>
#include <info_elements_DMS/info_elements_DMS.h>
#include <Frame_DMS/Frame_DMS.h>

uint64_t startTime= 0;

uint32_t ELEMENT_::get_working_time(){

    uint32_t time= 0;
    EEPROM.get(EEPROM_WORKING_TIME_ADDRESS, time);
    return time;
}


void ELEMENT_::register_working_time(uint64_t timein) {
    uint64_t currentTime = 0;
    EEPROM.get(EEPROM_WORKING_TIME_ADDRESS, currentTime);
    if (UINT64_MAX - currentTime < timein) currentTime = UINT64_MAX; // Evita desbordamiento
    else                                   currentTime += timein;
    currentTime /= 1000;
    EEPROM.put(EEPROM_WORKING_TIME_ADDRESS, currentTime);
    EEPROM.commit();
                                                                #ifdef DEBUG
                                                                  Serial.print("Guardado working time en EEPROM: ");
                                                                  Serial.println(String(currentTime));
                                                                #endif
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

uint8_t ELEMENT_::get_ID() {
    return ID;
}

void ELEMENT_::set_ID(uint8_t deviceID) {
    ID = deviceID;
    EEPROM.write(EEPROM_ID_FLAG_ADDRESS, EEPROM_ID_PROTECT);
    EEPROM.write(EEPROM_ID_ADDRESS, deviceID);
    delay(1);
    EEPROM.commit();
                                                                #ifdef DEBUG
                                                                  Serial.print("ID cambiada a ");
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