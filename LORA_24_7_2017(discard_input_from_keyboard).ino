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

}

void loop() {
  if (digitalRead (Button) == HIGH) {
    digitalWrite (Tx_Led, HIGH);
    mySerial.println(num);    
    digitalWrite (Tx_Led, LOW);
    //delay (20);
  }
 
  if(mySerial.available() > 1){
    String input = mySerial.readString();
    Serial.println(input);    

  }
  if (digitalRead (AUX) == LOW) {
       digitalWrite (Rx_Led, HIGH);
     //  delay (100);
      }
  else if (digitalRead (AUX) == HIGH) {
       digitalWrite (Rx_Led, LOW);
  }
  
  delay(20);
}

