#ifndef __TriggerBoxV2_H__
#define __TriggerBoxV2_H__


const int   TRS_TIP_PINS[]                = { 22, 36 }; // bottom jack (input- and output-capable), top jack (analog input only)
const int   TRS_RING_PINS[]               = { 23, 37 }; // bottom jack (input- and output-capable), top jack (analog input only)
const int   POTENTIOMETER_PINS[]          = { 29, 31 }; // left and right potentiometers as seen from the input side of the box
const float POTENTIOMETER_SCALING         = 1.0;
const int   PARALLEL_PORT_INPUT_PINS[]    = { 14, 15, 16, 17, 18, 19, 20, 21 }; // D0 through D7 respectively
const int   PARALLEL_PORT_OUTPUT_PINS[]   = {  8,  0,  1,  2,  3,  4,  5,  6 }; // D0 through D7 respectively
const int   BNC_PINS[]                    = { 30, 27 }; // left and right BNCs
const int   RJ45_INPUT_PINS[]             = { 12, 11 }; // these two pins correspond to RJ45 terminals 1 and 2; RJ45 terminals 7 & 8 are wired to ground; set the pinMode of these inputs to INPUT_PULLUP - then digitalRead() result will become LOW when the circuit is closed between input and ground
const int   SPARE_LED_PINS[]              = { 13,  7 }; // top and next-down-from-top

#endif // __TriggerBoxV2_H__
