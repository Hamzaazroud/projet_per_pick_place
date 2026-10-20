#include "command.h"

Robot robot = {0, 0, MM_BY_TOUR * TIMER_FREQ / (2*STEP_BY_TOUR*TB0CCR0), 0};

void set_position_command(const char *command, Robot *robot) {
    sscanf(command, "P%d-%d", &(robot->x), &(robot->y));

    // Calculate steps to move for X and Y axis based on the desired position
    int steps_x = 0;
    int steps_y = 0;

    steps_x = mmToStep(robot->x);
    steps_y = mmToStep(robot->y);

    // Set Steppers Direction
    if(steps_x > 0) {
        GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en avant
        GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en avant
    } else {
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en arrire
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en arrire
    }

    if(steps_y > 0) {
        GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN);   // en avant
    } else {
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);   // en arrire
    }

    abs_steps_x = steps_x >= 0 ? steps_x : -steps_x;
    abs_steps_y = steps_y >= 0 ? steps_y : -steps_y;

    enableMotors(1, 1);
    timer_on();

    uart_put_string("<P1>"); // ACK message from MSP430 board to PLC
}

void set_speed_command(const char *command, Robot *robot){
    int commanded_speed = 0;
    int mode_sim_alt = 0;
    int mode_rel_abs = 0;

    sscanf(command, "V%d-%d-%d", &commanded_speed, &mode_sim_alt, &mode_rel_abs);

    if (commanded_speed <= MAX_SPEED && commanded_speed >= MIN_SPEED) {
        robot->v = commanded_speed;
        // → bit 1 = simultaneous/alternative, bit 0 = relative/absolute
        robot->mode = (mode_sim_alt << 1) | mode_rel_abs;

        // Stop and reconfigure timer
        timer_off();
        TB0CCR0 = (MM_BY_TOUR * TIMER_FREQ) / (2 * STEP_BY_TOUR * robot->v);

        /* À ajouter: Gestion du comportement selon le mode :
        if (mode_sim_alt == 0) {
            // Simultaneous mode
            // -> X et Y bougent ensemble
        } else {
            // Alternative mode
            // -> X puis Y
        }

        if (mode_rel_abs == 0) {
            // Relative mode
            // -> Déplacement par rapport à la position actuelle
        } else {
            // Absolute mode
            // -> Déplacement vers la position donnée
        }*/

        uart_put_string("<S1>"); // ACK OK
    } else {
        uart_put_string("<S0>"); // Erreur : vitesse hors limites
    }
}

void home_position_command(const char *command, Robot *robot){
    HomePositionXY(1,1);
    abs_steps_x = 1000;
    abs_steps_y = 1000;

    enableMotors(1, 1);
    timer_on();
}

void get_command(const char *command) {
    switch(command[0]){
        case 'P': // Set Position Command
            set_position_command(command, &robot);
            break;
        case 'V': // Set Speed Command
            //set_speed_command(command, &robot);
            break;
        case 'H': // Go to Home Position
            home_position_command(command, &robot);
            break;
        default:
            uart_put_string("ERROR - Invalid Command");
            break;
    }
}

void auto_mode_command_test(const char *command, Robot *robot){
    HomePositionXY(1,1);
    abs_steps_x = 1000;
    abs_steps_y = 1000;

    enableMotors(1, 1);
}
