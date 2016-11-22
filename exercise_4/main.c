
/*	Exercise sheet 4
	Created on:  25.11.2016
	Author:      Boniface Bassey
*/


#include "templateEMP.h"

#define POTI		BIT5 // to P1.5
#define LDR 		BIT4 // to P1.4
#define RED_LED 	BIT0 // to P3.0
#define GREEN_LED 	BIT1 // to P3.1
#define BLUE_LED 	BIT2 // to P3.2
#define S0_SR2		BIT0 // to P2.0
#define	S1_SR2		BIT1 // to P2.1
#define	CLK	    	BIT4 // to P2.4
#define CLR 		BIT5 // to P2.5
#define SR	    	BIT6 // to P2.6

void pins_init(void)
{
	// select I/O pin function
	P3SEL  &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P3SEL2 &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P2SEL  &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P2SEL2 &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	// Secondary peripheral module function is selected.
	P1SEL  |=  ( POTI + LDR );
	P1SEL2 |=  ( POTI + LDR );

	// Output pins
	P3DIR |= ( RED_LED + GREEN_LED + BLUE_LED );
	P2DIR |= ( S0_SR2 + S1_SR2 + CLK + CLR + SR );

	// Pin values are initially zero
	P3OUT &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P2OUT &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
}

void adc_init(void)
{
	// Turn ADC on ; use 16 clocks as sample & hold time ( see p . 559 in the user guide )
	ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
	// Enable P1 .7 as AD input
	ADC10AE0 |= POTI ;
	// Select channel 7 as input for the following conversion
	ADC10CTL1 = INCH_5 ;
}

void sreg_init(void)
{
	// Reset shift register
	P2OUT &= ~CLR;
	// Shift right, enable shift register
	P2OUT |= S0_SR2 + CLR; 
}

// SREG clock pulse
void clock_pulse(void)
{
	// rising clock edge 
	P2OUT |= CLK;	
	// falling clock edge
	P2OUT &= ~CLK;	
}

int adc_value(void)
{
	// Start conversion
	ADC10CTL0 |= ENC + ADC10SC ;
	// Wait until result is ready
	while ( ADC10CTL1 & ADC10BUSY ) ;
	// If result is ready , copy it to val
	int ADCValue = ADC10MEM ;
	// Print val to the serial console
	serialPrintInt ( ADCValue );
	serialPrintln ( " " );
	// return the result of conversion
	return ADCValue;
}

int adc_range(int ADCValue)
{
	unsigned char ledRange = 0;
	if (ADCValue < 205)
		ledRange = 0;
	else if ((ADCValue > 204) && (ADCValue < 410))
		ledRange = 1;
	else if ((ADCValue > 409) && (ADCValue < 615))
		ledRange = 2;
	else if ((ADCValue > 614) && (ADCValue < 820))
		ledRange = 3;
	else
		ledRange = 4;
	// return led range
	return ledRange;
}

void adc_usage(unsigned char range)
{
	unsigned char i;
	if (range)
	{
		// Initialize shift register
		sreg_init();
		// SR data = 1
		P2OUT |= SR;
		for ( i = 0; i < range; i++)
		{
			// Toggle clock
			clock_pulse();					
		}
	}
	else{
		// Initialize shift register
		sreg_init();
		// SR data = 0
		P2OUT &= ~SR;
		for ( i = 0; i < 4; i++)
		{
			// Toggle clock
			clock_pulse();
		}		
	}
}


void main ( void ) {
	// Initialize the controller
	initMSP ();
	// Initialize pins
	pins_init();
	// Initialize the ADC
	adc_init();
	//initialize the shift register
	sreg_init();
	
	while (1)
	{
		// Light up leds according to ADC range
		adc_usage(adc_range(adc_value()));
	}
}




