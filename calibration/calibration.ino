/*
  Arduino BMM150 - Simple Magnetometer

  This example reads the magnetic field values from the BMM150
  sensor and continuously prints them to the Serial Monitor
  or Serial Plotter.

  The circuit:
  - Arduino Nano 33 BLE Sense Rev2

  created 10 Jul 2019
  by Riccardo Rizzo

  This example code is in the public domain.
*/

#include "Arduino_BMI270_BMM150.h"
#include <cmath>

struct vec3  {
  float x;
  float y;
  float z;
};

void printvec(vec3 vec)
{
  Serial.print('(');
  Serial.print(vec.x);
  Serial.print('\t');
  Serial.print(vec.y);
  Serial.print('\t');
  Serial.print(vec.z);
  Serial.print(")\t");
}

vec3 vecAdd(vec3 a, vec3 b){
  return { a.x+b.x,  a.y+b.y, a.z+b.z };
}

vec3 vecSub(vec3 a, vec3 b){
  return { a.x-b.x,  a.y-b.y, a.z-b.z };
}

vec3 vecMult(vec3 a, float c){
  return { c*a.x,  c*+a.y, c*a.z };
}


float vecDot(vec3 a, vec3 b){
  return { a.x*b.x +  a.y*b.y + a.z*b.z };
}

vec3 vecCross(vec3 a, vec3 b){
  vec3 ret;
  ret.x = (a.y * b.z - a.z * b.y);
  ret.y = (a.z * b.x - a.x * b.z);
  ret.z = (a.x * b.y - a.y * b.x);
  return ret;
}


float vecLen(vec3 a){
  return sqrt( vecDot(a,a)  );
}

vec3 vecNormalize(vec3 a){
  float l = vecLen(a);
  if(l < 0.001f) Serial.print("DENORM VALUE");
  return vecMult(a, 1/l) ;
}

void serialVec3(vec3 a){
  Serial.print(a.x);
  Serial.print(",");
  Serial.print(a.y);
  Serial.print(",");
  Serial.print(a.z);
  Serial.print("\n");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Magnetic field sample rate = ");
  Serial.print(IMU.magneticFieldSampleRate());
  Serial.println(" Hz");
  Serial.println();
  Serial.print("Accel sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
  Serial.println("Magnetic Field in uT");
  Serial.println("X\tY\tZ");  
}

float min_x = 1000;
float min_y = 1000;
float min_z = 1000;

float max_x = -1000;
float max_y = -1000;
float max_z = -1000;

vec3 grav;//raw accelerometer reading
vec3 mag;//raw magnetometer reading

int bufferindex;
vec3 calibratedbuffer[5]; //buffer of calibrated values, pre-normalized
vec3 getaverage()
{
  vec3 retval = { 0.0f, 0.0f, 0.0f };
  retval = vecAdd(calibratedbuffer[0], calibratedbuffer[1]);
  retval = vecAdd(retval, calibratedbuffer[2]);
  retval = vecAdd(retval, calibratedbuffer[3]);
  retval = vecAdd(retval, calibratedbuffer[4]);
  return retval;
}

void loop() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(grav.y, grav.x, grav.z);// NOTE Y, X, Z order
    grav.z *= -1.0f;
  }
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mag.x, mag.y, mag.z);
    mag.y *= -1.0f;
    mag.z *= -1.0f;
  }
  else return;



  vec3 gravnorm;
  vec3 magcal; //calibrated magnetometer
  vec3 flatwest;
  vec3 flatnorth;

  const float to_angle = 180 / 3.14159265358979;
  const float pi_2 = 3.14159265358979 / 2;
  
  float center_x = (min_x + max_x) * 0.5f;
  float center_y = (min_y + max_y) * 0.5f;
  float center_z = (min_z + max_z) * 0.5f;
  float mul_x = 1 / (max_x - center_x);
  float mul_y = 1 / (max_y - center_y);
  float mul_z = 1 / (max_z - center_z);

  magcal.x = mag.x - center_x;
  magcal.y = mag.y - center_y;
  magcal.z = mag.z - center_z;

  magcal.x *= mul_x;
  magcal.y *= mul_y;
  magcal.z *= mul_z;

  calibratedbuffer[bufferindex] = magcal;
  bufferindex++;
  bufferindex = bufferindex % 5;

  magcal = getaverage();
  magcal = vecNormalize(magcal);
  gravnorm = vecNormalize(grav);
  flatwest = vecCross(magcal, gravnorm);
  
  flatwest = vecNormalize(flatwest);
  flatnorth = vecCross(gravnorm, flatwest);
  flatnorth = vecNormalize(flatnorth);

  float theta_z = atan2(flatnorth.y, flatnorth.x) * to_angle;
  theta_z *= -1.f;
  theta_z += 90.f;
  float len = vecLen(flatnorth);


  min_x += 0.001f;
  min_y += 0.001f;
  min_z += 0.001f;
  max_x -= 0.001f;
  max_y -= 0.001f;
  max_z -= 0.001f;
  if(mag.x < min_x)
  {
    min_x = mag.x;
  }
  if(mag.x > max_x)
  {
    max_x = mag.x;
  }
  if(mag.y < min_y)
  {
    min_y = mag.y;
  }
  if(mag.y > max_y)
  {
    max_y = mag.y;
  }
  if(mag.z < min_z)
  {
    min_z = mag.z;
  }
  if(mag.z > max_z)
  {
    max_z = mag.z;
  }
  
  //print direction
  if(theta_z > -15 && theta_z < 15)
  {
    Serial.print("N");
  }
  else if(theta_z >= 15 && theta_z <= 75)
  {
    Serial.print("NW");
  }
  else if(theta_z > 75 && theta_z < 105)
  {
    Serial.print("W");
  }
  else if(theta_z >= 105 && theta_z <= 165)
  {
    Serial.print("SW");
  }
  else if(theta_z > 165 && theta_z < 195)
  {
    Serial.print("S");
  }
  else if(theta_z >= 195 && theta_z <= 255)
  {
    Serial.print("SE");
  }
  else if(theta_z >= -75 && theta_z <= -15)
  {
    Serial.print("NE");
  }
  else if(theta_z > 255 || theta_z < -75)
  {
    Serial.print("E");
  }
 //printvec(grav);
 //Serial.print('|');
 //printvec(flatnorth);
 Serial.print('\t');
 Serial.println(theta_z);

 //Serial.print("\t [");
 //Serial.print(min_x);
 //Serial.print(",\t");
 //Serial.print(max_x);
 //Serial.print("]");
 //
 //Serial.print("\t [");
 //Serial.print(min_y);
 //Serial.print(",\t");
 //Serial.print(max_y);
 //Serial.print("]");

 //Serial.print("\t [");
 //Serial.print(min_z);
 //Serial.print(",\t");
 //Serial.print(max_z);
 //Serial.print("]");

 //Serial.print('\n');

 
}
