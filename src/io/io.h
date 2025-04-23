#ifndef IO_H
#define IO_H

unsigned char insb(unsigned port);// Read a byte from the specified port
unsigned short insw(unsigned port);// Read a word from the specified port
void outb(unsigned port, unsigned char val);// Write a byte to the specified port
void outw(unsigned port, unsigned short val);// Write a word to the specified port

#endif