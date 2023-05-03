#include <msp430.h>
#include "motor_control.h"
/**
 * main.c
 */


int main(void)
{
    setup_bluetooth();
    setup_drivers();
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
//    standby_off();
//    right_on();

//    standby_off();
//    left_on();
//    right_on();
//    while(1);

    while(1){
        __bis_SR_register(LPM0_bits + GIE); // go to sleep in low-power mode 0, all interrupts enabled
    }
    return 0;
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void) {
    switch(UCA0IV) {
    case 0x00:
        // Vector 0: No interrupts
        break;
    case 0x02: // Received Char Interrupt
        while(!(UCA0IFG&UCTXIFG));

        if (UCA0RXBUF == 'R') {
            right_on();
            UCA0TXBUF = 'R';
        } else if (UCA0RXBUF == 'L') {
            left_on();
            UCA0TXBUF = 'L';
        } else if (UCA0RXBUF == 'S') {
            standby();
            UCA0TXBUF = 'S';
        } else if (UCA0RXBUF == 'O') {
            standby_off();
            UCA0TXBUF = 'O';
        } else if (UCA0RXBUF == 'r') {
            brake_right();
            UCA0TXBUF = 'r';
        } else if (UCA0RXBUF == 'l') {
            brake_left();
            UCA0TXBUF = 'l';
        }

        __no_operation();
        break;
    case 0x04: // Transmit buffer empty
        break;
    case 0x06: // START byte received
        break;
    case 0x08: // All transmit complete
        break;
    }
}
