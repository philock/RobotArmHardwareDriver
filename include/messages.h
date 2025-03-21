// Defined by the user
enum MSG : char{
    MSG_RESET, // Reset Microcontroller
    MSG_SOFT_RESET, // Reset all axis velocities and positions to zero (independent of actual arm position!)

    // Joint positions
    JT_R_P, 
    JT_A_P, 
    JT_B_P,
    JT_C_P,
    JT_D_P,

    // Joint velocity setpoints
    JT_R_V,
    JT_A_V,
    JT_B_V,
    JT_C_V,
    JT_D_V,

    QUERY_JT_STATES, // Send current joint positions and velocities

    GR_CLOSE, // Close gripper
    GR_OPEN,  // Open gripper

    MSG_END // End of transmission
};

enum MSG_TYPE : char{
    MSG_FLAG     = 0x00, 
    MSG_VARIABLE = 0x01, 
    MSG_CUSTOM   = 0x02  
};

struct rx_message {
    MSG msg;
    MSG_TYPE msg_type;
    int size;
};

