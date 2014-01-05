
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

// RFID
const int RST_PIN = 8;  
const int SS_PIN = 7;

// RCSwitch
const int rcPin = 2; 

boolean MusicOff = false;
boolean SwitchOff = false;


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

// tag: 1 Swipe
// tag: 2 Switch
// tag: 3 HID
// tag: 4 IRemote


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

  /* RFID RC522 */
  
  readrfid();

  if(mfrc522.uid.uidByte[1] == 0x6F){tag = 1;}
  if(mfrc522.uid.uidByte[1] == 0x90){tag = 2;}
  if(mfrc522.uid.uidByte[1] == 0x19){tag = 3;}
  if(mfrc522.uid.uidByte[1] == 0x45){tag = 4;}


  /* Sharp IR */

  int distance = sensorLeft.read();
  int distance2 = sensorRight.read();

  SwipeDetector::Swipe s = detector.detect(distance, distance2);
  
  //if (distance != -1 && distance2 != -1) {
  //Serial.println(distance);
  //Serial.println(distance2);
  // }

  /* Swipe Detection */

  if (tag == 1) {
    
    //Serial.println("Swipe Detection");

   setColor(255, 100, 100);

    if (s == SwipeDetector::SWIPE_LEFT) {
      Serial.println("Swipe: Previous");
      Remote.rewind();
      Remote.clear();
      setColor(255, 0, 0);
      delay(200);  
    }

    if (s == SwipeDetector::SWIPE_RIGHT) {
      Serial.println("Swipe: Next");
      Remote.forward();
      Remote.clear();
      setColor(0, 255, 0);
      delay(200);
    }
  }  


  /* RCSwitch */

if (SwitchOff == false && tag == 2) {
  setColor(230, 50, 0);
}
  
else if (SwitchOff == true && tag == 2) {
  setColor(0, 0, 0); // Lights off!
} 

if (tag == 2) {
    
  //Serial.println("Switch");

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


  /* USB HID */
  
  if (MusicOff == false && tag == 3) {
    setColor(100, 100, 100); 
  }

  else if (MusicOff == true && tag == 3) {
    setColor(0, 0, 0);
  }
    

  if (tag == 3 && distance != -1) { 

    // Serial.println("USB HID");

    if (distance >= 10 && distance <= 20) {
      Serial.println("Volume Up");
      Remote.increase();
      Remote.clear();
      setColor(255, 255, 255);
      delay(100);
      
      MusicOff = false;
    }

    if (distance >= 5 && distance <= 10) {
      Serial.println("Volume Down");
      Remote.decrease();
      Remote.clear();
      setColor(10, 10, 10);
      delay(100);
      
      MusicOff = false;
    }

    if (distance <= 4) {
      Serial.println("Mute");
      Remote.mute();
      Remote.clear();
      setColor(0, 0, 0);
      sensorLeft.clear();
      delay(1000);
      
      MusicOff = true;
    }
  }


  /* IRemote */
  
  if (tag == 4 && distance != -1) {
    setColor(150, 40, 20);
  }

  if (tag == 4 && distance != -1) {

    // Serial.println("IR Receiver");

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
}

void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue); 
}

