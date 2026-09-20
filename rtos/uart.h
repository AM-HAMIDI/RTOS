#ifndef UART_H
#define UART_H

#define UART0_BASE 0x4000c000u
#define UART0_DR_OFFSET 0x00u
#define UART0_FR_OFFSET 0x18u
#define UART0_DR *(volatile uint32_t*)(UART0_BASE + UART0_DR_OFFSET)
#define UART0_FR *(volatile uint32_t*)(UART0_BASE + UART0_FR_OFFSET)
#define UART_FR_TXFF (1<<5)

void uart_putc(char c);
void uart_puts(const char *s);

#endif