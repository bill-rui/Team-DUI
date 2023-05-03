/*
 * motor_control.c
 *
 *  Created on: Apr 28, 2023
 *      Author: bill
 */

#include "motor_control.h"

void setup_drivers()
{
    // PWM outputs to 0
    P1DIR |= BIT1 | BIT2;
    P1SEL0 &= ~(BIT1 | BIT2);
    P1SEL1 &= ~(BIT1 | BIT2);
    P1OUT |= (BIT1 | BIT2);

    // Direction controls and standby to 0
    P2DIR |= BIT0 | BIT1 | BIT2 | BIT3 | BIT6 | BIT7;
    P2SEL0 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT6 | BIT7);
    P2SEL1 &= ~(BIT0 | BIT1 | BIT2 | BIT3 | BIT6 | BIT7);
    P2OUT  = 0x50;  // set to 01010000 for motors in same direction and standby low
}

void setup_bluetooth()
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    __bis_SR_register(SCG0);
    CSCTL3 |= SELREF__REFOCLK;
    CSCTL0 = 0;
    CSCTL1 &= ~(DCORSEL_7);
    CSCTL1 |= DCORSEL_3;
    CSCTL2 = FLLD_0 + 243;
    __delay_cycles(3);
    __bic_SR_register(SCG0);

    while(CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1));
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BR0 = 52;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF1;

    P1SEL1 &= ~BIT5 & ~BIT4;
    P1SEL0 |= BIT5 | BIT4;

    PM5CTL0 &= ~LOCKLPM5;

    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
}

void standby()
{
    P2OUT &= ~(BIT6 | BIT7);
}

void standby_off()
{
    P2OUT |= BIT6 | BIT7;
}

void brake_left()
{
    P2OUT &= ~(BIT0 | BIT1);
}

void brake_right()
{
    P2OUT &= ~(BIT2 | BIT3);
}

void left_on()
{
    P2OUT |= BIT0;
    P2OUT &= ~BIT1;
}

void right_on()
{
    P2OUT |= BIT3;
    P2OUT &= ~BIT2;
}


