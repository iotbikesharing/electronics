#include <SoftwareSerial.h>
#define Tx_Led 9
#define Aux_Led 10
#define Button 11
#define AUX 6

SoftwareSerial mySerial(2, 3); // (2-RX) <<-- Connect TX pin of LoRa  ......  (3-TX) <<-- Connect RX pin of LoRa
// gnd SET_A and SET_B for Normal Mode (Send and Receive)


char data1[100] = {0x00 , 0x00, 0x17,1,21,1,3,6,7 ,7,7, 1, 10,5,8,34,64,61,99,99,99,5};                   // Create an array to store incoming data
char data [100];

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(Tx_Led, OUTPUT);
  pinMode(Aux_Led, OUTPUT);
  pinMode(Button,INPUT);
  pinMode(AUX, INPUT);

}

void loop() 
{
  // Sending the dummy data
  if (digitalRead(Button) == HIGH) 
  {
    digitalWrite(Tx_Led, HIGH);
    mySerial.write (data1, 100);
    digitalWrite(Tx_Led, LOW);
    //delay (3500);
    delay (60000);  
  }

  //Receive the confirmation from the station and print it
  if (mySerial.available() > 1)
  {
    String input = mySerial.readString();       // Read input from buffer like normal                   
    input.toCharArray(data,100);                // Convert input type String to store into array data[]
//    Serial.println (data);
    //delay (2000);
    switch (data [2]) 
      {
        case 0:
          Serial.println ("Transmission failed ... Re-sending message");
          mySerial.write (data1, 100);
          delay (1500);
          break;
        case 1:
          Serial.println ("Data sent");
          data [2] = 0;
          break;
    }
 
  }

  // LED for checking if the module is receiving or sending data
  if (digitalRead(AUX) == LOW) 
  {
    digitalWrite(Aux_Led, HIGH);
  }
  
  else if (digitalRead(AUX) == HIGH) 
  {
    digitalWrite(Aux_Led, LOW);   
  }
  delay(1000);
}     
