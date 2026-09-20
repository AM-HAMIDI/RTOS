#include <stdint.h>
#include <stdarg.h>
#include <uart.h>
#include <kprintf.h>

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
    va_list arg_list;
    va_start(arg_list , fmt);

    while(*fmt)
    {
        switch (*fmt)
        {
        case TERMINATOR:
            /* code */
            break;
        case TYPE_IDENTIFIER:

            break;
        case CHAR_IDENTIFIER:
            
            break;
        case STR_IDENTIFIER:

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

