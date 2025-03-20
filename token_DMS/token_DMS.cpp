#include <Arduino.h>
#include <defines_DMS/defines_DMS.h>
#include <token_DMS/token_DMS.h>
#include <play_DMS/play_DMS.h>
#include <Element_DMS/Element_DMS.h>
#include <encoder_handler/encoder_handler.h>
#include <token_DMS/token_DMS.h>
#include <Pulsadores_handler/Pulsadores_handler.h>

// Definición de pines I2C para ESP32 (ajústalos según tu hardware)
#define SDA_PIN 40
#define SCL_PIN 41

// Se instancian objetos globales para la comunicación con el PN532
static PN532_I2C pn532i2c(Wire);
static NfcAdapter nfcAdapter(pn532i2c);

TOKEN_::TOKEN_() : lastReadAttempt(0), readInterval(200), genre(0), lang(static_cast<uint8_t>(currentLanguage)) {
    currentUID = "";
    lastProcessedUID = "";
}

void TOKEN_::begin() {
  Serial.println("DEBUG: Inicializando token_DMS con PN532");
  
  const int MAX_RETRIES = 5;
  const int RETRY_DELAY = 1000;
  bool initialized = false;
  int retryCount = 0;
  
  // Crear una instancia temporaria de PN532 para acceder a sus métodos
  PN532 pn532(pn532i2c);
  
  while (!initialized && retryCount < MAX_RETRIES) {
    Serial.printf("Intento %d de inicializar PN532...\n", retryCount + 1);
    
    // Reiniciar el bus I2C primero
    Wire.end();
    delay(100);
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000); // Velocidad más baja para estabilidad
    
    // Inicializar el PN532
    pn532.begin();
    
    // Verificar la comunicación
    uint32_t versiondata = pn532.getFirmwareVersion();
    if (versiondata) {
      Serial.print("PN532 encontrado. Chip PN5");
      Serial.println((versiondata >> 24) & 0xFF, HEX);
      Serial.print("Firmware v");
      Serial.print((versiondata >> 16) & 0xFF, DEC);
      Serial.print('.');
      Serial.println((versiondata >> 8) & 0xFF, DEC);
      
      // Configurar el PN532
      pn532.SAMConfig();
      
      // Ahora inicializar el adaptador NFC
      nfcAdapter.begin();
      
      initialized = true;
      Serial.println("DEBUG: nfcAdapter iniciado correctamente. Esperando tarjeta NFC...");
    } else {
      Serial.println("Fallo al inicializar PN532. Reintentando...");
      retryCount++;
      delay(RETRY_DELAY);
    }
  }
  
  if (!initialized) {
    Serial.println("ERROR: No se pudo inicializar el módulo PN532 después de varios intentos.");
    Serial.println("Procediendo sin funcionalidad NFC.");
  }
}

bool TOKEN_::readCard(String &uid) {
  // Se utiliza un timeout de 1ms segundo para la detección
  if (!nfcAdapter.tagPresent(1)) {
      currentUID = "";
      return false;
  }

  NfcTag tag = nfcAdapter.read();

  // Verificar si el tag es válido
  if (!tag.hasNdefMessage() || tag.getUidLength() == 0) {
      Serial.println("⚠️ ERROR: Tag NFC detectado pero sin datos válidos.");
      currentUID = "";
      return false;
  }

  // Obtener el UID y verificar que no esté vacío
  String newUID = tag.getUidString();
  if (newUID.length() == 0) {
      Serial.println("⚠️ ERROR: UID de la tarjeta vacío. Ignorando...");
      currentUID = "";
      return false;
  }

  // Asignar el UID a las variables de control
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

  NfcTag tag = nfcAdapter.read();
  if (!tag.hasNdefMessage() || tag.getUidLength() == 0) {
      Serial.println("⚠️ ERROR: No se encontró un mensaje NDEF válido.");
      mensaje = "";
      return false;
  }

  NdefMessage message = tag.getNdefMessage();
  int recordCount = message.getRecordCount();
  if (recordCount <= 0) {
      Serial.println("⚠️ ERROR: No se encontraron registros NDEF en la tarjeta.");
      mensaje = "";
      return false;
  }

  NdefRecord record = message.getRecord(0);
  int payloadLength = record.getPayloadLength();
  if (payloadLength <= 0 || payloadLength > 52) {
      Serial.println("⚠️ ERROR: Payload fuera de rango. Ignorando mensaje.");
      mensaje = "";
      return false;
  }

  // Leer y procesar el payload
  byte payloadBuffer[256] = {0};
  record.getPayload(payloadBuffer);
  mensaje = decodeNdefText(payloadBuffer, payloadLength);

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
    guessbank = 0x0E; //eliminar esta linea
    fileRand = 0x02;  //eliminar esta linea
    if (guessbank > 0x09 && guessbank < 0x63) lang = static_cast<uint8_t>(currentLanguage) * 10;
    doitPlayer.play_file(guessbank + genre, fileRand + lang);
    propossedToken.addr.bank = guessbank;
    propossedToken.addr.file = fileRand;
}

// Función token_handler corregida
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

      // Iniciar reproducción de audio sin género y lenguaje añadido
      byte lang = 0;
      if (token.addr.bank > 0x09 && token.addr.bank < 0x63) {
          lang = static_cast<uint8_t>(currentLanguage) * 10;}
          
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

void TOKEN_::printFicha(const TOKEN_::TOKEN_DATA &f) {
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


// Función auxiliar para convertir dos caracteres ASCII hex a un byte
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

// Función que parsea el token (cadena) y actualiza TOKEN_DATA
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
  
  // Se leen los campos según el nuevo orden:
  data.cmd       = asciiHexToByte(content.charAt(0), content.charAt(1));
  data.cmd2      = asciiHexToByte(content.charAt(2), content.charAt(3));
  data.addr.bank = asciiHexToByte(content.charAt(4), content.charAt(5));
  data.addr.file = asciiHexToByte(content.charAt(6), content.charAt(7));
  data.color.r   = asciiHexToByte(content.charAt(8), content.charAt(9));
  data.color.g   = asciiHexToByte(content.charAt(10), content.charAt(11));
  data.color.b   = asciiHexToByte(content.charAt(12), content.charAt(13));
  
  // Se procesan los 8 pares de partners, cada pareja ocupa 4 caracteres
  for (int i = 0; i < 8; i++) {
    int idx = 14 + i * 4;
    data.partner[i].bank = asciiHexToByte(content.charAt(idx), content.charAt(idx+1));
    data.partner[i].file = asciiHexToByte(content.charAt(idx+2), content.charAt(idx+3));
  }
  return data;
}
