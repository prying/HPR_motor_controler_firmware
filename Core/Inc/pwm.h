/*
 * pwm.h
 *
 *  Created on: 2 Jul 2021
 *      Author: flynn
 * 
 * These functions require the timers to be set up correctly and are just a wrapper to hide the inner workings
 * to make the setting of the servos a cleaner task
 */

#ifndef INC_PWM_H_
#define INC_PWM_H_

// Starts the PWM siginal generation (this is just a function wrapper)
void PWM1_start();

// Sets the angle of PWM 1 (for servo motors). Angle is not the same as duty cycle!
// Takes in a value between 0 and 360 degrees and will try an move the servo the the aproxomate angle.
void PWM1_setPos(float angle);

// Returns the angle in degrees of PWM 1, Not the duty cycle.
float PWM1_getPos();

#endif /* INC_PWM_H_ */
