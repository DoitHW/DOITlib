#ifndef DEFINES_DMS_H
#define DEFINES_DMS_H

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

//DELFINES SUPER GLOBALES
                                                #define COLUMNA 
                      
                                                #define NORESET_FILE
                                                #define SERIAL_NUM (uint16_t) 0xF000
                                                #define DEBUG
                                                #define DEFAULT_CONFIG    true
                                                #define CUSTOM_ID_INIC    false
                                                #define CUSTOM_ID_NUM     0x01
//



#define ELEMENT_CONFIG_FILE_PATH  "/element_config.txt"

#define MAX_EXPECTED_TIME            0xFFFFFFFF
#define RF_TX_PIN     18 
#define RF_RX_PIN     17  
#define RF_CONFIG_PIN 46
#define RF_BAUD_RATE 9600

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

#define F_REQ_ELEM_INFO       0xA1  
#define L_REQ_ELEM_INFO       0x00
#define F_REQ_ELEM_STATE      0xA2
#define L_REQ_ELEM_STATE      0x00

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
#define L_SEND_TEST           0x00

#define F_RETURN_ELEM_INFO    0xD1    
#define L_RETURN_ELEM_INFO    0x2E7C  // OJITO que es mas grande que ROCIO JURADO.
#define F_RETURN_ELEM_STATE   0xD2
#define L_RETURN_ELEM_STATE   0x17
#define F_RETURN_ELEM_ERROR   0xDE
#define L_RETURN_ELEM_ERROR   0x60 

#define MIN_DEAF_TIME         0x01  
#define MAX_DEAF_TIME         0x05 

#define NORMAL_FADE           0x3FFF  // original
#define SLOWEST_FADE          0xC8
#define FASTEST_FADE          0x0F
#define SLOW_FADE             0x32
#define SLOWER_FADE           0x64

#define MAX_BRIGHTNESS        0xFF
#define MID_BRIGHTNESS        0x7F
#define OLD_COLOR_FUNCTION    0xCB
#define OLD_RELAY_FUNCTION    0xD3
#define NO_COLOR              0x00
// Definiciones de cosas del manejo de los tramadoles jajalolxd
#define MAX_BYTES_PER_INTERRUPT 0xFF
#define MAX_FRAME_LENGTH        0x90
#define MIN_FRAME_LENGTH        0x05
#define MAX_BUFFER_SIZE         0xFF
#define MAX_ALLOWED_TARGETS     0xFF
#define MAX_VALID_DEVICE_ID     0xFF
#define MAX_DATA_LENGTH         0xFF
#define INACTIVITY_TIMEOUT      0xFF

#define MAX_INTERRUPT_DURATION   0x20
#define MAX_FRAME_RECEPTION_TIME 0x1F4
#define FRAME_RECEPTION_TIMEOUT  0x64

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

// DEFINES COLUMNA
#define COLUMN_NUM_LEDS      1
#define COLUMN_LED_DATA_PIN 21
#define COLUMN_RELAY_PIN    42

enum COLUMN_MODE_LIST{
    COLUMN_CONTEST_MODE= 0,
    COLUMN_BASIC_MODE,
    COLUMN_FAST_MODE,
    COLUMN_MOTION_MODE,
    COLUMN_RB_MOTION_MODE,
    COLUMN_MIX_MODE,
    COLUMN_PASSIVE_MODE
};
// 

// DEFINES LIGHTSOURCE
#define LIGHTSOURCE_LED_DATA_PIN 0x02
#define LIGHTSOURCE_NUM_STRIPS   0x01
#define LIGHTSOURCE_LEDS_X_STRIP 0x0C
#define LIGHTSOURCE_NUM_LEDS    (LIGHTSOURCE_NUM_STRIPS * LIGHTSOURCE_LEDS_X_STRIP)

//








#endif