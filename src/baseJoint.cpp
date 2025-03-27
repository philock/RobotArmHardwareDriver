#include <baseJoint.h>
#include <Arduino.h>

BaseJoint::BaseJoint(const AxisConfig& conf):
_stepper(AccelStepper::DRIVER, conf.pinStep, conf.pinDir),
_endstop(conf.pinEs, conf.endstopActiveHigh, conf.endstopInputPullup){

    float radPerFullStep = conf.degPerFullStep * DEG_TO_RAD;

    _microstepping = conf.microsteps;
    _stepsPerRad = _microstepping/radPerFullStep;
    _transmission = conf.transmissionFactor;
    _convFactor = _stepsPerRad*_transmission;

    _pinEn = conf.pinEn;
    pinMode(_pinEn, OUTPUT);
    
    _stepper.disableOutputs();
    _stepper.setPinsInverted(false, false, true);
    _stepper.enableOutputs();
    
    _stepper.setMaxSpeed(radToSteps(3));
    _stepper.setSpeed(radToSteps(0));
    
    _endstop.limitRate(ENDSTOP_POLL_RATE);

    digitalWrite(_pinEn, LOW); // Enable stepper driver
}

void BaseJoint::run(){
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

// -----------------------------------------
// State functions
// -----------------------------------------
void BaseJoint::stateActive(){
    if(_tIntegrate > ACCEL_INTEGRATION_RATE){
        _tIntegrate = 0;

        float vNew = getSpeed() + _acceleration*ACCEL_INTEGRATION_RATE*0.000001;

        if(abs(vNew) < abs(_speedTarget)) setSpeed(vNew);
    }

    _stepper.runSpeed();
}

void BaseJoint::stateHoming(){
    // Once endstop is activated, backoff the endstop to release it again
    if(_endstop.read()){ 
        setSpeed(-AX_A_HOMING_SPEED_BACKOFF);
        
        _state = JointStates::HOMING_BACKOFF;

        return;
    }

    _stepper.runSpeed();
}

void BaseJoint::stateHomingBackoff(){
    // Set position to zero once the endstop is released again
    if(!_endstop.read()){ 
        setSpeed(0);
        setPos(0);

        _isHomed = true;
        _state = JointStates::STOP;

        return;
    }

    _stepper.runSpeed();
}

// -----------------------------------------
// State entry functions
// -----------------------------------------
void BaseJoint::home(){
    if(_state == JointStates::ESTOP) return;

    _stepper.enableOutputs();
    setSpeed(AX_A_HOMING_SPEED);
    
    _state = JointStates::HOMING;
}

void BaseJoint::emergencyStopActivation(){
    setSpeed(0);
    digitalWrite(_pinEn, HIGH); // Disable stepper driver
    _stepper.disableOutputs();  // Disable stepper outputs

    _isHomed = false;

    _state = JointStates::ESTOP;
}

void BaseJoint::emergencyStopDeactivation(){
    stop();
}

void BaseJoint::stop(){
    setSpeed(0);
    digitalWrite(_pinEn, LOW); // Enable stepper driver
    _stepper.disableOutputs(); // Disable stepper outputs

    _state = JointStates::STOP;
}

void BaseJoint::start(){
    if(!_isHomed || _state != JointStates::STOP) return;

    _stepper.enableOutputs();

    _state = JointStates::ACTIVE;
}

// -----------------------------------------
// Utility functions
// -----------------------------------------
bool BaseJoint::isHomed(){
    return _isHomed;
}

int BaseJoint::radToSteps(float rad){
    return (int)round(rad*_convFactor);
}

float BaseJoint::stepsToRad(int steps){
    return steps/_convFactor;
}

void BaseJoint::setPos(float rad){
    _stepper.setCurrentPosition(radToSteps(rad)); // Force position
}

float BaseJoint::getPos(){
    return stepsToRad(_stepper.currentPosition());
}

void BaseJoint::setSpeed(float radPerSecond){
    _stepper.setSpeed(radToSteps(radPerSecond)); // for _stepper.runSpeedToPosition()
}

void BaseJoint::accelToSpeed(float radPerSecond, unsigned long accelPeriodMicros){
    _acceleration = (radPerSecond - getSpeed())/(accelPeriodMicros * 0.000001);
    _speedTarget = radPerSecond;
}

float BaseJoint::getSpeed(){
 return stepsToRad(_stepper.speed());
}
