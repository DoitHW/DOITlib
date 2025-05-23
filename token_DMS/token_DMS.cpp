#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>
#include <SPIFFS_handler/SPIFFS_handler.h>

// Definición de pines I2C para ESP32 (ajústalos según tu hardware)
#define SDA_NFC_PIN 40
#define SCL_NFC_PIN 41

// Definición de pines para el PN532 en modo I2C con IRQ
//#define PN532_IRQ   (42)    // Ajusta este valor según tu conexión
#define PN532_RESET (-1)    // Normalmente -1 si no se utiliza reset

// Instancia global del PN532 utilizando la librería Adafruit_PN532
//static Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
TwoWire PN532_Wire = TwoWire(1);
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET, &PN532_Wire);



// Variables para manejo de IRQ (para uso interno de la lectura)
extern int irqCurr;
extern int irqPrev;
extern const int DELAY_BETWEEN_CARDS;  // Si lo defines globalmente en main o aquí
extern unsigned long timeLastCardRead; // Lo mismo para estas variables
extern boolean readerDisabled;         // Estas pueden definirse globalmente en main
extern bool cardIsRead;
extern TOKEN_ token;

TOKEN_::TOKEN_() : lastReadAttempt(0), readInterval(200), genre(0), lang(static_cast<uint8_t>(currentLanguage)) {
    currentUID = "";
    lastProcessedUID = "";
}


void TOKEN_::begin() {
  const int MAX_RETRIES = 5;
  int retryCount = 0;

  PN532_Wire.begin(SDA_NFC_PIN, SCL_NFC_PIN);  // SDA_2 / SCL_2
  PN532_Wire.setClock(100000);
  delay(100);

  while (retryCount < MAX_RETRIES) {
      nfc.begin();
      uint32_t versiondata = nfc.getFirmwareVersion();
      if (versiondata) {
          nfc.SAMConfig();
          startListeningToNFC();
          return;
      }

      Serial.printf("ERROR: No se encontró el módulo PN532 (intento %d de %d). Reintentando...\n", retryCount + 1, MAX_RETRIES);
      retryCount++;
      delay(500);
  }

  Serial.println("FATAL: No se pudo inicializar el PN532 tras múltiples intentos. Reiniciando dispositivo...");
  delay(2000);
  ESP.restart();
}

bool TOKEN_::isCardPresent() {
  uint8_t _uid[7] = {0};
  uint8_t uidLength = 0;
  
  // Usamos un timeout muy corto para un sondeo rápido
  bool result = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &uidLength, 100);
  
  return result;
}

void TOKEN_::resetReader() {
  Serial.println("⚠️ Forzando reinicio completo del lector NFC...");
  
  // Cancelar cualquier operación pendiente
  nfc.inListPassiveTarget();
  
  // Pausa para estabilizar
  delay(100);
  
  // Reiniciar configuración completa
  nfc.begin();
  delay(100);
  nfc.SAMConfig();
  delay(100);
  
  // Reiniciar indicadores de IRQ
  irqPrev = HIGH;
  irqCurr = HIGH;
  
  // Iniciar detección pasiva
  startListeningToNFC();
}
void TOKEN_::startListeningToNFC() {
  // Reiniciar los indicadores de IRQ
  irqPrev = HIGH;
  irqCurr = HIGH;

  // Cancelar cualquier operación pendiente primero
  nfc.inListPassiveTarget();
  
  // Pequeña pausa para estabilizar
  delay(5);
    
  // Reintentar varias veces si falla
  bool success = false;
 
    success = nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
    if (success) {
      Serial.println("Detección iniciada correctamente.");
    }
      
  if (!success) {
    Serial.println("No card found. Waiting...");
  } else {
    Serial.println("Card already present.");
    handleCardDetected();
  }
}

void TOKEN_::handleCardDetected() {
  uint8_t success = false;
  uint8_t uidBuffer[7] = {0};
  uint8_t uidLength;
  
  // Intentar leer hasta 3 veces si falla
  for (int attempt = 0; attempt < 3; attempt++) {
      success = nfc.readDetectedPassiveTargetID(uidBuffer, &uidLength);
      if (success) break;
      delay(10);
  }
  
  Serial.println(success ? "Read successful" : "Read failed (not a card?)");
  
  if (success) {
    String newUID = "";
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uidBuffer[i] < 0x10) newUID += "0";
      newUID += String(uidBuffer[i], HEX);
    }
    newUID.toUpperCase();
    currentUID = newUID;
    uidDetected = true;
    mensajeLeido = false;
    uidDetectionTime = millis();

    Serial.print("Found an ISO14443A card, UID: ");
    Serial.println(currentUID);
  } else {
    // Si falló la lectura, reiniciar la detección
    startListeningToNFC();
  }
  timeLastCardRead = millis();
}

bool TOKEN_::readCard(String &uid) {
  // Se utiliza un timeout corto (por ejemplo, 100 ms) para la lectura
  uint8_t _uid[7] = {0};
  uint8_t uidLength = 0;

  // Intentar leer la tarjeta en modo pasivo usando la función readPassiveTargetID (aprovecha IRQ internamente)
  bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &uidLength, 100);
  if (!success || uidLength == 0) {
      currentUID = "";
      return false;
  }

  // Convertir el UID a cadena hexadecimal
  String newUID = "";
  for (uint8_t i = 0; i < uidLength; i++) {
    if (_uid[i] < 0x10) newUID += "0";
    newUID += String(_uid[i], HEX);
  }
  newUID.toUpperCase();

  uid = newUID;
  currentUID = newUID;

  Serial.print("✅ Tarjeta detectada con UID: ");
  Serial.println(currentUID);
  return true;
}

String TOKEN_::decodeNdefText(const byte* payload, int payloadLength) {
  if (payloadLength < 1) return "[Payload vacío]";
  byte statusByte = payload[0];
  int langCodeLength = statusByte & 0x3F;
  if (1 + langCodeLength >= payloadLength) return "[Payload inválido]";
  int textLength = payloadLength - 1 - langCodeLength;
  String decodedText = "";
  for (int i = 0; i < textLength && i < 255; i++) {
      decodedText += (char)payload[i + 1 + langCodeLength];
  }
  return decodedText;
}


// Función modificada para leer el mensaje NDEF (token) desde las páginas 6 a 18
bool TOKEN_::leerMensajeNFC(String &mensaje) {
  Serial.println("DEBUG: Procesando mensaje NDEF para la tarjeta con UID: " + currentUID);

  uint8_t rawBuffer[128] = {0};
  int len = 0;
  // Leer de la página 6 a la 18 (13 páginas * 4 bytes = 52 bytes)
  for (uint8_t page = 6; page <= 30; page++) {
    uint8_t pageBuffer[4] = {0};
    if (!nfc.ntag2xx_ReadPage(page, pageBuffer)) {
      Serial.print("⚠️ ERROR: Falló la lectura de la página ");
      Serial.println(page);
      mensaje = "";
      return false;
    }
    memcpy(rawBuffer + len, pageBuffer, 4);
    len += 4;
  }
  
  // Convertir el buffer a una cadena completa (cada byte se interpreta como carácter)
  String fullData = "";
  for (int i = 0; i < len; i++) {
    fullData += (char)rawBuffer[i];
  }
  Serial.println("Dump completo (páginas 6 a 18): " + fullData);
  
  // Buscar los delimitadores '#' en la cadena
  int firstHash = fullData.indexOf('#');
  if (firstHash < 0) {
    Serial.println("⚠️ ERROR: No se encontró el delimitador de inicio '#'.");
    mensaje = "";
    return false;
  }
  int secondHash = fullData.indexOf('#', firstHash + 1);
  if (secondHash < 0) {
    Serial.println("⚠️ ERROR: No se encontró el delimitador de fin '#'.");
    mensaje = "";
    return false;
  }
  
  // Extraer la cadena entre los dos delimitadores
  String tokenStr = fullData.substring(firstHash + 1, secondHash);
  Serial.println("Token string extraído: " + tokenStr);
  
  // Se espera que el token tenga 46 dígitos hexadecimales (23 bytes)
  if (tokenStr.length() != 94) {
    Serial.println("⚠️ ERROR: Longitud del token inválida. Se esperaba 46 dígitos hexadecimales, se obtuvo: " + String(tokenStr.length()));
    mensaje = "";
    return false;
  }
  
  // Función lambda para convertir dos caracteres ASCII hex a un byte
  auto hexToByte = [](const String &str) -> byte {
    return (byte)strtol(str.c_str(), nullptr, 16);
  };

  // Nuevo orden:
  // Bytes 0-1: CMD
  // Bytes 2-3: CMD2
  // Bytes 4-5: Bank
  // Bytes 6-7: File
  // Bytes 8-9: Color R
  // Bytes 10-11: Color G
  // Bytes 12-13: Color B
  // Bytes 14-45: 8 parejas para los partners (8×4 dígitos = 32 dígitos)
  token.currentToken.cmd = hexToByte(tokenStr.substring(0, 2));
  token.currentToken.cmd2 = hexToByte(tokenStr.substring(2, 4));
  token.currentToken.addr.bank = hexToByte(tokenStr.substring(4, 6));
  //updateBankList(token.currentToken.addr.bank);
  Serial.println("SPIFFS: Bank actualizado: " + String(token.currentToken.addr.bank, HEX));
  token.currentToken.addr.file = hexToByte(tokenStr.substring(6, 8));
  token.currentToken.color.r = hexToByte(tokenStr.substring(8, 10));
  token.currentToken.color.g = hexToByte(tokenStr.substring(10, 12));
  token.currentToken.color.b = hexToByte(tokenStr.substring(12, 14));
  
  for (int i = 0; i < 8; i++) {
    int startIdx = 14 + i * 4;
    token.currentToken.partner[i].bank = hexToByte(tokenStr.substring(startIdx, startIdx + 2));
    token.currentToken.partner[i].file = hexToByte(tokenStr.substring(startIdx + 2, startIdx + 4));
  }

  // Leer 24 bytes para el nombre de la familia (posiciones 46 a 93)
for (int i = 0; i < 24; i++) {
  token.currentToken.familyName[i] = hexToByte(tokenStr.substring(46 + i * 2, 48 + i * 2));
}
token.currentToken.familyName[24] = '\0'; // Asegurar string nulo-terminado

  updateBankAndFamilyList(token.currentToken.addr.bank, (char*)token.currentToken.familyName);
  bankList = readBankList();
  selectedBanks.resize(bankList.size(), false);
  // Imprimir valores para depuración
  Serial.println("Token decodificado:");
  Serial.print("CMD: 0x"); Serial.println(token.currentToken.cmd, HEX);
  Serial.print("CMD2: 0x"); Serial.println(token.currentToken.cmd2, HEX);
  Serial.print("Bank: 0x"); Serial.println(token.currentToken.addr.bank, HEX);
  Serial.print("File: 0x"); Serial.println(token.currentToken.addr.file, HEX);
  Serial.print("Color R: 0x"); Serial.println(token.currentToken.color.r, HEX);
  Serial.print("Color G: 0x"); Serial.println(token.currentToken.color.g, HEX);
  Serial.print("Color B: 0x"); Serial.println(token.currentToken.color.b, HEX);
  for (int i = 0; i < 8; i++) {
    Serial.print("Partner "); Serial.print(i); Serial.print(": Bank=0x");
    Serial.print(token.currentToken.partner[i].bank, HEX);
    Serial.print(", File=0x");
    Serial.println(token.currentToken.partner[i].file, HEX);
  }
  Serial.print("🏷️  Familia: ");
  Serial.println(token.currentToken.familyName);

  
  mensaje = tokenStr;
  // Actualizar el UID procesado para evitar relecturas mientras la misma tarjeta esté presente
  lastProcessedUID = currentUID;
  mensajeLeido = true;

  return true;
}

bool TOKEN_::leerPagina(uint8_t pagina, uint8_t *buffer) {
    // En la nueva lógica se utiliza la lectura completa de NDEF, por lo que esta función queda sin implementar.
    return false;
}

void TOKEN_::proponer_token(byte guessbank) {
    lang = 0;
    int fileRand = random(1, 8);  // Genera un número aleatorio entre 1 y 7
    if (guessbank > 0x09 && guessbank < 0x63) lang = static_cast<uint8_t>(currentLanguage) * 10;
    doitPlayer.play_file(guessbank + genre, fileRand + lang);
    propossedToken.addr.bank = guessbank;
    propossedToken.addr.file = fileRand;
}

void TOKEN_::token_handler(TOKEN_DATA token, uint8_t lang_in, bool genre_in, uint8_t myid, std::vector<uint8_t> targets) {
  // Calcular offset de lenguaje si corresponde
  byte lang = 0;
  if (token.addr.bank > 0x09 && token.addr.bank < 0x63) {
      lang = lang_in * 10;
  }
  
  // Procesamos si la ficha es de efecto (TOKEN_FX) o sin efecto (TOKEN_NOFX)
  if (token.cmd == TOKEN_FX || token.cmd == TOKEN_NOFX) {
      // Enviar el color de la ficha
      COLOR_T colorout;
      colorout.red   = token.color.r;
      colorout.green = token.color.g;
      colorout.blue  = token.color.b;
      send_frame(frameMaker_SEND_RGB(myid, targets, colorout));

      delay(200); // Pausa para asegurar que se aplique el color

      // Iniciar reproducción de audio (usa bank + genre y file + lang)
      doitPlayer.play_file(token.addr.bank + genre_in, token.addr.file + lang);
      delay(50);
      while (doitPlayer.is_playing()) { delay(10); }
      delay(500);
      // Procesar según el modo actual
      if (tokenCurrentMode == TOKEN_BASIC_MODE) {
          // En BASIC: al terminar el audio, si el color es temporal se apaga (se envía negro)
          if (token.cmd2 == TEMP_COLOR_CONF) {
              send_frame(frameMaker_SEND_COLOR(myid, targets, 8)); // negro
          }
          // Si es PERM_COLOR_CONF, no se realiza ningún cambio (se mantiene el color)
      }
      else if (tokenCurrentMode == TOKEN_PARTNER_MODE) {
        // Declaración estática para retener los datos de la primera ficha
        static bool firstTokenStored = false;
        static byte firstTokenBank = 0;
        static byte firstTokenFile = 0;
        
        if (!waitingForPartner) {
            // Primera ficha: guardar bank y file permanentemente
            firstTokenBank = token.addr.bank;
            firstTokenFile = token.addr.file;
            firstTokenStored = true;
            Serial.printf("Primer token registrado: Bank = 0x%02X, File = 0x%02X\n", firstTokenBank, firstTokenFile);
            
            waitingForPartner = true;
            
            // Al finalizar la reproducción de la primera ficha, si es temporal se apaga el color
            if (token.cmd2 == TEMP_COLOR_CONF) {
                delay(50);
                send_frame(frameMaker_SEND_COLOR(myid, targets, 8)); // Enviar negro
                Serial.println("Primer token: Color temporal, apagando después del audio.");
            }
        } else {
            // Segunda ficha: verificar si alguno de sus partners coincide con el primer token
            if (!firstTokenStored) {
                Serial.println("Error: No se encontró el primer token almacenado.");
                return;
            }
            bool match = false;
            Serial.printf("Comparando primer token (Bank = 0x%02X, File = 0x%02X) con los partners de la segunda ficha:\n", firstTokenBank, firstTokenFile);
            for (int i = 0; i < 8; i++) {
                Serial.printf("Partner %d: Bank = 0x%02X, File = 0x%02X\n", i, token.partner[i].bank, token.partner[i].file);
                if (token.partner[i].bank == firstTokenBank && token.partner[i].file == firstTokenFile) {
                    match = true;
                    Serial.printf("Match encontrado en partner %d\n", i);
                    break;
                }
            }
            
            delay(50);
            while (doitPlayer.is_playing()) { delay(10); }
            delay(200);
            int fileNum = match ? random(1, 4) : random(1, 3);
            send_frame(frameMaker_SEND_RESPONSE(myid, targets, match ? WIN : FAIL));
            doitPlayer.play_file((match ? WIN_RESP_BANK : FAIL_RESP_BANK) + genre_in, fileNum + lang);
            delay(50);
            while (doitPlayer.is_playing()) { delay(10); }
            delay(600);
            
            // Según el tipo de configuración de color
            if (token.cmd2 == TEMP_COLOR_CONF) {
                send_frame(frameMaker_SEND_COLOR(myid, targets, 8)); // Apagar color (negro)
                Serial.println("Segundo token: TEMP_COLOR_CONF, apagando color.");
            } else if (token.cmd2 == PERM_COLOR_CONF) {
                COLOR_T colorout;
                colorout.red   = token.color.r;
                colorout.green = token.color.g;
                colorout.blue  = token.color.b;
                send_frame(frameMaker_SEND_RGB(myid, targets, colorout)); // Mantener el color
                Serial.println("Segundo token: PERM_COLOR_CONF, manteniendo color.");
            }
            
            waitingForPartner = false;
            firstTokenStored = false; // Reiniciamos para la próxima pareja
        }
    }    
      else if (tokenCurrentMode == TOKEN_GUESS_MODE) {
          bool isCorrect = (token.addr.bank == propossedToken.addr.bank && token.addr.file == propossedToken.addr.file);
          while (doitPlayer.is_playing()) { delay(10); }
          delay(600);
          // Enviar color de respuesta: verde (7) si es correcto, rojo (3) si no
          send_frame(frameMaker_SEND_RESPONSE(myid, targets, isCorrect ? WIN : FAIL));
          doitPlayer.play_file((isCorrect ? WIN_RESP_BANK : FAIL_RESP_BANK) + genre_in, random(1, isCorrect ? 4 : 3) + lang);
          while (doitPlayer.is_playing()) { delay(10); }
          delay(600);
          if (token.cmd2 == TEMP_COLOR_CONF) {
              send_frame(frameMaker_SEND_COLOR(myid, targets, 8)); // color temporal: apagar
          } else if (token.cmd2 == PERM_COLOR_CONF) {
              send_frame(frameMaker_SEND_RGB(myid, targets, colorout)); // color permanente: reenvía el color de la ficha
          }
      }
  }
  // (No se incluye aquí la lógica de reproducción al presionar el botón del relé, que se gestionará en el loop)
}

void TOKEN_::printFicha(const TOKEN_DATA &f) {
  Serial.print("CMD: 0x"); Serial.println(f.cmd, HEX);
  Serial.print("CMD2: 0x"); Serial.println(f.cmd2, HEX);
  Serial.print("Bank: 0x"); Serial.println(f.addr.bank, HEX);
  Serial.print("File: 0x"); Serial.println(f.addr.file, HEX);
  Serial.print("R: 0x"); Serial.println(f.color.r, HEX);
  Serial.print("G: 0x"); Serial.println(f.color.g, HEX);
  Serial.print("B: 0x"); Serial.println(f.color.b, HEX);
  for (int i = 0; i < 8; i++) {
    Serial.print("P"); Serial.print(i);
    Serial.print(".Bank: 0x"); Serial.print(f.partner[i].bank, HEX);
    Serial.print("  P"); Serial.print(i);
    Serial.print(".File: 0x"); Serial.println(f.partner[i].file, HEX);
  }
}

byte TOKEN_::asciiHexToByte(char high, char low) {
  byte value = 0;
  if(high >= '0' && high <= '9')
    value = (high - '0') << 4;
  else if(high >= 'A' && high <= 'F')
    value = (high - 'A' + 10) << 4;
  else if(high >= 'a' && high <= 'f')
    value = (high - 'a' + 10) << 4;
  
  if(low >= '0' && low <= '9')
    value |= (low - '0');
  else if(low >= 'A' && low <= 'F')
    value |= (low - 'A' + 10);
  else if(low >= 'a' && low <= 'f')
    value |= (low - 'a' + 10);
  
  return value;
}

byte TOKEN_::hexToByte(const String &hex) {
  if (hex.length() < 2) return 0;
  char high = hex.charAt(0);
  char low  = hex.charAt(1);

  auto nibble = [](char c) -> byte {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
  };

  return (nibble(high) << 4) | nibble(low);
}


TOKEN_::TOKEN_DATA TOKEN_::parseTokenString(const String &tokenStr) {
  TOKEN_DATA data = {}; // Inicializa en 0

  // Se remueven los delimitadores '#' del inicio y fin
  if(tokenStr.charAt(0) != '#' || tokenStr.charAt(tokenStr.length()-1) != '#') {
    Serial.println("DEBUG: Token sin delimitadores válidos");
    return data;
  }
  String content = tokenStr.substring(1, tokenStr.length()-1);

  // Ahora se espera que el contenido tenga 46 caracteres (23 bytes)
  if(content.length() != 94) {
    Serial.print("DEBUG: Longitud incorrecta del token: ");
    Serial.println(content.length());
    return data;
  }
  
  data.cmd       = asciiHexToByte(content.charAt(0), content.charAt(1));
  data.cmd2      = asciiHexToByte(content.charAt(2), content.charAt(3));
  data.addr.bank = asciiHexToByte(content.charAt(4), content.charAt(5));
  data.addr.file = asciiHexToByte(content.charAt(6), content.charAt(7));
  data.color.r   = asciiHexToByte(content.charAt(8), content.charAt(9));
  data.color.g   = asciiHexToByte(content.charAt(10), content.charAt(11));
  data.color.b   = asciiHexToByte(content.charAt(12), content.charAt(13));
  
  for (int i = 0; i < 8; i++) {
    int idx = 14 + i * 4;
    data.partner[i].bank = asciiHexToByte(content.charAt(idx), content.charAt(idx+1));
    data.partner[i].file = asciiHexToByte(content.charAt(idx+2), content.charAt(idx+3));
  }
  return data;
}


