/*
 * fsm.h
 *
 *  Created on: Mar 30, 2021
 *      Author: Flynn
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

#define STATE_NAME_LENGTH 12

// States
typedef enum
{
    Idle_State,
    Standby_State,
    Igniter_On_State,
    Valve_Open_State,
    Igniter_Off_State,
    Aborted_State,
    Last_State
} eFsmState;

// State names 
const static char eFsmStateNames[][STATE_NAME_LENGTH] =
{
    "Idle",
    "Standby",
    "Igniter On",
    "Valve Open",
    "Igniter Off",
    "Aborted",
    "PLACEHOLDER"
};
// Fsm events
typedef enum
{
    Error_Event,
    Arm_Event,
    Launch_Event,
    Open_Valve_Timer_Event,
    Stop_Igniter_Timer_Event,
    Reset_Event,
    Last_Event
} eFsmEvent;

// Struct for handing pointers for external stuff if needed 
// TODO remove if not used
typedef struct PeripheriesData
{
    int i;     // Just a placeholder
}eFsmPeripheriesData;

// Initialize finite state machine
void FSM_init();

// Get current state;
eFsmState FSM_state();

// Get event
void FSM_sendEvent(eFsmEvent Event);

// Step the finite state machines logic
void FSM_step(eFsmPeripheriesData *sPeripheries);


#endif /* INC_FSM_H_ */
