/*	Exercise sheet 2
	Created on:  10.11.2016
	Author:      Boniface Bassey
*/
	
#include <stdbool.h>
#include <msp430g2553.h>

#define BUTTON5	    BIT3	// PB5 
#define BUTTON6     BIT4	// PB6
#define RED_LED	    BIT5
#define GREEN_LED   BIT6 
#define YELLOW_LED  BIT7
#define BLUE_LED    BIT0

bool redLedLogic = false;
bool greenLedLogic = false;
bool blueLedLogic = false;
bool yellowLedLogic = false;
bool PB5_switch = false;
bool PB6_switch = false;
bool blink_once = true;
bool mode = false;
	

void __blinkRed()
{	
	// flash red LED - do it once
	if ((redLedLogic)){						// Only PB5 should turn red LED on	
		if (blink_once)	{
			blink_once =  false;	
			P1OUT &= ~(BLUE_LED + GREEN_LED + YELLOW_LED); 	// Other LEDs are off when red LED is on
				
			unsigned char i;				
			for (i = 0; i < 10; i++){			// red LED will flash a couple of times
				P1OUT ^= RED_LED;			// toggle red LED
				__delay_cycles(100000);
			}
			P1OUT &= ~RED_LED;				// Turn off red LEDs after flashing it once		
		}	
	}else {
		blink_once =  true;
	}
}
/***************************************************
 * MAIN - program starts here
 ***************************************************/
int main(void)
{
	// Stop Watchdog Timer
    	WDTCTL = WDTPW + WDTHOLD;
	
	// select I/O function
	P1SEL  &= ~(BUTTON5 + BUTTON6 + RED_LED + GREEN_LED + BLUE_LED + YELLOW_LED);	
	P1SEL2 &= ~(BUTTON5 + BUTTON6 + RED_LED + GREEN_LED + BLUE_LED + YELLOW_LED);	
	
	// set pin directions
	P1DIR |= (RED_LED + GREEN_LED + BLUE_LED + YELLOW_LED);	// output pins 
	P1DIR &= ~(BUTTON5 + BUTTON6);				// input pins

	// Ennable pull-resistors - keep pin high until pressed
	P1REN |= (BUTTON5 + BUTTON6);		

	// LEDs are initially turned off
	P1OUT &= ~(RED_LED + GREEN_LED + BLUE_LED + YELLOW_LED);

	P1IE |= BUTTON5;	// Enable interrupt on Port 1 for pin P1.5
	P1IES |= BUTTON5;	// Trigger interrupt on falling edge - High/Low transition
	P1IFG &= ~BUTTON5;	// Clear interrupt flag

	__enable_interrupt();	// enable all interrupts

/***************************************************
 * Do this forever
 ***************************************************/
	while(1)
	{
		// get button status
		// PB5_switch = (((P1IN & BUTTON5)==0)?true:false);
		PB6_switch = (((P1IN & BUTTON6)==0)?true:false);
		
		redLedLogic    = ((PB5_switch) && (PB6_switch == false));
		greenLedLogic  = (PB6_switch && (PB5_switch == false));
		blueLedLogic   = (PB5_switch && PB6_switch);		
		yellowLedLogic = (greenLedLogic || blueLedLogic && ~redLedLogic);
		
		// on PB5 button interrupt, flash red LED
		__blinkRed();    

		/*
		// flash red LED - do it once
		if ((redLedLogic)){						// Only PB5 should turn red LED on	
			if (blink_once)	{
				blink_once =  false;	
				P1OUT &= ~(BLUE_LED + GREEN_LED + YELLOW_LED); 	// Other LEDs are off when red LED is on
				
				unsigned char i;				
				for (i = 0; i < 10; i++){			// red LED will flash a couple of times
					P1OUT ^= RED_LED;			// toggle red LED
					__delay_cycles(100000);
				}
				P1OUT &= ~RED_LED;				// Turn off red LEDs after flashing it once		
			}	
		}else {
			blink_once =  true;
		}
		*/

		// Button (PB6) activates the Green LED
		if ((greenLedLogic)) { 				// Only PB6 should turn red LED on
			P1OUT &= ~(RED_LED + BLUE_LED);		// red and blue LEDs are turned off	
			P1OUT |= GREEN_LED;			// green LED is on while button PB6 is pressed
		}
		else
			P1OUT &= ~GREEN_LED;			// green LED goes off when button PB6 is not pressed
	
		// Button (PB5 and PB6) activates the Blue LED
		if ((blueLedLogic)) {				// Both PB5 and PB6 turn on blue LED
			P1OUT &= ~(RED_LED + GREEN_LED);	// red and grren LEDs are turned off
			P1OUT |= BLUE_LED;			// blue LED is on while both PB5 and PB6 are pressed;
		}
		else{
			P1OUT &= ~BLUE_LED;			// green LED goes off when either PB5 or PB6 is not released
		}

		// Turn yellow LED on only when red LED is off
		if (yellowLedLogic == false){
			P1OUT |= YELLOW_LED;
		}
 
	}
}


// Port 1 interrupt vector
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
	P1IFG &= ~BUTTON5;		// clear pin pin interrupt
	
	if (mode == false)
	{	// when  Port1 interrupt occurs
		PB5_switch = true;
		mode = true;
		P1IES &= ~BUTTON5;	// set pin interrupt to trigger on rising edge	
	}
	else  // Debounce - switch status set to false on button released
	{
		PB5_switch = false;
		mode = false;
		P1IES |= BUTTON5;	// set pin interrupt to trigger on falling edge
	}	
}


