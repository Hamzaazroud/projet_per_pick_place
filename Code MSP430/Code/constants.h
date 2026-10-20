/*
 * constants.h
 *
 *  Created on: 22 nov. 2023
 *      Author: alexa
 */

#ifndef CONSTANTS_H_
#define CONSTANTS_H_


//---------------------------------------------------------------------------
//------------------CONSTANTS DEFINITION--------------------------------------
//---------------------------------------------------------------------------
#define RX_BUFFER_SIZE         20
#define TX_BUFFER_SIZE         32
#define MSG_START_CHAR         '<'
#define MSG_END_CHAR           '>'

#define USE_UART0              // PLC
//#define USE_UART1            // BACKCHANNEL USB

#define MAX_SPEED              100  // mm/s
#define MIN_SPEED              1    // mm/s
#define DEFAULT_SPEED          25   // mm/s

#define STEP_BY_TOUR        200                             // definit dans le datasheet du moteur
#define MM_BY_TOUR          15                              // cette valeur est defini en faisant une calibration du systeme
#define SECURITY_DISTANCE   80                              // quand les axes passe sur une fin de course il faut qu'il le relache dans le sens inverse. on utilise une distance de securit�

#define TIMER_FREQ          1000000

#define UART_A1_PORT        GPIO_PORT_P4
#define UART_A1_TX_PIN      GPIO_PIN3
#define UART_A1_RX_PIN      GPIO_PIN2

#define UART_A0_PORT        GPIO_PORT_P1
#define UART_A0_TX_PIN      GPIO_PIN7
#define UART_A0_RX_PIN      GPIO_PIN6

#define LED_RED_PORT        GPIO_PORT_P1
#define LED_RED_PIN         GPIO_PIN0

#define LED_GREEN_PORT      GPIO_PORT_P6
#define LED_GREEN_PIN       GPIO_PIN6

// Motor Y
#define STEP_MY_PORT        GPIO_PORT_P2
#define STEP_MY_PIN         GPIO_PIN1

#define DIR_MY_PORT         GPIO_PORT_P6
#define DIR_MY_PIN          GPIO_PIN0

#define EN_MY_PORT          GPIO_PORT_P6
#define EN_MY_PIN           GPIO_PIN1

// Motor X1
#define STEP_MX1_PORT       GPIO_PORT_P3
#define STEP_MX1_PIN        GPIO_PIN7

#define DIR_MX1_PORT        GPIO_PORT_P2
#define DIR_MX1_PIN         GPIO_PIN4

#define EN_MX1_PORT         GPIO_PORT_P3
#define EN_MX1_PIN          GPIO_PIN2

// Motor X2
#define STEP_MX2_PORT       GPIO_PORT_P6
#define STEP_MX2_PIN        GPIO_PIN2

#define DIR_MX2_PORT        GPIO_PORT_P6
#define DIR_MX2_PIN         GPIO_PIN3

#define EN_MX2_PORT         GPIO_PORT_P6
#define EN_MX2_PIN          GPIO_PIN4

#define FCY_PORT        GPIO_PORT_P1
#define FCY_PIN         GPIO_PIN4

#define FCX_PORT        GPIO_PORT_P1
#define FCX_PIN         GPIO_PIN5

#endif /* CONSTANTS_H_ */
