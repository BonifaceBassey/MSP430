/*
	Date   : 04.11.2016
	Boniface Bassey
*/

#include  <msp430.h>
#include "templateEMP.h"

#define RED_LED   BIT4                // red LED on PIN4
#define BLUE_LED BIT5                 // blue LED on PIN5
/*
The blue LED D7 is used to indicate the operational state of the heater. It is controlled from X10.
The heater is operated by writing value 1 at X10 and bridging pin 1 to 2 or 3 on JP4.
In this way, Transistor T2 allows current flow from VBUS necessary to operate the heater, thus lighting
up the blue LED in the process.
*/


volatile unsigned int red_count = 0;
volatile unsigned int blue_count = 0;
char LED_on[25] = "<blue LED status> : ON"; 
char LED_off[25] = "<blue LED status> : OFF";

//***************************************
void counts(){
    __delay_cycles(10000);
    red_count++;
    blue_count++;
}


//***************************************
int main(void){
   /*
    * Initialization of the controller
    * This function sets the clock, stops the watchdog and - if allowed -
    * initialize the USART-machine.
    */
    	initMSP();

    	P1SEL &= ~(RED_LED + BLUE_LED);     // select pin I/O function
    	P1DIR |= RED_LED + BLUE_LED;        // set pins direction to output
    	P1OUT &= ~(RED_LED + BLUE_LED);     // LEDs are initially turned off
    	
	while(1) {                        // forever loop
        
    	counts();

	/*
	Calculation for 2Hz
	MCLK = 1MHz
	Instruction cycle  time = 1us (= 1/MCLK)
	Required delay = (T = 1/2Hz = 0.5s)
	Number of counts = Required delay / Instruction cycle  time (= 500000 i.e 10000*50)
	*/
        if (red_count == 50){         // flash red LED at 2Hz
	    P1OUT ^= RED_LED;          // toggle LED
	    red_count = 0;            // reset the red LED counter
        }
	
	/*
	Calculation for 1Hz
	MCLK = 1MHz
	Instruction cycle  time = 1us (= 1/MCLK)
	Required delay = (T = 1/1Hz = 1s)
	Number of counts = Required delay / Instruction cycle  time (= 1000000 i.e 10000*100)		
	*/
        if (blue_count == 100){       // flash blue LED at half the frequency of the red LED
	    P1OUT ^= BLUE_LED;         // toogle the blue LED
	    blue_count = 0;           // reset the blue LED counter
        }

	//print blue LED status(on/off) to the serial whenever serial input is received
	if (serialRead() != -1)
	{	
	    if(P1IN & BIT5)
		serialPrintln(LED_on);
	    else
		serialPrintln(LED_off);
	}
    }
}
