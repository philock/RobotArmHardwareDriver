#include <axisCDR.h>

AxisCDR::AxisCDR(const AxisConfig conf):
BaseJoint(conf.degPerFullStep, conf.microsteps, conf.transmissionFactor, conf.pinStep, conf.pinDir),
driver(conf.uartSerialPort, TMC2226_R_SENSE, conf.spiAddress),
endstop(conf.pinEs, conf.endstopActiveHigh, conf.endstopInputPullup){

    endstop.limitRate(ENDSTOP_POLL_RATE);

    driver.begin();             
    //driver.internal_Rsense(0);
    driver.I_scale_analog(0);
    driver.rms_current(500, 1.0);
    
    driver.en_spreadCycle(0);
    driver.pwm_autoscale(1);
    delay(100);
    driver.microsteps(8);
}

void AxisCDR::run(){
    switch(_state){
        case JointStates::ACTIVE:
            stateActive();
            break;

        case JointStates::HOMING:
            stateHoming();
            break;

        case JointStates::HOMING_BACKOFF:
            stateHomingBackoff();
            break;

        default: // ESTOP and STOP states 
            break;
    }
}

void AxisCDR::stateActive(){
    if(tIntegrate > ACCEL_INTEGRATION_RATE){
        tIntegrate = 0;
        setSpeed(getSpeed() + _acceleration*ACCEL_INTEGRATION_RATE*0.000001);
    }

    stepper.runSpeed();
}

void AxisCDR::stateHoming(){
    // Once endstop is activated, backoff the endstop to release it again
    if(endstop.read()){ 
        setSpeed(-AX_A_HOMING_SPEED_BACKOFF);
        
        _state = JointStates::HOMING_BACKOFF;

        return;
    }

    stepper.runSpeed();
}

void AxisCDR::stateHomingBackoff(){
    // Set position to zero once the endstop is released again
    if(!endstop.read()){ 
        setSpeed(0);
        setPos(0);

        _isHomed = true;
        _state = JointStates::ACTIVE;

        return;
    }

    stepper.runSpeed();
}

void AxisCDR::home(){
    _state = JointStates::HOMING;

    setSpeed(AX_A_HOMING_SPEED);
}

bool AxisCDR::isHomed(){
    return _isHomed;
}

