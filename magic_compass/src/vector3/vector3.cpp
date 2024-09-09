#include "vector3.h"

void vec3::printvec(vec3 vec)
{
  Serial.print('(');
  Serial.print(vec.x);
  Serial.print('\t');
  Serial.print(vec.y);
  Serial.print('\t');
  Serial.print(vec.z);
  Serial.print(")\t");
}


// vec3 vec3::operator+(const vec3& a){
//     return { x+a.x,  y+a.y, z+a.z };
// }

vec3 vecAdd(vec3 a, vec3 b){
  return { a.x+b.x,  a.y+b.y, a.z+b.z };
}

vec3 vecSub(vec3 a, vec3 b){
  return { a.x-b.x,  a.y-b.y, a.z-b.z };
}

vec3 vec3::vecMult(vec3 a, float c){
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


float vec3::vecLen(){
  return sqrt( vecDot(*this,*this)  );
}

vec3 vec3::vecNormalize(){
  float l = this->vecLen();
  if(l < 0.001f) Serial.print("DENORM VALUE");
  return vecMult(*this , 1/l) ;
}

void vec3::serialVec3(){

  Serial.print(this->x);
  Serial.print(",");
  Serial.print(this->y);
  Serial.print(",");
  Serial.print(this->z);
  Serial.print("\n");
}

