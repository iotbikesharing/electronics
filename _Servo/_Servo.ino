#include <Servo.h>
#include <EEPROM.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9
#define Servo_PIN 7
#define LED_RED 4
#define LED_GREEN 5
#define sensorPin  A0

Servo myservo;  // create servo object to control a servo
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
int uidsize;
char yn;
int potpin = 0;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin
int sensorValue = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  mfrc522.PCD_SetAntennaGain(0x07 << 4);
  myservo.attach(Servo_PIN);  // attaches the servo on pin 9 to the servo object
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, HIGH);
}


void loop() {
  IR();
  RC_Servo();

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
        //  Serial.println("Card in database");
        no = 1;
        return addr;
      }
    }
    //7-byte UID
    if (mfrc522.uid.size == 7) {
      if (((EEPROM.read(addr + 0) == storeduid[0]) &&
           (EEPROM.read(addr + 1) == storeduid[1]) &&
           (EEPROM.read(addr + 2) == storeduid[2]) &&
           (EEPROM.read(addr + 3) == storeduid[3]) &&
           (EEPROM.read(addr + 4) == storeduid[4]) &&
           (EEPROM.read(addr + 5) == storeduid[5]) &&
           (EEPROM.read(addr + 6) == storeduid[6])
          ) ) {
        //     Serial.println("Card in database");
        return addr;
      }
    }
    //If the compare go to the third last value, no match
    if (addr == 1021) {
      //Serial.println("No match");
      no = -1;
      return ;
    }
  }
}
void IR() {
  sensorValue = analogRead (sensorPin);
  if (sensorValue >= 820) {
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_RED, LOW);
    Serial.println(sensorValue);
  }
  else {
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH);
    myservo.write(val);
    Serial.println(sensorValue);
  }
}
void RC_Servo() {
  if ( ! mfrc522.PICC_IsNewCardPresent())
    return;

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial())
    return;

  byte j = 0;
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], DEC);
    storeduid[j] = mfrc522.uid.uidByte[i];
    j++;
  }
  Serial.println();

  //Print the storeduid to make sure it is correct, will erase it later
  for (int i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(storeduid[i]);
    Serial.print(" ");
  }
  Serial.println();

  //Compare the storeduid with the values in EEPROM
  compare(storeduid);
  if (no == 1) {
    digitalWrite(LED_RED, LOW);
    delay(2000);
    myservo.write(val);
    for (int i = 0; i < 3; i++) {
      digitalWrite(LED_GREEN, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_GREEN, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);
    }

  }
  if ( no == -1) {
    digitalWrite(LED_GREEN, LOW);
    for (int i = 0; i < 5; i++) {
      digitalWrite(LED_RED, LOW);
      delay(1000);
      digitalWrite(LED_RED, HIGH);    // turn the LED off by making the voltage LOW
      delay(1000);
    }
    Serial.println(sensorValue);
  }

  Serial.println();
  // Halt PICC
  mfrc522.PICC_HaltA();
  // Stop encryption on PCD
  mfrc522.PCD_StopCrypto1();
}

