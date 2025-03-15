#ifndef AXIS_CDR_H
#define AXIS_CDR_H

#include <baseJoint.h>
#include <TMCStepper.h>
#include <input.h>
#include <systemConfig.h>
#include <pinConfig.h>


class AxisCDR : BaseJoint{
    public:
        AxisCDR(const AxisConfig conf);

        void home();
        bool isHomed();

        void run();

    private:
        TMC2209Stepper driver; // Equivalent to TMC2226
        Input endstop;

        bool _isHomed = false;

        elapsedMicros tIntegrate;

        void stateActive();

        void stateHoming();
        void stateHomingBackoff();

};

#endif