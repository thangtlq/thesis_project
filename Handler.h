#include "TM4C123.h"                 // Device header
#include "gpio.h"
#include "pwm.h"
#include "SysTick.h"
#include "PLL.h"
#include "adc.h"
#include <string.h>
#include <math.h>
/*DEFINE PARAMETER*/
#define MAX 				0x00FFFFFF		//max load systick
#define MAX_LOAD 		0x00FFFFFF		//max load 
#define SERVO_LOAD 	25000					//load servo
#define SERVO_CENLR	1875 					//pwm servo turn 90 deg
#define SERVO_CENUD	1875
#define SERVO_DEG		165						//servo turn 30deg/cmd
#define SERVO_STEP	5						//speed turning of servo
#define SERVO_MAX   2875					//servo max position
#define SERVO_MIN		875						//servo min position
#define MOTOR_LOAD 	25000		//max load motor	
#define MOTOR_LEFT	12500		//speed left motor
#define MOTOR_RIGHT	12300
#define clear()		{int i;for(i=0;i<5;i++) buffer[i] = 0;}
//
typedef enum
{
	STOP,
	RIGHT,
	LEFT,
	BACK,
	FORWARD
}STATE;

/*GOBAL FUNCTION*/
void Trigger(void);
void Send(void);
/*GOBAL VARIANT*/
uint32_t a;
uint32_t adc[2];

char buffer[5];
int length;
STATE state = STOP;
uint32_t tick,srf[3];
uint32_t _tmp[3],_tick[3],cur;
int flag[3];
/*INTERRUP HANDLER*/
void UART5_Handler(void)
{
//	length =0;
	if(UART5->RIS & 0x10)
	{
		while((!(UART5->FR & 0x10)))
		{
			buffer[length] = UART5->DR;
			length++;
			length = length % 5;
			if(UART5->DR == '@') length =0;
		}
		UART5->ICR |= 0x10;
	}
}
void ADC0SS1_Handler(void)
{
	uint32_t status;
	int n;
	status = ADC0->SSFSTAT1;
	n = status & 0x0F;
	adc[n] = ADC0->SSFIFO1;
	ADC0->ISC |= ADC_SS_1;
}
void TIMER0A_Handler(void)
{
	uint32_t status;
	status = TIMER0->RIS;
	if(status & 0x01)
	{
		Send();
		Trigger();
		ADC0->PSSI |= 0x02;
		TIMER0->ICR |= 0x01;
	}
}
//

void Trigger(void)
{
	int i;
	GPIO_IntAct(GPIOB,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,0);
	GPIO_Write(GPIOB,GPIO_PIN_0);
	for(i=0;i<200;i++);
	GPIO_Write(GPIOB,0);
	GPIO_IntAct(GPIOB,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,1);
//	for(i=0;i<10000;i++);
}
void Send(void)
{
	int i,j;
	uint8_t dist;
	UART5->DR = 'S';
	for(i=0;i<3;i++)
	{
		dist = (uint8_t)(srf[i]*0.000207f);
		UART5->DR = dist/100 + 48;
		dist %= 100; 
		UART5->DR = dist/10 +48;
		dist %= 10;
		UART5->DR = dist + 48;
	}
	UART5->DR = 'T';
	dist = adc[0]*0.08f;
	UART5->DR = dist/10 +48;
		dist %= 10;
	UART5->DR = dist +48;
	dist = 1036.5f/pow((4096/adc[1]-1),2.392f);
	UART5->DR = dist/1000 +48;
		dist %= 1000;
	UART5->DR = dist/100 +48;
		dist %= 100;
	UART5->DR = dist/10 +48;
		dist %= 10;
	UART5->DR = dist +48;
}


void SysTick_Handler(void)
{
	tick++;
}

void GPIOB_Handler(void)
{
	unsigned status;
	status = GPIOB->RIS;
	cur = SysTick->VAL;
	if(status & GPIO_PIN_4)
	{
		if(flag[0])
		{
			srf[0] = MAX*(tick - _tick[0]) + _tmp[0] - cur;
			flag[0] = 0;	
		}
		else
		{
			_tmp[0] = cur;
			_tick[0] = tick;
			flag[0] = 1;
		}
		GPIOB->ICR |= GPIO_PIN_4;
	}
	else if(status & GPIO_PIN_5)
	{
		if(flag[1])
		{
			srf[1] = MAX*(tick - _tick[1]) + _tmp[1] - cur;
			flag[1] = 0;	
		}
		else
		{
			_tmp[1] = cur;
			_tick[1] = tick;
			flag[1] = 1;
		}
		GPIOB->ICR |= GPIO_PIN_5;
	}
	else if(status & GPIO_PIN_6)
	{
		if(flag[2])
		{
			srf[2] = MAX*(tick - _tick[2]) + _tmp[2] - cur;
			flag[2] = 0;	
		}
		else
		{
			_tmp[2] = cur;
			_tick[2] = tick;
			flag[2] = 1;
		}
		GPIOB->ICR |= GPIO_PIN_6;
	}
}