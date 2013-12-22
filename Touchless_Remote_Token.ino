
// RFID Lib
#include <SPI.h>
#include <RFID.h>

// RCSwitch Lib
#include <RCSwitch.h>

// IRremote Lib
#include <IRremote.h>


#define irPin1 0 // analog Sharp IR 1
#define irPin2 1 // analog Sharp IR 1

#define rcPin 2 // RCSwitch

#define RST_PIN 9   // RFID
#define SS_PIN 10   // RFID

// Default IR Led Pin 3 (Uno), Pin 13 (Micro)


// RFID Setup
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;

RFID rfid(SS_PIN, RST_PIN); 

// IRemote Setup
IRsend irsend;

// RCSwitch Lib
RCSwitch mySwitch = RCSwitch();    


void setup(){
  
 /* Sharp IR */
  Serial.begin(9600);
  SPI.begin(); 
  rfid.init();

 /* RCSwitch */
  mySwitch.enableTransmit(rcPin); 
}


void loop(){
  
 /* Sharp IR */
  
  float volts = analogRead(irPin1)*0.0048828125;   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
  int distance1 = 12*pow(volts, -1);               // worked out from graph
  distance1 = constrain (distance1, 0, 30);
  Serial.println(distance1);                       
  delay(100);   


 /* RFID RC522 */

  if (rfid.isCard()) {
    if (rfid.readCardSerial()) {
      if (rfid.serNum[0] != serNum0
        && rfid.serNum[1] != serNum1
        && rfid.serNum[2] != serNum2
        && rfid.serNum[3] != serNum3
        && rfid.serNum[4] != serNum4
        ) {

        serNum0 = rfid.serNum[0];
        serNum1 = rfid.serNum[1];
        serNum2 = rfid.serNum[2];
        serNum3 = rfid.serNum[3];
        serNum4 = rfid.serNum[4];

        Serial.print(rfid.serNum[0],HEX);
        Serial.print(rfid.serNum[1],HEX);
        Serial.print(rfid.serNum[2],HEX);
        Serial.print(rfid.serNum[3],HEX);
        Serial.print(rfid.serNum[4],HEX);
        
        } else {
        // Same ID = Do nothing
        Serial.print(".");
      }
    } 


/* RCSwitch */

if (rfid.serNum[1] == 0x90) {
  
  Serial.println("Switch");

        
  if (distance1 <= 5) {  

    Serial.println("Switch Off");  

    mySwitch.switchOff("00000", "10000");
    delay(10);

    mySwitch.switchOff("00000", "01000");
    delay(10);

    mySwitch.switchOff("00000", "00100");
    delay(10);

    mySwitch.switchOff("11111", "10000");
    delay(10);

    mySwitch.switchOff("11111", "01000");
    delay(10);

    mySwitch.switchOff("11111", "00100");
    delay(10);
  }


  if (distance1 > 10 && distance1 < 20) {

    Serial.println("Switch On");  

    mySwitch.switchOn("00000", "10000");
    delay(10);

    mySwitch.switchOn("00000", "01000");
    delay(10);

    mySwitch.switchOn("00000", "00100");
    delay(10);

    mySwitch.switchOn("11111", "10000");
    delay(10);

    mySwitch.switchOn("11111", "01000");
    delay(10);

    mySwitch.switchOn("11111", "00100");
    delay(10);
  }
}


 /* Keyboard Maestro */

  if (rfid.serNum[1] == 0xF9) {
    
    Serial.println("Keyboard");


    if (distance1 > 15 && distance1 < 25){
      Serial.println("Mac Lauter");
      Keyboard.press(KEY_F12);
      Keyboard.release(KEY_F12);
      delay(10);
    }
    
    if (distance1 <= 10) {
      Serial.println("Mac Leiser");
      Keyboard.press(KEY_F11);
      Keyboard.release(KEY_F11);
      delay(10);
    }
    
    if (distance1 <= 5) {
      Serial.println("Mute");
      Keyboard.press(KEY_F10);
      Keyboard.release(KEY_F10);
      delay(1000);
    }  
  }


/* IRemote */

  if (rfid.serNum[1] == 0x45) {
    
    Serial.println("IR Receiver");

   
    if(distance1 > 15 && distance1 < 25) {
    Serial.println(" IR Lauter");
    irsend.sendNEC(0xA55A50AF, 32);
    delay(40);
    }

    if (distance1 <= 10) {
      Serial.println("IR Leiser");
      irsend.sendNEC(0xA55AD02F, 32);
      delay(40);
    }

    if (distance1 <= 5) {
      Serial.println("IR Mute");
      irsend.sendNEC(0xA55A48B7, 32);
      delay(1000);
    }
  }



  }

  rfid.halt(); 

}



