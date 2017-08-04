#include <SoftwareSerial.h>
#define Tx_Led 9
#define Aux_Led 10
#define Button 11
#define AUX 6

SoftwareSerial mySerial(2, 3); // (2-RX) <<-- Connect TX pin of LoRa  ......  (3-TX) <<-- Connect RX pin of LoRa
// gnd SET_A and SET_B for Normal Mode (Send and Receive)


char data[100] = {0,0,0,0,0,0,0};                   // Create an array to store incoming data
unsigned long wait_time = 3000;
unsigned long prev_wait_time = 0;

void setup() {
  Serial.begin(115200);
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
    mySerial.println("Bike 1 Lat/Long: 10.729546 / 106.69603299999994");    
    digitalWrite(Tx_Led, LOW);
    delay (2000);
  }

  //Receive the confirmation from the station and print it
  if (mySerial.available() > 1)
  {
    String input = mySerial.readString();       // Read input from buffer like normal     
    Serial.println (input);               
    input.toCharArray(data,100);                // Convert input type String to store into array data[]
    switch (data [7]) 
      {
        case '0':
          Serial.println ("Transmission failed ... Re-sending message");
          mySerial.println ("Bike 1 Lat/Long: 10.729546 / 106.69603299999994");
          break;
        case '1':
          Serial.println ("Data sent");
          data [7] = '0';
          break;
    }


//    if (data [7] =='1') {
//      Serial.println ("Data sent");
//    }
  
  //If the bike does not receive confirmation resend data
  
//    unsigned long currentTime = millis ();
//    if ((unsigned long)(currentTime - prev_wait_time) = wait_time ) 
//    {
//      switch (data [5]) 
//      {
//        case '0':
//          Serial.println ("Transmission failed");
//          mySerial.println ("Bike 1 Lat/Long: 10.729546 / 106.69603299999994");
//          break;
//        case '1':
//          Serial.println ("Data sent");
//          break;
//      }
//       prev_wait_time = milis ();
//    }
    
  //////////////////////////////////////////////////////////
  }
 //Serial.println (digitalRead (AUX));
  // LED for checking if the module is receiving or sending data
  if (digitalRead(AUX) == LOW) 
  {
    digitalWrite(Aux_Led, HIGH);
  }
  
  else if (digitalRead(AUX) == HIGH) 
  {
    digitalWrite(Aux_Led, LOW);   
  }
  delay(20);
}     
