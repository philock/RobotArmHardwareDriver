#include <axisCDR.h>

AxisCDR::AxisCDR(const AxisConfig& conf):
BaseJoint(conf),
driver(conf.uartSerialPort, TMC2226_R_SENSE, conf.spiAddress){

    driver.begin();             
    //driver.internal_Rsense(0);
    driver.I_scale_analog(0);
    driver.rms_current(500, 1.0);
    
    driver.en_spreadCycle(0);
    driver.pwm_autoscale(1);
    delay(100);
    driver.microsteps(8);
}


