#include <Arduino.h>
#include <comm.h>
#include <axisCDR.h>
#include <baseJoint.h>

Comm cm(115200);

enum class SystemStates{
    ESTOP,
    STOP,
    ACTIVE,
    HOMING_A,
    HOMING_BC,
    HOMING_DR,
};
SystemStates state = SystemStates::STOP;

Input emergencyStop(PIN_EMS, false, true);

#define PSU_ON  digitalWrite(PIN_RELAY, LOW);
#define PSU_OFF digitalWrite(PIN_RELAY, HIGH);

AxisCDR axisR(axisConfigC);
AxisCDR axisA(axisConfigC);
AxisCDR axisB(axisConfigC);
AxisCDR axisC(axisConfigC);
AxisCDR axisD(axisConfigC);

unsigned long updateVelocitiesInterval = 50000; // 50ms
elapsedMicros t_lastVelocityUpdate;
elapsedMicros t_systemTick;

// Declarations
void stop();
bool allHomed();

// -----------------------------------------
// IO callbacks
// -----------------------------------------
void cb_emergencyStopActivation(){
    axisA.emergencyStopActivation();
    axisB.emergencyStopActivation();
    axisC.emergencyStopActivation();
    axisD.emergencyStopActivation();
    axisR.emergencyStopActivation();

    PSU_OFF

    state = SystemStates::ESTOP;
}

void cb_emergencyStopDeactivation(){
    PSU_ON
    delay(100);

    stop();
}

// -----------------------------------------
// communication
// -----------------------------------------
void processMessageFlag(rx_message& rxm){
    switch(rxm.msg){
        case MSG::QUERY_JT_STATES:
            cm.addVariableToken<float>(axisC.getPos(), MSG::JT_C_P);
            cm.addVariableToken<float>(axisC.getSpeed(), MSG::JT_C_V);
            break;

        case MSG::MSG_RESET:
            SCB_AIRCR = 0x05FA0004; // Reset Microcontroller
            break;
        
        default:
            break;
    }

}   

void processMessagePayload(rx_message& rxm){
    float v;
    switch(rxm.msg){
        case MSG::JT_C_V:
            cm.getPayload<float>(v);
            axisC.accelToSpeed(v, updateVelocitiesInterval);
            break;

        default:
            break;

    }
}

void processRespondMessage(){
    // Read first message
    rx_message rxm = cm.getNextMsg();

    while(rxm.msg != MSG::MSG_END){

        // Process flag and payload messages seperatly
        if(rxm.msg_type == MSG_TYPE::MSG_FLAG) processMessageFlag(rxm);    // Flag message
        else                                   processMessagePayload(rxm); // Variable message

        // Get next message in packet
        rxm = cm.getNextMsg();
    }

    // Update interval for velocity updates with outlier rejection
    if(t_lastVelocityUpdate < 200000){
        updateVelocitiesInterval = (updateVelocitiesInterval + t_lastVelocityUpdate)*0.5;
    }
    t_lastVelocityUpdate = 0;

    // Transmit response
    cm.transmit();
}

// -----------------------------------------
// State entry functions
// -----------------------------------------
void homing(){
    if(state == SystemStates::ESTOP) return;
    
    state = SystemStates::HOMING_A;
    axisA.home();
}

void start(){
    if(state == SystemStates::ESTOP) return;
    if(!allHomed()) return;

    axisA.start();
    axisB.start();
    axisC.start();
    axisD.start();
    axisR.start();

    state = SystemStates::ACTIVE;
}

void stop(){
    axisA.stop();
    axisB.stop();
    axisC.stop();
    axisD.stop();
    axisR.stop();

    state = SystemStates::STOP;
}

// -----------------------------------------
// State machine
// -----------------------------------------
void runSystem(){
    switch(state){
    case SystemStates::ACTIVE:
        break;
    
    case SystemStates::HOMING_A:
        if(axisA.isHomed()){
            state = SystemStates::HOMING_BC;
            axisB.home();
            axisC.home();
        }
        break;
    
    case SystemStates::HOMING_BC:
        if(axisB.isHomed() && axisC.isHomed()){
            state = SystemStates::HOMING_DR;
            axisD.home();
            axisR.home();
        }
        break;

    case SystemStates::HOMING_DR:
        if(axisD.isHomed() && axisR.isHomed()){
            state = SystemStates::STOP;
        }
        break;

    
    case SystemStates::STOP:
        break;

    case SystemStates::ESTOP:
        break;

    default:
        break;
    }
}

bool allHomed(){
    return axisA.isHomed() && axisB.isHomed() && axisC.isHomed() && axisD.isHomed() && axisR.isHomed();
}

void setup(void){
    emergencyStop.limitRate(EMERGENCY_STOP_POLL_RATE);
    emergencyStop.setActivationHandler(cb_emergencyStopActivation);
    emergencyStop.setDeactivationHandler(cb_emergencyStopDeactivation);

    pinMode(PIN_RELAY, OUTPUT);
    PSU_ON

    stop();  
}

void loop(){
    emergencyStop.poll();

    // Process communication with host computer and update system FSM
    if(t_systemTick > 5000){
        t_systemTick = 0;
        processRespondMessage();
        runSystem();
    }


    axisA.run();
    axisB.run();
    axisC.run();
    axisD.run();
    axisR.run();
}
