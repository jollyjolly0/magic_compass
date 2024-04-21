
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

vec3 curCross;



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

void updateCross(){

  curCross = vecCross(curAcc, curMag);

  float test1 = vecDot(curCross, curAcc);
  float test2 = vecDot(curCross, curMag);

  Serial.print(test1);
  Serial.print(", ");
  Serial.println(test2);

}



void setup() {
  // this will fail if usb serial is not plugged in???
  Serial.begin(9600);

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



  Serial1.begin(9600);
  

}









void loop() {
  pollSensors();
  updateCross();
  
  // Serial.println("current acc");
  // printVec3(curAcc);
  // Serial.println("current mag");
  // serialVec3(curMag);

  delay(20);


}







