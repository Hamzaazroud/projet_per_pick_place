/*
 * isr.c
 *
 *  Created on: 3 déc. 2025
 *      Author: Douae CHOUBRI
 */
#include "isr.h"

volatile unsigned int rx_index=0, command_flag=0;

//-----------------------------------------------------------------------------
//ISRs
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0_MOTORX(void){
    //GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);

    if(timer_compt<(2*abs_steps_x)){
        GPIO_toggleOutputOnPin(STEP_MX1_PORT, STEP_MX1_PIN);
        GPIO_toggleOutputOnPin(STEP_MX2_PORT, STEP_MX2_PIN);
    } else {
        abs_steps_x = 0;
    }

    if(timer_compt<(2*abs_steps_y)){
        GPIO_toggleOutputOnPin(STEP_MY_PORT, STEP_MY_PIN);
    } else {
        abs_steps_y = 0;
    }

    if(timer_compt<(2*abs_steps_x) || timer_compt<(2*abs_steps_y)){
        timer_compt++;
    } else {
        //Stop and reset Timer counter
        TB0CTL |= MC__STOP; // Stop Timer
        TB0R = 0; // Reset Timer Counter
        timer_compt = 0;
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void ISR_CAPTEURS_FDC(void){
    // STOP motors
    if(P1IFG & FCX_PIN){
        enableMotorsX(0);
        robot.x = 0; // Home Position (X)
    }
    if(P1IFG & FCY_PIN){
        enableMotorsY(0);
        robot.y = 0; // Home Position (Y)
    }
    P1IFG &= ~(FCX_PIN | FCY_PIN);
}

#ifdef USE_UART1
#pragma vector = EUSCI_A1_VECTOR
__interrupt void ISR_EUSCI_A1(void) {
    switch(__even_in_range(UCA1IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE:
            break;

        case USCI_UART_UCRXIFG: // Receive interruption
            char RX_received_char;
            RX_received_char = UCA1RXBUF;

            if(rx_index==0){
                memset(RX_Buffer, 0, RX_BUFFER_SIZE);
            }

            switch(RX_received_char) {
                case MSG_START_CHAR:
                    command_flag = 1;
                    rx_index=0;
                    break;
                case MSG_END_CHAR:
                case '\n':
                case '\r':
                    if(command_flag){
                        RX_Buffer[rx_index]='\0';
                        get_command(RX_Buffer);
                        command_flag = 0;
                        rx_index = 0;
                    }
                    break;
                case ' ':
                    break;
                default:
                    if(rx_index < RX_BUFFER_SIZE-1){
                        RX_Buffer[rx_index++] = RX_received_char;
                    } else {
                        // Handle buffer overflow error...
                        rx_index = 0; // Reset Index
                        command_flag = 0; // Reset command detected flag
                    }
                    break;
            }

            // UART TEST
            if(UCA1RXBUF=='t'){
                GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
            }
            break;

        case USCI_UART_UCTXCPTIFG:
            if(tx_index_A1<tx_length){
                EUSCI_A_UART_transmitData(EUSCI_A1_BASE, TX_Buffer[tx_index_A1++]); //put next char into UART A1 Tx Buffer
            } else {
                tx_index_A1 = 0;
            }
            break;
        default:
            break;

    }


}
#endif
#ifdef USE_UART0
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void){
    switch(__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
    {
        case USCI_NONE:
            break;

        case USCI_UART_UCRXIFG: // Receive interruption
            char RX_received_char;
            RX_received_char = UCA0RXBUF;

            if(rx_index==0){
                memset(RX_Buffer, 0, RX_BUFFER_SIZE);
            }

            switch(RX_received_char) {
                case MSG_START_CHAR:
                    command_flag = 1;
                    rx_index=0;
                    break;
                case MSG_END_CHAR:
                case '\n':
                case '\r':
                    if(command_flag){
                        RX_Buffer[rx_index]='\0';
                        get_command(RX_Buffer);
                        command_flag = 0;
                        rx_index = 0;
                    }
                    break;
                case ' ':
                    break;
                default:
                    if(rx_index < RX_BUFFER_SIZE-1){
                        RX_Buffer[rx_index++] = RX_received_char;
                    } else {
                        // Handle buffer overflow error...
                        rx_index = 0; // Reset Index
                        command_flag = 0; // Reset command detected flag
                    }
                    break;
            }

            // UART TEST
            if(UCA1RXBUF=='t'){
                GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
            }
            break;

        case USCI_UART_UCTXCPTIFG:
            if(tx_index_A0<tx_length){
                EUSCI_A_UART_transmitData(EUSCI_A0_BASE, TX_Buffer[tx_index_A0++]); //put next char into UART A1 Tx Buffer
            } else {
                tx_index_A0 = 0;
            }
            break;
        default:
            break;

    }


}

#endif



