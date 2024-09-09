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

    float min_x = -32;
    float min_y = -45;
    float min_z = -38;
    float max_x = 63;
    float max_y = 51;
    float max_z = 73;

    vec3 grav;//raw accelerometer reading
    vec3 mag;//raw magnetometer reading

    int bufferindex;
    vec3 calibratedbuffer[5]; //buffer of calibrated values, pre-normalized

    vec3 getaverage();

};

#endif