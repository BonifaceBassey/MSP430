
/*	Exercise sheet 4
	Created on:  26.11.2016
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
#define LEN		5

unsigned char adcPoti = 1;
unsigned char adcLDR = 0;
unsigned int red;
unsigned int blue;
unsigned int green;
unsigned int black;
unsigned int white_red[LEN], white_green[LEN], white_blue[LEN];
unsigned int avgWR, avgWG, avgWB, white;

void delay_ms(unsigned int ms)
{
 while (ms--)
    {
        __delay_cycles(1000);
    }
}

// I/O pins initialization
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

	// Initialize output pins
	P3OUT |= ( RED_LED + GREEN_LED + BLUE_LED );
	P2OUT &= ~( S0_SR2 + S1_SR2 + CLK + CLR + SR );
}

// ADC initialization
void adc_init(unsigned char channel)
{
	// modified ADC10CTL0 only when ENC = 0
	ADC10CTL0 &= ~ENC;
	// Turn ADC on ; use 16 clocks as sample & hold time
	ADC10CTL0 = ADC10ON + ADC10SHT_2 ;
	// Enable P1.5 or P1.4 as AD input
	ADC10AE0 |= (channel == 1 ? POTI : LDR);
	// Select input chanel for the conversion
	ADC10CTL1 = (channel == 1 ? INCH_5 : INCH_4);
}

// Shift register initialization
void sreg_init(void)
{
	// Reset shift register
	P2OUT &= ~CLR;
	// Shift right, enable shift register
	P2OUT |= S0_SR2 + CLR; 
}

// Shift register clock pulse
void clock_pulse(void)
{
	// rising clock edge 
	P2OUT |= CLK;	
	// falling clock edge
	P2OUT &= ~CLK;	
}

// Do ADC and get result
int ADC_Read(void)
{
	// Start conversion
	ADC10CTL0 |= ENC + ADC10SC ;
	// Wait until result is ready
	while ( ADC10CTL1 & ADC10BUSY ) ;
	// If result is ready , copy it to val
	int ADCValue = ADC10MEM ;
	// return the result of conversion
	return ADCValue;
}

// Split ADC result into 5 sections
unsigned char adc_range(int ADCValue)
{
	unsigned char ledRange = 0;
	if (ADCValue < 205)				// ADC result - 1st section = 0 led on
		ledRange = 0;
	else if ((ADCValue > 204) && (ADCValue < 410))	// ADC result - 2nd section = 1 led on
		ledRange = 1;
	else if ((ADCValue > 409) && (ADCValue < 615))	// ADC result - 3rd section = 2 leds on
		ledRange = 2;
	else if ((ADCValue > 614) && (ADCValue < 820))	// ADC result - 4th section = 3 leds on
		ledRange = 3;
	else						// ADC result - 5th section = 4 leds on
		ledRange = 4;
	// Number of leds to switch on
	return ledRange;
}

// LEDs ON according to ADC sections
void adc_usage(unsigned char range)
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
		// Toggle clock
		clock_pulse();					
	}
}

unsigned char detect_colors(void)
{ 
	unsigned char idx;

	// Red led ON
	P3OUT = 0x01;
	// wait		
	delay_ms(100);
	// Read ADC and store it in the registry as red.		
	red = ADC_Read();
	// Red led off	
	P3OUT = 0x00;		

	// Green led ON
	P3OUT = 0x02;	
	// wait	
	delay_ms(100);	
	// Read ADC and store it in the registry as green.	
	green = ADC_Read();	
	// Green led off
	P3OUT = 0x00;		

	// Blue led ON
	P3OUT = 0x04;	
	// wait	
	delay_ms(100);
	// Read ADC and store it in the registry as blue.
	blue = ADC_Read();
	// Blue led off
	P3OUT = 0x00;	

	//*** To detect the white color ***//
	// Take multiple LDR readings while Red led is on
	P3OUT = 0x01;	
	delay_ms(100);
	for (idx = 0; idx < LEN; idx++){
		white_red[idx] = ADC_Read();	
	}
	P3OUT = 0x00;	// leds off
	// Take multiple LDR readings while Green led is on
	P3OUT = 0x02;	
	delay_ms(100);
	for (idx = 0; idx < LEN; idx++){
		white_green[idx] = ADC_Read();
	}
	P3OUT = 0x00;	// leds off
	// Take multiple LDR readings while Green led is on
	P3OUT = 0x04;	
	delay_ms(100);
	for (idx = 0; idx < LEN; idx++){
		white_blue[idx] = ADC_Read();

	}
	P3OUT = 0x00;	// leds off
	delay_ms(100);

	// Sum up the multiple LDR readings
	for (idx = 0; idx < LEN; idx++){
		avgWR += white_red[idx];
		avgWG += white_green[idx];
		avgWB += white_blue[idx];
	}
	// Take the average of the multiple readings
	avgWR /= LEN;
	avgWG /= LEN;
	avgWB /= LEN;
	// white color is average readings divide by 3
	white = (avgWR + avgWG + avgWB)/3;

	serialPrintInt ( red ); 
	serialPrintln ( " " ); 
	serialPrintInt ( green ); 
	serialPrintln ( " " ); 
	serialPrintInt ( blue ); 
	serialPrintln ( " " ); 
	serialPrintInt ( white ); 
	serialPrintln ( " " ); 

	// Black color detection
	if ((red< 80) || (green < 80) || (blue < 80)) 
	{  	
		serialFlush ();
		serialPrintln ( "Black" ); 
		delay_ms(10); } 
	else{
		// Red color detection
		if ((red > green) & (red > blue) & (red > (white-10))) 
		{ 	
			serialFlush ();
			serialPrintln ( "Red" ) ; 
			delay_ms(10) ; 
		}
		// Green color detection
		if ((green > red) & (green > blue) & (green > (white-10))) 
		{ 
			serialFlush ();
			serialPrintln ( "Green" ); 
			delay_ms(10); 
		} 
		// Blue color detection
		if ((blue > green) & (blue > red) & (blue > (white-2))) 
		{ 
			serialFlush ();
			serialPrintln ( "Blue" ); 
			delay_ms(10); 
		} 
		// White color detection
		if ((white > red) & (white > green) & (white > blue)) 
		{ 
			if (white > 250)
			{
				serialFlush ();
				serialPrintln ( "White" ); 
				delay_ms(10); 
			}
		}	
	}
}

void main ( void ) {
	// Initialize the controller
	initMSP ();
	// Initialize pins
	pins_init();
	//initialize the shift register
	sreg_init();
	
	while (1)
	{
		// Initialize the ADC for potentiometer
		adc_init(adcPoti);
		// Light up leds according to ADC range
		adc_usage(adc_range(ADC_Read()));

		// Initialize the ADC for LDR
		adc_init(adcLDR);
		// Detect colors
		detect_colors();
	}
}




