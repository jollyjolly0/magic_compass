#define SerialLoRa Serial2

#include "lora.h"
#include "vector3.h"


LoRA lora = LoRA();


void setup()
{
  lora.init();
}



void loop()
{
  lora.update();
}

