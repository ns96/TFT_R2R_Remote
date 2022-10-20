/*
 An simple project making use of the examples of the TTGO display and a
 an ULN2803 Eight-Way transistor array for a R2R remote for a Teac X-10R 

 The code is largely based on a digital clock example from thelink below
 This examples uses the hardware SPI only. 
 
 Based on clock sketch by Gilchrist 6/2/2014 1.0
 Adapted by Nathan Stevens 10/14/2022
 @version 1.0 10/19/2022 
 
A few colour codes:
 
code	color
0x0000	Black
0xFFFF	White
0xBDF7	Light Gray
0x7BEF	Dark Gray
0xF800	Red
0xFFE0	Yellow
0xFBE0	Orange
0x79E0	Brown
0x7E0	Green
0x7FF	Cyan
0x1F	Blue
0xF81F	Pink

 */
 
#include <TFT_eSPI.h> // Graphics and font library for ST7735 driver chip
#include <SPI.h>

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

BluetoothSerial SerialBT;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h

uint32_t targetTime = 0;       // for next 1 second timeout

boolean initial = true;
byte xcolon1 = 0;
byte xcolon2 = 0;
unsigned int colour = 0;

// keep track of time in hours minutes and seconds
uint8_t hh=0, mm=0, ss=0;
uint8_t ohh=0, omm=0, oss=0; // old time values

// holds the data received from serial or bluetooth
char command = 'x';

// set the pins used for control and indicate the command character
const int playPIN = 27;     // command = 1
const int ffPIN = 26;       // command = 3
const int revPIN = 25;      // command = 2
const int stopPIN = 33;     // command = 0
const int pausePIN = 32;    // command = 4
const int recPIN = 12;      // command = 5
const int recMutePIN = 13;  // command = 7
const int revPlayPIN = 15;  // command = 6

// the command characters
const char CONNECT = 'c';
const char STOP = '0';
const char PLAY = '1';
const char REV = '2';
const char FF = '3';
const char PAUSE = '4';
const char REC = '5';
const char REV_PLAY = '6';
const char REC_MUTE = '7';

// keep track if we are playing or stopped
boolean playing = false;
String mode = "STOP"; 

void setup(void) {
  // set the pins to output
  pinMode(playPIN, OUTPUT);
  pinMode(ffPIN, OUTPUT);
  pinMode(revPIN, OUTPUT);
  pinMode(stopPIN, OUTPUT);
  pinMode(pausePIN, OUTPUT);
  pinMode(recPIN, OUTPUT);
  pinMode(recMutePIN, OUTPUT);
  pinMode(revPlayPIN, OUTPUT);
  
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_YELLOW, TFT_BLACK); // Note: the new fonts do not draw the background colour

  targetTime = millis() + 1000;

  Serial.begin(115200);
  SerialBT.enableSSP();
  SerialBT.begin("ESP32_R2R"); //Bluetooth device name
  
  Serial.println("Device started, ready to pair with bluetooth!");
}

void loop() {
  // get data from serial or bluetooth ports
  if (Serial.available()) {
    char input = Serial.read();
    if(input != '\r' && input != '\n') {
      command = input;
      Serial.print("Command: ");
      Serial.println(input);
    }
  }
  
  if (SerialBT.available()) {
    char input = SerialBT.read();
    if(input != '\r' && input != '\n') {
      command = input;
      Serial.print("BT Command: ");
      Serial.println(input);
    }
  }

  // add small delay here?
  delay(20);

  // run any command commands are
  switch(command) {
     case PLAY:
        ss = oss;
        mm = omm;
        hh = ohh;
        setCommandPin(playPIN, "PLAY", true);
        break;
     case FF:
        oss = 0; omm = 0; ohh = 0;
        setCommandPin(ffPIN, "FF", false);
        break;
     case REV:
        oss = 0; omm = 0; ohh = 0;
        setCommandPin(revPIN, "REV", false);
        break;
     case STOP:
        oss = 0; omm = 0; ohh = 0;
        setCommandPin(stopPIN, "STOP", false);
        break;
     case PAUSE:
        setCommandPin(pausePIN, "PAUSE", false);
        break;
     case REC:
        oss = 0; omm = 0; ohh = 0;
        setCommandPin2(pausePIN, recPIN, "REC", false);
        break;
     case REC_MUTE:
        setCommandPin(recMutePIN, "REC MUTE", true);
        break;
     case REV_PLAY:
        ss = oss;
        mm = omm;
        hh = ohh;
        setCommandPin(revPlayPIN, "REV PLAY", true);
        break;
     default:
        break;
  }
  
  // keep track of time
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

    // if not playing reset the hour minuetes
    if(!playing) {
      ss = 0; mm = 0; hh = 0;  
    } else {
      oss = ss; omm = mm; ohh = hh;  
    }
    
    if (initial) {
      initial = false;    
      tft.setTextColor(TFT_BLUE, TFT_BLACK);
      tft.drawString("WAITING FOR CONNECTION ...",0,50,2); // Next size up font 2
    }
    
    // Update digital time
    byte xpos = 6;
    byte ypos = 0;
    
    //if (mss != mm) { // redraw every minute
    if (true) { // redraw every second
      // Uncomment ONE of the next 2 lines, using the ghost image demonstrates text overlay as time is drawn over it
      tft.setTextColor(0x39C4, TFT_BLACK);  // Leave a 7 segment ghost image, comment out next line!
      //tft.setTextColor(TFT_BLACK, TFT_BLACK); // Set font colour to black to wipe image
      // Font 7 is to show a pseudo 7 segment display.
      // Font 7 only contains characters [space] 0 1 2 3 4 5 6 7 8 9 0 : .
      tft.drawString("88:88:88",xpos,ypos,7); // Overwrite the text to clear it
      tft.setTextColor(0xFBE0, TFT_BLACK); // Orange
      
      if (hh<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos += tft.drawNumber(hh,xpos,ypos,7);
      xcolon1=xpos;
      
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (mm<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      xpos += tft.drawNumber(mm,xpos,ypos,7);
      xcolon2=xpos;
      
      xpos+= tft.drawChar(':',xpos,ypos,7);
      if (ss<10) xpos+= tft.drawChar('0',xpos,ypos,7);
      tft.drawNumber(ss,xpos,ypos,7);
    }  

    if (ss%2) { // Flash the colon
      tft.setTextColor(0x39C4, TFT_BLACK);
      xpos+= tft.drawChar(':',xcolon1,ypos,7);
      xpos+= tft.drawChar(':',xcolon2,ypos,7);
      tft.setTextColor(0xFBE0, TFT_BLACK);
    }
    else {
      // check to see if we connected
      if(command == CONNECT) {
        command = 'X';        
        tft.fillRect (0, 50, 240, 15, TFT_BLACK);
        tft.setTextColor(TFT_BLUE, TFT_BLACK);
        tft.drawString("CONNECTED ...",0,50,2); // Next size up font 2 
        
        tft.setTextColor(0xFBE0, TFT_BLACK); // reset text color 
      }
      
      tft.drawChar(':',xcolon1,ypos,7);
      tft.drawChar(':',xcolon2,ypos,7);
      
      colour = random(0xFFFF);
      // Erase the old text with a rectangle, the disadvantage of this method is increased display flicker
      tft.fillRect (0, 70, 240, 20, TFT_BLACK);
      tft.setTextColor(colour);
      tft.drawRightString("MODE: ",75, 70, 4); // Right justified string drawing to x position 75
      tft.drawString(mode, 100, 70, 4);
    }
  }
}

// function to set a particular pin high then low for running commands on the R2R
void setCommandPin(int pin, String currentMode, boolean play) {
  mode = currentMode;
  playing = play;
  Serial.print("Mode: " + mode + " / ");
  Serial.print("PLAY: ");
  Serial.println(playing);
  
  digitalWrite(pin, HIGH);
  delay(250);
  digitalWrite(pin, LOW);
  command = 'X';
}

void setCommandPin2(int pin1, int pin2, String currentMode, boolean play) {
  mode = currentMode;
  playing = play;
  Serial.print("Mode: " + mode + " / ");
  Serial.print("PLAY: ");
  Serial.println(playing);
  
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  delay(250);
  digitalWrite(pin1, LOW);
  digitalWrite(pin2, LOW);
  command = 'X';
}
