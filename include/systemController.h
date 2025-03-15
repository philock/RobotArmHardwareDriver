#ifndef SYSTEM_CONTROLLER_H
#define SYSTEM_CONTROLLER_H

#include <input.h>
#include <systemConfig.h>
#include <pinConfig.h>
#include <axisCDR.h>


class SystemController{
    public: 
    SystemController();
    
    void run();
    
    private:
        Input emergencyStop;
        AxisCDR axisCDR;

};

#endif