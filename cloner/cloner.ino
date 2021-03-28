#include <SPI.h>
#include <MFRC522.h>
#include <string.h>

#define SS_PIN 10
#define RST_PIN 9


MFRC522 device(SS_PIN, RST_PIN); // Instance of the class

//MFRC522::MIFARE_Key key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; 

MFRC522::Uid uid = {0, {0}, 0};

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  device.PCD_Init(); // Init MFRC522 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
//  Serial.println(F("This code scan the MIFARE Classsic NUID."));
//  Serial.print(F("Using the following key:"));
//  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);  
}

void loop() {

//  if ( ! device.PICC_IsNewCardPresent()) return;
//  if ( ! device.PICC_ReadCardSerial()) return;
  if( !(device.PICC_IsNewCardPresent() && device.PICC_ReadCardSerial()) ){ delay(50); return; }

  MFRC522::PICC_Type piccType = device.PICC_GetType(device.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Your tag is not of type MIFARE Classic."));
    return;
  }

  if(uid.size == 0){
    Serial.println(F("New card uid read."));
    Serial.print(F("In hex: "));
    printHex(device.uid.uidByte, device.uid.size);
    uid = device.uid;
    digitalWrite(LED_BUILTIN, HIGH); 
    return;
  }

  // Write to new card
  if(strcmp(uid.uidByte, device.uid.uidByte) == 0){
     Serial.println(F("Same card UID presented."));
     return;  
  }

  if( device.MIFARE_SetUid(uid.uidByte, uid.size, true)){
    Serial.println(F("Wrote new UID to card."));
    uid = {0, {0}, 0};
    digitalWrite(LED_BUILTIN, LOW);
    return; 
  }

  Serial.println(F("Error: Writing to new card. Try again."));
}


void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}
