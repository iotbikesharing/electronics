#include <MFRC522.h>
#include <SoftwareSerial.h>
#define Tx_Led 5
#define Rx_Led 7
#define Button 8
#define AUX 6
#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Instance of the class

SoftwareSerial mySerial(2, 3); // (2-RX) <<-- Connect TX pin of LoRa  ......  (3-TX) <<-- Connect RX pin of LoRa
// gnd SET_A and SET_B for Normal Mode (Send and Receive)


char data1[100] = {0x00, 0x01, 0x17,1};
char data2[100] = {0x00, 0x02, 0x17,2};

void setup() {
  Serial.begin(9600);
  //LORA
  mySerial.begin(9600);
  pinMode (Tx_Led, OUTPUT);
  pinMode (Rx_Led, OUTPUT);
  pinMode (Button, INPUT);
  pinMode (AUX, INPUT);

  //RFID
  SPI.begin();
  mfrc522.PCD_Init(); // Init MFRC522
 
  mfrc522.PCD_SetAntennaGain(0x07 << 4);
}

void loop() {
  RFID ();
  LORA ();
}

void RFID () {
   // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent())
      return;

    // Verify if the NUID has been readed
    if ( ! mfrc522.PICC_ReadCardSerial())
      return;
      Serial.println ("UID: ");
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



void LORA () {
  if (digitalRead (Button) == HIGH) {
    digitalWrite (Tx_Led, HIGH);
   // mySerial.println(num);
    mySerial.write (data1, 100);
     //mySerial.write ("Received");
    digitalWrite (Tx_Led, LOW);
    delay (1500);
  }

  // Receive the data from the bikes and check which bike it is
  // then send a confirmation signal
  if (mySerial.available() > 1)
  {

        char data [1000];
        String input = mySerial.readString();   
        input.toCharArray (data,1000);
        
        switch (data [0]){

          //BIKE 1
          case 1:

          mySerial.write (data1, 100);
//          Serial.print ("Bike 1 Lat/Long: ");
//          Serial.print (data [1], DEC);
//          Serial.print (".");
//          for (int i = 2; i <9; i++) {
//            Serial.print (data [i], DEC);
//          }
//          Serial.print ("/");
//          Serial.print (data[9], DEC);
//          Serial.print (data[10], DEC);
//          Serial.print (".");
//          for (int j=11; j <19; j++) {
//            Serial.print (data [j], DEC);
//          }
//          Serial.println ();

          data [0] = 0;
          Serial.println ("Bike 1 current at RMIT Vietnam Sports Hall");
          
          
          delay (100);
          break;

          //BIKE 2
          case 2:

          mySerial.write (data2,100);
          Serial.print ("Bike 2 Lat/Long: ");
          Serial.print (data [1],DEC);
          Serial.print (".");
          for (int i=2; i<7; i++) {
            Serial.print (data[i], DEC);
          }
          Serial.print ("/");
          Serial.print (data[8], DEC);
          Serial.print (data[9], DEC);
          Serial.print (".");
          for (int j =10; j <17; j ++) {
            Serial.print (data[j],DEC);
          }
          Serial.println();

          data [0] = 0;
          delay (100);
          break;

        }
  }

  if (digitalRead (AUX) == LOW)
  {
    digitalWrite (Rx_Led, HIGH);
  }

  else if (digitalRead (AUX) == HIGH)
  {
    digitalWrite (Rx_Led, LOW);
  }

  delay(20);
}

