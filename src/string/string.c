#include "string.h"

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

bool isdigit(char c)
{
    return (c >= 48 && c <= 57);// 57 is from ascii talbe
}

int tonumericdigit(char c)
{
    return c - 48; // convert char to int
}