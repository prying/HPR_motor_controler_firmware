/*
 * fsm.c
 *
 *  Created on: Mar 30, 2021
 *      Author: flynn
 *  based off https://aticleworld.com/state-machine-using-c/
 */

#include <stddef.h>
#include <stdio.h>
#include "fsm.h"
#include "usart.h"
#include "gpio.h"
#include "pwm.h"

// Defines
#define MSGBUFF_SIZE             25
#define ERROR_MSG_SIZE           30
#define IGN_CONTROL_OFF_DELAY    20 //ms
#define SERVO_CLOSED_ANGLE       0  //degrees
#define SERVO_OPEN_ANGLE         90 //degrees

// Nasty global vars
static eFsmState  eFsmCurrentState = Last_State;
static eFsmEvent  eFsmNewEvent     = Last_Event;

static const char errorMsg[] = "\r\nSomthing went wrong! MOVED TO ERROR STATE\r\n";
static const char resetMsg[] = "\r\nReturned to idel state\r\n";

// Event handler function pointer
typedef eFsmState (*pfEventHandler) (eFsmPeripheriesData *sPeripheries);


// 2d array of fsm linkages that will return a function pointer
typedef eFsmState (*const afEventHandler[Last_State][Last_Event])(eFsmPeripheriesData *sPeripheries);

// Private helper functions
//**************************************

// Anounce on UART that it has moved to a state
void SendStateMsg(eFsmState state);

// Event handles
//**************************************

// When an error is detected call Error_Event and go to the aborted state
eFsmState ErrorHandler(eFsmPeripheriesData *sPeripheries)
{
    // TODO turn everything off and to the safe position
    // Turn off igniter and power to the ignition sorce
    // Turn off IGN Control
    HAL_GPIO_WritePin(IGN_CONTROL_GPIO_Port, IGN_CONTROL_Pin, GPIO_PIN_RESET);

    // Turn on the buck converter for the igniter
    HAL_Delay(IGN_CONTROL_OFF_DELAY);
    HAL_GPIO_WritePin(IGN_PWR_GPIO_Port, IGN_PWR_Pin, GPIO_PIN_RESET);

    // Move valve to suitable position
    // TODO pwm control

    // Anounce over usart and i2c to let everyone know
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)errorMsg, sizeof(errorMsg) / sizeof(char));
    // TODO Some i2c related method
    return Aborted_State;
}

// Go from idle to standby
eFsmState RecivedArmHandler(eFsmPeripheriesData *sPeripheries)
{
    // TODO turn on pwr supply for igiter (i.e. turn on the buck converter)
    // Turn on the buck converter for the igniter
    HAL_GPIO_WritePin(IGN_PWR_GPIO_Port, IGN_PWR_Pin, GPIO_PIN_SET);
    return Standby_State;
}

// Turn igniter on
eFsmState RevicedLaunchHandler(eFsmPeripheriesData *sPeripheries)
{
    // TODO turn on mosfet to allow current to travel to the igiter
    HAL_GPIO_WritePin(IGN_CONTROL_GPIO_Port, IGN_CONTROL_Pin, GPIO_PIN_SET);
    return Igniter_On_State;
}

// Timer finishes counting to open clock
eFsmState AlarmOpenValveHandler(eFsmPeripheriesData *sPeripheries)
{
    PWM1_setPos(SERVO_OPEN_ANGLE);
    return Valve_Open_State;
}

// Timer finishes counting to turn igniter off
eFsmState AlarmTurnOffIgniterHandler(eFsmPeripheriesData *sPeripheries)
{
    // TODO turn off mosfet and turn off pwr supply for igniter
    // Turn off IGN Control
    HAL_GPIO_WritePin(IGN_CONTROL_GPIO_Port, IGN_CONTROL_Pin, GPIO_PIN_RESET);

    // Turn on the buck converter for the igniter
    HAL_Delay(IGN_CONTROL_OFF_DELAY);
    HAL_GPIO_WritePin(IGN_PWR_GPIO_Port, IGN_PWR_Pin, GPIO_PIN_RESET);

    return Igniter_Off_State;
}

// When in the aborted state it can be reset
eFsmState ResetHandler(eFsmPeripheriesData *sPeripheries)
{
    // TODO move all vars needed to be in the idle state

    // Turn off IGN Control
    HAL_GPIO_WritePin(IGN_CONTROL_GPIO_Port, IGN_CONTROL_Pin, GPIO_PIN_RESET);

    // Turn on the buck converter for the igniter
    HAL_Delay(IGN_CONTROL_OFF_DELAY);
    HAL_GPIO_WritePin(IGN_PWR_GPIO_Port, IGN_PWR_Pin, GPIO_PIN_RESET);

    // Move valve to suitable position
    PWM1_setPos(SERVO_CLOSED_ANGLE);

    // Anounce over usart and i2c to let everyone know
    HAL_UART_Transmit_DMA(&huart2, (uint8_t *)resetMsg, sizeof(resetMsg) / sizeof(char));

    return Idle_State;
}

// Public function
//*****************************************

// Initialize finite state machine
void Fsm_Init()
{
    eFsmCurrentState = Idle_State;
    return;
}

// Get current state;
eFsmState Fsm_State()
{
    return eFsmCurrentState;
}

// Get event
void Fsm_SendEvent(eFsmEvent Event)
{
    // TODO! validation ect;
    eFsmNewEvent = Event;
    return;
}

// Step the finite state machines logic
void Fsm_Step(eFsmPeripheriesData *sPeripheries)
{
    // Setup linkages for the FSM               Might make a global if it is wasiting alot of loop resourses?
    static afEventHandler FSM =
    {
        [Idle_State]        = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler, [Arm_Event]                = RecivedArmHandler          },
        [Standby_State]     = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler, [Launch_Event]             = RevicedLaunchHandler       },
        [Igniter_On_State]  = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler, [Open_Valve_Timer_Event]   = AlarmOpenValveHandler      },
        [Valve_Open_State]  = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler, [Stop_Igniter_Timer_Event] = AlarmTurnOffIgniterHandler },
        [Igniter_Off_State] = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler                                                          },
        [Aborted_State]     = {[Error_Event] = ErrorHandler, [Reset_Event]              = ResetHandler                                                          }
    };

    // Validate that both state and event are valid and that there is a event handler at the event for this state
    if ((eFsmCurrentState < Last_State) && (eFsmNewEvent < Last_Event) && FSM[eFsmCurrentState][eFsmNewEvent] != NULL)
    {
        // Call the event handler at the end of the function pointer
        eFsmCurrentState = (*FSM[eFsmCurrentState][eFsmNewEvent])(sPeripheries);
        SendStateMsg(eFsmCurrentState);
    }
    else
    {
        //TODO Maybe this could also trigger and abort???
    }


    return;
}

// Private helper function implementations
//*****************************************

// Anounce on UART that it has moved to a state
void SendStateMsg(eFsmState state)
{
    // Needs to be a static becuase the memory is deallocated when this function is removed from the stack before DMA has finished moving it
    static char msgBuff[MSGBUFF_SIZE];
    int n = 0;

    n = sprintf(msgBuff, "State: %s\r\n", eFsmStateNames[state]);
    if (n <= 0)
    {
        // Somthing went wrong BUT DONT CRASH
    }
    else
    {
        if (HAL_UART_Transmit_DMA(&huart2, (uint8_t *)msgBuff, n) != HAL_OK)
        {
        }
    }

    return;
}

