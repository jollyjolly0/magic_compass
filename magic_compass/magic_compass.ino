#define SerialLoRa Serial2

#include "lora.h"
#include "vector3.h"


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64,

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


LoRA lora = LoRA(SerialLoRa, Serial);



void update_display(void){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.println(F("My Pos: 1 2 3"));
  display.println(F("Last recv pos / time"));
  display.println(F("lora state: awd"));
  display.println(F("metrics ..."));

  display.display();
  Serial.println("diisplayed");

  delay(2000);
}



void setup()
{  
  Serial.begin(9600);
  SerialLoRa.begin(115200);
  delay(2000);
  Serial.println("awd");

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
  }

  lora.init();

}



void loop()
{
  lora.update();

  update_display();
}
