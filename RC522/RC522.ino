#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN);

MFRC522::StatusCode status;
MFRC522::MIFARE_Key key;

byte buffer[34];
byte block;
byte bufferATQA[4];
byte bufferSize = sizeof(bufferATQA);
byte result;
int storeduid[10];
byte value = 0;
int addr = 0;
int no = 0;

char yn;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println("Antenna gain:");
  Serial.println(mfrc522.PCD_GetAntennaGain());
  mfrc522.PCD_SetAntennaGain(0x07 << 4);
  Serial.println("New antenna gain:");
  Serial.println(mfrc522.PCD_GetAntennaGain());
  Serial.println("---------------------------------------");
  Serial.println("Try a card");

}
void loop() {
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  // Show some details of the PICC (that is: the tag/card)
  mfrc522.PICC_DumpDetailsToSerial(&(mfrc522.uid));

  Serial.println();

  //Print the UID in DEC and store it into storeduid for comparision
  byte j = 0;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], DEC);
    storeduid[j] = mfrc522.uid.uidByte[i];
    j++;
  }
  Serial.println();

  //Print the storeduid to make sure it is correct, will erase it later
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(storeduid[i]);
    Serial.print(" ");
  }
  Serial.println();

  //Compare the storeduid with the values in EEPROM
  compare(storeduid);


  //Ask the users to save the new UID into the system or not
  if (no == -1) {
    addr = 0;
    Serial.println ("Do you want to add this UID into database? \n press (y) for yes or (n) for no");

    while (!Serial.available())
      if (Serial.available()) {
        yn = Serial.read();
      }
    Serial.println(yn);
    
    //If yes, save into the EEPROM
    if (yn == 'y') {
      Serial.println("Saving");
      unsigned int i = 0;
      for (int k = addr + 4; k < (addr + 4 + mfrc522.uid.size); k++) {
        EEPROM.update(k, storeduid[i]);
        i++;
      }
      //      for (int k = addr + 4; k < (addr + 4 + mfrc522.uid.size); k++) {
      //        EEPROM.put(k, storeduid[i]);
      //        i++;
      //      }
      Serial.println("Saved");
    }
    
    //If no, continue to end of loop
    if (yn == 'n') {
      Serial.println("Sequence terminated");
    }
  }
  Serial.println("----------------------------------------------");
  Serial.println("----------------------------------------------");
  Serial.end();
  Serial.begin(9600);
}

//Compare function
void compare(int *storeduid) {
  no = 0;
  for (addr = 0; addr < 1024; addr++) {
    //4-byte UID case
    if (mfrc522.uid.size == 4) {
      if ((EEPROM.read(addr + 0) == storeduid[0]) &&
          (EEPROM.read(addr + 1) == storeduid[1]) &&
          (EEPROM.read(addr + 2) == storeduid[2]) &&
          (EEPROM.read(addr + 3) == storeduid[3])) {
        Serial.println("Card in database");
        //        no =1;
        return addr;
      }
    }
    //7-byte UID
    if (mfrc522.uid.size == 7) {
      if ((EEPROM.read(addr + 0) == storeduid[0]) &&
          (EEPROM.read(addr + 1) == storeduid[1]) &&
          (EEPROM.read(addr + 2) == storeduid[2]) &&
          (EEPROM.read(addr + 3) == storeduid[3]) &&
          (EEPROM.read(addr + 4) == storeduid[4]) &&
          (EEPROM.read(addr + 5) == storeduid[5]) &&
          (EEPROM.read(addr + 6) == storeduid[6])) {
        Serial.println("Card in database");
        return addr;
      }
    }
    //If the compare go to the third last value, no match
    if (addr = 1021) {
      Serial.println("No match");
      no = -1;
      return ;
    }
  }
}


