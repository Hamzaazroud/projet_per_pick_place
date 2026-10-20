/*
 * motor.h
 *
 *  Created on: 3 dec. 2025
 *      Author: Douae CHOUBRI
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include <math.h>
#include <motor.h>
#include <stdlib.h>
#include "constants.h"

// MOTOR CONTROL VARIABLES
extern volatile unsigned int timer_compt;
extern int abs_steps_x;
extern int abs_steps_y;

// PROTOTYPES
void timer_on(void);
void timer_off(void);
void motor_gpio_init(void);
void motor_timer_init(void);

void HomePositionXY(int flagX, int flagY);
void enableMotors(int flagX, int flagY);
void enableMotorsX(int enable);
void enableMotorsY(int enable);

int mmToStep(float mm);
int stepToMm(float step);

#endif /* MOTOR_H_ */
