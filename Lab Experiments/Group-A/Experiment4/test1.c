/*

 * Team id: 7A

 * Author: Roshan, Deepanjan Kundu

 * Filename: test1.c

 * Functions: main()

 * Global Variables: None

 */

#include<stdio.h>
#include<stdint.h>
#include<stdbool.h>
#include"inc/hw_memmap.h"
#include"inc/hw_types.h"
#include"driverlib/gpio.h"
#include"driverlib/pin_map.h"
#include"driverlib/sysctl.h"
#include"driverlib/uart.h"
#include"driverlib/adc.h"
#include <string.h>

int main(void) {
	uint32_t ui32ADC0Value[4];
	char msg[22];
	strcpy(msg,"Current Temperature ");

	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;

	// Configure system clock
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	// Configure ADC0 FIFO1 - Read temperature sensor in all steps.
	//						  In last step, after reading, raise interrupt and end sequence
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_TS);
	ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_TS|ADC_CTL_IE|ADC_CTL_END);
	ADCSequenceEnable(ADC0_BASE, 1);

	// Configure UART0 to interface with laptop
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	while(1)
	{
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false)) // Wait for ADC sequence to finish and raise interrupt
		{
		}

		// Get ADC sequence into array
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10; // Convert ADC value to meaningful celsius value
		ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;

		// Print to terminal
		int i;
		for(i=0;i<20;i++)
			UARTCharPut(UART0_BASE,msg[i]);
		UARTCharPut(UART0_BASE,ui32TempValueC/10+'0');
		UARTCharPut(UART0_BASE,ui32TempValueC%10+'0');
		char deg=(char)167;
		UARTCharPut(UART0_BASE,deg);
		UARTCharPut(UART0_BASE,'C');
		UARTCharPut(UART0_BASE,'\r');
		UARTCharPut(UART0_BASE,'\n');
		SysCtlDelay(1000000);
	}
}
