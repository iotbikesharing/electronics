//Woon Jun Shen
//UM402 (433 MHz UART)

#include <SoftwareSerial.h>
#define Tx_Led 9
#define Rx_Led 10
#define Button 11
#define AUX 5

SoftwareSerial mySerial(2, 3); //TX, RX
int num = 20;
// gnd SET_A and SET_B for Normal Mode (Send and Receive)

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode (Tx_Led, OUTPUT);
  pinMode (Rx_Led, OUTPUT);
  pinMode (Button,INPUT);
  pinMode (AUX, INPUT);
  //digitalWrite(Tx_Led, LOW);
}

void loop() {

  
 // if(Serial.available() > 0)
  if (digitalRead (Button) == HIGH) {
    digitalWrite (Tx_Led, HIGH);
    
  //Read from serial monitor and send over UM402
  //  String input = Serial.readString();
    mySerial.println(num);    
    digitalWrite (Tx_Led, LOW);
    //delay (20);
  }
  
 
  if(mySerial.available() > 1){//Read from UM402 and send to serial monitor
    String input = mySerial.readString();
    Serial.println(input);    
//     if (digitalRead (AUX) == LOW) {
//       digitalWrite (Rx_Led, HIGH);
//       //delay (100);
//      }
//       digitalWrite (Rx_Led, LOW);
  }
  if (digitalRead (AUX) == LOW) {
       digitalWrite (Rx_Led, HIGH);
     //  delay (100);
      }
  else if (digitalRead (AUX) == HIGH) {
       digitalWrite (Rx_Led, LOW);
  }
  // Serial.println (digitalRead (AUX));
  
  delay(20);
}

