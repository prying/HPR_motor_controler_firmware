/*
 * pwm.c
 *
 *  Created on: 20 Jun 2021
 *      Author: flynn
 */

#include "tim.h"
#include "main.h"

#define CLK_SPEED 16E6
#define MILLISECOND 1E-3
#define SERVO_RANGE 180

// TODO catch the HAL_StatusTypeDef and check for failures 
// TODO figure out a good 'defualt' position for the servo
void PWM1_start()
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
}

// Sets the angle of PWM 1 (for servo motors). Angle is not the same as duty cycle!
void PWM1_setPos(float angle)
{
    // Determin counts per pulse
    float fPwm = CLK_SPEED/((htim2.Init.Prescaler + 1)*(htim2.Init.Period + 1));
    float cps = (htim2.Init.Period + 1)*fPwm;

    // Determin pulse length to get the desisered angle on the servo (servos 1ms 0, 2ms 180)
    // Round down
    int pulse = (int)(cps*MILLISECOND*(1 + angle/SERVO_RANGE));

    // There might be a HAL function for this but i cant find it
    TIM2->CCR1 = pulse;
}

// Returns the angle of PWM 1, Not the duty cycle.
float PWM1_getPos()
{
    int pulse = TIM2->CCR1;

    float fPwm = CLK_SPEED/((htim2.Init.Prescaler + 1)*(htim2.Init.Period + 1));
    float cps = (htim2.Init.Period + 1)*fPwm;

    pulse = pulse - cps*MILLISECOND;
    return pulse*SERVO_RANGE/(cps*MILLISECOND);
}