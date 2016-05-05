/*

 * Team id: 7A

 * Author: Roshan, Deepanjan Kundu

 * Filename: lab-2.c

 * Functions: setup(), ledPinConfig(), switchPinConfig(), pwmConfig(),
 			  detectSW1Press(), detectSW2Press(), mode0(), mode1(), main()

 * Global Variables: sw1Status, sw2Status, mode, pin, ui8Adjust, ui32Load, rate

 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

#define PWM_FREQUENCY 55
/*
 ------ Global Variable Declaration
 */
int sw1Status = 0;
int sw2Status = 0;

int mode=0;
int pin=1;
volatile uint8_t ui8Adjust[3] = {250,10,10}; // RBG value for LED
volatile uint32_t ui32Load;
double rate=1.5; // Rate of color change

/*

 * Function Name: setup()

 * Input: none

 * Output: none

 * Description: Set crystal frequency and enable GPIO Peripherals

 * Example Call: setup();

 */
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
}

/*

 * Function Name: ledPinConfig()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 to take values from PWM1 pins 5, 6, 7.

 * Example Call: ledPinConfig();

 */
void ledPinConfig(void)
{
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
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

/*

 * Function Name: pwmConfig()

 * Input: none

 * Output: none

 * Description: Configure PWM pulse of PWM1 on pin 5 with GEN2 and pins 6, 7 with GEN3.
 				Pulse width is set based on RGB values.

 * Example Call: pwmConfig();

 */
void pwmConfig(void)
{
	volatile uint32_t ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);

	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust[0] * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT, true);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust[1] * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT, true);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust[2] * ui32Load / 1000);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT, true);

	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);
}

/*

 * Function Name: detectSW1Press()

 * Input: none

 * Output: 1 if switch press detected, 0 if not

 * Description: State machine for switch 1 press.

 * Example Call: detectSW1Press();

 */
unsigned char detectSW1Press(void) {
	switch (sw1Status) {
	case 0:
		sw1Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) ? 0 : 1;
		break;
	case 1:
		sw1Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) ? 0 : 2;
		if(sw1Status == 2) return 1;
		break;
	case 2:
		sw1Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)) ? 0 : 2;
		break;
	default:
		break;
	}

	return 0;
}

/*

 * Function Name: detectSW2Press()

 * Input: none

 * Output: 1 if switch press detected, 0 if not

 * Description: State machine for switch 2 press.

 * Example Call: detectSW2Press();

 */
unsigned char detectSW2Press(void) {
	switch (sw2Status) {
	case 0:
		sw2Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)) ? 0 : 1;
		break;
	case 1:
		sw2Status = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) ? 0 : 2;
		if(sw2Status == 2) return 1;
		break;
	case 2:
		sw2Status = (GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)) ? 0 : 2;
		break;
	default:
		break;
	}

	return 0;
}

/*

 * Function Name: mode0()

 * Input: none

 * Output: none

 * Description: mode 0 - Change LED color in a cycle R->B->G

 * Example Call: mode0();

 */
void mode0(void) {
	// In this mode, pin indicates the part of the R->B->G cycle the program is in
	switch(pin){
	case 1: // red -> blue
		ui8Adjust[0] -= rate;
		ui8Adjust[1] += rate;
		// On reaching a threshhold, transition to next part of the cycle
		if(ui8Adjust[1] > 250 || ui8Adjust[0] < 10) {
			pin = 2;
			ui8Adjust[1] = 250;
			ui8Adjust[0] = 10;
		}
		break;
	case 2: // blue -> green
		ui8Adjust[1] -= rate;
		ui8Adjust[2] += rate;
		// On reaching a threshhold, transition to next part of the cycle
		if(ui8Adjust[2] > 250 || ui8Adjust[1] < 10) {
			pin = 3;
			ui8Adjust[2] = 250;
			ui8Adjust[1] = 10;
		}
		break;
	case 3: // green -> red
		ui8Adjust[2] -= rate;
		ui8Adjust[0] += rate;
		// On reaching a threshhold, transition to next part of the cycle
		if(ui8Adjust[0] > 250 || ui8Adjust[2] < 10) {
			pin = 1;
			ui8Adjust[0] = 250;
			ui8Adjust[2] = 10;
		}
		break;
	}

	// Set PWM pulse width based on RBG values to change LED color
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust[0] * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust[1] * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust[2] * ui32Load / 1000);
	SysCtlDelay(100000);
}

/*

 * Function Name: mode1()

 * Input: none

 * Output: none

 * Description: mode1 - Manually adjust LED color

 * Example Call: mode1();

 */
void mode1(void) {
	// In this mode, pin indicates the submode
	// 0 - initial, 1 - change red, 2 - change blue, 3 - change green
	if(pin==0) {
		int check = 0; // 1 if SW1 was long pressed while SW2 was pressed
		int sw1press = 0; // number of SW1 presses
		int duration = 0; // switch press duration for detecting long press
		while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0) == 0) {

			int lp = 0; // detect long press of SW1
			while(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4) == 0) {
				duration++; if(duration>20000) lp=1;
				sw1press++;
				SysCtlDelay(1000);
			}
			if (lp==1) check = 1;
			SysCtlDelay(500);
		}
		if (check == 1) pin = 3; // submode 3 if sw1 long pressed
		else if (sw1press == 1) pin = 1; // submode 3 if sw1 pressed once
		else if (sw1press >= 2) pin = 2; // submode 3 if sw1 pressed more than once
	} else {
		// Adjust color value on switch press
		if(detectSW1Press() == 1) {
			ui8Adjust[pin-1]+=10;
			if (ui8Adjust[pin-1] > 250) ui8Adjust[pin-1] = 250;
		}
		if(detectSW2Press() == 1) {
			ui8Adjust[pin-1]-=10;
			if (ui8Adjust[pin-1] < 10) ui8Adjust[pin-1] = 10;
		}

		// Set PWM pulse width based on RBG values to change LED color
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust[0] * ui32Load / 1000);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust[1] * ui32Load / 1000);
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust[2] * ui32Load / 1000);
	}
}

int main(void)
{
	setup();
	ledPinConfig();
	switchPinConfig();
	pwmConfig();
	while(1)
	{
		// Enter/Reset mode1 if both switches pressed
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0) {
			mode = 1;
			pin = 0;
		}

		// In mode0, change rate of color change on switch press
		if(mode==0) {
			if(detectSW1Press() == 1) {
				rate += 0.05;
				if(rate>2) rate=2;
			}
			if(detectSW2Press() == 1) {
				rate -= 0.05;
				if(rate<1) rate=1;
			}
		}

		if(mode==0) {
			mode0();
		} else {
			mode1();
		}
	}
}
