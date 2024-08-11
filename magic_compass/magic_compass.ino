#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <stdio.h>
#include <math.h>

#include <TinyGPS.h>

#include "lora.h"
#include "vector3.h"

#define GPS_SERIAL Serial1
#define SerialLoRa Serial2

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64,

#define PI 3.14159265358979323846


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
LoRA lora = LoRA(SerialLoRa, Serial);
TinyGPS gps;


float flat, flon;
unsigned long age = 0;


float recv_lat, recv_lon;

float heading_degrees; //0 is north, 90 is east

void update_display(void){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner

  display.print(("x: "));
  display.print(flat);
  display.print(", ");
  display.print(flon);
  display.println("");

  display.print(("y: "));
  display.print(recv_lat);
  display.print(", ");
  display.print(recv_lon);
  display.println("");

  display.println(("metrics ..."));

  display.display();
}




// gps
static void smartdelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (GPS_SERIAL.available())
      gps.encode(GPS_SERIAL.read());
  } while (millis() - start < ms);
}


float calculate_heading(float lat1, float lon1, float lat2, float lon2) {
    // Convert latitude and longitude to radians
    lat1 = lat1 * PI / 180.0f;
    lon1 = lon1 * PI / 180.0f;
    lat2 = lat2 * PI / 180.0f;
    lon2 = lon2 * PI / 180.0f;

    float dlon = lon2 - lon1;

    float x = sinf(dlon) * cosf(lat2);
    float y = cosf(lat1) * sinf(lat2) - sinf(lat1) * cosf(lat2) * cosf(dlon);

    float heading = atan2f(x, y);

    // Convert heading to degrees
    float heading_degrees = heading * 180.0f / PI;

    // Normalize to 0-360
    heading_degrees = fmodf(heading_degrees + 360.0f, 360.0f);

    return heading_degrees;
}




void setup()
{  
  Serial.begin(9600);
  GPS_SERIAL.begin(9600);
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


  smartdelay(0);
  gps.f_get_position(&flat, &flon, &age);
	// Serial.print("gps lat/lon");
	// Serial.print(flat);
	// Serial.print(", ");
	// Serial.print(flon);
	// Serial.println("");

  lora.set_send_lat_lon(flat, flon);
  lora.update();

  lora.get_rcv_lat_lon(recv_lat, recv_lon);

  	// Serial.print("recv lat/lon");
	// Serial.print(recv_lat);
	// Serial.print(", ");
	// Serial.print(recv_lon);
	// Serial.println("");

  heading_degrees = calculate_heading(flat, flon, recv_lat, recv_lon);
	// Serial.print("heading_degrees");
	// Serial.println(heading_degrees);



  update_display();
}
