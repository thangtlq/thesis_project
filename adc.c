#include "TM4C123GH6PM.h"
#include "utils.h"
#include "adc.h"


void ADC_Init(ADC0_Type * adc,unsigned char ss,unsigned int pri_ss,unsigned long event_trigger,unsigned long mux,unsigned long ctl)
{
	int i;
	if(adc == ADC0)
		SYSCTL->RCGCADC |= BIT0;
	else if(adc == ADC1)
			SYSCTL->RCGCADC |= BIT1;
	for(i=0;i<100;i++);
	adc->ACTSS &= ~0xF;
	adc->SSPRI |= pri_ss;
	adc->EMUX |= event_trigger;
	adc->SPC |= 0x04;
//	adc->SAC |= 0x04;
	switch(ss)
	{
		case ADC_SS_0:
			adc->SSMUX0 &= 0x00000000;
			adc->SSMUX0 |= mux;
			adc->SSCTL0 |= ctl;
			break;
		case ADC_SS_1:
			adc->SSMUX1 &= 0x00000000;
			adc->SSMUX1 |= mux;
			adc->SSCTL1 |= ctl;
			break;
		case ADC_SS_2:
			adc->SSMUX2 &= 0x00000000;
			adc->SSMUX2 |= mux;
			adc->SSCTL2 |= ctl;
			break;
		case ADC_SS_3:
			adc->SSMUX3 &= 0x00000000;
			adc->SSMUX3 |= mux;
			adc->SSCTL3 |= ctl;
			break;
	}
	adc->IM |= ss;
	adc->ACTSS |= ss;
}
void ADC_SS0_Read(ADC0_Type * adc,unsigned long * ptrvalue,int num)
{
	int i,n;
	adc->PSSI |= ADC_SS_0;
	if(num > 8) n = 8;
	else n = num;
	while(!((adc->SSFSTAT0) & BIT8));
	for(i=0;i<n;i++)
	{
		ptrvalue[adc->SSFSTAT0 & 0x0f] = adc->SSFIFO0;
	}
	adc->ISC |= ADC_SS_0;
}
void ADC_SS1_Read(ADC0_Type * adc,unsigned long * ptrvalue,int num)
{
	int i,n;
	adc->PSSI |= ADC_SS_1;
	if(num > 4) n =4;
	else n = num;
	while(!((adc->SSFSTAT1) & BIT8));
	for(i=0;i<n;i++)
	{
		ptrvalue[adc->SSFSTAT1 & 0x0f] = adc->SSFIFO1;
	}
	adc->ISC |= ADC_SS_1;
}
void ADC_SS2_Read(ADC0_Type * adc,unsigned long * ptrvalue,int num)
{
	int i,n;
	adc->PSSI |= ADC_SS_2;
	if(num > 4) n =4;
	else n = num;
	while(!((adc->SSFSTAT2) & BIT8));
	for(i=0;i<n;i++)
	{
		ptrvalue[adc->SSFSTAT2 & 0x0f] = adc->SSFIFO2;
	}
	adc->ISC |= ADC_SS_2;
}
void ADC_SS3_Read(ADC0_Type * adc,unsigned long * ptrvalue,int num)
{
	int i;
	adc->PSSI |= ADC_SS_3;
	while(!((adc->SSFSTAT3) & BIT8));
	*ptrvalue = adc->SSFIFO3;
	adc->ISC |= ADC_SS_3;
}