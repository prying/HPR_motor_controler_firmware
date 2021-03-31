/*
 * fsm.h
 *
 *  Created on: Mar 30, 2021
 *      Author: Flynn
 */

#ifndef INC_FSM_H_
#define INC_FSM_H_

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

// Struct for handing pointers for external stuff like peripheries
// TODO figure out how best to implement this struct and how it interacts with HAL. Test first with a GPIO. Also might be renamed if it ends up
// doing somthing else
typedef struct PeripheriesData
{
	int i; // Just a placeholder
}eFsmPeripheriesData;

// Initialize finite state machine
void Fsm_Init();

// Get current state;
eFsmState Fsm_State();

// Get event
void Fsm_sendEvent(eFsmEvent Event);

// Step the finite state machines logic
void Fsm_Step(eFsmPeripheriesData *sPeripheries);


#endif /* INC_FSM_H_ */
