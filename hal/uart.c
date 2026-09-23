#include "uart.h"
#include <stdint.h>

// Wait until FIFO is not full
static void uart_tx(char c)
{
    while(UART0_FR & UART_FR_TXFF) {}
    UART0_DR = (uint32_t)c;
}

void uart_putc(char c)
{
    if(c == '\n')
        uart_tx('\r'); // put \r before \n
    
    uart_tx(c);
}

void uart_puts(const char* s)
{
    while(*s)
    {
        uart_putc(*s++);
    }
}