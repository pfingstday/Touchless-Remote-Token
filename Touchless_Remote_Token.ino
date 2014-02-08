//Include Metro Lib
#include <Metro.h> 

// RFID Lib
#include <SPI.h>
#include <MFRC522.h>

// RCSwitch Lib
#include <RCSwitch.h>

// IRremote Lib
#include <IRremote.h>

// Swipe Detector Lib
#include "SwipeDetector.h"
#include "ProximitySensor.h"


// Default IR Led 
// Pin 3 (Uno), Pin 13 (Micro)

// RGB Led Setup
const int redPin = 12;
const int greenPin = 11;
const int bluePin = 10;

int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// RFID
const int RST_PIN = 8;  
const int SS_PIN = 7;

// RCSwitch
const int rcPin = 2; 

//RFID Setup
MFRC522 mfrc522(SS_PIN, RST_PIN);

// IRemote Setup
IRsend irsend;

// RCSwitch Setup
RCSwitch mySwitch = RCSwitch();

// Sharp IR Setup
SharpIRProximitySensor sensorLeft(1);
SharpIRProximitySensor sensorRight(0);
SwipeDetector detector;

// RFID Tags Setup
byte tag;

//Tilt
//int tilt_s1 = 3;
//int tilt_s2 = 4;

boolean MusicOff = false;
boolean SwitchOff = false;
boolean MuteOn = false;

boolean delayOn = true;
Metro delaySensor = Metro(1000);


void setup(){

  Serial.begin(9600);
  
  /* Sharp IR */  
  SPI.begin(); 

  /* Init MFRC522 card */
  mfrc522.PCD_Init();

  /* RCSwitch */
  mySwitch.enableTransmit(rcPin); 
  
  /* RGB Led */
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
// pinMode(tilt_s1, INPUT);
// pinMode(tilt_s2, INPUT);
}


void readrfid(){

  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
  
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.

  for (byte i = 0; i < mfrc522.uid.size; i++) {
  //  Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  //  Serial.println();
}


void loop(){
  
//  int position = getTiltPosition();
//  Serial.println(position);
  

  /* RFID RC522 */
  
  readrfid();

  if(mfrc522.uid.uidByte[1] == 0x6F){tag = 1;} // Swipe
  if(mfrc522.uid.uidByte[1] == 0x19){tag = 2;} // HID
  //if(mfrc522.uid.uidByte[1] == 0x45){tag = 3;} // IRemote
  if(mfrc522.uid.uidByte[0] == 0xDA){tag = 3;} // IRemote
  if(mfrc522.uid.uidByte[1] == 0x90){tag = 4;} // RCSwitch
  if(mfrc522.uid.uidByte[1] == 0x79){tag = 5;} // MasterSwitch
  
  
  /* Sharp IR */

  int distance = sensorLeft.read();
  int distance2 = sensorRight.read();

  SwipeDetector::Swipe s = detector.detect(distance, distance2);
  
//  if (distance != -1 && distance2 != -1) {
//  Serial.println(distance);
//  Serial.println(distance2);
//   }
   
   
 /* Sensor Delay */  
   
   if (delaySensor.check() == 1) {
     
   if (delayOn == true && distance != -1 && distance < 28 && distance2 < 28){
    delayOn = false;
     Serial.println("delayOn = false");
    }else{
     delayOn = true;
     Serial.println("delayOn = true");
    }
 }

//if (distance2 >= 4 && distance2 <= 25){
//   analogWrite(bluePin, brightness); 
//  analogWrite(greenPin, brightness);  
//
//  // change the brightness for next time through the loop:
//  brightness = brightness + fadeAmount;
//
//  // reverse the direction of the fading at the ends of the fade: 
//  if (brightness == 0 || brightness == 255) {
//    fadeAmount = -fadeAmount ; 
//  }     
//  // wait for 30 milliseconds to see the dimming effect    
//  delay(30);        
//}else{
//   setColor(0, 0, 0);
//}
  

/* Tag 1: Swipe & HID Volume */ 
  
    if (MusicOff == false && tag == 1) {
     setColor(255, 100, 100);    
  }

  else if (MusicOff == true && tag == 1) {
    setColor(0, 0, 0);
  }


 /* Swipe Detection */

  if (tag == 1 && delayOn == true && MusicOff == false && MuteOn == false) {
    
   if (s == SwipeDetector::SWIPE_LEFT) {
      Serial.println("Swipe: Previous");
      Remote.rewind();
      Remote.clear();
      setColor(255, 0, 0);
      delay(300);
    }

    if (s == SwipeDetector::SWIPE_RIGHT) {
      Serial.println("Swipe: Next");
      Remote.forward();
      Remote.clear();
      setColor(0, 255, 0);
      delay(300);
    }
  }
    
 
/* HID Volume Control */  
   
  if(delayOn == false){   
     
   if (distance2 != -1 && distance2 >= 12 && distance2 <= 25) {
      Serial.println("Volume Up");
      Remote.increase();
      Remote.clear();
      sensorRight.clear();
      delaySensor.reset();
      //setColor(255, 100, 100);
      delay(200);
      
      MusicOff = false;
    }

    if (distance2 != -1 && distance2 >= 5 && distance2 <= 8) {
      Serial.println("Volume Down");
      Remote.decrease();
      Remote.clear();
      sensorRight.clear();
      delaySensor.reset();
      //setColor(255, 100, 100);
      delay(200);
      
      MusicOff = false;
    }
   
   }
   
   
if (tag == 1 && MuteOn == true && delayOn == true) {
    
    if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {
      Serial.println("Play");
      Remote.mute();
      Remote.clear();
      sensorRight.clear();
      //delaySensor.reset();
      //delayOn = false;
      //MusicOff = false;
      MuteOn = false;
    }
  }
  
  if (distance2 != -1 && distance2 <= 4) {
      Serial.println("Mute");
      Remote.mute();
      Remote.clear();
      sensorRight.clear();
      //setColor(0, 0, 0);
      //MusicOff = true;
      MuteOn = true;
      delay(900);
      delaySensor.reset();
      
      
      
    }


//  /* USB HID */
//  
//  if (MusicOff == false && tag == 2) {
//    setColor(100, 100, 100); 
//  }
//
//  else if (MusicOff == true && tag == 2) {
//    setColor(0, 0, 0);
//  }
//    
//
//  if (tag == 2 && distance != -1) { 
//    
//    Serial.println("USB HID");
//
//    if (distance >= 10 && distance <= 20) {
//      Serial.println("Volume Up");
//      Remote.increase();
//      Remote.clear();
//      setColor(255, 255, 255);
//      delay(100);
//      
//      MusicOff = false;
//    }
//
//    if (distance >= 5 && distance <= 8) {
//      Serial.println("Volume Down");
//      Remote.decrease();
//      Remote.clear();
//      setColor(10, 10, 10);
//      delay(100);
//      
//      MusicOff = false;
//    }
//
//    if (distance <= 4) {
//      Serial.println("Mute");
//      Remote.mute();
//      Remote.clear();
//      setColor(0, 0, 0);
//      sensorLeft.clear();
//      delay(1000);
//      
//      MusicOff = true;
//    }
//  }


  /* IRemote */
  
  if (tag == 3) {
    setColor(150, 40, 20);
  }

  if (tag == 3 && distance != -1) {

    //Serial.println("IR Receiver");

    if (distance >= 10 && distance <= 20) {
      Serial.println("IR Lauter");
      irsend.sendNEC(0xA55A50AF, 32);
      delay(40);
    }

    if (distance >= 5 && distance <= 8) {
      Serial.println("IR Leiser");
      irsend.sendNEC(0xA55AD02F, 32);
      delay(40);
    }

    if (distance <= 4) {
      Serial.println("IR Mute");
      irsend.sendNEC(0xA55A48B7, 32);
      sensorLeft.clear();
      delay(1000);
    }
  }


  /* RCSwitch */

  if (SwitchOff == false && tag == 4) {
    setColor(230, 50, 0);
  }
    
  else if (SwitchOff == true && tag == 4) {
    setColor(0, 0, 0); // Lights off!
  } 

  if (tag == 4) {
    
    Serial.println("Switch");

    if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {

      Serial.println("Switch On");  
      setColor(0, 255, 0);
      
        mySwitch.switchOn("00000", "10000");
        delay(10);
        mySwitch.switchOn("00000", "01000");
        delay(10);
        mySwitch.switchOn("00000", "00100");
        delay(10);      

//      mySwitch.switchOn("11111", "10000");
//      delay(10);
//      mySwitch.switchOn("11111", "01000");
//      delay(10);
//      mySwitch.switchOn("11111", "00100");
//      delay(10);
      
      SwitchOff = false;
    }
 
    if (distance >0 && distance <= 4) {  

      Serial.println("Switch Off");  
      setColor(255, 0, 0);
      
        mySwitch.switchOff("00000", "10000");
        delay(10);
        mySwitch.switchOff("00000", "01000");
        delay(10);
        mySwitch.switchOff("00000", "00100");
        delay(10);
//      mySwitch.switchOff("11111", "10000");
//      delay(10);
//      mySwitch.switchOff("11111", "01000");
//      delay(10);
//      mySwitch.switchOff("11111", "00100");
//      delay(10);
      
      SwitchOff = true;
    }
  }


  /* MasterSwitch */

  if (SwitchOff == false && tag == 5) {
    setColor(0, 0, 255);
  }
  
  else if (SwitchOff == true && tag == 5) {
    setColor(0, 0, 0); // Lights off!
  } 

  if (tag == 5) {
    
    Serial.println("Master Switch");

    if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {

      Serial.println("Switch On");  
      setColor(0, 255, 0);

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
      
      SwitchOff = false;
    }
 
    if (distance >0 && distance <= 4) {  

      Serial.println("Switch Off");  
      setColor(255, 0, 0);

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
      
      SwitchOff = true;
    }
  }
}
  

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue); 
}

//int getTiltPosition(){
//   int s1 = digitalRead(tilt_s1);
//   int s2 = digitalRead(tilt_s2);
//   return (s1 << 1) | s2; //bitwise math to combine the values
//}
