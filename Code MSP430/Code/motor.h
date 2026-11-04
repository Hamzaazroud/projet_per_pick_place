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
#include <stdlib.h>
#include "constants.h"

// MOTOR CONTROL VARIABLES
extern volatile unsigned long timer_compt;
extern volatile unsigned long abs_steps_x, abs_steps_y;   // pas restants du mouvement en cours
extern volatile long pos_x, pos_y;                         // position en pas (signee)
extern volatile signed char dir_x, dir_y;                  // +1 avant, -1 arriere

// PROTOTYPES
void timer_on(void);
void timer_off(void);
void motor_gpio_init(void);
void motor_timer_init(void);

void HomePositionXY(int flagX, int flagY);
void enableMotors(int flagX, int flagY);
void enableMotorsX(int enable);
void enableMotorsY(int enable);

long  mmToStep(float mm);
float stepToMm(long step);
float getPosY_mm(void) ;
float getPosX_mm(void) ;

#endif /* MOTOR_H_ */