
/*	Exercise sheet 7
	Created on:  14.01.2017
	Author:      Boniface Bassey
*/

#include "templateEMP.h"

#define GREEN		BIT0
#define PB5			BIT3
#define REL_STAT	BIT4
#define REL_DYN 	BIT5
#define RED			BIT2
#define NTC			BIT5
#define S0_SR2		BIT0 // to P2.0
#define	S1_SR2		BIT1 // to P2.1
#define	CLK	    	BIT4 // to P2.4
#define CLR 		BIT5 // to P2.5
#define SR	    	BIT6 // to P2.6

unsigned char pbFlag = 0;
unsigned short counter = 0;
unsigned short wdFlag = 0;
unsigned char ledRange = 0;


// Pins Initialization
void pins_init(void)
{
	// select I/O function	
	P1SEL  &= ~(GREEN + PB5);
	P1SEL2 &= ~(GREEN + PB5);
	P2SEL  &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P2SEL2 &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P3SEL  &= ~(RED + REL_STAT);
	P3SEL2 &= ~(RED + REL_STAT);
	// P1.5 A5 option -  analog input
	P1SEL  |= NTC;
	P1SEL2 |= NTC;
	// set pin directions
	P1DIR |= GREEN;
	P1DIR &= ~PB5;
	P2DIR |= ( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P3DIR |= (RED + REL_STAT);
	// Ennable pull-resistors
	P1REN |= PB5;
	// Led off
	P1OUT &= ~GREEN;
	P2OUT &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	//P3OUT &= ~(RED + REL_STAT);
	P3OUT &= ~RED;

	P1IE |= PB5;	// P1.3 interrupt
	P1IES |= PB5;	// falling edge - High/Low
	P1IFG &= ~PB5;	// Clear interrupt flag
    __enable_interrupt();	// enable all interrupts

}

// ADC initialization
void adcInit(void)
{
	// modified ADC10CTL0 only when ENC = 0
	ADC10CTL0 &= ~ENC;
	// Turn ADC on ; use 16 clocks as sample & hold time
	ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
	// Enable P1.5 as AD input
	ADC10AE0 |= NTC;
	// Select input chanel for the conversion
	ADC10CTL1 = INCH_5;
}

// initialize timer/PWM
void initTimerPWM(void)
{
	// Timer Initialization
    TA1CCTL0  = CCIE;			// Timer_A Capture/compare interrupt enable
    TA1CCR0   = 10000;			// Timer A Capture/Compare 0
    TA1CTL  = TASSEL_2 + MC_1;	// SMCLK, Up mode - Up to CCR0

    // PWM Initialization
	P3DIR |= BIT5;			// P3.5 output
	P3SEL |= BIT5;			// P3.5 TA0.1 option
	TA0CCR0 = 255;				// PWM period
	TA0CCTL1 = OUTMOD_3;		// TA0CCTL1 - Set/ Reset mode
	TA0CTL = TASSEL_2 + MC_1;	// SMCLK, Up mode
}

// Shift register initialization
void sreg_init(void)
{
	// Reset shift register
	P2OUT &= ~CLR;
	// Shift right, enable shift register
	P2OUT |= S0_SR2 + CLR;
}

// Get ADC Result
int adcRead(void)
{
	// start conversion
	ADC10CTL0 |= ENC + ADC10SC;
	// wait until result is ready
	while(ADC10CTL1 & ADC10BUSY);
	// return the result of conversion
	return ADC10MEM;
}


// Shift register clock pulse
void clock_pulse(void)
{
	// rising clock edge
	P2OUT |= CLK;
	// falling clock edge
	P2OUT &= ~CLK;
}

// Split ADC result into 5 sections
unsigned char NTCrange(int ADCValue)
{
	/* From Measurement:
	 * NTC minimum @ room temperature : 315
	 * NTC maximum : 575
	 * 6 sections approximately: 358, 401, 444, 478, 530, 573
	 * */
	if (ADCValue < 350)							// Leds off
		ledRange = 0;
	else if ((ADCValue > 350) && (ADCValue < 401))	// 1 led on
		ledRange = 1;
	else if ((ADCValue > 400) && (ADCValue < 444))	// 2 leds on
		ledRange = 2;
	else if ((ADCValue > 443) && (ADCValue < 478))	// 3 leds on
		ledRange = 3;
	else if ((ADCValue > 477) && (ADCValue < 530))	// 4 leds on
		ledRange = 4;
	else if ((ADCValue > 529) && (ADCValue < 575))	// Red leds on(over-heating)
		ledRange = 5;
	else											// Red leds on(over-heating)
		ledRange = 5;

	return ledRange;	// Number of leds to switch on
}

// Thermometer
void thermometer(unsigned char range)
{
	unsigned char i;
	if (range == 0)
	{
		// Initialize shift register
		sreg_init();
		// SR data = 0
		P2OUT &= ~SR;
		// all leds shall be off
		range = 4;
	}else{
		// Initialize shift register
		sreg_init();
		// SR data = 1
		P2OUT |= SR;
	}

	// shift in values according to range
	for ( i = 0; i < range; i++)
	{
		if (range == 5)
			P3OUT |= RED;
		else
			P3OUT &= ~RED;

		// Toggle clock
		clock_pulse();
	}
}


void main ( void )
{
	//Initialization of the controller
	initMSP();
	// Pins initialization
	pins_init();
	// ADC initialization
	adcInit();
	// SREG initialization
	sreg_init();
	// initialize timer/PWM
	initTimerPWM();

	do{	// Green led 4Hz=250ms flash until PB5 pressed
		P1OUT ^= GREEN;
		__delay_cycles(250000);
	}while(pbFlag!=1);

	// configure ACLK to source from VLO = 12KHz
	BCSCTL3 |= LFXT1S_2;

	// WDT is clocked by fACLK (12KHz)
	WDTCTL = WDT_ADLY_1000;				// WDT interval timer
	IE1 |= WDTIE;                   	// Enable WDT interrupt
	_BIS_SR(LPM0_bits + GIE);           // Enter LPM0 w/ interrupt


	while (1)
	{
	}

}


// Watchdog Interval Timer interrupt service  
#pragma vector=WDT_VECTOR  
__interrupt void watchdog_timer(void) 
{   
	wdFlag++;

	//if (wdFlag == 2) 	// 32768/12000 = 2.7s * 2 ~ 5.4s
	if (wdFlag == 8) 	// 32768/12000 = 2.7s * 8 ~ 21.6s
	{
		wdFlag = 0;
		WDTCTL = WDT_ARST_1_9;      // Watchdog mode -> reset after expired time
	}
}  

// Timer A0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void Timer_A (void)
{
	counter++;				// increment counter
	if (counter == 200){	// 2s reached?
		counter = 0;		// reset timer counter

/*		// regulate temprature with REL_STAT
		if (ledRange == 4) {
			P3OUT &= ~REL_STAT;
		}else {
			P3OUT |= REL_STAT;
		}
*/
		// regulate temprature with REL_DYN
		P3DIR &= ~REL_STAT; // set REL_STAT pin to high impedance
		if (ledRange == 4) {
			TA0CCR1 = 0;
		}
		else {
			TA0CCR1 = 20;
		}

		// read and display temperature
		thermometer(NTCrange(adcRead()));
	}
}

// Port 1 interrupt vector
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	P1IFG &= ~BIT3;		// clear pin pin interrupt
	pbFlag = 1;
}

