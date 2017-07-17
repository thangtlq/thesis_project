                    // Device header
#include "Handler.h"
int32_t servo1 = SERVO_CENUD,servo2 = SERVO_CENLR;


void checkstate(void)
{
	
	if(!strcmp(buffer,"CAMD@"))
	{
		servo1 -= SERVO_DEG;
		if(servo1 <SERVO_MIN) servo1 = SERVO_MIN;
		PWM_Load(MODULE1,PWM_5,SERVO_LOAD,(SERVO_LOAD - servo1-1));
		clear();
	}
	if(!strcmp(buffer,"CAMU@"))
	{
		servo1 += SERVO_DEG;
		if(servo1 > SERVO_MAX) servo1 = SERVO_MAX;
		PWM_Load(MODULE1,PWM_5,SERVO_LOAD,(SERVO_LOAD - servo1-1));
		clear();
	}

	if(!strcmp(buffer,"CAML@"))
	{
		servo2 += SERVO_DEG;
		if(servo2 > SERVO_MAX) servo2 = SERVO_MAX;
		PWM_Load(MODULE1,PWM_4,SERVO_LOAD,(SERVO_LOAD - servo2-1));
		clear();
	}
	if(!strcmp(buffer,"CAMR@"))
	{
		servo2 -= SERVO_DEG ;
		if(servo2 <SERVO_MIN) servo2 = SERVO_MIN;		
		PWM_Load(MODULE1,PWM_4,SERVO_LOAD,(SERVO_LOAD - servo2-1));
		clear();
	}
	if(!strcmp(buffer,"CAMC@"))
	{
		PWM_Load(MODULE1,PWM_5,SERVO_LOAD,(SERVO_LOAD-SERVO_CENUD-1));
		PWM_Load(MODULE1,PWM_4,SERVO_LOAD,(SERVO_LOAD-SERVO_CENLR-1));
		servo1 = SERVO_CENUD;servo2 = SERVO_CENLR;
		clear();
	}
	if(!strcmp(buffer,"MTRF@")) state = FORWARD;
	if(!strcmp(buffer,"MTRB@")) state = BACK;
	if(!strcmp(buffer,"MTRL@")) state = LEFT;
	if(!strcmp(buffer,"MTRR@")) state = RIGHT;
	if(!strcmp(buffer,"MTRS@")) state = STOP;
}
void motor(int32_t left,int32_t right)
{
	uint32_t data;
	if(right <0)
	{
		data |= GPIO_PIN_5;
		right = -right;
	}
	else data |= GPIO_PIN_4;
	if(left <0)
	{
		data |= GPIO_PIN_7;
		left = -left;
	}
	else data |= GPIO_PIN_6;
	GPIO_Write(GPIOC,data);
		PWM_Load(MODULE1,PWM_6,MOTOR_LOAD,MOTOR_LOAD - left-1);
		PWM_Load(MODULE1,PWM_7,MOTOR_LOAD,MOTOR_LOAD - right-1);
}
void run(STATE s)
{
	checkstate();
	switch(s)
	{
		case STOP:
			motor(0,0);
			break;
		case FORWARD:
			if(srf[0] <= 48320) state = STOP;
			else
				motor(MOTOR_LEFT,MOTOR_RIGHT);
			break;
		case RIGHT:
			if(srf[0] <= 48320) state = STOP;
			else
				motor(MOTOR_LEFT,MOTOR_RIGHT/4);
			break;
		case LEFT:
			if(srf[0] <= 48320) state = STOP;
			else
				motor(MOTOR_LEFT/4,MOTOR_RIGHT);
			break;
		case BACK:
			motor(-MOTOR_LEFT,-MOTOR_RIGHT);
			break;
	}
}

int main(void)
{
	int i,j;
	PLL_Init(80000000);
	Systick_Init(MAX_LOAD);
	GPIO_RCGC(GPIOE_BASE);
	GPIO_AinInit(GPIOE,GPIO_PIN_0|GPIO_PIN_1);
	
	ADC_Init(ADC0,ADC_SS_1,ADC_SSPRI_DEFAULT,0x00,ADC_SSMUX_0(3)|ADC_SSMUX_1(2),ADC_SSCTL_END1|ADC_SSCTL_IE1|ADC_SSCTL_IE0);
	NVIC_EnableIRQ(ADC0SS1_IRQn);
	NVIC_SetPriority(ADC0SS1_IRQn,7);
	GPIO_RCGC(GPIOB_BASE);
	GPIO_DinInit(GPIOB,GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,0x01);
	GPIO_IntConfig(GPIOB,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6,0,0,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6);
	GPIO_RCGC(GPIOC_BASE);
	GPIO_DinInit(GPIOC,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7,GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);
	GPIO_RCGC(GPIOF_BASE);
	GPIO_Unlock(GPIOF,GPIO_PIN_0);
	GPIO_AFInit(GPIOF,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_FUNC_5);
	//PWM_SYSCLOCK(PWM_CLOCK_64);
	PWM_Init(MODULE1,PWM_4,PWM_CTRGENA_LC,PWM_CAPMODE_ACTLOAD_HIGH|PWM_CAPMODE_ACTCMPAD_LOW);
	PWM_Init(MODULE1,PWM_5,PWM_CTRGENB_LC,PWM_CAPMODE_ACTLOAD_HIGH|PWM_CAPMODE_ACTCMPBD_LOW);
	PWM_Init(MODULE1,PWM_6,PWM_CTRGENA_LC,PWM_CAPMODE_ACTLOAD_HIGH|PWM_CAPMODE_ACTCMPAD_LOW);
	PWM_Init(MODULE1,PWM_7,PWM_CTRGENB_LC,PWM_CAPMODE_ACTLOAD_HIGH|PWM_CAPMODE_ACTCMPBD_LOW);
	PWM_Load(MODULE1,PWM_4,SERVO_LOAD-1,(SERVO_LOAD - SERVO_CENLR)-1);
	PWM_Load(MODULE1,PWM_5,SERVO_LOAD-1,(SERVO_LOAD - SERVO_CENUD)-1);
	//
	SYSCTL->RCGCTIMER |= 0x01;
	for(j=0;j<100;j++);
	TIMER0->CTL &= ~0x01;
	TIMER0->CFG = 0x00;
	TIMER0->TAMR |= 0x02;
	TIMER0->TAILR = 20000000;
	TIMER0->IMR |= 0x01;
	NVIC_EnableIRQ(TIMER0A_IRQn);
	NVIC_SetPriority(TIMER0A_IRQn,7);
	TIMER0->CTL |= 0x01;
	TIMER0->ICR |= 0x01;
	//
//	GPIO_RCGC(GPIOA_BASE);
//	GPIO_AFInit(GPIOA,GPIO_PIN_0|GPIO_PIN_1,GPIO_FUNC_1);
//	GPIO_Unlock(GPIOA,GPIO_PIN_0);
	GPIO_AFInit(GPIOE,GPIO_PIN_4|GPIO_PIN_5,GPIO_FUNC_1);
	SYSCTL->RCGCUART |= 0x20;
	for(j=0;j<100;j++);
	UART5->CTL &=~0x01;
	UART5->IBRD = 43;
	UART5->FBRD = 26;
	UART5->LCRH = 0x70;
	UART5->IFLS = 0x00;
	UART5->IM  |= (1UL<<4);
	NVIC_EnableIRQ(UART5_IRQn);
	UART5->ICR |= 0x10;
	UART5->CTL |= 0x01;
	while(1)
	{
		run(state);
//		UART5->DR = 'A';
		for(j=0;j<1000;j++);
	}
}