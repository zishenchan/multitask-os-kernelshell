#ifndef STRING_H
#define STRING_H
#include <stdbool.h>
int strlen(const char* ptr);
bool isdigit(char c);
int tonumericdigit(char c);
int strnlen(const char* ptr, int max);

char* strcpy(char* dest, const char* src);

// update the string functaility
int strncmp(const char* str1, const char* str2, int n);
int istrncmp(const char* s1, const char* s2, int n);
int strnlen_terminator(const char* str, int max, char terminator);
char tolower(char s1);
#endif