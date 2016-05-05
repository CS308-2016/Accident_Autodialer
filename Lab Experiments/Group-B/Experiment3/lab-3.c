/*
▪ *
▪ * Team Id: Team 7B
▪ * Author List: Sanket Kanjalkar(120050011), 
				 Karan Ganju(120050021)
▪ * Filename: lab-3.c
▪ * Functions: change_intensity(int)
▪ * Global Variables: mode, count, check, lpress, man_state, ui8_Adjust, ui32Load, rate, colour>
*/

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

int mode, count, check, lpress;
int man_state;
volatile uint8_t ui8Adjust[3];
volatile uint32_t ui32Load;
double rate;
int colour;

#define PWM_FREQUENCY 55

/*

 * Function Name: change_intensity(int)

 * Input: int

 * Output: none

 * Description: Change the intensity of the LED light corresponding to input provided depending on whether switch 0 is pressed or switch 1 is pressed

 * Example Call: switchPinConfig(0);

 */

void change_intensity(int i){
	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) {
		ui8Adjust[i]++;
		if (ui8Adjust[i] > 254) ui8Adjust[i] = 254;
	}
	if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
		ui8Adjust[i]--;
		if (ui8Adjust[i] < 10) ui8Adjust[i] = 10;
	}
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust[0] * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust[1] * ui32Load / 1000);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust[2] * ui32Load / 1000);
}

int main(void)
{
	mode = 0;
	man_state = 0;
	volatile uint32_t ui32PWMClock;

	ui8Adjust[0] = 254;
	ui8Adjust[1] = 10;
	ui8Adjust[2] = 10;
	//Main setup
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//PWM and LED configs
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_2);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_3);
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR)  |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	ui32PWMClock = SysCtlClockGet() / 64;
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

	rate = 1;
	double slope = 0.01;
	int some_value = 17000;

	colour = 0;


	while(1)
	{
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00 && GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
			mode = 1;
			man_state = 4;
		}
		//Mode 0 is automatic mode where intensity changes as a function of time periodically
		if (mode == 0){
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00) {
				rate += slope;
				if (rate > 32) rate = 32;
			}
			if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00) {
				rate -= slope;
				if (rate < 1) rate = 1;
			}
			switch(colour){
			case 0:
				ui8Adjust[0] -= rate;
				ui8Adjust[1] += rate;
				if(ui8Adjust[1] > 254 || ui8Adjust[0] < 10) {
					colour = 1;
					ui8Adjust[0] = 10;
					ui8Adjust[1] = 254;
				}
				break;
			case 1:
				ui8Adjust[1] -= rate;
				ui8Adjust[2] += rate;
				if(ui8Adjust[1] < 10 || ui8Adjust[2] > 254) {
					colour = 2;
					ui8Adjust[1] = 10;
					ui8Adjust[2] = 254;
				}
				break;
			case 2:
				ui8Adjust[2] -= rate;
				ui8Adjust[0] += rate;
				if(ui8Adjust[2] < 10 || ui8Adjust[0] > 254) {
					colour = 0;
					ui8Adjust[2] = 10;
					ui8Adjust[0] = 254;
				}
				break;
			}
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust[0] * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust[1] * ui32Load / 1000);
			PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust[2] * ui32Load / 1000);
			SysCtlDelay(100000);
		}
		//Mode 1 is manual state where sitches dictate which LED light has to change intensity and how
		else if(mode == 1){
			if(man_state == 4){
				check = 0;
				count = 0;
				lpress = 0;
				while(!GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)) {
					int temp = 0;
					while(!GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)) {
						lpress++;
						temp = 1;
						SysCtlDelay(1000);
					}
					if (temp == 1 && lpress > some_value) check = 1;
					else if (temp == 1) {
						count++;
						temp = 0;
					}
					SysCtlDelay(500);
				}
				if (check == 1) man_state = 3;
				else if (count == 1) man_state = 1;
				else if (count >= 2) man_state = 2;
			}
			else {
				change_intensity(man_state - 1);
			}
			SysCtlDelay(100000);
		}
	}
}

