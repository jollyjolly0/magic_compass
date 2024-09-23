#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

// the version for the 128x128 screen
#include <Adafruit_SH110X.h>

#include <stdio.h>
#include <math.h>

#include <TinyGPS.h>

#include "src/lora/lora.h"
#include "src/vector3/vector3.h"
#include "src/magnet/magnet.h"


#define GPS_SERIAL Serial1
#define SerialLoRa Serial2
// UART SerialLoRa(digitalPinToPinName(2), digitalPinToPinName(3), NC, NC);


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 128 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x128

#define PI 3.14159265358979323846


Adafruit_SH1107 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
LoRA lora = LoRA(SerialLoRa, Serial);
TinyGPS gps;
Magnet magnet = Magnet(Serial);


float flat = 420.0;
float flon = 69.0;

unsigned long age = 0;

float recv_lat, recv_lon, recv_time;

float heading_degrees; //0 is north, 90 is east
float heading_dist;

float mag_heading;

void update_display(void){
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SH110X_WHITE);        // Draw white text

  display.setCursor(0,0);           

  char buffer[50];

  // memset(buffer, 0, 50);
  // sprintf(buffer, "%.4f", flat);
  // display.print(buffer);
  display.print(flat);
  display.print(",");

  // memset(buffer, 0, 50);
  // sprintf(buffer, "%.4f", flon);
  // display.print(buffer);
  display.print(flon);
  display.println("");

  // sprintf(buffer, "%.4f", recv_lat);
  // display.print(buffer);
  display.print(recv_lat);
  display.print(",");

  // sprintf(buffer, "%.4f", recv_lon);
  // display.print(buffer);
  display.print(recv_lon);
  display.println("");

  // display.print((" "));
  display.print(heading_degrees);
  display.print((" / "));
  display.print(heading_dist);
  display.println("");

  int time_diff = (millis() - recv_time)/1000;
  display.print(("dt: "));
  display.print(time_diff);
  display.println("");

  display.print(("mag_heading: "));
  display.print(mag_heading);
  display.println("");

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

float calculate_distance(float lat1, float lon1, float lat2, float lon2) {
    const float R = 6371000; // Earth's radius in meters
    
    // Convert latitude and longitude to radians
    float lat1_rad = lat1 * PI / 180.0f;
    float lon1_rad = lon1 * PI / 180.0f;
    float lat2_rad = lat2 * PI / 180.0f;
    float lon2_rad = lon2 * PI / 180.0f;
    
    // Differences
    float dlat = lat2_rad - lat1_rad;
    float dlon = lon2_rad - lon1_rad;
    
    // Haversine formula
    float a = sinf(dlat/2) * sinf(dlat/2) +
              cosf(lat1_rad) * cosf(lat2_rad) * 
              sinf(dlon/2) * sinf(dlon/2);
    float c = 2 * atan2f(sqrtf(a), sqrtf(1-a));
    
    float distance = R * c;
    
    return distance;
}




void setup()
{  
  Serial.begin(9600);
  GPS_SERIAL.begin(9600);
  SerialLoRa.begin(115200);

  if(!display.begin(SCREEN_ADDRESS, true)) {
    Serial.println(F("SH1107 allocation failed"));
  }

  lora.init();

    Serial.println(F("inited"));

  magnet.init();
}



void loop()
{
  smartdelay(0);
  gps.f_get_position(&flat, &flon, &age);

  lora.set_send_lat_lon(flat, flon);
  lora.update();

  lora.get_rcv_lat_lon(recv_lat, recv_lon);
  lora.get_rcv_time(recv_time);


  heading_degrees = calculate_heading(flat, flon, recv_lat, recv_lon);
  heading_dist = calculate_distance(flat, flon, recv_lat, recv_lon);

  magnet.update();
  // Serial.println(magnet.heading);
  mag_heading = magnet.heading;

  update_display();
}
