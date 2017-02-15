
/*	Exercise sheet 3
	Created on:  19.11.2016
	Author:      Boniface Bassey
*/

#include <msp430g2553.h>
#include <templateEMP.h>

#define S0_SR2	BIT0
#define	S1_SR2	BIT1
#define S0_SR1	BIT2
#define	S1_SR1	BIT3
#define	CLK	    BIT4
#define CLR 	BIT5
#define SR	    BIT6
#define QD	    BIT7
#define DATA_PORT2 0x7F

void __resetSR(void);
void __setSR(void);
void __modeSelectSR(void);
void __modeSelectSL(void);
void __clockPulse(void);
void __shiftRight_SREG1(void);
void __rotating_light(unsigned char x);
void __configuration(void);
unsigned char __buttons(void);
void delay(unsigned int);

unsigned char val, state;

// function - shift from left to right
void __left_to_right(unsigned int ms)
{
	__configuration();			// configure the SREG
	__clockPulse();				// toggle clock
	P2OUT &= ~SR;				// data = 0
	__clockPulse();				// toggle clock
	__clockPulse();				// toggle clock
	P2OUT |= CLK;				// clock high
	delay(ms);					// wait
	__rotating_light(0);		// run a rotating light

	__configuration();			// configure the SREG
	__clockPulse();				// toggle clock
	P2OUT &= ~SR;				// data =  0
	__clockPulse();				// toggle clock
	P2OUT |= CLK;				// clock high
	delay(ms);					// wait
	__rotating_light(1);		// run a rotating light

	__configuration();			// configure the SREG
	__clockPulse();				// toggle clock
	P2OUT &= ~SR;				// data = 0
	P2OUT |= CLK;				// clock high
	delay(ms);					// wait
	__rotating_light(2);		// run a rotating light

	__configuration();			// configure the SREG
	P2OUT |= CLK;				// clock high
	P2OUT &= ~SR;				// data = 0
	P2OUT &= ~CLK;				// clock low
	delay(ms);					// wait
	__rotating_light(3);		// run a rotating light
}

// function - shift from right to left
void __right_to_left(unsigned int ms)
{
	__modeSelectSR();				// select shift right on SREG2
	P2OUT |= SR;					// High input on SREG2 SR

	unsigned char idx;				// index counter
	for ( idx = 0 ; idx < 4 ; idx++ )
	{
		__clockPulse();				// Toggle clock
		P2OUT &= ~SR;				// data  = 0
		delay(ms);					// wait

		__rotating_light(idx + 1);	// run a rotating light
	}
}

// Function - get button status
unsigned char __buttons(void)
{
	unsigned char i;

	P2OUT |= S0_SR1 + S1_SR1;		// select parallel input on SREG1

	for ( i = 0 ; i < 4 ; i++ )
	{
		__clockPulse();					// Toggle clock
		val = (P2IN & QD);

		if (val && (i == 0)){			// PB4 - fastforward
			return 4;
		}
		else
		{
			__shiftRight_SREG1();

			if (val && (i == 1)){		// PB3 - playback
				return 3;
			}

			if (val && (i == 2)){	    // PB2 - pause
				P2OUT &= ~(S0_SR2 + S1_SR2);
				return 2;
			}

			if (val && (i == 3)){		// PB1 - rewind
				return 1;
			}
		}
	}
	return 0;
}



/*
 * main.c
 */
int main(void)
{
	// Initialization of the controller
    initMSP();

	// select I/O function on PORT2
	P2SEL  &= ~(DATA_PORT2 + QD);		// P2.0 - P2.7
	P2SEL2 &= ~(DATA_PORT2 + QD);		// P2.0 - P2.7
	P1SEL &= ~(BIT5 + BIT6 + BIT7);		// P1.5 - P1.7
	P1SEL2 &= ~(BIT5 + BIT6 + BIT7);	// P1.5 - P1.7

	// Pin direction - Output
	P2DIR |= DATA_PORT2;				// P2.0 - P2.6
	P1DIR |= (BIT5 + BIT6 + BIT7);		// P1.5 - P1.7

	// Pin direction - Input
	P2DIR &= ~QD;						// P2.7

	// initialize port - clear all bits
	P2OUT &= ~DATA_PORT2;				// P2.0 - P2.6
	P2OUT &= ~QD;						// P2.7
	P1OUT &= ~(BIT5 + BIT6 + BIT7);		// P1.5 - P1.7

	// Set shift register - stop the reset mode
	__setSR();


	while(1)
	{
		state = __buttons();

		if (!(state))
			__right_to_left(250);

		if (state == 1)
		{     state = 0;
			__left_to_right(125);
		}

		//if (state == 2){

		//if (state == 3)

		if (state == 4)
		{     state = 0;
			__right_to_left(125);
		}
	}
}

// Reset shift register
void __resetSR(void){
	P2OUT &= ~CLR ;
}

// Set shift register
void __setSR(void){
	P2OUT |= CLR ;
}

// Mode control - shift right
void __modeSelectSR(void){
	P2OUT |= S0_SR2 ;
	P2OUT &= ~S1_SR2 ;
}

// Mode control - shift left
void __modeSelectSL(void){
	P2OUT |= S1_SR2 ;
	P2OUT &= ~S0_SR2 ;
}

void __shiftRight_SREG1(void)
{
	P2OUT |= S0_SR1 ;
	P2OUT &= ~S1_SR1 ;
}

// Pulse the clock pin
void __clockPulse(void)
{
	P2OUT |= CLK;	// apply a rising clock edge = > shift data in
	P2OUT ^= CLK;	// reset the clock
}

void __rotating_light(unsigned char x)
{
	if (x == 1)
		P1OUT |= BIT5;
	else
		P1OUT &= ~BIT5;

	if (x == 2)
		P1OUT |= BIT6;
	else
		P1OUT &= ~BIT6;

	if (x == 3)
		P1OUT |= BIT7;
	else
		P1OUT &= ~BIT7;
}

// configure the SREG
void __configuration(void)
{
	 __resetSR();   // reset SREG
	P2OUT = S0_SR2 + CLR + SR;  // SREG2 shift right, set SREG, data
}
// Delay in ms
void delay(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000);
    }
}


