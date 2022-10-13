#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D8
#define RST_PIN D1


MFRC522 device(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}}; 

MFRC522::Uid uid = {0, {0}, 0};

void setup() {
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  device.PCD_Init(); // Init MFRC522 
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();
  Serial.println(F("This code scan the MIFARE Classsic NUID."));

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
    Serial.println();
    uid = device.uid;
    digitalWrite(LED_BUILTIN, LOW); 
    device.PICC_HaltA();
    return;
  }

  // Write to new card
  if(device.uid.uidByte[0] == uid.uidByte[0] && 
     device.uid.uidByte[1] == uid.uidByte[1] && 
     device.uid.uidByte[2] == uid.uidByte[2] && 
     device.uid.uidByte[3] == uid.uidByte[3] ){
    
    Serial.print(F("Same card UID presented("));
    printHex(device.uid.uidByte, device.uid.size);
    Serial.println(")");
    blink2();
    device.PICC_HaltA();
    return;
  }

  if( device.MIFARE_SetUid(uid.uidByte, uid.size, true)){
    Serial.println(F("Wrote new UID to card."));
    uid = {0, {0}, 0};
    digitalWrite(LED_BUILTIN, HIGH);

    // Dump card info and halt the card.
    if ( device.PICC_IsNewCardPresent() && device.PICC_ReadCardSerial() ) {
      device.PICC_DumpToSerial(&device.uid);
      Serial.print("New uid: ");
      printHex(device.uid.uidByte, device.uid.size);
      Serial.println();
    }
    return; 
  }

  Serial.println(F("Error writing to new card. Try again."));
  blink2();
  return;
}

void blink2(){
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(100);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH); 
  delay(100);
  digitalWrite(LED_BUILTIN, LOW); 
  delay(100);
}

void printHex(byte *buf, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(i > 0 ? " " : ""); 
    Serial.print(buf[i] < 0x10 ? "0" : "");
    Serial.print(buf[i], HEX);
  }
}
