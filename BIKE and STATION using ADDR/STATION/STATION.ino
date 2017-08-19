#include <SoftwareSerial.h>
#define Tx_Led 9
#define Rx_Led 10
#define Button 11
#define AUX 6

SoftwareSerial mySerial(2, 3); // (2-RX) <<-- Connect TX pin of LoRa  ......  (3-TX) <<-- Connect RX pin of LoRa
// gnd SET_A and SET_B for Normal Mode (Send and Receive)

//String num = "3333";
//char data [1000];

char data1[100] = {0x00, 0x01, 0x17,1};
char data2[100] = {0x00, 0x02, 0x17,2};

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode (Tx_Led, OUTPUT);
  pinMode (Rx_Led, OUTPUT);
  pinMode (Button, INPUT);
  pinMode (AUX, INPUT);
}

void loop() {

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
          Serial.println ("Bike 1");
          Serial.println ("Latitude: ");
          Serial.print (data [1], DEC);
          Serial.print (".");
          for (int i =2; i< 9; i++) {
              Serial.print (data[i],DEC);
          }
          Serial.println ("");
          
          Serial.println ("Longitude: ");
          Serial.print (data [9],DEC);
          Serial.print (data [10],DEC);
          Serial.print (".");
          for (int j= 11; j < 19; j++) {
            Serial.print (data [j], DEC);
          }
          Serial.println (""); 
          data [0] = 0;
          delay (100);
          break;

          //BIKE 2
          case 2:
         
          mySerial.write (data2, 100);
          Serial.println ("Bike 2");
          Serial.println ("Latitude: ");
          Serial.print (data [1], DEC);
          Serial.print (".");
          for (int i =2; i< 7; i++) {
             Serial.print (data[i],DEC);
          }
          Serial.println ("");
          
          Serial.println ("Longitude: ");
          Serial.print (data [8],DEC);
          Serial.print (data [9],DEC);
          Serial.print (".");
          for (int j= 10; j < 17; j++) {
            Serial.print (data [j], DEC);
          }
          Serial.println (""); 
          data [0] = 0;
          delay (100);
          break;

        }
  }

  //Serial.println (digitalRead (AUX));

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

