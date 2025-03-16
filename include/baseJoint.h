#ifndef BASEJOINT_H
#define BASEJOINT_H

#include <AccelStepper.h>
#include <input.h>
#include <systemConfig.h>
#include <pinConfig.h>

enum JointStates{
    STOP,
    ACTIVE,
    HOMING,
    HOMING_BACKOFF,
    ESTOP
};

class BaseJoint{
    public:
        BaseJoint(const AxisConfig& conf);

        void home();
        bool isHomed();

        void emergencyStopActivation();
        void emergencyStopDeactivation();
        void stop();
        void start();

        void run();

        // Accelerate to setpoint speed over the time updatePeriodMicros in microseconds
        void  accelToSpeed(float radPerSecond, unsigned long accelPeriodMicros);
        
        void  setSpeed(float radPerSecond);
        float getSpeed();
        void  setPos(float rad);
        float getPos();
        
    private:
        bool _invertDir;
    
        int _pinEn;

        float _stepsPerRad;
        int   _microstepping;
        float _transmission; // Divider for motor rotation. E.g. transmission = 3 means for one joint rotation, 3 motor rotations.
        float _convFactor;   // Equals stepsPerRad*microstepping*transmission

        elapsedMicros _tIntegrate;
        float _acceleration = 0;
        float _speedTarget  = 0;
        
        int radToSteps(float rad);
        float stepsToRad(int steps);

        void stateActive();

        void stateHoming();
        void stateHomingBackoff();
        
    protected:
        AccelStepper _stepper;
        Input        _endstop;
    
        JointStates _state  = JointStates::STOP;
        bool  _isHomed      = false;

};


#endif