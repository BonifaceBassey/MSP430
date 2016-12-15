/*************************************
 * Exercise 5
 * Author: Boniface Bassey
  Date: 09.12.2016
 *************************************/

#include "templateEMP.h"
#include "tone.h"

#define AUDIO_OUT		BIT6 // P3.6
#define VIBRATION		BIT3 // P1.3
#define RELAY			BIT0 // P1.0

void wait(unsigned int ms);
void initPWM(void);
void melody_1(void);
void melody_2(void);
void initTimer(void);
void pinCONFIG(void);

unsigned int counter=0;
unsigned char once=1;
unsigned char melody=0;
unsigned char count=0;



/*************************************
 * main.c
 *************************************/
int main(void) {
	// Initialization of the controller
	initMSP();
	// PWM Initialization
	initPWM();
	// Pins configuration
	pinCONFIG();


	while(1)
	{
		__delay_cycles(2000000);
		switch (melody)		// select melody
		{
		case 1:
			melody_1();		// play melody_1
			break;
		case 2:
			melody_2();		// play melody_2
			break;
		}
	}
}


// Delay in ms
void wait(unsigned int ms)
{
	while (ms--){
        __delay_cycles(1000); // 1000 for 1 Mhz
    }
}

// PWM Initialization
void initPWM(void)
{
	P3DIR |= AUDIO_OUT;		// P3.6 output
	P3SEL |= AUDIO_OUT;		// P3.6 TA0.2 option
	TA0CCTL2 = OUTMOD_3;		// TA0CCTL2 - Set/ Reset mode
	TA0CTL = TASSEL_2 + MC_1;	// SMCLK, Up mode
}

void initTimer(void)
{
    	TA1CCTL0  = CCIE;			// Timer_A Capture/compare interrupt enable
    	TA1CCR0   = 10000;			// Timer A Capture/Compare 0
   	TA1CTL  = TASSEL_2 + MC_1;		// SMCLK, Up mode - Up to CCR0
}

void pinCONFIG(void)
{
	// configure relay pin
	P1SEL  &= ~RELAY; 		// I/O funtion
	P1SEL2 &= ~RELAY;
	P1OUT  &= ~RELAY;		// no output
    	P1DIR  |= RELAY;		// P1.0 output
    	// configure vibration pin
	P1SEL  &= ~VIBRATION; 	// I/O funtion
	P1SEL2 &= ~VIBRATION;
	P1REN  &= ~VIBRATION;	// pull down resistor
	P1OUT  &= ~VIBRATION;	// no output
	P1DIR  &= ~VIBRATION;	// P1.3 inputs
	P1IE  |= VIBRATION;	// P1.3 interrupt enable
	P1IES |= VIBRATION;	// Trigger interrupt on falling edge - High/Low transition
	P1IFG &= ~VIBRATION;	// Clear interrupt flag
	__enable_interrupt();	// enable all interrupts
}

// Play tone 1
void melody_1(void)
{
	// switch relay to PWM output
	P1OUT |= RELAY;
	// don't repeat melody until another tap is detected
	melody = 0;
	unsigned char i;
	for (i = 0; i < 25; i++)
	{
    		TA0CCR0 = HappyBirthday[i];	// PWM period
    		TA0CCR2 = HappyBirthday[i]/2;	// 50% duty cycle
    		wait(interval1[i]);		// tone duration
    		TA0CCR0 = 0;			// make period 0 -> no tone
    		wait(100);			// no tone  duration
    	}
	TA0CCR2 = 0;
    	// switch relay to detect vibration
    	P1OUT &= ~RELAY;
}

// Play tone 2
void melody_2(void)
{
	// switch relay to PWM output
	P1OUT |= RELAY;
	// don't repeat melody until another tap is detected
	melody = 0;
	unsigned char i;
	for (i = 0; i < 16; i++)
	{
    		TA0CCR0 = OTannenBaum[i];	// PWM period
    		TA0CCR2 = OTannenBaum[i]/2;	// 50% duty cycle
    		wait(interval2[i]);		// tone duration
    		TA0CCR0 = 0;			// make period 0 -> no tone
    		wait(100);			// no tone  duration
    	}
    	TA0CCR2 = 0;
    	// switch relay back to detect vibration
    	P1OUT &= ~RELAY;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	counter++;			// increment counter
	if (counter == 100){		// 1s reached?
		counter = 0;		// reset timer counter
		count = 0;		// reset interrupt counter
	}
}


// Port 1 interrupt vector
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	P1REN  |= VIBRATION;	// pull down resistor
	P1REN  &= ~VIBRATION;	// pull down resistor

	if (P1IFG & VIBRATION){	// P1.3 interrupt?
		if (once == 1){		// setup the 1s timer only once
			once = 2;		// lock up re-executing initTimer()
			initTimer();	// Initialize timer
		}
		__delay_cycles(1000);
		count++;			// increment counter
		P1IFG &= ~VIBRATION;// clear P1.3 interrupt

		if ((count > 1) && (count < 3)) {	// Button pressed once within 1s
			melody = 1;		// set melody_1 flag
			counter = 0; 	// reset timer
		}
		if (count > 3){	// Button pressed twice within 1s
			melody = 2;		// set melody_2 flag
			counter = 0;	// reset timer
		}
	}
}
