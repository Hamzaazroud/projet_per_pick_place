/*
 * uart.c
 *
 *  Created on: 3 déc. 2025
 *      Author: Douae CHOUBRI
 */
#include "uart.h"

char TX_Buffer[TX_BUFFER_SIZE];
char RX_Buffer[RX_BUFFER_SIZE];
volatile unsigned int tx_index_A0 = 0, tx_index_A1 = 0;
unsigned int tx_length=0;

void uart_init(){
    #ifdef USE_UART1
        // Configure UART A1 pins (Tx: 4.3 - Rx: 4.2)
        GPIO_setAsPeripheralModuleFunctionInputPin(UART_A1_PORT, UART_A1_TX_PIN | UART_A1_RX_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    #endif
    #ifdef USE_UART0
        // Configure UART A0 pins (Tx: 1.7 - Rx: 1.6)
        GPIO_setAsPeripheralModuleFunctionInputPin(UART_A0_PORT, UART_A0_TX_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
        GPIO_setAsPeripheralModuleFunctionInputPin(UART_A0_PORT, UART_A0_RX_PIN, GPIO_PRIMARY_MODULE_FUNCTION);

        // Configure UART A0 pins direction
        P1DIR |= BIT7;
        P1DIR &= ~BIT6;
    #endif

    EUSCI_A_UART_initParam param = {0};

    param.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK; //SMCLK
    param.clockPrescalar = 6; // Prescaler
    param.firstModReg = 8; // Set Low Freq
    param.secondModReg = 17; // Set Modulation
    param.parity = EUSCI_A_UART_NO_PARITY; // No parity
    param.msborLsbFirst = EUSCI_A_UART_LSB_FIRST; // LSB First
    param.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT; // 1 stop bit
    param.uartMode = EUSCI_A_UART_MODE;
    param.overSampling = 1;

    #ifdef USE_UART0
        if (EUSCI_A_UART_init(EUSCI_A0_BASE, &param) != STATUS_SUCCESS)
            {
                // Init failed, stop the program and blink Red Led
                while (1)
                {
                    GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
                    __delay_cycles(100000);
                }
            }
    #endif
    #ifdef USE_UART1
        if (EUSCI_A_UART_init(EUSCI_A1_BASE, &param) != STATUS_SUCCESS)
        {
            // Init failed, stop the program and blink Red Led
            while (1)
            {
                GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
                __delay_cycles(100000);
            }
        }
    #endif

    // Enable UART module
    #ifdef USE_UART1
        EUSCI_A_UART_enable(EUSCI_A1_BASE); // Take UART A1 out of software reset.
    #endif
    #ifdef USE_UART0
        EUSCI_A_UART_enable(EUSCI_A0_BASE); // Take UART A1 out of software reset.
    #endif

    // Enable IRQs
    #ifdef USE_UART1
        EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        EUSCI_A_UART_enableInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
    #endif
    #ifdef USE_UART0
        EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        EUSCI_A_UART_enableInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
    #endif

    // Clear IRQs
    #ifdef USE_UART1
        EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        EUSCI_A_UART_clearInterrupt(EUSCI_A1_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
    #endif
    #ifdef USE_UART0
        EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
        EUSCI_A_UART_clearInterrupt(EUSCI_A0_BASE, EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT);
    #endif
}

void uart_put_string(char *string){
    // Clear TX_Buffer
    memset(TX_Buffer, 0, TX_BUFFER_SIZE);

    // Copy the string into the TX_Buffer
    strcpy(TX_Buffer, string);

    // Update length
    tx_length = strlen(TX_Buffer);

    // Start transmitting characters
    // The ISR will take care of the following characters
#ifdef USE_UART1
    tx_index_A1 = 0;
    EUSCI_A_UART_transmitData(EUSCI_A1_BASE, TX_Buffer[tx_index_A1++]);
#endif
#ifdef USE_UART0
    tx_index_A0 = 0;
    EUSCI_A_UART_transmitData(EUSCI_A0_BASE, TX_Buffer[tx_index_A0++]);
#endif
}

