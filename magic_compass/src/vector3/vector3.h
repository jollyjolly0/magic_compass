#ifndef vector3
# define vector3 

#include "Arduino.h"



class vec3{

public:

    float x;
    float y;
    float z;

    void printvec(vec3 vec);
    void serialVec3();

    vec3 vecSub(vec3 a, vec3 b);
    vec3 vecMult(vec3 a, float c);

    float vecLen();
    vec3 vecNormalize();

private:
};

vec3 vecAdd(vec3 a, vec3 b);
float vecDot(vec3 a, vec3 b);
vec3 vecCross(vec3 a, vec3 b);


#endif