#ifndef MAGNET_H
# define MAGNET_H

#include "../vector3/vector3.h"
#include "Arduino.h"
#include <cmath>
#include "Arduino_BMI270_BMM150.h"


class Magnet{

public:

    Magnet(arduino::Stream & logging_serial);
    void init();
    void update();

    float heading;

private:

    arduino::Stream & logging_serial;

    // *** 2331 Board ***
    // float min_x = -30;
    // float min_y = -59;
    // float min_z = -52;
    // float max_x = 65;
    // float max_y = 37;
    // float max_z = 42;

    // *** 2331 Board With Electronics ***
    // float min_x = -32;
    // float min_y = -42;
    // float min_z = -19;
    // float max_x = 65;
    // float max_y = 49;
    // float max_z = 80;

    // *** 2316 Board With Electronics ***
    float min_x = -19;
    float min_y = -48;
    float min_z = -66;
    float max_x = 65;
    float max_y = 49;
    float max_z = 30;

    vec3 grav;//raw accelerometer reading
    vec3 mag;//raw magnetometer reading

    int bufferindex;
    vec3 calibratedbuffer[5]; //buffer of calibrated values, pre-normalized

    vec3 getaverage();

};

#endif