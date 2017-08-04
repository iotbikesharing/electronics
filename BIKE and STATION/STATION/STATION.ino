#include <SoftwareSerial.h>
#define Tx_Led 9
#define Rx_Led 10
#define Button 11
#define AUX 6

SoftwareSerial mySerial(2, 3); // (2-RX) <<-- Connect TX pin of LoRa  ......  (3-TX) <<-- Connect RX pin of LoRa
// gnd SET_A and SET_B for Normal Mode (Send and Receive)

String num = "3333";
char data [100];

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode (Tx_Led, OUTPUT);
  pinMode (Rx_Led, OUTPUT);
  pinMode (Button,INPUT);
  pinMode (AUX, INPUT);
}

void loop() {
  
  if (digitalRead (Button) == HIGH) {
    digitalWrite (Tx_Led, HIGH);
    mySerial.println(num);    
    digitalWrite (Tx_Led, LOW);
  }

// Receive the data from the bikes and check which bike it is
// then send a confirmation signal
  if(mySerial.available() > 1)
  {
    String input = mySerial.readString();
    Serial.println(input);  
    input.toCharArray (data,100);
    //Serial.println (data [5]);
//
    switch (data [5]){
      case '1':
      Serial.println ("Sending confirmation");
      mySerial.println ("Bike 1: Received");
      break;
      case '2':
      mySerial.println ("Bike 2: Received");
      break;
    }
    
//   if (data [5] =='1') 
//   {
//      mySerial.println ("Bike 1: Received");
//      Serial.println ("Sending confirmation");
//    }
  }
    
  

  if (digitalRead (AUX) == LOW) 
  {
       digitalWrite (Rx_Led, HIGH);    
//       mySerial.println ("Bike 1: Received");
  }
  
  else if (digitalRead (AUX) == HIGH)
  {
       digitalWrite (Rx_Led, LOW);
  }
  
  delay(20);
}

