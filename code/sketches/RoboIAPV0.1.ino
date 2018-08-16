/*
 
 * liquid sensors connected to ports A0, A1, A2 and A3
 * For vegetronix sensors the black line is the data line (confusingly) and the silver line is ground. Don't forget!
 * sensors powered by 5v pin on arduino
 * button powered by 3.3v pin on arduino
 * 
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

// data format: A0,A1,A2,A3,buttonState

 created  12 September 2017
 v1.3
 by Clayton Barnes


 */

// Common source of error: be sure to have these libraries installed: (Sketch>Include Library>Manage Libraries...> find library you need
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "RTClib.h"
RTC_PCF8523 rtc; // if you have older (or who knows, maybe newer) SD shield, may need to change this 
//RTC_DS1307 rtc;
int currentStreak = 0; // used to prevent writing more than one 1 if the button is held down for longer than a second
int buttonState = 0; // state of the button

void setup() {
  
  // Open serial communications and wait for port to open:
  // Good for debugging purposes, but don't need for a true experiment
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

 if (! rtc.initialized()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  Serial.print("Initializing SD card...");

  // Test for presence of SD card
  if (!SD.begin()) {
    Serial.println("Card failed, or not present");
    // idle
    return;
  }
  Serial.println("card initialized.");
  if (SD.exists("datalog.txt")){
    SD.remove("datalog.txt");
  }
  
  pinMode(2, INPUT); // set as input for the pushbutton
}

void loop() {
    // time object, use to ensure record every second
    DateTime now = rtc.now();
    int startSecond = now.second(); // second we are collecting data for
    String dataString; // data that will be written to the SD card
    int buttonStateWrite; // state the button is in. 
    if(buttonState == 1){ // if detected button press last second
      if(currentStreak != 1){ // if didnt write the button state as 1 last time
        buttonStateWrite = 1; // write button state as 1 this time
        currentStreak = buttonState; // remembers we wrote the button state as one lasts time
      }
    }
    else{ // button wasnt pressed, used to reset the button state write as much as write it as a 0
      buttonStateWrite = 0;
      currentStreak = buttonState;
    }
    // where the analog data is recorded. Converts the analogRead into voltage and then into a string. Also written with a timestamp
    dataString = String(analogRead(A0)*3.0/1024.0)+","+ String(analogRead(A1)*3.0/1024.0)+","+ String(analogRead(A2)*3.0/1024.0)+","+ String(analogRead(A3)*3.0/1024)+","+String(buttonStateWrite);
    Serial.println(dataString);
    delay(1); // for stability
    File dataFile = SD.open("datalog.txt", FILE_WRITE); // open file to write to
    
    // if file is available, write to it
    if (dataFile) {
     dataFile.println(dataString);
     dataFile.close();
    }
    else {
      Serial.println("error opening datalog.txt");
    }

    // following code continuously checks for button presses 
    bool pressed = false;
     while (rtc.now().second()==startSecond){
      if(digitalRead(2)){
        pressed = true;
      }
      delay(1);
     }
     if (pressed){
      Serial.println("button pressed, setting button state + 1");
      buttonState = 1;
     }
     else{
      buttonState = 0;
     }
     
}
// Enjoy!
