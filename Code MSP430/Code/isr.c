/*
 * isr.c
 *
 *  Created on: 25 nov 2026
 *      Author: Hamza Azroud
 */
#include "isr.h"
#define F_CP 1000000UL
#define delay_ms(x) __delay_cycles((F_CP/1000)*x)

volatile unsigned int rx_index=0, command_flag=0;

void send_position(void);

static int long_to_str(long v, char *buf){
    char tmp[12];
    int i = 0, n = 0;
    if(v < 0){ buf[n++] = '-'; v = -v; }
    if(v == 0) tmp[i++] = '0';
    while(v > 0){ tmp[i++] = '0' + (v % 10); v /= 10; }
    while(i > 0) buf[n++] = tmp[--i];
    return n;
}

#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_TB0_CCR0_MOTORX(void){

    if(timer_compt < (2*abs_steps_x)){
        GPIO_toggleOutputOnPin(STEP_MX1_PORT, STEP_MX1_PIN);
        GPIO_toggleOutputOnPin(STEP_MX2_PORT, STEP_MX2_PIN);
        if(timer_compt & 1) pos_x += dir_x;
    } else {
        abs_steps_x = 0;
    }

    if(timer_compt < (2*abs_steps_y)){
        GPIO_toggleOutputOnPin(STEP_MY_PORT, STEP_MY_PIN);
        if(timer_compt & 1) pos_y += dir_y;
    } else {
        abs_steps_y = 0;
    }

    if(timer_compt < (2*abs_steps_x) || timer_compt < (2*abs_steps_y)){
        timer_compt++;
    } else {
        TB0CTL |= MC__STOP;
        TB0R = 0;
        timer_compt = 0;

        if(x_pending){                   
            x_pending = 0;
            abs_steps_x = x_pending_steps;
            enableMotors(1, 0);        
            timer_on();
        }
    }
}

// Envoi bloquant sur UART1 (n'utilise pas l'interruption TX)
void send_position(void){
    long x, y;
    unsigned int n = 0, i;

    x = (long)(getPosX_mm() * 100);
    y = (long)(getPosY_mm() * 100);

    TX_Buffer[n++] = MSG_START_CHAR;
    TX_Buffer[n++] = 'P'; TX_Buffer[n++] = 'O'; TX_Buffer[n++] = 'S'; TX_Buffer[n++] = ',';
    n += long_to_str(x, (char*)&TX_Buffer[n]);
    TX_Buffer[n++] = ',';
    n += long_to_str(y, (char*)&TX_Buffer[n]);
    TX_Buffer[n++] = MSG_END_CHAR;
    TX_Buffer[n++] = '\r';
    TX_Buffer[n++] = '\n';

    for(i = 0; i < n; i++){
        while(!(UCA1IFG & UCTXIFG));
        UCA1TXBUF = TX_Buffer[i];
    }
}

#pragma vector = PORT1_VECTOR
__interrupt void ISR_CAPTEURS_FDC(void){
    
    if((P1IFG & FCX_PIN)){
        enableMotors(0,-1);
        GPIO_setOutputLowOnPin(DIR_MX1_PORT, DIR_MX1_PIN);   
        GPIO_setOutputLowOnPin(DIR_MX2_PORT, DIR_MX2_PIN);
        dir_x = +1;
        x_pending = 0;
        pos_x = 0;  robot.x = 0;
        abs_steps_x = 25;
        enableMotorsX(1);
        timer_on();
    }
   if((P1IFG & FCY_PIN)){          
    enableMotors(-1,0);
    GPIO_setOutputHighOnPin(DIR_MY_PORT, DIR_MY_PIN);    
    dir_y = +1;
    x_pending = 0;
    pos_y = 0;  robot.y = 0;
    abs_steps_y = 50;
    enableMotorsY(1);
    timer_on();
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

            // ECHO (test) : renvoie le caractère reçu
            while(!(UCA1IFG & UCTXIFG));
            UCA1TXBUF = RX_received_char;

            if(RX_received_char == 't'){
                GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
                send_position();
            }

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
                        rx_index = 0;
                        command_flag = 0;
                    }
                    break;
            }
            break;

        case USCI_UART_UCTXCPTIFG:
            if(tx_index_A1<tx_length){
                EUSCI_A_UART_transmitData(EUSCI_A1_BASE, TX_Buffer[tx_index_A1++]);
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
                        rx_index = 0;
                        command_flag = 0;
                    }
                    break;
            }

            if(RX_received_char == 't'){
                GPIO_toggleOutputOnPin(LED_RED_PORT, LED_RED_PIN);
            }
            break;

        case USCI_UART_UCTXCPTIFG:
            if(tx_index_A0<tx_length){
                EUSCI_A_UART_transmitData(EUSCI_A0_BASE, TX_Buffer[tx_index_A0++]);
            } else {
                tx_index_A0 = 0;
            }
            break;
        default:
            break;
    }
}
#endif