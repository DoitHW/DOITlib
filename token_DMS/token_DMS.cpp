#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <Pulsadores_handler/Pulsadores_handler.h>

// Definición de pines I2C para ESP32 (ajústalos según tu hardware)
#define SDA_PIN 40
#define SCL_PIN 41

// Definición de pines para el PN532 en modo I2C con IRQ
#define PN532_IRQ   (42)    // Ajusta este valor según tu conexión
#define PN532_RESET (-1)    // Normalmente -1 si no se utiliza reset

// Instancia global del PN532 utilizando la librería Adafruit_PN532
static Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

// Variables para manejo de IRQ (para uso interno de la lectura)
extern int irqCurr;
extern int irqPrev;
extern const int DELAY_BETWEEN_CARDS;  // Si lo defines globalmente en main o aquí
extern unsigned long timeLastCardRead; // Lo mismo para estas variables
extern boolean readerDisabled;         // Estas pueden definirse globalmente en main

TOKEN_::TOKEN_() : lastReadAttempt(0), readInterval(200), genre(0), lang(static_cast<uint8_t>(currentLanguage)) {
    currentUID = "";
    lastProcessedUID = "";
}

void TOKEN_::begin() {
  const int MAX_RETRIES = 5;
  int retryCount = 0;

  Serial.println("DEBUG: Inicializando token_DMS con PN532 (Adafruit)");

  Wire.begin(SDA_PIN, SCL_PIN);
  delay(100);

  while (retryCount < MAX_RETRIES) {
    nfc.begin();

    uint32_t versiondata = nfc.getFirmwareVersion();
    if (versiondata) {
      Serial.print("Found chip PN5");
      Serial.println((versiondata >> 24) & 0xFF, HEX);
      Serial.print("Firmware ver. ");
      Serial.print((versiondata >> 16) & 0xFF, DEC);
      Serial.print('.');
      Serial.println((versiondata >> 8) & 0xFF, DEC);

      nfc.SAMConfig();
      Serial.println("DEBUG: PN532 configurado correctamente. Esperando tarjeta NFC...");

      startListeningToNFC();
      return; // Salir si inicialización fue exitosa
    }

    Serial.printf("ERROR: No se encontró el módulo PN532 (intento %d de %d). Reintentando...\n", retryCount + 1, MAX_RETRIES);
    retryCount++;
    delay(1000); // Espera entre reintentos
  }

  Serial.println("FATAL: No se pudo inicializar el PN532 tras múltiples intentos. Reiniciando dispositivo...");
  delay(2000); // Pequeña pausa para ver mensaje
  ESP.restart();
}

bool TOKEN_::isCardPresent() {
  uint8_t _uid[7] = {0};
  uint8_t uidLength = 0;
  
  // Usamos un timeout muy corto para un sondeo rápido
  bool result = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, _uid, &uidLength, 30);
  
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
  Serial.println("Starting passive read for an ISO14443A Card ...");
  
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

bool TOKEN_::leerMensajeNFC(String& mensaje) {
  Serial.println("DEBUG: Procesando mensaje NDEF para la tarjeta con UID: " + currentUID);

  uint8_t buffer[128] = {0};
  int len = 0;
  for (uint8_t page = 4; page < 4 + 32; page++) { // Leer hasta 32 páginas (máximo 128 bytes)
    uint8_t pageBuffer[4] = {0};
    if (!nfc.ntag2xx_ReadPage(page, pageBuffer)) {
      Serial.println("⚠️ ERROR: Falló la lectura de la página");
      mensaje = "";
      return false;
    }
    memcpy(buffer + len, pageBuffer, 4);
    len += 4;
    for (int j = 0; j < 4; j++) {
      if (pageBuffer[j] == 0xFE) {
        len = len - (4 - j);
        goto end_read;
      }
    }
  }
end_read:
  if (buffer[0] != 0x03) {
      Serial.println("⚠️ ERROR: No se encontró el inicio del mensaje NDEF.");
      mensaje = "";
      return false;
  }
  uint8_t ndefLength = buffer[1];
  if (ndefLength == 0 || ndefLength > 52) {
      Serial.println("⚠️ ERROR: Longitud del mensaje NDEF fuera de rango.");
      mensaje = "";
      return false;
  }

  byte payloadBuffer[256] = {0};
  memcpy(payloadBuffer, buffer + 2, ndefLength);
  mensaje = decodeNdefText(payloadBuffer, ndefLength);

  if (mensaje.startsWith("#") && mensaje.endsWith("#")) {
      currentToken = parseTokenString(mensaje);
      lastProcessedUID = currentUID;
      return true;
  } else {
      Serial.println("⚠️ ERROR: El formato del token es inválido.");
      mensaje = "";
      return false;
  }
}
bool TOKEN_::leerPagina(uint8_t pagina, uint8_t *buffer) {
    // En la nueva lógica se utiliza la lectura completa de NDEF, por lo que esta función queda sin implementar.
    return false;
}

void TOKEN_::proponer_token(byte guessbank) {
    lang = 0;
    int fileRand = random(1, 8);  // Genera un número aleatorio entre 1 y 7
    guessbank = 0x0E; // eliminar esta línea
    fileRand = 0x02;  // eliminar esta línea
    if (guessbank > 0x09 && guessbank < 0x63) lang = static_cast<uint8_t>(currentLanguage) * 10;
    doitPlayer.play_file(guessbank + genre, fileRand + lang);
    propossedToken.addr.bank = guessbank;
    propossedToken.addr.file = fileRand;
}

void TOKEN_::token_handler(TOKEN_DATA token, uint8_t lang_in, bool genre_in, uint8_t myid, std::vector<uint8_t> targets) {
  byte lang = 0;
  bool audioStarted = false;
  if (token.addr.bank > 0x09 && token.addr.bank < 0x63) lang = lang_in * 10;
  
  if (token.cmd == TOKEN_CMD) {
      // Enviar color
      COLOR_T colorout;
      colorout.red   = token.color.r;
      colorout.green = token.color.g;
      colorout.blue  = token.color.b;
      send_frame(frameMaker_SEND_RGB(myid, targets, colorout));

      // Pequeña pausa para asegurar que el color se aplica antes de iniciar el audio
      delay(100);

      // Iniciar reproducción de audio sin género y con lenguaje añadido
      byte lang = 0;
      if (token.addr.bank > 0x09 && token.addr.bank < 0x63) {
          lang = static_cast<uint8_t>(currentLanguage) * 10;
      }
          
      doitPlayer.play_file(token.addr.bank + genre, token.addr.file + lang);
      delay(50);

      // Verificar si el audio está reproduciéndose
      for (int i = 0; i < 10; i++) {
          if (doitPlayer.is_playing()) {
              audioStarted = true;
              break;
          }
          delay(50);
      }

      // Modo PAREJAS
      if (tokenCurrentMode == TOKEN_PARTNER_MODE) {
          if (!waitingForPartner) {
              currentToken = token;
              waitingForPartner = true;
          } else {
              bool match = false;
              for (auto &partner : currentToken.partner) {
                  if (token.addr.bank == partner.bank && token.addr.file == partner.file) {
                      match = true;
                      break;
                  }
              }

              while (doitPlayer.is_playing()) { delay(10); }
              int fileNum = match ? random(1, 4) : random(1, 3);
              send_frame(frameMaker_SEND_COLOR(myid, targets, match ? 7 : 3)); // verde si hay match, rojo si no
              doitPlayer.play_file((match ? WIN_RESP_BANK : FAIL_RESP_BANK) + genre_in, fileNum + lang);
              while (doitPlayer.is_playing()) { delay(10); }

              send_frame(frameMaker_SEND_COLOR(myid, targets, 8));
              waitingForPartner = false;
          }
      }

      // Modo ADIVINAR
      if (tokenCurrentMode == TOKEN_GUESS_MODE) {
          bool isCorrect = (token.addr.bank == propossedToken.addr.bank && token.addr.file == propossedToken.addr.file);

          while (doitPlayer.is_playing()) { delay(10); }
          send_frame(frameMaker_SEND_COLOR(myid, targets, isCorrect ? 7 : 3)); // verde si es correcto, rojo si no
          doitPlayer.play_file((isCorrect ? WIN_RESP_BANK : FAIL_RESP_BANK) + genre_in, random(1, isCorrect ? 4 : 3) + lang);
          while (doitPlayer.is_playing()) { delay(10); }

          send_frame(frameMaker_SEND_COLOR(myid, targets, 8));
      }
  }

  // Esperar a que termine la reproducción del audio si se inició correctamente
  if (doitPlayer.is_playing())
  {
    Serial.println("Esperando a que termine el audio...");
    while (doitPlayer.is_playing())
    {
      delay(10);
    }
  }
  else
  {
    Serial.println("No se detectó reproducción de audio o terminó rápidamente");
  }
  send_frame(frameMaker_SEND_COLOR(myid, targets, 8));
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

TOKEN_::TOKEN_DATA TOKEN_::parseTokenString(const String &tokenStr) {
  TOKEN_DATA data = {}; // Inicializa en 0

  // Se remueven los delimitadores '#' del inicio y fin
  if(tokenStr.charAt(0) != '#' || tokenStr.charAt(tokenStr.length()-1) != '#') {
    Serial.println("DEBUG: Token sin delimitadores válidos");
    return data;
  }
  String content = tokenStr.substring(1, tokenStr.length()-1);

  // Ahora se espera que el contenido tenga 46 caracteres (23 bytes)
  if(content.length() != 46) {
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
