#include <stdint.h>
#include <stdarg.h>
#include "uart.h"
#include "kprintf.h"

#define TERMINATOR         '\0'
#define TYPE_IDENTIFIER    '%'

#define STR_IDENTIFIER     's'
#define CHAR_IDENTIFIER    'c'
#define INT_IDENTIFIER     'd'
#define UINT_IDENTIFIER    'u'
#define HEX_IDENTIFIER     'x'
#define POINTER_IDENTIFIER 'p'

#define MINUS_SIGN         '-'
#define DEC_BASE           10u
#define HEX_BASE           16u

static char digit_to_ascii(uint32_t digit)
{
    return (char)(digit < 10u ? ('0' + digit) : ('a' + (digit - 10u)));
}

static void print_uint(uint32_t num, uint32_t base, uint32_t min_width)
{
    char buffer[32];
    uint32_t i = 0;

    do {                                
        buffer[i++] = digit_to_ascii(num % base);
        num /= base;
    } while (num > 0);

    // Zero left padding
    for (uint32_t pad = i; pad < min_width; pad++)
        uart_putc('0');

    while (i > 0)
        uart_putc(buffer[--i]);
}

static void print_sint(int32_t num, uint32_t base)
{
    uint32_t magnitude = (uint32_t)num;

    if (num < 0) {
        uart_putc(MINUS_SIGN);
        magnitude = 0u - magnitude;
    }
    print_uint(magnitude, base, 0);
}

void kprintf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    for (; *fmt; fmt++) {
        if (*fmt != TYPE_IDENTIFIER) {
            uart_putc(*fmt);
            continue;
        }

        fmt++;

        switch (*fmt) {
            case CHAR_IDENTIFIER:
                uart_putc((char)va_arg(ap, int));      /* char is promoted to int */
                break;
            case STR_IDENTIFIER: {
                const char *s = va_arg(ap, const char *);
                uart_puts(s ? s : "(null)");
                break;
            }           
            case INT_IDENTIFIER:
                print_sint(va_arg(ap, int), DEC_BASE);
                break;
            case UINT_IDENTIFIER:
                print_uint(va_arg(ap, uint32_t), DEC_BASE, 0);
                break;
            case HEX_IDENTIFIER:
                print_uint(va_arg(ap, uint32_t), HEX_BASE, 0);
                break;
            case POINTER_IDENTIFIER:
                uart_puts("0x");
                print_uint((uint32_t)(uintptr_t)va_arg(ap, void *), HEX_BASE, 8);
                break;
            case TYPE_IDENTIFIER:
                uart_putc(TYPE_IDENTIFIER);
                break;
            case TERMINATOR:                         
                va_end(ap);
                return;
            default:
                uart_putc(TYPE_IDENTIFIER);
                uart_putc(*fmt);
                break;
        }
    }
    va_end(ap);
}