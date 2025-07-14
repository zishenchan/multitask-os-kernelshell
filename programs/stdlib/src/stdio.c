#include "stdio.h"
#include "multitaskos.h"
#include <stdarg.h>

int putchar(int c)
{
    multitaskos_putchar((char)c);
    return 0;
}

int printf(const char *fmt, ...)// from tutorialspoint.com C function printf manual page
{
    va_list ap;
    const char *p;// two pointer varibale
    char* sval;
    int ival;

    va_start(ap, fmt);
    for (p = fmt; *p; p++)
    {
        if (*p != '%')
        {
            putchar(*p);
            continue;
        }

        switch (*++p)
        {
        case 'i':
            ival = va_arg(ap, int);
            print(itoa(ival));
            break;

        case 's':
            sval = va_arg(ap, char *);
            print(sval);
            break;

        default:
            putchar(*p);
            break;
        }
    }

    va_end(ap);

    return 0;
}