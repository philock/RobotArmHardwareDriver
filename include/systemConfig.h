#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

#define TMC2226_R_SENSE 0.15f // Ohms

#define ACCEL_INTEGRATION_RATE 5000 // microseconds

// Inputs
#define ENDSTOP_POLL_RATE        10 // milliseconds
#define EMERGENCY_STOP_POLL_RATE 50 // milliseconds

// Axis A
#define AX_A_HOMING_SPEED         0.02  // rad/sec
#define AX_A_HOMING_SPEED_BACKOFF 0.05 // rad/sec

enum class JointStates{
    STOP,
    ACTIVE,
    HOMING,
    HOMING_BACKOFF,
    ESTOP
};

enum class SystemStates{
    ESTOP,
    STOP,
    ACTIVE,
    HOMING_A,
    HOMING_BC,
    HOMING_DR,
};

#endif