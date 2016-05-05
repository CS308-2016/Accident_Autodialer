/*

 * Team id: 7A

 * Author: Roshan, Deepanjan Kundu

 * Filename: test1.c

 * Functions: setup(), led_pin_config(), main()

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

 * Function Name: led_pin_config()

 * Input: none

 * Output: none

 * Description: Set PORTF Pin 1, Pin 2, Pin 3 as output.

 * Example Call: led_pin_config();

 */
void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

int main(void) {

	setup();
	led_pin_config();

	uint8_t ui8LED = 2;
	uint32_t ui32ADC0Value[4];
	char msg1[22],msg2[14],msg3[40],msg4[40];
	strcpy(msg1,"Current Temp = ");
	strcpy(msg2,"Set Temp = ");
	strcpy(msg3,"Enter the Temperature : ");
	strcpy(msg4,"Set Temperature updated to ");

	volatile uint32_t ui32TempAvg;
	volatile uint32_t ui32TempValueC;
	volatile uint32_t ui32TempValueF;
	uint32_t ui32SetValueC=25;

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


	int mode=1;
	while(1)
	{
		// Temperature monitor mode
		if(mode==1){
			if (UARTCharsAvail(UART0_BASE)) {
				if(UARTCharGet(UART0_BASE)=='S')mode=2;
			}
			ADCIntClear(ADC0_BASE, 1);
			ADCProcessorTrigger(ADC0_BASE, 1);
			while(!ADCIntStatus(ADC0_BASE, 1, false))
			{
			}
			ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
			ui32TempAvg = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
			ui32TempValueC = (1475 - ((2475 * ui32TempAvg)) / 4096)/10;
			ui32TempValueF = ((ui32TempValueC * 9) + 160) / 5;
			int i;
			for(i=0;i<15;i++)
				UARTCharPut(UART0_BASE,msg1[i]);
			UARTCharPut(UART0_BASE,ui32TempValueC/10+'0');
			UARTCharPut(UART0_BASE,ui32TempValueC%10+'0');
			char deg=(char)167;
			UARTCharPut(UART0_BASE,'0'+119);
			UARTCharPut(UART0_BASE,'C');
			UARTCharPut(UART0_BASE,'\r');
			UARTCharPut(UART0_BASE,'\n');

			for(i=0;i<11;i++)
				UARTCharPut(UART0_BASE,msg2[i]);
			UARTCharPut(UART0_BASE,ui32SetValueC/10+'0');
			UARTCharPut(UART0_BASE,ui32SetValueC%10+'0');
			UARTCharPut(UART0_BASE,'0'+119);
			UARTCharPut(UART0_BASE,'C');
			UARTCharPut(UART0_BASE,'\r');
			UARTCharPut(UART0_BASE,'\n');
			if (ui32SetValueC < ui32TempValueC)
			{
					ui8LED = 2;
			}
			else
			{
					ui8LED = 8;
			}
			GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);

			SysCtlDelay(1000000);
		}
		// Temperature monitor mode
		else if(mode==2){
			int i;
			for(i=0;i<24;i++)
				UARTCharPut(UART0_BASE,msg3[i]);
			char dig1,dig2;
			int count=0;
			while(count<=2){
				if (UARTCharsAvail(UART0_BASE)) {
					count++;
					if(count==1){
						dig1=UARTCharGet(UART0_BASE);
						UARTCharPut(UART0_BASE,dig1);

					}
					if(count==2){
						dig2=UARTCharGet(UART0_BASE);
						UARTCharPut(UART0_BASE,dig2);
					}
				}
			}


			UARTCharPut(UART0_BASE,'\r');
			UARTCharPut(UART0_BASE,'\n');
			for(i=0;i<27;i++)
				UARTCharPut(UART0_BASE,msg4[i]);
			UARTCharPut(UART0_BASE,dig1);
			UARTCharPut(UART0_BASE,dig2);
			char deg=(char)167;
			UARTCharPut(UART0_BASE,deg);
			UARTCharPut(UART0_BASE,'C');
			UARTCharPut(UART0_BASE,'\r');
			UARTCharPut(UART0_BASE,'\n');
			ui32SetValueC=(dig1-'0')*10+dig2-'0';
			mode=1;
		}
	}
}
