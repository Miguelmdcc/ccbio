#include <Arduino.h>
#include <ESPSupabase.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

Supabase db;

// Put your supabase URL and Anon key here...
String supabase_url = "https://fkobhwxaljobbnznmhih.supabase.co";
String anon_key = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6ImZrb2Jod3hhbGpvYmJuem5taGloIiwicm9sZSI6ImFub24iLCJpYXQiOjE3MzMyMzU5NDEsImV4cCI6MjA0ODgxMTk0MX0.u2f6_v84xO6ww7PJo3HaIlT35E3hGXxj0yNwVYkRRRk";

// Put your target table here
String table = "Usuários";

bool upsert = false;

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(4, 5);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif


Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

void setup() {
  Serial.begin(9600);

  Serial.print("Connecting to WiFi");
  WiFi.begin("Felipe-2.4G.", "42611752");
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  Serial.println("Connected!");

  // Beginning Supabase Connection
  db.begin(supabase_url, anon_key);

    // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
  Serial.print(F("Security level: ")); Serial.println(finger.security_level);
  Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);
}

uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  // int code = db.insert(table, JSON, upsert);
  // Serial.println(code);
  // db.urlQuery_reset();

  // Select query with filter and order, limiting the result is mandatory here
  String leitura = db.from("Leitura").select("*").eq("Ler", "True").order("Ler", "asc", true).limit(1).doSelect();
    // Encontrar o valor de "Ler" no JSON
  int startIndex = leitura.indexOf("\"Ler\":") + strlen("\"Ler\":"); // Posição após a chave "Ler"
  int endIndex = leitura.indexOf(",", startIndex); // Encontra a vírgula após o valor

  // Se "Ler" for o último campo, não há vírgula, então procurar o fechamento do array "]"
  if (endIndex == -1) {
    endIndex = leitura.indexOf("]", startIndex);
  }

  // Extrair o valor de "Ler"
  String lerStr = leitura.substring(startIndex, endIndex);
  
  // Verificar se o valor é "true" ou "false"
  boolean ler = (lerStr == "true"); // Se for "true", a variável 'ler' será verdadeira, caso contrário, será falsa
  if(ler == true){
      // Select query with filter and order, limiting the result is mandatory here
  leitura = db.from("Leitura").select("*").eq("Ler", "True").order("Ler", "asc", true).limit(1).doSelect();
    // Encontrar o valor de "Ler" no JSON
  startIndex = leitura.indexOf("\"Ler\":") + strlen("\"Ler\":"); // Posição após a chave "Ler"
  endIndex = leitura.indexOf(",", startIndex); // Encontra a vírgula após o valor

  // Se "Ler" for o último campo, não há vírgula, então procurar o fechamento do array "]"
  if (endIndex == -1) {
    endIndex = leitura.indexOf("]", startIndex);
  }

  // Extrair o valor de "Ler"
  lerStr = leitura.substring(startIndex, endIndex);

  // Verificar se o valor é "true" ou "false"
  ler = (lerStr == "true"); // Se for "true", a variável 'ler' será verdadeira, caso contrário, será falsa

    //Serial.println("Leitura de Digital");
    int digital_id = -1;
    while(digital_id == -1){
      digital_id = getFingerprintIDez();
    }
    Serial.println(digital_id);
    String digital = "{\"digital\": \"" + String(digital_id) + "\", \"Ler\": false}";
    int digital_code = db.update("Leitura").eq("digital", "0").doUpdate(digital);
  }else{
      // Select query with filter and order, limiting the result is mandatory here
  String read = db.from("Usuários").select("*").eq("Cadastrado", "0").order("Cadastrado", "asc", true).limit(1).doSelect();
  //Serial.println(read);

  // Consulta geral à tabela "Usuários" (sem filtros)
  String registro = db.from("Usuários").select("*").limit(1).doSelect();

  // Verificando se a tabela está vazia
  if (registro == "[]" || registro.length() <= 2) { // JSON vazio tem formato "[]"
    //Serial.println("A tabela 'Usuários' está vazia.");
    finger.emptyDatabase();
  } else {
    //Serial.println("A tabela 'Usuários' contém registros.");
    //Serial.println(registro); // Exibe o registro retornado
      // Encontrar a posição onde começa o valor de "Digital_number"
  int startIndex = read.indexOf("\"Digital_number\":") + strlen("\"Digital_number\":");
  int endIndex = read.indexOf(",", startIndex); // Localiza a próxima vírgula após o valor
  
  // Se "Digital_number" for o último valor, não há vírgula, então podemos usar o índice de fechamento do array
  if (endIndex == -1) {
    endIndex = read.indexOf("]", startIndex);
  }
  
  // Extrair o valor de "Digital_number"
  String digitalNumberStr = read.substring(startIndex, endIndex);
  
  // Converter para um número inteiro
  int digitalNumber = digitalNumberStr.toInt();

  // Exibir o valor extraído
  //Serial.print("Digital_number: ");
  //Serial.println(digitalNumber);

  if(digitalNumber != 0){
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    id = digitalNumber;

    Serial.print("Enrolling ID #");
    Serial.println(id);

    while (! getFingerprintEnroll() );

    int value = id;

    // Put your JSON that you want to insert rows
    // You can also use library like ArduinoJson generate this
    String JSON = "{\"Digital_number\": \""+String(value)+"\", \"Cadastrado\": 1}";

    int code = db.update("Usuários").eq("Cadastrado", "0").doUpdate(JSON);
    Serial.println(code);

  }


  // Reset Your Query before doing everything else
  db.urlQuery_reset();
  }
  }

}

uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      break;
    default:
      Serial.println("Unknown error");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  Serial.print("Creating model for #");  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  return true;
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);

  return finger.fingerID;
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;

  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID);
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID;
}
