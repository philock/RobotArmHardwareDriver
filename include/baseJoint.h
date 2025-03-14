#ifndef BASEJOINT_H
#define BASEJOINT_H

#include <AccelStepper.h>

enum JointStates{
    STOP,
    ACTIVE,
    HOMING,
    HOMING_BACKOFF,
    ESTOP
};

class BaseJoint{
    public:
        BaseJoint(float degPerFullStep, int microstepping, float transmissionFactor, int stepPin, int dirPin);

        void setPos(float rad);
        float getPos();

        void setSpeed(float radPerSecond);
        float getSpeed();

        void setAccel(float radPerSS);

    private:
        bool _invertDir;
    
        float _stepsPerRad;
        int   _microstepping;
        float _transmission; // Divider for motor rotation. E.g. transmission = 3 means for one joint rotation, 3 motor rotations.
        float _convFactor;   // Equals stepsPerRad*microstepping*transmission
        
        int radToSteps(float rad);
        float stepsToRad(int steps);
        
    protected:
        JointStates _state = STOP;
        AccelStepper stepper;

};


#endif