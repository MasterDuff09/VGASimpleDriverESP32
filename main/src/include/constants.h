#ifndef CONSTANTS_H
#define CONSTANTS_H

#define H_ACTIVE_FRAMES                 640
#define H_FRONT_PORCH_FRAMES            16
#define H_SYNC_PULSE_FRAMES             96
#define H_BACK_PORCH_FRAMES             48


#define V_ACTIVE_FRAMES                 480
#define V_FRONT_PORCH_FRAMES            10
#define V_SYNC_PULSE_FRAMES             2
#define V_BACK_PORCH_FRAMES             33
#define TOTAL_V_FRAMES                  (V_ACTIVE_FRAMES + V_FRONT_PORCH_FRAMES + V_BACK_PORCH_FRAMES + V_SYNC_PULSE_FRAMES)

#define H_LOW_V_LOW     0b00000000
#define H_HIGH_V_LOW    0b00000001
#define H_LOW_V_HIGH    0b00000010
#define H_HIGH_V_HIGH   0b00000011


#endif