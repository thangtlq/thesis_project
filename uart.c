#include "uart.h"


void UART_Init(UART0_Type *uart,uint32_t sysclock, uint32_t baud)
{
	uart->CTL &= ~0x01;
	if(uart->CTL & UART_CTL_HSE)
	{
		uart->IBRD = sysclock/(16*baud);
		uart->FBRD =	(uint32_t)(((float)(sysclock/(16*baud)- uart->IBRD))*64+0.5f);
	}
	else	
	{
		uart->IBRD = sysclock/(8*baud);
		uart->FBRD =	(uint32_t)(((float)(sysclock/(8*baud)- uart->IBRD))*64+0.5f);
	}
	uart->LCRH = 0x60;
	uart->IFLS = 0x02;
	uart->CTL = UART_CTL_EN;
}