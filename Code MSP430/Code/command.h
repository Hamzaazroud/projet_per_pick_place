/*
 * command.h
 *
 *  Created on: 3 dec. 2025
 *      Author: Douae CHOUBRI
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <stdio.h>
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include <math.h>
#include "motor.h"
#include <stdlib.h>
#include "constants.h"
#include "Board.h"
#include "uart.h"

typedef struct {
    int x;     // Position X (mm)
    int y;     // Position Y (mm)
    int v;     // Vitesse (mm/s)
    char mode; // Mode de mouvement
    
} Robot;

typedef struct {

 int x  ; 
 int y ; 
 int rayon ; 

} position  ;

extern Robot robot;
extern int pos_x1 ,pos_x2 , pos_x3 , pos_y1 ,pos_y2 ; 

// PROTOTYPES
void auto_motor_position() ;
void set_position_command(const char *command, Robot *robot);
void set_speed_command(const char *command, Robot *robot);
void home_position_command(const char *command, Robot *robot);
void get_command(const char *command);
void auto_mode_command_test(const char *command, Robot *robot);
void move(const char *command) ;
bool in_circle(Robot *robot, position *tab_pos, int n, int *delta_x, int *delta_y);

#endif /* COMMAND_H_ */
