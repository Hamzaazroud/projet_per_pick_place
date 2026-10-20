/*
 * isr.h
 *
 *  Created on: 3 déc. 2025
 *      Author: Douae CHOUBRI
 */

#ifndef ISR_H_
#define ISR_H_

#include <stdint.h>
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include <math.h>
#include "motor.h"
#include <stdlib.h>
#include "constants.h"
#include "uart.h"
#include "command.h"

extern volatile unsigned int rx_index;
extern volatile unsigned int command_flag;

// PROTOTYPES
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0_MOTORX(void);

#pragma vector = PORT1_VECTOR
__interrupt void ISR_CAPTEURS_FDC(void);

#ifdef USE_UART1
#pragma vector = EUSCI_A1_VECTOR
__interrupt void ISR_EUSCI_A1(void);
#endif

#ifdef USE_UART0
#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void);
#endif


#endif /* ISR_H_ */
