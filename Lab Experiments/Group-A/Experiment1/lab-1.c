/*

 * Team id: 7A

 * Author: Roshan, Deepanjan Kundu

 * Filename: lab-1.c

 * Functions: setup(), ledPinConfig(), switchPinConfig(), main()

 * Global Variables: sw2count

 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

/*
 ------ Global Variable Declaration
 */
int sw2count = 0;

/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

 * Function Name: ledPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

 * Example Call: ledPinConfig();

 */
void ledPinConfig(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);  // Pin-1 of PORT F set as output. Modifiy this to use other 2 LEDs.
}

/*

 * Function Name: switchPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 0 and Pin 4 as input. Note that Pin 0 is locked.

 * Example Call: switchPinConfig();

 */
void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4 | GPIO_PIN_0,GPIO_DIR_MODE_IN);
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4 |GPIO_PIN_0 ,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);
}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	uint8_t sw1press=0;
	uint8_t sw2press=0;
	/*---------------------*/
	uint8_t ui8LED = 2;
	while(1)
	{
		// Check for SW2 press and wait till release
		while(!GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)){
			sw2press=1;
		}

		// Check for SW1 press and turn on the LED till release
		while(!GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4))
		{
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
			sw1press=1;
		}

		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);

		// If SW2 was pressed increment counter
		if(sw2press == 1) {
			sw2count++;
			sw2press=0;
		}

		// If SW1 was presses cycle through Red, Green and Blue LEDs
		if(sw1press == 1) {
			sw1press=0;
			if (ui8LED == 8){
				ui8LED = 2;
			}
			else{
				ui8LED = ui8LED*2;
			}
		}

	}

}
