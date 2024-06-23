# Magic Compass
(/u /i)

## Overview

Magic Compass always comes in pairs. They use GPS to broadcast their location and LoRA transievers so even in the darkest of times, friends can be found. 

## Hardware 

Magic compass uses an Arduino Nano (tested with BLE sense rev2) as the primary chip
A GT-U7 GPS module
A RYLR896 LoRA transiever module

## Getting Started 

Prereqs, python, arduino-cli (https://arduino.github.io/arduino-cli/1.0/installation/)

- TODO flesh this out and figure out all prereqs when starting from scratch. FIigure out and pull in all arduino libraries (tiny gps?) that are needed into project

To start, download the arduino IDE (2.3.2). The projects should be able to be built and deployed through there. 

## Hardware setup 

Info on wiring is included in the docs folder

## Pin Configuration

Note! This project uses a custom pin configuration. The GPS module and the Lora Module both use UART for communication, so an additional UART rx/tx pair is bound. To update, run the prebuild python script



install the requirements 
`pip install -r requirements.txt`
Run prebuild
`python pre_build.py`

(At this stage all it really does is copies the `pins_arduino.h` and `defines.txt` into the variant build dir, so you can just manually do it if you want)

If you mess up your ARduino 15 directory, you can safely delete it, and redownload it using Arduino IDE



## Wiring Diagram

Diagram was made using https://www.circuit-diagram.org/editor/