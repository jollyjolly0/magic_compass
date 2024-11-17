
#include "magnet.h"

Magnet::Magnet(arduino::Stream & logging_uart)
 : logging_serial(logging_uart)
{
}

void Magnet::init() {
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


vec3 Magnet::getaverage()
{
  vec3 retval = { 0.0f, 0.0f, 0.0f };
  retval = vecAdd(calibratedbuffer[0], calibratedbuffer[1]);
  retval = vecAdd(retval, calibratedbuffer[2]);
  retval = vecAdd(retval, calibratedbuffer[3]);
  retval = vecAdd(retval, calibratedbuffer[4]);
  return retval;
}

void Magnet::update() {
  char buff[100];
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(grav.y, grav.x, grav.z);// NOTE Y, X, Z order
    // known magnet vector in earth coords points north and into earth
    // known gravity vector in earth coords points into earth 
    // tested by determining positive mag axis (poingint in positive direction wrt to known mag field north and into earth)
    grav.y *= -1.0f;
  }
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(mag.x, mag.y, mag.z);
  }
  else{
    // logging_serial.println("can't load mag field");
    return;
  } 
    // Serial.println("Data Vals");
    // sprintf(buff , "\t acc %f , %f, %f", grav.x, grav.y, grav.z);
    // Serial.println(buff);
    // sprintf(buff , "\t raw : %f , %f, %f", mag.x, mag.y, mag.z);
    // Serial.println(buff);


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

  // sprintf(buff , "\t cal : %f , %f, %f", magcal.x, magcal.y, magcal.z);
  // Serial.println(buff);

  calibratedbuffer[bufferindex] = magcal;
  bufferindex++;
  bufferindex = bufferindex % 5;

  magcal = getaverage();
  magcal = magcal.vecNormalize();

  gravnorm = grav.vecNormalize();

  // sprintf(buff , "\t avg : %f , %f, %f", magcal.x, magcal.y, magcal.z);
  // Serial.println(buff);

  flatwest = vecCross(magcal, gravnorm);

  
  flatwest = flatwest.vecNormalize();
  flatnorth = vecCross(gravnorm, flatwest);
  flatnorth = flatnorth.vecNormalize();

  // sprintf(buff , "\t wst : %f , %f, %f", flatwest.x, flatwest.y, flatwest.z);
  // Serial.println(buff);
  // sprintf(buff , "\t nth : %f , %f, %f", flatnorth.x, flatnorth.y, flatnorth.z);
  // Serial.println(buff);

  float theta_z = atan2(flatnorth.y, flatnorth.x) * to_angle;
  float len = flatnorth.vecLen();

  // sprintf(buff , "\t theta :  %f", theta_z);
  // Serial.println(buff);


// For now, don't update it, just pass the values in
//   min_x += 0.001f;
//   min_y += 0.001f;
//   min_z += 0.001f;
//   max_x -= 0.001f;
//   max_y -= 0.001f;
//   max_z -= 0.001f;
//   if(mag.x < min_x)
//   {
//     min_x = mag.x;
//   }
//   if(mag.x > max_x)
//   {
//     max_x = mag.x;
//   }
//   if(mag.y < min_y)
//   {
//     min_y = mag.y;
//   }
//   if(mag.y > max_y)
//   {
//     max_y = mag.y;
//   }
//   if(mag.z < min_z)
//   {
//     min_z = mag.z;
//   }
//   if(mag.z > max_z)
//   {
//     max_z = mag.z;
//   }
  
  //print direction
  // if(theta_z > -15 && theta_z < 15)
  // {
  //   Serial.print("N");
  // }
  // else if(theta_z >= 15 && theta_z <= 75)
  // {
  //   Serial.print("NW");
  // }
  // else if(theta_z > 75 && theta_z < 105)
  // {
  //   Serial.print("W");
  // }
  // else if(theta_z >= 105 && theta_z <= 165)
  // {
  //   Serial.print("SW");
  // }
  // else if(theta_z > 165 && theta_z < 195)
  // {
  //   Serial.print("S");
  // }
  // else if(theta_z >= 195 && theta_z <= 255)
  // {
  //   Serial.print("SE");
  // }
  // else if(theta_z >= -75 && theta_z <= -15)
  // {
  //   Serial.print("NE");
  // }
  // else if(theta_z > 255 || theta_z < -75)
  // {
  //   Serial.print("E");
  // }
 //printvec(grav);
 //Serial.print('|');
 //printvec(flatnorth);

//  Serial.print('\t');
//  Serial.println(theta_z);
//  Serial.print(min_x);
//  Serial.print(",");
//  Serial.print(min_y);
//  Serial.print(",");
//  Serial.print(min_z);
//  Serial.print(",");
//  Serial.print(max_x);
//  Serial.print(",");
//  Serial.print(max_y);
//  Serial.print(",");
//  Serial.print(max_z);
//  Serial.println(",");

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

    // this is based on the device being arranged with "+X" being forward. with degrees east as positive 
    //  heading = -theta_z -11.0f; // approximate declination correction for irvine CA
     heading = -theta_z; // approximate declination correction for irvine CA

    if (heading < 0){
        heading = heading + 360.0;
    }
    if (heading >= 360.0){
        heading = heading - 360.0f;
    }
    // sprintf(buff , "\t heading :  %f", heading);
    // Serial.println(buff);
}
