#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

void setup() {
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522
  //  Serial.println("Antenna gain:");
  //  Serial.println(mfrc522.PCD_GetAntennaGain());
  mfrc522.PCD_SetAntennaGain(0x07 << 4);
  //  Serial.println("New antenna gain:");
  //  Serial.println(mfrc522.PCD_GetAntennaGain());
  //  Serial.println("---------------------------------------");
  //  Serial.println("Try a card");
  }


  void loop() {

    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
      return;

    // Verify if the NUID has been readed
    if ( ! mfrc522.PICC_ReadCardSerial())
      return;

    //  Serial.println("Card UID: ");
    //
    //  Serial.print("In HEX:");
    //  for (byte i = 0; i < mfrc522.uid.size; i++) {
    //    if(mfrc522.uid.uidByte[i] < 0x10)
    //      Serial.print(F(" 0"));
    //    else
    //      Serial.print(F(" "));
    //    Serial.print(mfrc522.uid.uidByte[i], HEX);
    //  }
    //  Serial.println();
    //  Serial.print("In DEC:");
    //  for (byte i = 0; i < mfrc522.uid.size; i++) {
    //    if(mfrc522.uid.uidByte[i] < 0x10)
    //      Serial.print(F(" 0"));
    //    else
    //      Serial.print(F(" "));
    //    Serial.print(mfrc522.uid.uidByte[i], DEC);
    //  }
    //  Serial.println();
    //  Serial.print("SAK value: ");
    //  Serial.println(mfrc522.uid.sak,HEX);
    //
    //  Serial.print(F("PICC type: "));
    //  MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    //  Serial.println(mfrc522.PICC_GetTypeName(piccType));

    for (byte i = 0; i < mfrc522.uid.size; i++) {
      if (mfrc522.uid.uidByte[i] < 0x10)
        Serial.print(F(" 0"));
      else
        Serial.print(F(" "));
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println();
    // Halt PICC
    mfrc522.PICC_HaltA();

    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
 }

