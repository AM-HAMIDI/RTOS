#include <stdint.h>
#include <stdarg.h>
#include "uart.h"
#include "kprintf.h"

#define TERMINATOR        '\0'
#define TYPE_IDENTIFIER   '%'

#define STR_IDENTIFIER    's'
#define CHAR_IDENTIFIER   'c'
#define INT_IDENTIFIER    'd'
#define UINT_IDENTIFIER   'u'

#define MINUS_SIGN '-'
#define DECIMAL_BASE 10
#define HEXADECIMAL_BASE 16

#define IS_DECIMAL_DIGIT(X) (0 <= X && X <= 9)
#define IS_HEXADECIMAL_DIGIT(X) (0 <= X && X <= 15)
#define CONVERT_DEC_TO_ASCII(DIGIT) ((DIGIT) + '0')
#define CONVERT_HEX_TO_ASCII(DIGIT) (IS_DECIMAL_DIGIT(DIGIT) ? CONVERT_TO_ASCII(DIGIT) : ((DIGIT) + 'a'))

static void print_sint(int32_t num , uint32_t base);
static void print_uint(uint32_t num , uint32_t base);

// Signed integer kprints
static void print_sint(int32_t num , uint32_t base)
{
    if(num < 0)
    {
        uart_putc(MINUS_SIGN);
        num = 0u - num;
    }

    print_uint(num , base);
}

// Unsigned integer kprints
static void print_uint(uint32_t num , uint32_t base)
{   
    if(num == 0)
    {
        uart_putc(CONVERT_DEC_TO_ASCII(0));
        return;
    }

    while(num > 0)
    {
        uart_putc(CONVERT_DEC_TO_ASCII(num % base));
        num /= base;
    }
}

void kprintf(const char* fmt , ...)
{
    va_list ap;
    va_start(ap , fmt);

    for(;*fmt;fmt++)
    {
        if(*fmt != TYPE_IDENTIFIER)
        {
            uart_putc(*fmt);
            continue;
        }
        
        fmt++;

        switch (*fmt)
        {
        case CHAR_IDENTIFIER:
            uart_putc((char)va_arg(ap , int)); // char promoted to int
            break;
        case STR_IDENTIFIER:
            const char* s = va_arg(ap , const char*);
            uart_puts(s ? s : "(null)");
            break;
        case INT_IDENTIFIER:
            int num = va_arg(ap , int);
            print_sint((int32_t)num , DECIMAL_BASE);
            break;
        case UINT_IDENTIFIER:
            unsigned int num = va_arg(ap , unsigned int);
            print_uint((uint32_t)num , DECIMAL_BASE);
            break;  
        default:
            break;
        }
    }
}

