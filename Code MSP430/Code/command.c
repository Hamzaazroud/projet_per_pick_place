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

void set_position_command(Robot *robot) {
    long delta_x, delta_y;
    float posy;

    // Mouvement déjà en cours : on ignore
    if(abs_steps_x != 0 || abs_steps_y != 0 || x_pending) return;

    delta_x = mmToStep((float)robot->x) - pos_x;
    delta_y = mmToStep((float)robot->y) - pos_y;

    // Axe X : low = avant (+), high = arrière (-)
    if(delta_x >= 0){
        GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);
        GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
        dir_x = +1;
    } else {
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
        dir_x = -1;
    }

    // Axe Y : high = avant (+), low = arrière (-)
    if(delta_y >= 0){
        GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN);
        dir_y = +1;
    } else {
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);
        dir_y = -1;
    }

    abs_steps_x = (delta_x >= 0) ? delta_x : -delta_x;
    abs_steps_y = (delta_y >= 0) ? delta_y : -delta_y;

    posy = getPosY_mm();
    x_pending = 0;

    if(abs_steps_x != 0 || abs_steps_y != 0){
        if(delta_y < 0 && abs_steps_x != 0 && abs_steps_y != 0
           && posy >= (BOX_POSITION_Y - 3*DELTA)){
            // Zone du box : Y d'abord, puis X
            x_pending_steps = abs_steps_x;
            abs_steps_x = 0;
            x_pending = 1;
            enableMotors(0, 1);
            timer_on();
        } else {
            // Cas normal : X et Y ensemble
            enableMotors(1, 1);
            timer_on();
        }
    }

    uart_put_string("<P1>");
}

void set_speed_command(const char *command, Robot *robot){
    int commanded_speed = 0;
    int mode_sim_alt = 0;
    int mode_rel_abs = 0;

    sscanf(command, "V%d-%d-%d", &commanded_speed, &mode_sim_alt, &mode_rel_abs);

    if (commanded_speed <= MAX_SPEED && commanded_speed >= MIN_SPEED) {
        robot->v = commanded_speed;
        robot->mode = (mode_sim_alt << 1) | mode_rel_abs;

        timer_off();
        TB0CCR0 = (MM_BY_TOUR * TIMER_FREQ) / (2 * STEP_BY_TOUR * robot->v);

        uart_put_string("<S1>"); // ACK OK
    } else {
        uart_put_string("<S0>"); // Erreur : vitesse hors limites
    }
}

void home_position_command(const char *command, Robot *robot){
    x_pending = 0;
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
        case 'S': // Stop
            enableMotors(0, 0);
            timer_off();
            abs_steps_x = 0;
            abs_steps_y = 0;
            x_pending = 0;
            break;
        case 'J':
            move(command);
            break;
        case 'R':
            auto_motor_position(&robot);
            break;
        default:
            uart_put_string("ERROR - Invalid Command");
            break;
    }
}

void move(const char *Command) {
    int indice = 0;

    sscanf(Command, "J%d", &indice);

    switch (indice) {
    case 1:
        GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);
        GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
        dir_x = +1;
        abs_steps_x = 2000;
        abs_steps_y = 0;
        enableMotors(1, 0);
        timer_on();
        break;
    case 2:
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
        dir_x = -1;
        abs_steps_x = 2000;
        abs_steps_y = 0;
        enableMotors(1, 0);
        timer_on();
        break;
    case 3:
        GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN);
        dir_y = +1;
        abs_steps_x = 0;
        abs_steps_y = 2000;
        enableMotors(0, 1);
        timer_on();
        break;
    case 4:
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);
        dir_y = -1;
        abs_steps_x = 0;
        abs_steps_y = 2000;
        enableMotors(0, 1);
        timer_on();
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
    int tx = 0, ty = 0;
    if (!in_circle(robot, tab_pos, 7, &tx, &ty)) {
        return;
    }
    robot->x = tx;      // cible absolue (mm)
    robot->y = ty;
    set_position_command(robot);
}

bool in_circle(Robot *robot, const position *tab_pos, int n, int *tx, int *ty) {
    int i;
    int cx = (int)getPosX_mm();     // position réelle
    int cy = (int)getPosY_mm();
    for (i = 0; i < n; i++) {
        int dx = tab_pos[i].x - cx;
        int dy = tab_pos[i].y - cy;
        if ((long)dx*dx + (long)dy*dy < (long)tab_pos[i].rayon * tab_pos[i].rayon) {
            *tx = tab_pos[i].x;
            *ty = tab_pos[i].y;
            return true;
        }
    }
    return false;
}