/*
 * motor_control.h
 *
 *  Created on: Apr 28, 2023
 *      Author: bill
 */

#ifndef MOTOR_CONTROL_H_
#define MOTOR_CONTROL_H_
#include <msp430.h>

void setup_drivers();

void setup_bluetooth();

void standby();

void standby_off();

void brake_left();

void brake_right();

void left_on();

void right_on();

#endif /* MOTOR_CONTROL_H_ */
