/*
* Read a card using a mfrc522 reader on your SPI interface
* Pin layout should be as follows (on Arduino Uno):
* MOSI: Pin 11 / ICSP-4
* MISO: Pin 12 / ICSP-1
* SCK: Pin 13 / ISCP-3
* SS/SDA: Pin 10
* RST: Pin 9
*/

#include <SPI.h>
#include <RFID.h>

#define SS_PIN 10
#define RST_PIN 9
#define LED 7
#define power 8

RFID rfid(SS_PIN,RST_PIN);




int serNum[5];
int cards[][5] = {
  {70,37,234,171,34}
  
};

bool access = false;

void setup(){

    Serial.begin(9600);
    SPI.begin();
    rfid.init();

    pinMode(LED, OUTPUT);
    pinMode(power,OUTPUT);
    digitalWrite(LED, LOW);
    digitalWrite(power, LOW);
}

void loop(){
    
    if(rfid.isCard()){
    
        if(rfid.readCardSerial()){
            Serial.print(rfid.serNum[0]);
            Serial.print(" ");
            Serial.print(rfid.serNum[1]);
            Serial.print(" ");
            Serial.print(rfid.serNum[2]);
            Serial.print(" ");
            Serial.print(rfid.serNum[3]);
            Serial.print(" ");
            Serial.print(rfid.serNum[4]);
            Serial.println("");
            
            for(int x = 0; x < sizeof(cards); x++){
              for(int i = 0; i < sizeof(rfid.serNum); i++ ){
                  if(rfid.serNum[i] != cards[x][i]) {
                      access = false;
                      break;
                  } else {
                      access = true;
                  }
              }
              if(access) break;
            }
           
        }
        
       if(access){
          Serial.println("Welcome!");
           digitalWrite(LED, HIGH); 
           delay(3000);
           digitalWrite(LED, LOW);
          
           
      } else {
           Serial.println("Not allowed!"); 
           digitalWrite(LED, HIGH);
           digitalWrite(power, HIGH);
           delay(500);
           digitalWrite(LED, LOW); 
           delay(500);
           digitalWrite(LED, HIGH);
           delay(500);
           digitalWrite(LED, LOW);         
           delay(1000);
           digitalWrite(power, LOW);
       }        
    }
    
    
    
    rfid.halt();

}


