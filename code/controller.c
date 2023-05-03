#include <msp430.h> 
/**
 * main.c
 */
#include <stdbool.h>
#include <stdint.h>
#define BUTTON_MASK (BIT1 + BIT2) // Port 1
void ConfigureAdc();
unsigned int ADC_Result;
unsigned int counter = 0;
volatile bool drive_en = false;
volatile bool button = false;
void button_init()
{
    // Switches are pins 1.1, 1.2. They pull low when depressed.
    P1DIR &= ~BUTTON_MASK; // Set pins to input
    P1REN |= BUTTON_MASK;   // Set pull-up/pull-down resistor
    P1OUT |= BUTTON_MASK;  // Set resistor to be a pull-up

    P1IE |= BUTTON_MASK; // Enable button interrupts
    P1IES &= BUTTON_MASK; //Interrupt triggers on high-low transition
    P1IFG &= ~BUTTON_MASK; //Clear IFG register

    button = false;
}

int main(void)
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

    while (CSCTL7 & (FLLUNLOCK0 | FLLUNLOCK1))
        ;
    CSCTL4 = SELMS__DCOCLKDIV | SELA__REFOCLK;

    UCA0CTLW0 |= UCSWRST;
    UCA0CTLW0 |= UCSSEL__SMCLK;
    UCA0BR0 = 52;
    UCA0MCTLW = 0x4900 | UCOS16 | UCBRF1;

    P1SEL1 &= ~BIT5 & ~BIT4;
    P1SEL0 |= BIT5 | BIT4;

    SYSCFG2 = ADCPCTL0;

    PM5CTL0 &= ~LOCKLPM5;

    UCA0CTLW0 &= ~UCSWRST;
    UCA0IE |= UCRXIE;

    button_init();
    ConfigureAdc();

    P2DIR |= BIT0;
    P2OUT &= ~BIT0;

    UCA0TXBUF = 'S';
    while (1)
    {
        ADCCTL0 |= ADCENC | ADCSC;
        __bis_SR_register(LPM0_bits + GIE); // go to sleep in low-power mode 0, all interrupts enabled
        if (!drive_en)
        {
            if (ADC_Result < 500)
                counter++;
            else
                counter = 0;
        }
        if (counter > 20)
        {
            drive_en = true;
            P2OUT |= BIT0;
            UCA0TXBUF = 'O';
        }
    }
    return 0;
}

#pragma vector = USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
    switch (UCA0IV)
    {
    case 0x00:
        // Vector 0: No interrupts
        break;
    case 0x02: // Received Char Interrupt
        break;
    case 0x04: // Transmit buffer empty
        break;
    case 0x06: // START byte received
        break;
    case 0x08: // All transmit complete
        break;
    }
}
#pragma vector=PORT1_VECTOR
__interrupt void button_vector(void)
{
    if (P1IFG & BIT1)
    {
        P1IES ^= BIT1; // Toggle edge which triggers
        P1IFG &= ~BIT1; //Clear IFG register
        if (button)
        {
            UCA0TXBUF = 'r';
        }
        else
        {
            UCA0TXBUF = 'R';
        }
        button = !button;

    }
    __low_power_mode_off_on_exit();
}
// Function containing ADC set-up
void ConfigureAdc(void)
{
    ADCCTL0 |= ADCSHT_2 | ADCON;
    ADCCTL1 |= ADCSHP;
    ADCCTL2 |= ADCRES;
    ADCMCTL0 |= ADCINCH_0;
    ADCIE |= ADCIE0;
}

#pragma vector=ADC_VECTOR
__interrupt void ADC_ISR(void)
{
    switch (ADCIV)
    {
    case ADCIV_NONE:
        break;
    case ADCIV_ADCOVIFG:
        break;
    case ADCIV_ADCTOVIFG:
        break;
    case ADCIV_ADCHIIFG:
        break;
    case ADCIV_ADCLOIFG:
        break;
    case ADCIV_ADCIFG:
        ADC_Result = ADCMEM0;
        __bic_SR_register_on_exit(LPM0_bits);
        break;
    default:
        break;
    }
}

