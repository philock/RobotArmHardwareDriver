#include <Arduino.h>
#include <comm.h>
#include <axisCDR.h>
#include <baseJoint.h>

Comm cm(115200);
AxisCDR axisC(axisConfigC);

unsigned long updateVelocitiesInterval = 50000; // 50ms
elapsedMicros t_updateVelocities;
elapsedMicros t_checkSerial;

void processMessageFlag(rx_message& rxm){
    switch(rxm.msg){
        case MSG::QUERY_JT_STATES:
            cm.addVariableToken<float>(axisC.getPos(), MSG::JT_C_P);
            cm.addVariableToken<float>(axisC.getSpeed(), MSG::JT_C_V);
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

    updateVelocitiesInterval = (updateVelocitiesInterval + t_updateVelocities)*0.5;
    t_updateVelocities = 0;

    // Transmit response
    cm.transmit();
}

void setup(void){
}

void loop(){
    // Process communication with host computer every 5ms
    if(t_checkSerial > 5000){
        t_checkSerial = 0;
        processRespondMessage();
    }

    axisC.run();
}
