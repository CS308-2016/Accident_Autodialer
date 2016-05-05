#define TARGET_IS_BLIZZARD_RB1

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "uartstdio.h"
#include "sensorlib/hw_mpu9150.h"
#include "sensorlib/hw_ak8975.h"
#include "sensorlib/i2cm_drv.h"
#include "sensorlib/ak8975.h"
#include "sensorlib/mpu9150.h"
#include "sensorlib/comp_dcm.h"
#include "string.h"
#include <math.h>

volatile bool g_bMPU9150Done;
volatile bool SIM908_status = false;
volatile char buf[500];
volatile int it=0;
char command[200];
//
// The function that is provided by this example as a callback when MPU9150
// transactions have completed.
//

void print_str(char a[], int x)
{
	int i=0;
	while(a[i]!='\0')
	{
		if(x == 0)UARTCharPut(UART0_BASE, a[i]);
		if(x == 2)UARTCharPut(UART2_BASE, a[i]);
		if(x == 3)UARTCharPut(UART3_BASE, a[i]);
		i++;
	}
}

char* itoa(int i, char b[]){
	char const digit[] = "0123456789";
	char* p = b;
	if(i<0){
		*p++ = '-';
		i *= -1;
	}
	int shifter = i;
	do{ //Move to where representation ends
		++p;
		shifter = shifter/10;
	}while(shifter);
	*p = '\0';
	do{ //Move back, inserting digits as u go
		*--p = digit[i%10];
		i = i/10;
	}while(i);
	return b;
}

void
MPU9150Callback(void *pvCallbackData, uint_fast8_t ui8Status)
{
	//
	// See if an error occurred.
	//
	if(ui8Status != I2CM_STATUS_SUCCESS)
	{
		//
		// An error occurred, so handle it here if required.
		//
	}
	//
	// Indicate that the MPU9150 transaction has completed.
	//
	g_bMPU9150Done = true;
}
//
// The MPU9150 example.
//

void
ConfigureUART(void)
{
	//
	// Enable the GPIO Peripheral used by the UART.
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	//
	// Enable UART0
	//
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);

	//
	// Configure GPIO Pins for UART mode.
	//
	ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
	ROM_GPIOPinConfigure(GPIO_PA1_U0TX);

	//GSM MOUDLE
	//    ROM_GPIOPinConfigure(GPIO_PD6_U2RX);
	//    ROM_GPIOPinConfigure(GPIO_PD7_U2TX);

	ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
	ROM_GPIOPinConfigure(GPIO_PC7_U3TX);


	ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);
	//    ROM_GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	//
	// Use the internal 16MHz oscillator as the UART clock source.
	//
	UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
	//    UARTClockSourceSet(UART2_BASE, UART_CLOCK_PIOSC);
	//    UARTClockSourceSet(UART3_BASE, UART_CLOCK_PIOSC);
	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 9600, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	//
	// Initialize the UART for console I/O.
	//
	UARTStdioConfig(0, 115200, 16000000);

	IntEnable(INT_UART3);

	UARTIntEnable(UART3_BASE, UART_INT_RX | UART_INT_RT);
}

tI2CMInstance sI2CInst;
void
MPU9150I2CIntHandler(void)
{
	//
	// Pass through to the I2CM interrupt handler provided by sensor library.
	// This is required to be at application level so that I2CMIntHandler can
	// receive the instance structure pointer as an argument.
	//
	I2CMIntHandler(&sI2CInst);
}


void send_AT_command(char* cmd, char* copybuf)
{
	print_str(cmd,3);
	//	print_str("YO\n",0);
	print_str("\r\n",3);
	//	print_str("YO\n",0);
	while(!SIM908_status){}
	//  print_str(buf,0);
	if(copybuf != NULL) strcpy(copybuf, buf);
	it=0;
	SIM908_status = false;
	SysCtlDelay(SysCtlClockGet()/3);
	// while(j--)
	// {
	//   int i=0;
	//   while(!UARTCharsAvail(UART3_BASE)) {};
	//   while(UARTCharsAvail(UART3_BASE)) //loop while there are chars
	//   {
	//     char x = UARTCharGet(UART3_BASE);
	//     UARTCharPut(UART0_BASE, x);
	//     s[i] = x;
	//     i++;
	//   }
	// }
}

void UARTIntHandler(void)
{
	//	UARTCharPut(UART0_BASE, 'a');
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART3_BASE, true); //get interrupt status
	//	UARTCharPut(UART0_BASE, 'a');
	UARTIntClear(UART3_BASE, ui32Status); //clear the asserted interrupts

	//	UARTCharPut(UART0_BASE, 'a');
	while(UARTCharsAvail(UART3_BASE)) //loop while there are chars
	{
		//		UARTCharPut(UART0_BASE, 'a');
		char x = UARTCharGetNonBlocking(UART3_BASE);
		UARTCharPut(UART0_BASE, x);
		buf[it++] = x;
	}
	buf[it]='\0';
	char *ptr = strstr(buf,"OK\r\n");
	if(ptr != NULL) {
		SIM908_status = true;
	}
	//	UARTCharPutNonBlocking(UART0_BASE, 'a');
	//  buf[it]='\0';
	//
	//  if(strncmp(buf, "OK", 2) == 0) SIM908_status = true;
	//  else if(strncmp(buf,  "ERROR", 5) == 0) {
	//
	//  }
	//  else {
	//
	//  }
}

float thres = 1000;
float max_avg = 1;

int driver_rating(float param)
{
	int rating = 6-param/max_avg;
	return rating<0?0:(rating>5?5:rating);
}
float check_acc(float fAccel[],float backup[])
{
	float val=0;
	int i=0;
	//	float temp;
	//	*side = 0;
	//	*sign = 0;
	for(;i<3;i++)
	{
		//		if(i == 0 && i < 2) temp= (backup[i] - fAccel[i])*(backup[i] - fAccel[i]);
		//		else if(i<2 && temp < (backup[i] - fAccel[i])*(backup[i] - fAccel[i]))
		//		{
		//			temp = (backup[i] - fAccel[i])*(backup[i] - fAccel[i]);
		//			*side = i;
		//		}
		val+=(backup[i] - fAccel[i])*(backup[i] - fAccel[i]);
	}
	//	if(backup[*side] > fAccel[*side])*sign = 1;
	return val;
}



int is_int(float fAccel[],float fGyro[])
{
	return 1;
}

void ConfigureGPRS(){
	send_AT_command("AT",NULL);
	send_AT_command("AT+CMGF=1",NULL);
	//  send_AT_command("AT+CIPSHUT");
	//  send_AT_command("AT+CIPMUX=0");
	send_AT_command("AT+CGDCONT=1,\"IP\",\"www\"",NULL);
	send_AT_command("AT+CGACT=1,1",NULL);
	send_AT_command("AT+CGATT=1",NULL);
	send_AT_command("AT+CSTT",NULL);
	//  send_AT_command("AT+CIPSTATUS");
	send_AT_command("AT+CIICR",NULL);
	//  send_AT_command("AT+CIFSR");
	send_AT_command("AT+SAPBR=1,1",NULL);
	send_AT_command("AT+HTTPINIT",NULL);
}

void send_accident_data(int side, int sign) {
	// Get GPS coordinates
	char coords[100];

	//		UARTCharPut(UART0_BASE, 'a');
	send_AT_command("AT+CGPSINF=32", coords);
	//		UARTCharPut(UART0_BASE, 'a');
	char *x = strstr(coords,"AT+CGPSINF=32\r\n");
	x = strstr(x+15,"\r\n");
	x=x+2;
	char *y = strstr(x,"\r");
	*y =  '\0';


	UARTprintf(x);

	// Send GPRS
	memset(command,0,200);
	UARTprintf("Sending data to server\r\n");
	strcpy(command,"AT+HTTPPARA=\"URL\",\"embedded-roshanroshan.rhcloud.com/add/location=");
	strcpy(command+strlen(command),x);
	strcpy(command+strlen(command),"&side=");
	itoa(side, command+strlen(command));
	strcpy(command+strlen(command),"&sign=");
	itoa(sign, command+strlen(command));
	strcpy(command+strlen(command), "\"");
	send_AT_command(command,NULL);
	send_AT_command("AT+HTTPACTION=0",NULL);

	// Send SMS
//	memset(command,0,200);
//	strcpy(command,"AT+CMGS=\"+919769562944\"\r\n");
//	print_str(command,3);
//	memset(command,0,200);
//	strcpy(command,"\"Accident has occured!!! Coordinates are \n");
//	strcpy(command+strlen(command),x);
//	strcpy(command+strlen(command),"\"");
//	command[strlen(command)+1] = '\0';
//	command[strlen(command)] = 26;
//	while(true){
//		char *ptr = strstr(buf,">");
//		if(ptr != NULL) break;
//	}
//	send_AT_command(command,NULL);
	SIM908_status = false;
}

void main (void)
{

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	ROM_SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ | SYSCTL_OSC_MAIN);
	IntMasterEnable();
	ConfigureUART();
	ConfigureGPRS();

	float fAccel[3];
	tMPU9150 sMPU9150;

	UARTprintf("Point 0\n");
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C3);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	ROM_GPIOPinConfigure(GPIO_PD0_I2C3SCL);
	ROM_GPIOPinConfigure(GPIO_PD1_I2C3SDA);

	//
	// Select the I2C function for these pins.  This function will also
	// configure the GPIO pins pins for I2C operation, setting them to
	// open-drain operation with weak pull-ups.  Consult the data sheet
	// to see which functions are allocated per pin.
	//

	GPIOPinTypeI2CSCL(GPIO_PORTD_BASE, GPIO_PIN_0);
	ROM_GPIOPinTypeI2C(GPIO_PORTD_BASE, GPIO_PIN_1);

	//
	// Initialize the MPU9150.  This code assumes that the I2C master instance
	// has already been initialized.
	//
	I2CMInit(&sI2CInst, I2C3_BASE, INT_I2C3, 0xff, 0xff,
			ROM_SysCtlClockGet());

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
	UARTprintf("Point 2\n");
	g_bMPU9150Done = false;
	MPU9150Init(&sMPU9150, &sI2CInst, 0x68, MPU9150Callback, 0);
	while(!g_bMPU9150Done)
	{
	}
	//
	// Configure the MPU9150 for +/- 4 g accelerometer range.
	//

	UARTprintf("Point 3\n");
	g_bMPU9150Done = false;
	//  MPU9150ReadModifyWrite(&sMPU9150, MPU9150_O_ACCEL_CONFIG,
	//      ~MPU9150_ACCEL_CONFIG_AFS_SEL_M,
	//      MPU9150_ACCEL_CONFIG_AFS_SEL_16G, MPU9150Callback,
	//      0);

	sMPU9150.pui8Data[0] = MPU9150_CONFIG_DLPF_CFG_94_98;
	sMPU9150.pui8Data[1] = MPU9150_GYRO_CONFIG_FS_SEL_250;
	sMPU9150.pui8Data[2] = (MPU9150_ACCEL_CONFIG_ACCEL_HPF_5HZ |
			MPU9150_ACCEL_CONFIG_AFS_SEL_16G);
	MPU9150Write(&sMPU9150, MPU9150_O_CONFIG, sMPU9150.pui8Data, 3,
			MPU9150Callback, 0);
	//  while(1){}
	while(!g_bMPU9150Done)
	{
	}
	//
	// Loop forever reading data from the MPU9150.  Typically, this process
	// would be done in the background, but for the purposes of this example,
	// it is shown in an infinite loop.
	//
	int count=0;
	int prev_count=-100;
	int is_acc=0;
	float imp[3];
	int j=0;
	for(;j<2;j++) imp[j]=0;
	float curr_avg=0;

	while(1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
		//
		// Request another reading from the MPU9150.
		//
		g_bMPU9150Done = false;
		if(!MPU9150DataRead(&sMPU9150, MPU9150Callback, 0)) continue;
		while(!g_bMPU9150Done)
		{
		}
		//
		// Get the new accelerometer, gyroscope, and magnetometer readings.
		//

		float backup[3];
		int l=0;
		if(count!=0)for(;l<3;l++)backup[l]=fAccel[l];
		MPU9150DataAccelGetFloat(&sMPU9150, &fAccel[0], &fAccel[1],
				&fAccel[2]);
		//   MPU9150DataGyroGetFloat(&sMPU9150, &fGyro[0], &fGyro[1], &fGyro[2]);
		//    MPU9150DataMagnetoGetFloat(&sMPU9150, &fMagneto[0], &fMagneto[1],
		//        &fMagneto[2]);
		//    float factor = 0.0011970964;

		// UARTprintf("%d %d %d ",(int)(fMagneto[0]*1000), (int)(fMagneto[1]*1000),(int)(fMagneto[2]*1000));
		//   UARTprintf("Accel %d %d %d \n",(int)(fAccel[0]*1000), (int)(fAccel[1]*1000),(int)(fAccel[2]*1000));
		//   if(count ==0)UARTprintf("\n");
		//   UARTprintf("Gyro %d %d %d \n",(int)(fGyro[0]*1000), (int)(fGyro[1]*1000),(int)(fGyro[2]*1000));
		// int iter;
		//    for(iter=0;iter<6;iter++)
		//    UARTprintf("%f %f %f \n",fAccel[0]*factor, fAccel[1] *factor,fAccel[2]*factor );

		float temp = check_acc(fAccel, backup);

		curr_avg = curr_avg + (temp - curr_avg)/(count +1);

		if(is_acc && count< prev_count+ 400)
		{
			int j=0;
			for(;j<2;j++)imp[j]+=(fAccel[j] - backup[j]);
		}

		if(is_acc && count == prev_count + 400)
		{
			is_acc = 0;
			UARTprintf("Impulse %d %d %d \n",(int)(imp[0]*1000), (int)(imp[1]*1000),(int)(imp[2]*1000));
			int side = 0;
			int sign=0;
			int j=0, max_imp = 0;
			for(;j<2;j++) if(imp[j]*imp[j] > max_imp) {
				max_imp = imp[j]*imp[j];
				side = j;
				sign = imp[j] > 0 ? 1 : -1;
			}
			send_accident_data(side, sign);
			j=0;
			for(;j<2;j++)imp[j]=0;
		}

		if(count!=0 && temp >= thres && count >= prev_count + 400)
		{
			UARTprintf("Accel %d %d %d ",(int)(fAccel[0]*1000), (int)(fAccel[1]*1000),(int)(fAccel[2]*1000));
			prev_count = count;
			is_acc=1;
		}

		count++;
		if(count % 5000 ==0) UARTprintf("Driver stats : %d\r\n", (int)(curr_avg));
		if(count % 50000 == 0)
		{
			int rating = driver_rating(curr_avg);
			//			UARTprintf("Sending data to server\r\n");
			memset(command,0,200);
			strcpy(command,"AT+HTTPPARA=\"URL\",\"embedded-roshanroshan.rhcloud.com/add/Driver_rating=");
			itoa(rating, command+strlen(command));
			strcpy(command+strlen(command), "\"");
			send_AT_command(command,NULL);
			send_AT_command("AT+HTTPACTION=0",NULL);
			count=0;
			curr_avg = 0;
		}
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		//    SysCtlDelay(5000000);
		//
		// Do something with the new accelerometer, gyroscope, and magnetometer
		// readings.
		//
	}
}
