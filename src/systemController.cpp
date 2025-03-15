#include <systemController.h>

void SystemController::run(){
    emergencyStop.limitRate(EMERGENCY_STOP_POLL_RATE);

    
}