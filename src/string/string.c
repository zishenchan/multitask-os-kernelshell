#include "string.h"

char tolower(char s1)
{
    if (s1 >= 65 && s1 <= 90) // by ASCII table
    {
        s1 += 32;
    }

    return s1;
}

int strlen(const char* ptr)
{
    int i = 0;
    while (*ptr != 0)
    {
        i++;
        ptr++;
    }
    return i; // return any length of string
}

int strnlen(const char* ptr, int max)
{
    int i = 0;
    for (i = 0; i < max; i++)
    {
        if(ptr[i] == 0)
            break;
    }
    return i;
}

int strnlen_terminator(const char* str, int max, char terminator) 
{
    int i = 0;
    for(i = 0; i < max; i++)
    {
        if (str[i] == '\0' || str[i] == terminator)
            break;
    }

    return i;
}

int istrncmp(const char* s1, const char* s2, int n) // doesn't care about capital letters
{
    unsigned char u1, u2;
    while(n-- > 0)
    {
        u1 = (unsigned char)*s1++;
        u2 = (unsigned char)*s2++;
        if (u1 != u2 && tolower(u1) != tolower(u2))
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

int strncmp(const char* str1, const char* str2, int n)
{
    unsigned char u1, u2;

    while(n-- > 0)
    {
        u1 = (unsigned char)*str1++;
        u2 = (unsigned char)*str2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }

    return 0;
}

char* strcpy(char* dest, const char* src)
{
    char* res = dest;
    while(*src != 0)
    {
        *dest = *src;
        src += 1;
        dest += 1;
    }

    *dest = 0x00;// set the final bit as well

    return res;
}

char* strncpy(char* dest, const char* src, int count)
{
    int i = 0;
    for (i = 0; i < count-1; i++)
    {
        if (src[i] == 0x00)
            break;

        dest[i] = src[i];
    }

    dest[i] = 0x00;
    return dest;
}


bool isdigit(char c)
{
    return (c >= 48 && c <= 57);// 57 is from ascii talbe
}

int tonumericdigit(char c)
{
    return c - 48; // convert char to int
}