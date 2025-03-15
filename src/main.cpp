#include <Arduino.h>
#include <axisCDR.h>

AxisCDR axisC(axisConfigC);

void setup(void){

}

void loop(){
  axisC.run();
}
