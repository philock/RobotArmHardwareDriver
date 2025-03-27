#include <Arduino.h>
#include <systemConfig.h>
#include <comm.h>
#include <axisCDR.h>
#include <baseJoint.h>
#include <logger.h>
#include <ui.h>

Comm cm(115200);
UI   ui;

SystemStates state = SystemStates::STOP;

Input emergencyStop(PIN_EMS, true, true);

#define PSU_ON  digitalWrite(PIN_RELAY, LOW);
#define PSU_OFF digitalWrite(PIN_RELAY, HIGH);

AxisCDR axisR(axisConfigC);
AxisCDR axisA(axisConfigA);
AxisCDR axisB(axisConfigB);
AxisCDR axisC(axisConfigC);
AxisCDR axisD(axisConfigC);

unsigned long updateVelocitiesInterval = 50000; // 50ms
elapsedMicros t_lastVelocityUpdate;
elapsedMicros t_systemTick;
elapsedMicros t_uiUpdate;
elapsedMicros t_uiJointUpdate;

bool start();
void stop();
void homing();

bool allHomed(){
    return axisA.isHomed() && axisB.isHomed() && axisC.isHomed() && axisD.isHomed() && axisR.isHomed();
}

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

    logger.error("Emergency stop pressed");
}

void cb_emergencyStopDeactivation(){
    PSU_ON
    delay(100);

    stop();

    logger.info("Emergency stop released");
}

void cb_homingButton(){
    homing();
}

void cb_startStopButton(){
    if(state == SystemStates::STOP){ // Start pressed
        bool isStarted = start();

        if(isStarted) ui.setStartStopButtonState(false); // Show "Stop" button
        else          ui.setStartStopButtonState(true);  // Show "Start" button
    }
    else if(state == SystemStates::ACTIVE){ // Stop pressed
        stop();

        ui.setStartStopButtonState(true); // Show "Start" button
    }
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
    if(state != SystemStates::STOP) return;
    
    PSU_ON

    logger.info("Homing axis A...");
    state = SystemStates::HOMING_A;
    axisA.home();
}

bool start(){
    if(state != SystemStates::STOP) return false;
    if(!allHomed()){
        logger.warning("Not fully homed");
        return false;
    } 

    axisA.start();
    axisB.start();
    axisC.start();
    axisD.start();
    axisR.start();

    state = SystemStates::ACTIVE;

    logger.info("System started");

    return true;
}

void stop(){
    axisA.stop();
    axisB.stop();
    axisC.stop();
    axisD.stop();
    axisR.stop();

    state = SystemStates::STOP;

    logger.info("System stopped");
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
            /* state = SystemStates::HOMING_BC;
            logger.info("Homing axis B and C");
            axisB.home();
            axisC.home(); */
            stop();
        }
        break;
    
    case SystemStates::HOMING_BC:
        if(axisB.isHomed() && axisC.isHomed()){
            state = SystemStates::HOMING_DR;
            logger.info("Homing axis D and R");
            axisD.home();
            axisR.home();
        }
        break;

    case SystemStates::HOMING_DR:
        if(axisD.isHomed() && axisR.isHomed()){
            logger.info("All axis homed");
            stop();
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

void setup(){
    //Serial.begin(115200);
    emergencyStop.limitRate(EMERGENCY_STOP_POLL_RATE);
    emergencyStop.setActivationHandler(cb_emergencyStopActivation);
    emergencyStop.setDeactivationHandler(cb_emergencyStopDeactivation);

    pinMode(PIN_RELAY, OUTPUT);
    PSU_OFF

    stop();  

    ui.init();
    ui.registerCbButtonHome(cb_homingButton);
    ui.registerCbButtonStartStop(cb_startStopButton);
}

elapsedMicros loopTimer;
void loop(){
    emergencyStop.poll();

    axisA.run();
    axisB.run();
    axisC.run();
    axisD.run();
    axisR.run();

    // Process communication with host computer and update system FSM
    if(t_systemTick > 5000){
        t_systemTick = 0;
        processRespondMessage();
        runSystem();
    } 

    // Process touchscreen inputs and update log box
    if(t_uiUpdate > 100000){
        t_uiUpdate = 0;

        ui.update();
    }

    // Update joint state indicators
    if(t_uiJointUpdate > 500000){
        ui.setJointIndicator(0, axisR.getState());
        ui.setJointIndicator(1, axisA.getState());
        ui.setJointIndicator(2, axisB.getState());
        ui.setJointIndicator(3, axisC.getState());
        ui.setJointIndicator(4, axisD.getState());
        //ui.setJointIndicator(5, axisG.getState());
    }

    /* if(loopTimer > 50){
        Serial.println(loopTimer);
    } 
    loopTimer = 0; */
}
