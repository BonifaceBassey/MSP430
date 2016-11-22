
/*	Exercise sheet 4
	Created on:  25.11.2016
	Author:      Boniface Bassey
*/


#include "templateEMP.h"

#define POTI		BIT5 // to CON3:P1.5
#define LDR 		BIT4 // to CON3:P1.4
#define RED_LED 	BIT0 // to CON3:P3.0
#define GREEN_LED 	BIT1 // to CON3:P3.1
#define BLUE_LED 	BIT2 // to CON3:P3.2
#define S0_SR2		BIT0 // to P2.0
#define	S1_SR2		BIT1 // to P2.1
#define	CLK	    	BIT4 // to P2.4
#define CLR 		BIT5 // to P2.5
#define SR	    	BIT6 // to P2.6

int adcResult=0;

void pins_init(void)
{
	// select I/O pin function
	P3SEL  &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P3SEL2 &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P2SEL  &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P2SEL2 &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
	P1SEL  &= ~( POTI + LDR );
	P1SEL2 &= ~( POTI + LDR );

	// Output pins
	P3DIR |= ( RED_LED + GREEN_LED + BLUE_LED );
	P2DIR |= ( S0_SR2 + S1_SR2 + CLK + CLR + SR );

	// Input pins
	P1DIR &= ~( POTI + LDR );

	// Pin values are initially zero
	P3OUT &= ~( RED_LED + GREEN_LED + BLUE_LED );
	P2OUT &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
}

void adc_init(void)
{
	// Turn ADC on ; use 16 clocks as sample & hold time ( see p . 559 in the user guide )
	ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
	// Enable P1 .7 as AD input
	ADC10AE0 |= BIT7 ;
	// Select channel 7 as input for the following conversion
	ADC10CTL1 = INCH_7 ;
}

void sreg_init(void)
{
	// Reset shift register
	P2OUT &= ~CLR;
	// Shift right, enable shift register
	P2OUT |= S0_SR2 + CLR; 
	// Shift right data
	P2OUT |= SR;
}

// SREG clock pulse
void __clockPulse(void)
{
	P2OUT |= CLK;	// rising clock edge 
	P2OUT &= ~CLK;	// falling clock edge
}

int adc_value(void)
{
	// Start conversion
	ADC10CTL0 |= ENC + ADC10SC ;
	// Wait until result is ready
	while ( ADC10CTL1 & ADC10BUSY ) ;
	// If result is ready , copy it to val
	int val = ADC10MEM ;
	// Print m1 to the serial console
	serialPrintInt ( val );
	serialPrintln ( " " );
	// return the result of conversion
	return val;
}

int adc_range(int adc)
{
	unsigned char var = 0;
	adcResult = adc/5;

	if (adcResult < 205)
		var = 0;
	else if ((adcResult > 204) && (adcResult < 410))
		var = 1;
	else if ((adcResult > 409) && (adcResult < 615))
		var = 2;
	else if ((adcResult > 614) && (adcResult < 820))
		var = 3;
	else
		var = 4;
	return var;
}

void adc_usage(unsigned char range)
{
	if (range)
	{
		for ( i = 0; i < range; i++)
		{

		}
	}
	else{

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
		adc_usage(adc_range(adc_value()));
	}
}




