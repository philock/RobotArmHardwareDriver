#ifndef AXIS_CDR_H
#define AXIS_CDR_H

#include <baseJoint.h>
#include <TMCStepper.h>
#include <systemConfig.h>
#include <pinConfig.h>


class AxisCDR : public BaseJoint{
    public:
        AxisCDR(const AxisConfig& conf);

        void home();

    private:
        TMC2209Stepper driver; // Equivalent to TMC2226
        

};

#endif