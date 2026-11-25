#include "motor.h"

// MOTOR CONTROL VARIABLES
volatile long pos_x = 0, pos_y = 0;                  // position en pas (signée)
volatile signed char dir_x = 1, dir_y = 1;           // +1 avant, -1 arrière
volatile unsigned long timer_compt = 0;
volatile unsigned long abs_steps_x = 0, abs_steps_y = 0;   // pas restants
volatile unsigned char x_pending = 0;                // X en attente (après Y)
volatile unsigned long x_pending_steps = 0;

void timer_on(){
    timer_compt = 0;
    TB0R = 0;
    TB0CTL |= MC__UP;
}
void timer_off(){
    TB0CTL |= MC__STOP;
    TB0R = 0;
    timer_compt = 0;
}

void motor_timer_init(void)
{
    TB0CTL |= TBCLR;
    TB0CTL |= MC__STOP;
    TB0CTL |= TBSSEL__SMCLK;        // SMCLK (1MHz)
    TB0CCR0 = 1000;                 // Speed = 37.5 mm/s

    TB0CCTL0 |= CCIE;
    TB0CCTL0 &= ~CCIFG;
}

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
    if(flagX){
        GPIO_setOutputHighOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   // en arriere
        GPIO_setOutputHighOnPin(DIR_MX2_PORT, DIR_MX2_PIN);   // en arriere
        dir_x = -1;
        enableMotorsX(1);
    }
    if(flagY){
        GPIO_setOutputLowOnPin(DIR_MY_PORT, DIR_MY_PIN);      // en arriere
        dir_y = -1;
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

void enableMotorsX(int enable){ enableMotors(enable, -1); }
void enableMotorsY(int enable){ enableMotors(-1, enable); }

long mmToStep(float mm) {
    return (long)(mm * STEP_BY_TOUR / MM_BY_TOUR);
}

float stepToMm(long step) {
    return (float)step * MM_BY_TOUR / STEP_BY_TOUR;
}

float getPosX_mm(void){
    long s;
    __disable_interrupt(); s = pos_x; __enable_interrupt();
    return stepToMm(s);
}

float getPosY_mm(void){
    long s;
    __disable_interrupt(); s = pos_y; __enable_interrupt();
    return stepToMm(s);
}