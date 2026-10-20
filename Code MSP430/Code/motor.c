#include "motor.h"

// MOTOR CONTROL VARIABLES
volatile unsigned int timer_compt = 0;
int abs_steps_x=0, abs_steps_y=0;

void timer_on(){
    // Activate Timer
    timer_compt = 0;
    TB0R = 0;
    TB0CTL |= MC__UP;
}
void timer_off(){
    TB0CTL |= MC__STOP; // Stop Timer
    TB0R = 0; // Reset Timer Counter
    timer_compt = 0;
}

void motor_timer_init(void)
{
    // Setup Timers
    TB0CTL |= TBCLR;                // Reset Timer TB0
    TB0CTL |= MC__STOP;             // Put Timer in Stop mode (We'll put it in UP mode afterwards)
    TB0CTL |= TBSSEL__SMCLK;        // Choose SMCLK (1MHz)
    TB0CCR0 = 1000;                 // Speed = 37.5 mm/s

    TB0CCTL0 |= CCIE;               // Enable Timer Compares Interrupt
    TB0CCTL0 &= ~CCIFG;             // Clear Flag
}


// Configure GPIOs
void motor_gpio_init(){
    // Motor axe y
    GPIO_setAsOutputPin(STEP_MY_PORT, STEP_MY_PIN);
    GPIO_setAsOutputPin(DIR_MY_PORT, DIR_MY_PIN);
    GPIO_setAsOutputPin(EN_MY_PORT, EN_MY_PIN);
    // Motor axe x - 1
    GPIO_setAsOutputPin(STEP_MX1_PORT, STEP_MX1_PIN);
    GPIO_setAsOutputPin(DIR_MX1_PORT, DIR_MX1_PIN);
    GPIO_setAsOutputPin(EN_MX1_PORT, EN_MX1_PIN);

    // Motor axe x - 2
    GPIO_setAsOutputPin(STEP_MX2_PORT, STEP_MX2_PIN);
    GPIO_setAsOutputPin(DIR_MX2_PORT, DIR_MX2_PIN);
    GPIO_setAsOutputPin(EN_MX2_PORT, EN_MX2_PIN);
}

void HomePositionXY(int flagX, int flagY) {
    //  cette fonction permet de mettre les moteur x et y en position initiale en utilisant les capteur de fin de course
    if(flagX){
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en arri�re
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en arri�re
        enableMotorsX(1);
    }
    if(flagY){
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);   // en arri�re
        enableMotorsY(1);
    }
}

void enableMotors(int flagX, int flagY)
{
    // Motors activate on low
    switch(flagX) {
        case 1:
            GPIO_setOutputLowOnPin(EN_MX1_PORT, EN_MX1_PIN);
            GPIO_setOutputLowOnPin(EN_MX2_PORT, EN_MX2_PIN);
            break;
        case 0:
            GPIO_setOutputHighOnPin(EN_MX1_PORT, EN_MX1_PIN);
            GPIO_setOutputHighOnPin(EN_MX2_PORT, EN_MX2_PIN);
            break;
        default:
            break;
    }

    switch(flagY) {
            case 1:
                GPIO_setOutputLowOnPin(EN_MY_PORT, EN_MY_PIN);
                break;
            case 0:
                GPIO_setOutputHighOnPin(EN_MY_PORT, EN_MY_PIN);
                break;
            default:
                break;
        }
}

void enableMotorsX(int enable){
    // Motors activate on low
    enableMotors(enable, -1);
}

void enableMotorsY(int enable){
    // Motors activate on low
    enableMotors(-1, enable);
}


int mmToStep(float mm) {
    int step=mm*STEP_BY_TOUR/MM_BY_TOUR;
    return step;
}

float stepToMm(int step) {
    float mm=step*MM_BY_TOUR/STEP_BY_TOUR;
    return mm;
}


