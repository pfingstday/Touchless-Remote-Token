
// RFID Lib
#include <SPI.h>
#include <MFRC522.h>

// RCSwitch Lib
#include <RCSwitch.h>

// IRremote Lib
#include <IRremote.h>

// Swipe Detector
#include "SwipeDetector.h"
#include "ProximitySensor.h"

#define rcPin 2 // RCSwitch

#define RST_PIN 8   // RFID
#define SS_PIN 7   // RFID

// Default IR Led Pin 3 (Uno), Pin 13 (Micro)

SharpIRProximitySensor sensorLeft(0);
SharpIRProximitySensor sensorRight(1);
SwipeDetector detector;

//RFID Setup
MFRC522 mfrc522(SS_PIN, RST_PIN);

// IRemote Setup
IRsend irsend;

// RCSwitch Setup
RCSwitch mySwitch = RCSwitch();
boolean SwitchOff = false;

// RGB Led Setup
int redPin = 12;
int greenPin = 11;
int bluePin = 10;

boolean MusicOff = false;


void setup(){

  /* Sharp IR */
  Serial.begin(9600);
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


void loop(){
  

  /* Sharp IR */

  int distance = sensorLeft.read();
  int distance2 = sensorRight.read();

  SwipeDetector::Swipe s = detector.detect(distance, distance2);

  if (distance != -1 && distance2 != -1) {  // -1 = kein gültiges Signal
    Serial.println(distance);
    Serial.println(distance2);
  }


  /* Swipe Detection */

  if (mfrc522.uid.uidByte[1] == 0x6F) {

    
    //Serial.println("Swipe Detection");
   setColor(255, 100, 100);

    if (s == SwipeDetector::SWIPE_LEFT) {
      Serial.println("Swipe: Previous");
      Remote.rewind();
      Remote.clear();
      setColor(255, 0, 0);
      delay(500);  
    }

    if (s == SwipeDetector::SWIPE_RIGHT) {
      Serial.println("Swipe: Next");
      Remote.forward();
      Remote.clear();
      setColor(0, 255, 0);
      delay(500);
    }
  }  


  /* RFID RC522 */

  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  // Now a card is selected. The UID and SAK is in mfrc522.uid.


  for (byte i = 0; i < mfrc522.uid.size; i++) {
  //	Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  //	Serial.println();


  /* RCSwitch */

if (SwitchOff == false && mfrc522.uid.uidByte[1] == 0x90) {
  
  setColor(230, 50, 0);
}
  
  else if (SwitchOff == true && mfrc522.uid.uidByte[1] == 0x90) {
  
  setColor(0, 0, 0); // Lights off!
  
} 

if (mfrc522.uid.uidByte[1] == 0x90 && distance !=-1) {
    
    //Serial.println("Switch");

    if (distance <= 5) {  

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
      
      SwitchOff = true;
  
    }


    if (distance != 0 && distance > 10 && distance < 20) {

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
      
      SwitchOff = false;
    }
  }



  /* USB HID */
  
  if (MusicOff == false && mfrc522.uid.uidByte[1] == 0x19) {
    
    setColor(100, 100, 100); 
    
  } else if (MusicOff == true && mfrc522.uid.uidByte[1] == 0x19) {
    
    setColor(0, 0, 0);
  }
    

  if (mfrc522.uid.uidByte[1] == 0x19 && distance !=-1) { 

    // Serial.println("USB HID");

    if (distance > 10 && distance < 20) {
      Serial.println("Volume Up");
      Remote.increase();
      Remote.clear();
      setColor(255, 255, 255);
      delay(100);
      
      MusicOff = false;
    }

    if (distance > 5 && distance < 8) {
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
      delay(1000);
      
      MusicOff = true;
    }
  }


  /* IRemote */
  
  if (mfrc522.uid.uidByte[1] == 0x45) {
    
    setColor(150, 40, 20);
  }

  if (mfrc522.uid.uidByte[1] == 0x45 && distance != -1) {

    //    Serial.println("IR Receiver");

    if (distance > 15 && distance < 25) {
      Serial.println(" IR Lauter");
      irsend.sendNEC(0xA55A50AF, 32);
      delay(40);
    }

    if (distance <= 10) {
      Serial.println("IR Leiser");
      irsend.sendNEC(0xA55AD02F, 32);
      delay(40);
    }

    if (distance <= 5) {
      Serial.println("IR Mute");
      irsend.sendNEC(0xA55A48B7, 32);
      delay(1000);
    }
  }
}

void setColor(int red, int green, int blue)
{
analogWrite(redPin, red);
analogWrite(greenPin, green);
analogWrite(bluePin, blue); 
}

