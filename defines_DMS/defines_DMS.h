#ifndef DEFINES_DMS_H
#define DEFINES_DMS_H

#include <Arduino.h>
//DELFINES GLOBALES
                                                #define NO_ELEM
                                                /*COLUMNA, FIBRAS, WALLWASHER, ETC*/
                                                #define NOPLAYER                   // -> PLAYER / NOPLAYER
                                                #define DEBUG                    // -> Desactivar en produccion 
                                                #define SERIAL_NUM      0xCACA   // -> 0xVV00= VERSION + 0x00MM= MES
                                                #define NOSERIAL_BY_FILE         // -> NOSERIAL_BY_FILE / SERIAL_BY_FILE --> Activar Serial por FileSystem, si esta definido, ignora el SERIAL_NUM.
                                                #define SHOW_MAC                 // -> Opcional disparar MAC al inicio  (No sirve pa ná...) 
                                                #define SLOW_RF                  // -> FAST_RF= 115200 / SLOW_RF= 9600 
/*                                                                                                  
                                     .-+***+-:....                                                  
                                      .+@@@@@@@@@+:......:::.....                                   
                                        .=@@@@@@@@@@@@@@@@@@@@@@@@@@+:.                             
                                         .-@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%*:...                      
                                      ..=@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#:.                     
                                    ..#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+..                  
                                  ..#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+.                 
                                 .*@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@#.                
                              ..-@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+...             
                              .+@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@+..           
                             .#@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@*.           
                           ..#@@@@@@@@@@@@@@@@@@@%#+:......+@@@@@@@-.....................           
                           .#@@@@@@@@@@@@@@@@=....         -@@@@@@%.                                
                          .+@@@@@@@@@@@@@*:               .#@@@@@%..                                
                         .:@@@@@@@@@@@%:..              ..*@@@@@=..                                 
                         .@@@@@@@@@@%.                  .=@@#:                                      
                         +@@@@@@@@%-.                                                               
                       ..%@@@@@@@+.                                                                 
                       .=@@@@@@%:                                                                   
                       .@@@@@@+..                                                                   
                       :@@@@@=.                                                                     
                      .=@@@@=                                                                       
                  ..*%@@@@@@-..                                                                     
             ...-#@@@@@@@@@@@@=.                                                                    
            ..-@@@@@@@@@@@@@@@@=                                                                    
            .%@@@%=:......-#@@@%.                                                                   
           .*%-..          ..+@@.                                                                   
           ..                .:=.                                                                                                                                                                  
*/






//

#define BOTONERA
    // OJO CHEINCH 

#define ELEMENT_ID_FILE_PATH             "/ID_FILE.txt"
#define ELEMENT_SERIALNUM_FILE_PATH      "/SERIALNUM_FILE.txt"
#define ELEMENT_EVENT_REGISTER_FILE_PATH "/EVENT_REG_FILE.txt"

#define MAX_EVENTS     1000 

#define MAX_REG_EVENTS    1000
#define LIFETIME_UPDATE_INTERVAL  60000
#define MAX_EXPECTED_TIME    0xF
#define RF_TX_PIN         18 
#define RF_RX_PIN         17  
#define RF_CONFIG_PIN     46
#if defined (FAST_RF)
  #define RF_BAUD_RATE      115200
#else
  #define RF_BAUD_RATE      9600
#endif

#define UART_RX_BUFFER_SIZE 1024

// DEFINES FRAME
#define NEW_START             0xE1
#define OLD_START             0x3A
#define NEW_END               0xBB
#define OLD_END               0x3F
#define BROADCAST             0xFF  
#define OLD_FRAME             0x0F
#define NEW_FRAME             0x01
#define UNVALID_FRAME         0x00  
#define UNDEF_DEVICE_ID       0x00  
#define DEFAULT_BOTONERA      0xDB  
#define DEFAULT_CONSOLE       0xDC 
#define DEFAULT_DICE          0xDA
#define DEFAULT_DEVICE        0xDD 
#define DEFAULT_ERROR_ID      0xDE
#define DEFAULT_NFC           0xDF


#define F_REQ_ELEM_SECTOR     0xA0
#define L_REQ_ELEM_SECTOR     0x02
#define F_REQ_ELEM_INFO       0xA1  
#define L_REQ_ELEM_INFO       0x02
#define F_REQ_ELEM_ICON       0xA3
#define L_REQ_ELEM_ICON       0x01
#define F_REQ_ELEM_STATE      0xA2
#define L_REQ_ELEM_STATE      0x00 // OJO

#define F_SET_ELEM_ID         0xB1
#define L_SET_ELEM_ID         0x01
#define F_SET_ELEM_MODE       0xB2
#define L_SET_ELEM_MODE       0x01
#define F_SET_ELEM_DEAF       0xB3
#define L_SET_ELEM_DEAF       0x01

#define F_SEND_COLOR          0xC1
#define L_SEND_COLOR          0x01 
#define F_SEND_RGB            0xC2
#define L_SEND_RGB            0x05
#define F_SEND_BRIGHTNESS     0xC3
#define L_SEND_BRIGHTNESS     0x02
#define F_SEND_SENSOR_VALUE   0xCA
#define L_SEND_SENSOR_VALUE   0x06
#define F_SEND_SENSOR_VALUE_2 0xCB
#define L_SEND_SENSOR_VALUE_2 0x06
#define F_SEND_FILE_NUM       0xCC
#define L_SEND_FILE_NUM       0x02
#define F_SEND_PATTERN_NUM    0xCD  
#define L_SEND_PATTERN_NUM    0x01  
#define F_SEND_FLAG_BYTE      0xCE
#define L_SEND_FLAG_BYTE      0x01
#define F_SEND_TEST           0xCF
#define L_SEND_TEST           0x01

#define F_RETURN_ELEM_SECTOR     0xD0
#define L_RETURN_ELEM_SECTOR_01  0x01
#define L_RETURN_ELEM_SECTOR_02  0x02
#define L_RETURN_ELEM_SECTOR_03  0x03
#define L_RETURN_ELEM_SECTOR_04  0x04
#define L_RETURN_ELEM_SECTOR_05  0x05
#define L_RETURN_ELEM_SECTOR_24  0x18
#define L_RETURN_ELEM_SECTOR_64  0x40
#define L_RETURN_ELEM_SECTOR_128 0x80
#define L_RETURN_ELEM_SECTOR_192 0xC0
#define F_RETURN_ELEM_INFO       0xD1    
#define L_RETURN_ELEM_INFO       0x2E7C   //          0xE7E  // OJITO que es mas grande que ROCIO JURADO.
#define F_RETURN_ELEM_ICON       0xD3
#define L_RETURN_ELEM_ICON       0x00 // OJO
#define F_RETURN_ELEM_STATE      0xD2
#define L_RETURN_ELEM_STATE       0x10
#define F_RETURN_ELEM_ERROR      0xDE
#define L_RETURN_ELEM_ERROR      0x60 

#define MIN_DEAF_TIME         0x01  
#define MAX_DEAF_TIME         0x05 

#define NORMAL_FADE           0x4FF  // original
#define SLOWEST_FADE          0x13FF
#define FASTEST_FADE          0x0A
#define SLOW_FADE             0x32
#define RB_MOTION_VAL_FADE    0xFF
#define MOTION_VAL_FADE       0x1FF

#define MAX_BRIGHTNESS        0xFF
#define MID_BRIGHTNESS        0x7F
#define OLD_COLOR_FUNCTION    0xCB
#define OLD_RELAY_FUNCTION    0xD3
#define NO_COLOR              0x00

// Definiciones de cosas del manejo de los tramas y esas cosas
// NO AGREGAR RES sense consultar a los dioses del Olimpo
#define MAX_BYTES_PER_INTERRUPT 0xFF
#define MAX_FRAME_LENGTH        0xFFFF
#define MIN_FRAME_LENGTH        0x05
#define MAX_BUFFER_SIZE         0xFF
#define MAX_ALLOWED_TARGETS     0xFF
#define MAX_VALID_DEVICE_ID     0xFF
#define MAX_DATA_LENGTH         0xFF
#define INACTIVITY_TIMEOUT      0x0F

#define MAX_INTERRUPT_DURATION   0x10
#define MAX_FRAME_RECEPTION_TIME 0x64
#define FRAME_RECEPTION_TIMEOUT  0x0F

// COLORES 

#define WHITE                    0x00  //0xFFFFF0
#define YELLOW                   0x01  //0xFF9B00
#define ORANGE                   0x02  //0xFF4600
#define RED                      0x03  //0xFF0000
#define VIOLET                   0x04  //0xFF00D2
#define BLUE                     0x05  //0x0000FF
#define LIGHT_BLUE               0x06  //0x00FFC8
#define GREEN                    0x07  //0x00FF00
#define BLACK                    0x08  //0x000000
#define NEGRO                    0x00
#define RELAY                    0x09
////////////////////////////////
#define CREMA                    0x09 //0x806600 //0x8b4513 //0xFEFBEA //0xffcd80
#define ROSA                     0x0A //0xFF00D2 //0xFF8080
#define LILA                     0x0B //0x8F00FF //0xFF80E9 
#define CELESTE_CLARO            0x0C //0x5080FF //0x8080FF
#define TURQUESA                 0x0D //0x40E0D0 //0x80FFE4
#define VERDE_CLARO              0x0E //0x66FF00 //0x80FF80
#define NARANJA                  0x0F //0xFF4500 //0xFF7F00
#define ROSA_OSCURO              0x10 //0xFF1493 //0xFF4FD1
#define VERDE_OLIVA              0x11 //0x758A00 //0x808000 //0x7F7F80
#define VERDE_LIMA               0x12 //0x7FFF64 //0x7FFF64
#define VERDE_AMARILLO           0x13 //0xFFDF00 //0x7FFF00
#define MAGENTA                  0x14 //0xD9017A //0xFF0081
#define MORADO                   0x15 //0x9400d3 //0x9F00C5 //0x9B26B6 //0x800080
#define VERDE_AZULADO            0x16 //0x0DBA98 //0x80FFC4
#define MARRON                   0x17 //0xff4005 //0x8b4513
#define INDIGO                   0x18 //0x330099 //0x8000F1
#define AZUL_VERDOSO             0x19 //0x1f3438 //0x80FFD5
#define ESMERALDA                0x1A //0x50C878 //0x80FF81
#define CYAN                     0x1B //0x00FFFF
#define VERDE_MENTA              0x1C //0x98FF98 //0x80FFC8
#define AMARILLO_ANARANJADO      0x1D //0xFF7E00 //0xFFA300
#define ROJO_ANARANJADO          0x1E //0xFF2300
#define ROSA_FUERTE              0x1F //0xe4007c //0xFF23D1
#define MARRON_OSCURO            0x20 //0xff2500 //0x7F2380
#define VERDE_GRISACEO           0x21 //0x7FA364
#define VERDE_OLIVA_CLARO        0x22 //0x7FA300
#define SALMON                   0x23 //0xe73410
//

#define ICON_COLUMNS      64
#define ICON_ROWS         64
#define ICON_LENGTH      ICON_COLUMNS*ICON_ROWS

// DEFINES INFO_ELEMENTS
#define SPANISH_LANG        0x01
#define ENGLISH_LANG        0x02
#define FRENCH_LANG         0x03
#define GERMAN_LANG         0x04
#define CATALAN_LANG        0x05
#define MEXICAN_LANG        0x06
#define EUSKERA_LANG        0x07

#define MSB 0x00
#define LSB 0x01

#define ELEM_NAME           0x01
#define ELEM_DESC           0x02
#define ELEM_MODE_0_NAME    0x03
#define ELEM_MODE_0_DESC    0x04
#define ELEM_MODE_1_NAME    0x05
#define ELEM_MODE_1_DESC    0x06
#define ELEM_MODE_2_NAME    0x07
#define ELEM_MODE_2_DESC    0x08
#define ELEM_MODE_3_NAME    0x09
#define ELEM_MODE_3_DESC    0x0A
#define ELEM_MODE_4_NAME    0x0B
#define ELEM_MODE_4_DESC    0x0C
#define ELEM_MODE_5_NAME    0x0D
#define ELEM_MODE_5_DESC    0x0E
#define ELEM_MODE_6_NAME    0x0F
#define ELEM_MODE_6_DESC    0x10
#define ELEM_MODE_7_NAME    0x11
#define ELEM_MODE_7_DESC    0x12
#define ELEM_MODE_8_NAME    0x13
#define ELEM_MODE_8_DESC    0x14
#define ELEM_MODE_9_NAME    0x15
#define ELEM_MODE_9_DESC    0x16
#define ELEM_MODE_10_NAME   0x17
#define ELEM_MODE_10_DESC   0x18
#define ELEM_MODE_11_NAME   0x19
#define ELEM_MODE_11_DESC   0x1A
#define ELEM_MODE_12_NAME   0x1B
#define ELEM_MODE_12_DESC   0x1C
#define ELEM_MODE_13_NAME   0x1D
#define ELEM_MODE_13_DESC   0x1E
#define ELEM_MODE_14_NAME   0x1F    
#define ELEM_MODE_14_DESC   0x20    
#define ELEM_MODE_15_NAME   0x21
#define ELEM_MODE_15_DESC   0x22

// DEFINES ELEMENT
#define EEPROM_ID_ADDRESS           0x00
#define EEPROM_ID_FLAG_ADDRESS      0x01
#define EEPROM_ID_PROTECT           0x01
#define EEPROM_WORKING_TIME_ADDRESS 0x20

#define DEFAULT_BASIC_MODE      0x01

#define TYPE_BOTONERA       0xAB
#define TYPE_COLUMN         0x01
#define TYPE_LIGHTSOURCE    0x02
#define TYPE_LEDSTRIP       0x03
#define TYPE_ESCALERA       0x04
#define TYPE_BLACKLIGHT     0x05
#define TYPE_FAN            0x06
#define TYPE_TUNNEL         0x07
#define TYPE_DOT_WALL       0x08
#define TYPE_STORM_PANEL    0x09
#define TYPE_STAR_CEILING   0x0A
#define TYPE_SHOWER         0x0B
#define TYPE_CURTAIN        0x0C
#define GENERIC_TYPE        0x00 

// TESTS

enum TESTS_{
  BLACKOUT= 0,
  START_TEST,
  HELLO_TEST
  // añadir mas tiestos
};

#define RELAY_1_FLAG     0x00
#define RELAY_2_FLAG     0x01
// ETC

#define SET_RELAY        0x01
#define RESET_RELAY      0x00

// DEFINES COLUMNA
#define COLUMN_LED_DATA_PIN 21
#define COLUMN_RELAY_PIN    42

// DEFINES FIBRAS
#define LIGHTSOURCE_LED_DATA_PIN    21 
#define LIGHTSOURCE_FAN_RELAY_PIN   42

// DEFINES LEDSTRIPS
#define LEDSTRIP_LED_DATA_PIN 45 // 21= oficial

// DEFINES BOTONERA
#define BOTONERA_DATA_PIN     21




#if   defined (COLUMNA)
  #define NUM_LEDS 1
#elif defined (FIBRAS)
  #define NUM_LEDS 1
#elif defined (WALLWASHER)
  #define NUM_LEDS 36
#elif defined (BOTONERA)
  #define NUM_LEDS 9
#endif




enum COLUMN_MODE_LIST{
    COLUMN_CONTEST_MODE= 0,
    COLUMN_BASIC_MODE,
    COLUMN_SLOW_MODE,
    COLUMN_MOTION_MODE,
    COLUMN_RB_MOTION_MODE,
    COLUMN_MIX_MODE,
    COLUMN_PASSIVE_MODE,
    COLUMN_PATTERN_MODE
};

enum LIGHTSOURCE_MODE_LIST{
    LIGHTSOURCE_CONTEST_MODE= 0,
    LIGHTSOURCE_BASIC_MODE,
    LIGHTSOURCE_SLOW_MODE,
    LIGHTSOURCE_MOTION_MODE,
    LIGHTSOURCE_RB_MOTION_MODE,
    LIGHTSOURCE_MIX_MODE,
    LIGHTSOURCE_PASSIVE_MODE,
    LIGHTSOURCE_PATTERN_MODE
};

enum LEDSTRIP_MODE_LIST{
    LEDSTRIP_HIDDEN_MODE= 0,
    LEDSTRIP_BASIC_MODE,
    LEDSTRIP_SLOW_MODE,
    LEDSTRIP_MOTION_MODE,
    LEDSTRIP_RB_MOTION_MODE,
    LEDSTRIP_MIX_MODE,
    LEDSTRIP_PASSIVE_MODE,
    LEDSTRIP_PATTERN_MODE,
};

enum PATTERN_LIST{
    NO_PATTERN= 0,
    COLOR_PATT,
    FIRE_PATT,
    METEOR_PATT,
    BOUNCING_PATT, 
    RAINBOW_PATT,
    SNOW_PATT,
    CLOUD_PATT
};

enum SECTOR_LIST{

  ELEM_NAME_SECTOR= 0,
  ELEM_DESC_SECTOR,
  ELEM_SERIAL_SECTOR,
  ELEM_ID_SECTOR,
  ELEM_CMODE_SECTOR,
  ELEM_MODE_0_NAME_SECTOR,
  ELEM_MODE_0_DESC_SECTOR,
  ELEM_MODE_0_FLAG_SECTOR,
  ELEM_MODE_1_NAME_SECTOR,
  ELEM_MODE_1_DESC_SECTOR,
  ELEM_MODE_1_FLAG_SECTOR,
  ELEM_MODE_2_NAME_SECTOR,
  ELEM_MODE_2_DESC_SECTOR,
  ELEM_MODE_2_FLAG_SECTOR,
  ELEM_MODE_3_NAME_SECTOR,
  ELEM_MODE_3_DESC_SECTOR,
  ELEM_MODE_3_FLAG_SECTOR,
  ELEM_MODE_4_NAME_SECTOR,
  ELEM_MODE_4_DESC_SECTOR,
  ELEM_MODE_4_FLAG_SECTOR,
  ELEM_MODE_5_NAME_SECTOR,
  ELEM_MODE_5_DESC_SECTOR,
  ELEM_MODE_5_FLAG_SECTOR,
  ELEM_MODE_6_NAME_SECTOR,
  ELEM_MODE_6_DESC_SECTOR,
  ELEM_MODE_6_FLAG_SECTOR,
  ELEM_MODE_7_NAME_SECTOR,
  ELEM_MODE_7_DESC_SECTOR,
  ELEM_MODE_7_FLAG_SECTOR,
  ELEM_MODE_8_NAME_SECTOR,
  ELEM_MODE_8_DESC_SECTOR,
  ELEM_MODE_8_FLAG_SECTOR,
  ELEM_MODE_9_NAME_SECTOR,
  ELEM_MODE_9_DESC_SECTOR,
  ELEM_MODE_9_FLAG_SECTOR,
  ELEM_MODE_10_NAME_SECTOR,
  ELEM_MODE_10_DESC_SECTOR,
  ELEM_MODE_10_FLAG_SECTOR,
  ELEM_MODE_11_NAME_SECTOR,
  ELEM_MODE_11_DESC_SECTOR,
  ELEM_MODE_11_FLAG_SECTOR,
  ELEM_MODE_12_NAME_SECTOR,
  ELEM_MODE_12_DESC_SECTOR,
  ELEM_MODE_12_FLAG_SECTOR,
  ELEM_MODE_13_NAME_SECTOR,
  ELEM_MODE_13_DESC_SECTOR,
  ELEM_MODE_13_FLAG_SECTOR,
  ELEM_MODE_14_NAME_SECTOR,
  ELEM_MODE_14_DESC_SECTOR,
  ELEM_MODE_14_FLAG_SECTOR,
  ELEM_MODE_15_NAME_SECTOR,
  ELEM_MODE_15_DESC_SECTOR,
  ELEM_MODE_15_FLAG_SECTOR,
  ELEM_ICON_ROW_0_SECTOR,
  ELEM_ICON_ROW_1_SECTOR,
  ELEM_ICON_ROW_2_SECTOR,
  ELEM_ICON_ROW_3_SECTOR,
  ELEM_ICON_ROW_4_SECTOR,
  ELEM_ICON_ROW_5_SECTOR,
  ELEM_ICON_ROW_6_SECTOR,
  ELEM_ICON_ROW_7_SECTOR,
  ELEM_ICON_ROW_8_SECTOR,
  ELEM_ICON_ROW_9_SECTOR,
  ELEM_ICON_ROW_10_SECTOR,
  ELEM_ICON_ROW_11_SECTOR,
  ELEM_ICON_ROW_12_SECTOR,
  ELEM_ICON_ROW_13_SECTOR,
  ELEM_ICON_ROW_14_SECTOR,
  ELEM_ICON_ROW_15_SECTOR,
  ELEM_ICON_ROW_16_SECTOR,
  ELEM_ICON_ROW_17_SECTOR,
  ELEM_ICON_ROW_18_SECTOR,
  ELEM_ICON_ROW_19_SECTOR,
  ELEM_ICON_ROW_20_SECTOR,
  ELEM_ICON_ROW_21_SECTOR,
  ELEM_ICON_ROW_22_SECTOR,
  ELEM_ICON_ROW_23_SECTOR,
  ELEM_ICON_ROW_24_SECTOR,
  ELEM_ICON_ROW_25_SECTOR,
  ELEM_ICON_ROW_26_SECTOR,
  ELEM_ICON_ROW_27_SECTOR,
  ELEM_ICON_ROW_28_SECTOR,
  ELEM_ICON_ROW_29_SECTOR,
  ELEM_ICON_ROW_30_SECTOR,
  ELEM_ICON_ROW_31_SECTOR,
  ELEM_ICON_ROW_32_SECTOR,
  ELEM_ICON_ROW_33_SECTOR,
  ELEM_ICON_ROW_34_SECTOR,
  ELEM_ICON_ROW_35_SECTOR,
  ELEM_ICON_ROW_36_SECTOR,
  ELEM_ICON_ROW_37_SECTOR,
  ELEM_ICON_ROW_38_SECTOR,
  ELEM_ICON_ROW_39_SECTOR,
  ELEM_ICON_ROW_40_SECTOR,
  ELEM_ICON_ROW_41_SECTOR,
  ELEM_ICON_ROW_42_SECTOR,
  ELEM_ICON_ROW_43_SECTOR,
  ELEM_ICON_ROW_44_SECTOR,
  ELEM_ICON_ROW_45_SECTOR,
  ELEM_ICON_ROW_46_SECTOR,
  ELEM_ICON_ROW_47_SECTOR,
  ELEM_ICON_ROW_48_SECTOR,
  ELEM_ICON_ROW_49_SECTOR,
  ELEM_ICON_ROW_50_SECTOR,
  ELEM_ICON_ROW_51_SECTOR,
  ELEM_ICON_ROW_52_SECTOR,
  ELEM_ICON_ROW_53_SECTOR,
  ELEM_ICON_ROW_54_SECTOR,
  ELEM_ICON_ROW_55_SECTOR,
  ELEM_ICON_ROW_56_SECTOR,
  ELEM_ICON_ROW_57_SECTOR,
  ELEM_ICON_ROW_58_SECTOR,
  ELEM_ICON_ROW_59_SECTOR,
  ELEM_ICON_ROW_60_SECTOR,
  ELEM_ICON_ROW_61_SECTOR,
  ELEM_ICON_ROW_62_SECTOR,
  ELEM_ICON_ROW_63_SECTOR,
  ELEM_WORK_TIME_SECTOR,
  ELEM_LIFE_TIME_SECTOR,
  ELEM_CURRENT_COLOR_SECTOR,
  ELEM_CURRENT_BRIGHTNESS_SECTOR,
  ELEM_CURRENT_FLAGS_SECTOR,
  ELEM_CURRENT_PATTERN_SECTOR,
  ELEM_CURRENT_FILE_SECTOR,
  ELEM_CURRENT_XMANAGER_SECTOR
};



//




#endif





/*

{EV_START, 0, 0}
{EV_MODE_CHANGE, 1, 0} 
{EV_COLOR_CHANGE, 2, 12000} aqui se guarda un valor de 12000 por que el siguiente  EV_COLOR_CHANGE con eventVal distinto ha ocurrido 12000 ms despues de el de esta linea
{EV_FLAG_CHANGE, 1, 40000}  aqui se guarda un valor de 40000 por que el siguiente  EV_FLAG_CHANGE con eventVal distinto ha ocurrido 40000 ms despues de el de esta linea
{EV_COLOR_CHANGE, 6, 0} aqui se guarda 0 por que el siguiente valor de EV_COLOR_CHANGE es el mismo que el de esta linea. por lo tanto al no haber habido un cambio de color, el cronometro para este color sigue contando.
{EV_COLOR_CHANGE, 6, 50000}  aqui se guarda un valor de 50000 por que el siguiente  EV_COLOR_CHANGE con eventVal distinto ha ocurrido 50000 ms despues de el de esta linea
{EV_SECTOR_REQ, 9, 0} aqui se guarda simplemente el evento con su eventVal y su timeStamp igual a 0.
{EV_MODE_CHANGE, 3, 25000} igual que  todo lo anterior, 25000 son los ms que han pasado hasta el siguiente EV_CHANGE_MODE con eventVal distinto
{EV_MODE_CHANGE, 4, 0} etc 
{EV_SECTOR_REQ, 9, 0} etc 
{EV_FLAG_CHANGE, 0, 0} etc 
{EV_COLOR_CHANGE, 7, 0} etc 
{EV_COLOR_CHANGE, 1, 0} etc
{EV_END, 0, 67000} // 67000 son los ms transcurridos desde el EV_START, ademas aqui termina el contador de tiempo de todos los eventos que no han recibido un evento equivalente que detenga y registre el cronometro. es decir que en el ultimo EV_COLOR_CHANGE se registrara el tiempo transcurrido hasta este EV_END
























































Nombre: Columna
Numero de serie: 67A5D48283F1
Tiempo de trabajo: 9833 minutos
Tiempo de vida: 6532 horas.
numero de ciclos: 69


--NUEVO CICLO 24-- 

evento: DISPOSITIVO INICIADO EN MODO BASICO
valor: 0
duracion: 0

evento: CAMBIO DE MODO
valor: 1
duracion: 2 minutos

evento: CAMBIO DE COLOR
valor: 2
duracion: 4 minutos

evento: CAMBIO DE COLOR
valor: 6
duracion: 16 minutos

evento: CAMBIO DE MODO
valor: 3
duracion: 31 minutos

evento: APAGANDO DISPOSITIVO
valor: 0
duracion: 39 minutos

-- FIN DE CICLO -- 


--NUEVO CICLO 25-- 

evento: DISPOSITIVO INICIADO EN MODO BASICO
valor: 0
duracion: 0

evento: CAMBIO DE MODO
valor: 1
duracion: 4 minutos

evento: CAMBIO DE COLOR
valor: 2
duracion: 5 minutos

evento: CAMBIO DE COLOR
valor: 6
duracion: 15 minutos

evento: CAMBIO DE MODO
valor: 3
duracion: 32 minutos

evento: APAGANDO DISPOSITIVO
valor: 0
duracion: 34 minutos

-- FIN DE CICLO -- 

--NUEVO CICLO 26-- 

evento: DISPOSITIVO INICIADO EN MODO BASICO
valor: 0
duracion: 0

evento: CAMBIO DE MODO
valor: 1
duracion: 9 minutos

evento: CAMBIO DE COLOR
valor: 2
duracion: 14 minutos

evento: CAMBIO DE COLOR
valor: 6
duracion: 18 minutos

evento: CAMBIO DE MODO
valor: 3
duracion: 39 minutos

evento: APAGANDO DISPOSITIVO
valor: 0
duracion: 42 minutos

-- FIN DE CICLO -- 

*/