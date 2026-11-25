#include "command.h"
#include "math.h" 

Robot robot = {0, 0, MM_BY_TOUR * TIMER_FREQ / (2*STEP_BY_TOUR*TB0CCR0), 0};
static const position tab_pos[] = {
    { PIECE_POSITION_X,         PIECE_POSITION_Y,             100 },
    { BOX_POSITION_X,           BOX_POSITION_Y,              14 },
    { BOX_POSITION_X,           BOX_POSITION_Y - DELTA,       14 },
    { BOX_POSITION_X,           BOX_POSITION_Y - 2 * DELTA,   14 },
    { BOX_POSITION_X - DELTA,   BOX_POSITION_Y,               14 },
    { BOX_POSITION_X - DELTA,   BOX_POSITION_Y - DELTA,       14 },
    { BOX_POSITION_X - DELTA,   BOX_POSITION_Y - 2 * DELTA,   14 },
};



void set_position_command( Robot *robot) {
  

    // Calculate steps to move for X and Y axis based on the desired position
    int delat_x ; 
    int delata_y ; 
    int steps_x = 0;
    int steps_y = 0;

    steps_x = mmToStep(robot->x);
    steps_y = mmToStep(robot->y); 
    delat_x =   pos_x- steps_x ; 
    delata_y =  pos_y-steps_y ; 

    // Set Steppers Direction
    if(delat_x< 0) {
        GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en avant
        GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en avant
    } else {
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en arrire
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en arrire
    }

    if(delata_y < 0) {
        GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN);   // en avant
    } else {
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);   // en arrire
    }

    abs_steps_x = delat_x >= 0 ? delat_x : -delat_x;
    abs_steps_y = delata_y>= 0 ? delata_y : -delata_y;

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
    abs_steps_x = 4000;
    abs_steps_y = 4000;

    enableMotors(1, 1);
    timer_on();
     
}

void get_command(const char *command) {

    switch(command[0]){
        case 'P': // Set Position Command
             sscanf(command, "P%d-%d", &(robot.x), &(robot.y));
            set_position_command(&robot);
            break;
        case 'V': // Set Speed Command
            set_speed_command(command, &robot);
            break;
        case 'H': // Go to Home Position
            home_position_command(command, &robot);
            break;
        case 'S' : 
             enableMotors(0, 0) ; 
             break; 
        case 'J' :
             move(command) ;
            break;
        case 'R': 
            auto_motor_position(&robot) ;
            break; 
        default:
            uart_put_string("ERROR - Invalid Command");
            break;
        
    }
} 

void move(const char *Command) {
int indice = 0 ; 


sscanf(Command, "J%d", &indice) ; 
 
 switch (indice) {
case 1 : 

    GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   
    GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
    abs_steps_x =2000 ; 
    abs_steps_y =0 ;
    enableMotors(1,0);
   
    timer_on();
    break ; 
case 2 :

    GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   
    GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
    abs_steps_x =2000 ; 
    enableMotors(1,0);
    timer_on();
    break ; 
case 3  : 

 GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN); 
 abs_steps_y = 2000 ; 
 timer_on() ;
 enableMotors(0,1 );
 break ;

 case 4 : 

  GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN); 
   abs_steps_y = 2000 ; 
 timer_on() ;
 enableMotors(0,1 );
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




void auto_motor_position(Robot *robot) {


    int dx = 0, dy = 0;
    if (!in_circle(robot, tab_pos,8 , &dx, &dy)) {
        return; 
    } 

robot->x = dx ; 
robot-> y = dy ;
set_position_command( robot);
}


bool in_circle(Robot *robot, const position *tab_pos, int n, int *delta_x, int *delta_y) {
    for (int i = 0; i < n; i++) {
        int dx = tab_pos[i].x - robot->x;
        int dy = tab_pos[i].y - robot->y;

        if (dx * dx + dy * dy < tab_pos[i].rayon * tab_pos[i].rayon) {
            *delta_x = dx;
            *delta_y = dy;
            return true;
        }
    }
    return false;
}






