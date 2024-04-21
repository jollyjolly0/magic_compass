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

#include <cmath>
#include <BasicLinearAlgebra.h>
#include "Arduino_BMI270_BMM150.h"

struct vec3  {
  float x;
  float y;
  float z;
};


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
  return vecMult(a, 1/l) ;
}

void printVec3(vec3 a){
  Serial.print(a.x);
  Serial.print(",\t");
  Serial.print(a.y);
  Serial.print(",\t");
  Serial.print(a.z);
  Serial.print("\n");
}

void serialVec3(vec3 a){
  Serial.print(a.x);
  Serial.print(",");
  Serial.print(a.y);
  Serial.print(",");
  Serial.print(a.z);
  Serial.print("\n");
}



vec3 curAcc;
vec3 curMag;



// const vec3 MAG_BIAS = {20, -7, -4};
const vec3 MAG_BIAS = {0, 0, 0};

vec3 calibrateMag(vec3 mag){
  return vecSub(mag, MAG_BIAS );
}

void pollSensors(){
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(curMag.x, curMag.y, curMag.z);
    curMag = calibrateMag(curMag);
  }

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(curAcc.x, curAcc.y, curAcc.z);
  }

}




void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Started");

  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  Serial.print("Magneto sample rate = ");
  Serial.print(IMU.magneticFieldSampleRate());
  Serial.println(" Hz");

  Serial.print("Accel sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");
}









void loop() {
  pollSensors();
  
  // Serial.println("current acc");
  // printVec3(curAcc);
  // Serial.println("current mag");
  serialVec3(curMag);

  delay(20);


}







