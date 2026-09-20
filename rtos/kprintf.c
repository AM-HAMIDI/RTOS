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

// Signed integer kprints
static void print_sint(int32_t num , int32_t base)
{

}

// Unsigned integer kprints
static void print_uint(uint32_t num , uint32_t base)
{

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

            break;
        case UINT_IDENTIFIER:

            break;
        default:
            break;
        }
    }
}

