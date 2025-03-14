#include <BaseJoint.h>
#include <Arduino.h>

BaseJoint::BaseJoint(float degPerFullStep, int microstepping, float transmissionFactor, int stepPin, int dirPin):
stepper(AccelStepper::DRIVER, stepPin, dirPin){
    float radPerFullStep = degPerFullStep * DEG_TO_RAD;

    microstepping = microstepping;
    _stepsPerRad = microstepping/radPerFullStep;
    _transmission = transmissionFactor;
    _convFactor = _stepsPerRad*_transmission;

    stepper.disableOutputs();
    stepper.setPinsInverted(false, false, true);
    stepper.enableOutputs();

    stepper.setMaxSpeed(radToSteps(3));
    stepper.setSpeed(radToSteps(0));

}


int BaseJoint::radToSteps(float rad){
    return (int)round(rad*_convFactor);
}

float BaseJoint::stepsToRad(int steps){
    return steps/_convFactor;
}


void BaseJoint::setPos(float rad){
    stepper.setCurrentPosition(radToSteps(rad)); // Force position
}

float BaseJoint::getPos(){
    return stepsToRad(stepper.currentPosition());
}

void BaseJoint::setSpeed(float radPerSecond){
    stepper.setSpeed(radToSteps(radPerSecond)); // for stepper.runSpeedToPosition()
}

float BaseJoint::getSpeed(){
 return stepsToRad(stepper.speed());
}

void BaseJoint::setAccel(float radPerSS){
    stepper.setAcceleration(radToSteps(radPerSS));
}