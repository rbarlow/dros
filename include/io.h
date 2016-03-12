#ifndef _KERNEL_IO_H
#define _KERNEL_IO_H

#include <stddef.h>

/* The I/O ports */
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5

/* The I/O port commands */
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

void outb(uint16_t port, uint8_t data);

#endif