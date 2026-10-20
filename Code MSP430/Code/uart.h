/*
 * uart.h
 *
 *  Created on: 3 dec. 2025
 *      Author: Douae CHOUBRI
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "driverlib.h"
#include <msp430.h>
#include <string.h>
#include <math.h>
#include <motor.h>
#include <stdlib.h>
#include "constants.h"
#include "isr.h"

// UART Variables
extern char TX_Buffer[TX_BUFFER_SIZE];
extern char RX_Buffer[RX_BUFFER_SIZE];

extern volatile unsigned int tx_index_A0;
extern volatile unsigned int tx_index_A1;
extern unsigned int tx_length;

// PROTOTYPES
void uart_init(void);
void uart_put_string(char *string);


#endif /* UART_H_ */
