#include "motor.h"
#include <msp430.h>
#include <stdio.h>
#include <string.h>
#include "driverlib.h"
#include "Board.h"
#include "constants.h"
#include "uart.h"
#include "command.h"


void main(void)
{
    // Stop the watchdog timer
    WDT_A_hold(WDT_A_BASE);

    //Set ACLK = REFOCLK with clock divider of 1
    CS_initClockSignal(CS_ACLK,CS_REFOCLK_SELECT,CS_CLOCK_DIVIDER_1);
    //Set SMCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_SMCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);
    //Set MCLK = DCO with frequency divider of 1
    CS_initClockSignal(CS_MCLK,CS_DCOCLKDIV_SELECT,CS_CLOCK_DIVIDER_1);

    motor_gpio_init();
    motor_timer_init();

    // Led for debug
    GPIO_setAsOutputPin(LED_RED_PORT, LED_RED_PIN);
    GPIO_setAsOutputPin(LED_GREEN_PORT, LED_GREEN_PIN);

    // Configure Capteurs de Fin de course.
    GPIO_setAsInputPinWithPullUpResistor(FCY_PORT, FCY_PIN);
    GPIO_setAsInputPinWithPullUpResistor(FCX_PORT, FCX_PIN);

    uart_init();
    /*
     * Disable the GPIO power-on default high-impedance mode
     * to activate previously configured port settings
     */
    PMM_unlockLPM5(); // Turn On I/O

    // Set the outputs GPIO en LOW
    GPIO_setOutputLowOnPin(LED_GREEN_PORT, LED_GREEN_PIN);

    enableMotors(1, 1);

    GPIO_enableInterrupt(FCX_PORT, FCX_PIN);
    GPIO_enableInterrupt(FCY_PORT, FCY_PIN);

    GPIO_clearInterrupt(FCX_PORT, FCX_PIN);
    GPIO_clearInterrupt(FCY_PORT, FCY_PIN);

    // Select Sensivity (H-to-L) for GPIO pins (Capteurs de fin de course)
    GPIO_selectInterruptEdge(FCX_PORT, FCX_PIN, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_selectInterruptEdge(FCY_PORT, FCY_PIN, GPIO_HIGH_TO_LOW_TRANSITION);

    GPIO_setOutputHighOnPin(LED_GREEN_PORT, LED_GREEN_PIN);
    GPIO_setOutputLowOnPin(LED_RED_PORT, LED_RED_PIN);

    // Enable global interrupt
    __enable_interrupt();

    while (1)
    {
        // Your main code here
    }
}
