# TFT_R2R_Remote

A Bluetooth Remote Based on the Lilygo TTGO ESP32 Display Board For A Teac X10R Reel to Reel

## Introduction

This project looks at using the ESP32 based [LilyGo TTGO](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) display board and the [ULN2803 ](https://www.amazon.com/dp/B08CX79JSQ)Eight-Way NPN Darlington Transistor Array for creating a Bluetooth remote control for a Teac X10R Reel to Reel Deck. Overall the TTGO board is an excellent choice for creating a Bluetooth remote because it supports both BLE and SPP (classic) UART modes allowing for easy integration with both desktops and modern mobile devices. Moreover, the small display is great for debugging and provides a nice UI to see what the remote is doing. Since the primary goal is to control the X10R from a computer using a small Java program, the board is used in [SPP UART](https://randomnerdtutorials.com/esp32-bluetooth-classic-arduino-ide/) mode.  P.S. If you are interested in controlling a R2R deck via a smartphone without coding then take a look at this [Project](https://github.com/mpfisher69/R2R-Bluetooth-Remote) and [Devices](https://www.ebay.com/sch/dear.d12/m.html?_dkr=1&iconV2Request=true&_ssn=dear.d12&_oac=1) from eBay.  

![](img/TTGO_3.jpg)

![](img/X10R_2.jpg)

## Arduino IDE Setup

Running the above sketch just requires adding the[ ESP32 board](https://espressif-docs.readthedocs-hosted.com/projects/arduino-esp32/en/latest/installing.html) and the [TFT_eSPI](https://github.com/Xinyuan-LilyGO/TTGO-T-Display) library to the Arduino IDE. Once the boards are loaded select the "TTGO Lora32-OLED V1" board.

## Wiring Things Up

Creating the connection cable is probable the hardest part of this project since the Teac X10R and similar R2R decks make use of the WAKA 12 pin port for remote connectivity. Unfortunately, those pins are all most impossible to find and I had to settle for making one using pins from an [automotive connector](https://www.amazon.com/dp/B07HHYQ1W9). This mostly works, but they are not the best pins to use (sometimes the fail to make contact properly) and will try making another cable using [these pins](https://www.amazon.com/dp/B09F2KY485) instead. A detail description on making a custom cable for the WAKA connector can also be found [here](https://sharedinventions.com/?p=399) as well.

![](img/TTGO_1.jpg)

![](img/TTGO_2.jpg)

![](img/X10R_4.jpg)

![](img/X10R_5.jpg)
