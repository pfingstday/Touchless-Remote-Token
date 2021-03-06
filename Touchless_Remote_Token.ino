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
const int redPin = 5;
const int greenPin = 11;
const int bluePin = 10;

// Color Swipe & HID
int i1 = 255; 
int redVal1 = 255;
int greenVal1 = 0;
int blueVal1 = 255;

int i2 = 255; 
int redVal2 = 0;
int greenVal2 = 255;
int blueVal2 = 255;

//int redVal3 = 0;
//int greenVal3 = 120;
//int blueVal3 = 120;

// Color Switch
int redVal4 = 180;
int greenVal4 = 100;
int blueVal4 = 0;

// Color Master Switch
int redVal5 = 255;
int greenVal5 = 255;
int blueVal5 = 30;

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

// Delay Setup
boolean delayOn = true;
Metro delaySensor = Metro(1000);

boolean resetColor = true;
Metro delayColor = Metro(2000);

boolean MusicOff = false;
boolean SwitchOff = false;
boolean MuteOn = false;


//Tilt
//int tilt_s1 = 3;
//int tilt_s2 = 4;


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

  /* Tilt */
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

  for (byte i = 0; i < mfrc522.uid.size; i++) {
    // Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
    // Serial.println();
}


void loop(){
  
  //  int position = getTiltPosition();
  //  Serial.println(position);
  

  /* RFID RC522 */
  
  readrfid();

  if(mfrc522.uid.uidByte[1] == 0x6F){tag = 1;} // Swipe HID LILA
  if(mfrc522.uid.uidByte[1] == 0x19){tag = 2;} // Remote HID BLAU
  //if(mfrc522.uid.uidByte[1] == 0x45){tag = 3;} // -
  if(mfrc522.uid.uidByte[1] == 0x90){tag = 4;} // Switch GELB
  if(mfrc522.uid.uidByte[1] == 0x79){tag = 5;} // MasterSwitch
  
  
  /* Sharp IR */

  int distance = sensorLeft.read();
  int distance2 = sensorRight.read();

  SwipeDetector::Swipe s = detector.detect(distance, distance2);
  
  //if (distance != -1 && distance2 != -1) {
  //Serial.println(distance);
  //Serial.println(distance2);
  //}
   
   
 /* Sensor Delay */  
   
   if (delaySensor.check() == 1) {
     
   if (delayOn == true && distance != -1 && distance < 28 && distance2 < 28){
    delayOn = false;
     //Serial.println("delayOn = false");
    }else{
     delayOn = true;
     //Serial.println("delayOn = true");
    }
 }
 
 /* Color Reset */  
   
   if (delayColor.check() == 1) {
     
   if (resetColor == true && distance != -1 && distance < 28 && distance2 < 28){
    resetColor = false;
     Serial.println("resetColor = false");
    }else{
     resetColor = true;
     Serial.println("resetColor = true");
    }
 }


 /* Tag 1: HID Volume & Swipe  */ 
  

  if (tag == 1 && resetColor == true && MusicOff == false) {
    setColorTag1(redVal1,greenVal1,blueVal1);
  } 
   else if (tag == 1 && MusicOff == true) {
   setColor(16, 0, 16);
 }  

 // Skip Tracks

 if (tag == 1) {

    if (delayOn == true && MusicOff == false && MuteOn == false) {
      
     if (s == SwipeDetector::SWIPE_LEFT) {
        Serial.println("Swipe: Previous");
        Remote.rewind();
        Remote.clear();
        setColor(255, 0, 0);
        delay(300);
        setColorTag1(redVal1,greenVal1,blueVal1);
        delaySensor.reset();
      }

      if (s == SwipeDetector::SWIPE_RIGHT) {
        Serial.println("Swipe: Next");
        Remote.forward();
        Remote.clear();
        setColor(0, 255, 0);
        delay(300);
        setColorTag1(redVal1,greenVal1,blueVal1);
        delaySensor.reset();
      }
    }
       
    // Volume Control   
     
    if (delayOn == false && resetColor == false) {   
       
     if (distance2 != -1 && distance2 >= 9 && distance2 <= 30) {
        Serial.println("Volume Up");
        Remote.increase();
        Remote.clear();
        sensorRight.clear();
        delaySensor.reset();
        setColorTag1(redVal1,greenVal1,blueVal1);
    
        i1 += 16;
        redVal1  += 16;      
        greenVal1 = 0; 
        blueVal1  += 16;
    
        if (i1 >= 255){
          redVal1  = 255;
          greenVal1 = 0;
          blueVal1  = 255;
          i1 = 255;
        }

        delayColor.reset();
        delay(150);
        
        MusicOff = false;
      }

      if (distance2 != -1 && distance2 >= 4 && distance2 <= 7) {
        Serial.println("Volume Down");
        Remote.decrease();
        Remote.clear();
        sensorRight.clear();
        delaySensor.reset();         
        setColorTag1(redVal1,greenVal1,blueVal1);
    
        i1 -= 16;
        redVal1  -= 16;
        greenVal1 = 0; 
        blueVal1  -= 16;
    
        if (i1 <= 16){
          redVal1  = 16;
          greenVal1 = 0;
          blueVal1  = 16;
          i1 = 16;
        }

        delayColor.reset();
        delay(150);
        
        MusicOff = false;
      }  
    }

    // Mute & Play     
   
    if (distance2 != -1 && distance2 <= 3 && delayOn == false) {
      Serial.println("Mute");
      Remote.mute();
      Remote.clear();
      sensorRight.clear();
      setColor(255, 0, 0);
      MusicOff = true;
      MuteOn = true;
      delay(900);
      delaySensor.reset();
        
    }

    if (MuteOn == true && delayOn == true) {
      
      if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {
        Serial.println("Play");
        Remote.mute();
        Remote.clear();
        sensorRight.clear();
        setColorTag1(redVal1,greenVal1,blueVal1);
        MusicOff = false;
        MuteOn = false;
      }
    }
  }


  /* Tag 2: IRRemote & Swipe */
  
  if (tag == 2 && resetColor == true && MusicOff == false) {
    setColorTag1(redVal2,greenVal2,blueVal2);
  } 
   else if (tag == 2 && MusicOff == true) {
   setColor(0, 16, 16);
 }   

  if (tag == 2 && resetColor == false) {

   //Serial.println("IR Receiver");

   if (distance2 >= 9 && distance2 <= 30) {
    Serial.println("IR Lauter");
    irsend.sendNEC(0xA55A50AF, 32);

    setColorTag2(redVal2,greenVal2,blueVal2);

    i2 += 10;
    redVal2  = 0;      
    greenVal2 += 10; 
    blueVal2  += 10;

    if (i2 >= 255){
      redVal2  = 0;
      greenVal2 = 255;
      blueVal2  = 255;
      i2 = 255;
    }
    delay(100);
    delayColor.reset();
   }


   if (distance2 >= 4 && distance2 <= 7) {
    Serial.println("IR Leiser");
    irsend.sendNEC(0xA55AD02F, 32);
    
    setColorTag2(redVal2,greenVal2,blueVal2); 

    i2 -= 10;
    redVal2  = 0;
    greenVal2 -= 10; 
    blueVal2  -= 10;

    if (i2 <= 10){
      redVal2  = 0;
      greenVal2 = 10;
      blueVal2  = 10;
      i2 = 10;
    }
    delay(100);
    delayColor.reset();
   }


   if (distance2 <= 3 && distance2 != -1) {
     Serial.println("IR Mute");
     irsend.sendNEC(0xA55A48B7, 32);
     sensorRight.clear();
     delay(1000);
   }
  } 
  

  /* Tag 4: RCSwitch */

  if (SwitchOff == false && tag == 4) {
    setColorTag4(redVal4,greenVal4,blueVal4);
  }
    
  else if (SwitchOff == true && tag == 4) {
    setColor(0, 0, 0); // Lights off!
  } 

  if (tag == 4) {
    
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
      //mySwitch.switchOn("11111", "10000");
      //delay(10);
      //mySwitch.switchOn("11111", "01000");
      //delay(10);
      //mySwitch.switchOn("11111", "00100");

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
      //mySwitch.switchOff("11111", "10000");
      //delay(10);
      //mySwitch.switchOff("11111", "01000");
      //delay(10);
      //mySwitch.switchOff("11111", "00100");
      //delay(10);

      SwitchOff = true;
    }
  }


  /* Tag 5: MasterSwitch */

  if (SwitchOff == false && tag == 5) {
    setColorTag5(redVal5,greenVal5,blueVal5);
  }
  
  else if (SwitchOff == true && tag == 5) {
    setColor(0, 0, 0); // Lights off!
  } 

  if (tag == 5) {
    
    //Serial.println("Master Switch");

    if (s == SwipeDetector::SWIPE_LEFT || s == SwipeDetector::SWIPE_RIGHT) {

      Serial.println("Master Switch On");  
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

      Serial.println("Master Switch Off");  
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

void setColorTag1(int redVal1, int greenVal1, int blueVal1) {
  analogWrite(redPin, redVal1);
  analogWrite(greenPin, greenVal1);
  analogWrite(bluePin, blueVal1); 
}

void setColorTag2(int redVal2, int greenVal2, int blueVal2) {
  analogWrite(redPin, redVal2);
  analogWrite(greenPin, greenVal2);
  analogWrite(bluePin, blueVal2); 
}

void setColorTag3(int redVal3, int greenVal3, int blueVal3) {
  analogWrite(redPin, redVal3);
  analogWrite(greenPin, greenVal3);
  analogWrite(bluePin, blueVal3); 
}

void setColorTag4(int redVal4, int greenVal4, int blueVal4) {
  analogWrite(redPin, redVal4);
  analogWrite(greenPin, greenVal4);
  analogWrite(bluePin, blueVal4); 
}

void setColorTag5(int redVal5, int greenVal5, int blueVal5) {
  analogWrite(redPin, redVal5);
  analogWrite(greenPin, greenVal5);
  analogWrite(bluePin, blueVal5); 
}

//int getTiltPosition(){
//   int s1 = digitalRead(tilt_s1);
//   int s2 = digitalRead(tilt_s2);
//   return (s1 << 1) | s2; //bitwise math to combine the values
//}
