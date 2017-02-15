/*************************************
 * Exercise 5
 * Author: Boniface Bassey
 * Date: 03.12.2016
 *************************************/

//#include <msp430g2553.h>
#include <templateEMP.h>
#include "tone.h"

#define BUZZER	BIT6 // P3.6
#define PB5		BIT3 // P1.3
#define PB6		BIT4 // P1.4
#define LED     BIT0

void wait(unsigned int ms);
void initPWM(void);
void melody_1(void);
void melody_2(void);
void initTimer(void);
void pinCONFIG(void);

unsigned int counter=0;
unsigned char setFlag=0;
unsigned char mode=0;
unsigned char once=1;
unsigned char melody=0;
unsigned char PB5_count=0;
unsigned char PB5_switch=0;
unsigned char PB6_switch=0;


/*************************************
 * main.c
 *************************************/
int main(void) {
	//int val;

	// Initialization of the controller
	initMSP();
	// PWM Initialization
	initPWM();
	// Pins configuration
	pinCONFIG();

	while (1)
	{
		//melody_1();		// play melody_1
		//wait(1000);		// wait 1s
		//melody_2();		// play melody_2

		switch (melody)		// select melody
		{
		case 1:
			melody_1();		// play melody_1
			break;
		case 2:
			melody_2();		// play melody_2
			break;
		}

		// vibration detection
		//P3REN &= ~BUZZER;
		//P3DIR &= ~BUZZER;
		//val = (P3IN & BIT6);

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
	P3DIR |= BUZZER;			// P3.6 output
	P3SEL |= BUZZER;			// P3.6 TA0.2 option
	TA0CCTL2 = OUTMOD_3;		// TA0CCTL2 - Set/ Reset mode
	TA0CTL = TASSEL_2 + MC_1;	// SMCLK, Up mode
}

void initTimer(void)
{
    P1DIR |= BIT0;				// P1.0 output
    TA1CCTL0  = CCIE;			// Timer_A Capture/compare interrupt enable
    TA1CCR0   = 10000;			// Timer A Capture/Compare 0
    TA1CTL  = TASSEL_2 + MC_1;	// SMCLK, Up mode - Up to CCR0
}

void pinCONFIG(void)
{
	P1SEL  &= ~(PB5 + PB6); // I/O funtion
	P1SEL2 &= ~(PB5 + PB6);
	P1DIR &= ~(PB5 + PB6);	// P1.5/P1.6 as inputs
	P1REN |= (PB5 + PB6);	// Enable pull-up resistors for pin P1.5/P1.6
	P1IE |= (PB5 + PB6);	// Enable interrupt on Port 1 for pin P1.5/P1.6
	P1IES |= (PB5 + PB6);	// Trigger interrupt on falling edge - High/Low transition
	P1IFG &= ~(PB5 + PB6);	// Clear interrupt flag
	__enable_interrupt();	// enable all interrupts
}

// Play tone 1
void melody_1(void)
{
	unsigned char i;
	for (i = 0; i < 25; i++)
	{
		do{
			TA0CCR0 = 0;
		}while(PB6_switch == 1);		// PB6 pause/replay melody_1
		if (melody != 1)				// play melody_1 as long as melody_1 flag is set
			break;
    	TA0CCR0 = HappyBirthday[i];		// PWM period
    	TA0CCR2 = HappyBirthday[i]/2;	// 50% duty cycle
    	wait(interval1[i]);				// tone duration
    	TA0CCR0 = 0;					// make period 0 -> no tone
    	wait(100);						// no tone  duration
    }
    TA0CCR2 = 0;
}

// Play tone 2
void melody_2(void)
{
	unsigned char i;
	for (i = 0; i < 16; i++)
	{
		do{
			TA0CCR0 = 0;
		}while(PB6_switch == 1);		// PB6 pause/replay melody_2
		if (melody != 2)				// play melody_2 as long as melody_2 flag is set
			break;
    	TA0CCR0 = OTannenBaum[i];		// PWM period
    	TA0CCR2 = OTannenBaum[i]/2;		// 50% duty cycle
    	wait(interval2[i]);				// tone duration
    	TA0CCR0 = 0;					// make period 0 -> no tone
    	wait(100);						// no tone  duration
    }
    TA0CCR2 = 0;
}


// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	counter++;				// increment counter
	if (counter == 100){	// 1s reached?
		P1OUT ^= LED;       // Toggle P1.0
		counter = 0;		// reset timer counter
		PB5_count = 0;		// reset PB5 interrupt counter
	}
}

// Port 1 interrupt vector
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	if (P1IFG & PB5){		// P1.5 interrupt?
		if (once == 1){		// setup the 1s timer only once
			once = 2;		// lock up re-executing initTimer()
			initTimer();	// Initialize timer
		}
		PB5_count++;		// increment counter
		P1IFG &= ~PB5;		// clear P1.5 interrupt
		if (PB5_count == 1){// Button pressed once within 1s
			melody = 1;		// set melody_1 flag
			counter = 0; 	// reset timer
		}
		if (PB5_count == 2){// Button pressed twice within 1s
			melody = 2;		// set melody_2 flag
			counter = 0;	// reset timer
		}
	}

	if (P1IFG & PB6){		// P1.6 interrupt?
		P1IFG &= ~PB6;		// clear P1.6 interrupt
		PB6_switch++;		// increment counter
		if (PB6_switch == 2)// 2 times was needed for pause and replay
			PB6_switch = 0;
	}
}

