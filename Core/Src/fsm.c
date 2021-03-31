/*
 * fsm.c
 *
 *  Created on: Mar 30, 2021
 *      Author: flynn
 *  based off https://aticleworld.com/state-machine-using-c/
 */

#include <stddef.h>
#include "fsm.h"

// Nasty global vars
static eFsmState eFsmNextState = Last_State;
static eFsmEvent eFsmNewEvent = Last_Event;


// Event handler function pointer
typedef eFsmState (*pfEventHandler) (eFsmPeripheriesData *sPeripheries);


// 2d array of fsm linkages that will return a function pointer
typedef eFsmState (*const afEventHandler[Last_State][Last_Event])(eFsmPeripheriesData *sPeripheries);

// Event handles
//**************************************

// When an error is detected call Error_Event and go to the aborted state
eFsmState ErrorHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO turn everything off and to the safe position
	return Aborted_State;
}

// Go from idle to standby
eFsmState RecivedArmHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO turn on pwr supply for igiter (i.e. turn on the buck converter)
	return Standby_State;
}

// Turn igniter on
eFsmState RevicedLaunchHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO turn on mosfet to allow current to travel to the igiter
	return Igniter_On_State;
}

// Timer finishes counting to open clock
eFsmState AlarmOpenValveHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO Open servo
	return Valve_Open_State;
}

// Timer finishes counting to turn igniter off
eFsmState AlarmTurnOffIgniterHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO turn off mosfet and turn off pwr supply for igniter
	return Igniter_Off_State;
}

// When in the aborted state it can be reset
eFsmState ResetHandler(eFsmPeripheriesData *sPeripheries)
{
	// TODO move all vars needed to be in the idle state
	return Idle_State;
}

// Public function
//*****************************************

// Initialize finite state machine
void Fsm_Init()
{
	eFsmNextState = Idle_State;
	return;
}

// Get current state;
eFsmState Fsm_State()
{
	return eFsmNextState;
}

// Get event
void Fsm_sendEvent(eFsmEvent Event)
{
	// TODO! validation ect;
	eFsmNewEvent = Event;
	return;
}

// Step the finite state machines logic
void Fsm_Step(eFsmPeripheriesData *sPeripheries)
{
	// Setup linkages for the FSM 				Might make a global?
	static afEventHandler FSM =
	{
			[Idle_State] 		= {[Error_Event] = ErrorHandler, [Arm_Event] = RecivedArmHandler},
			[Standby_State] 	= {[Error_Event] = ErrorHandler, [Launch_Event] = RevicedLaunchHandler},
			[Igniter_On_State] 	= {[Error_Event] = ErrorHandler, [Open_Valve_Timer_Event] = AlarmOpenValveHandler},
			[Valve_Open_State] 	= {[Error_Event] = ErrorHandler, [Stop_Igniter_Timer_Event] = AlarmTurnOffIgniterHandler},
			[Igniter_Off_State] = {[Error_Event] = ErrorHandler, [Reset_Event] = ResetHandler},
			[Aborted_State]		= {[Error_Event] = ErrorHandler, [Reset_Event] = ResetHandler}
	};

	// Validate that both state and event are valid and that there is a event handler at the event for this state
	if ((eFsmNextState < Last_State) && (eFsmNewEvent < Last_Event) && FSM[eFsmNextState][eFsmNewEvent] != NULL)
	{
		// Call the event handler at the end of the function pointer
		eFsmNextState = (*FSM[eFsmNextState][eFsmNewEvent])(sPeripheries);
	}
	else
	{
		//TODO Maybe this could also trigger and abort???
	}

	return;
}


