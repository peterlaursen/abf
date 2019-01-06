/* $Id$

This file contains compatibility functions for FreeBSD.
*/

#ifndef LINUXCOMPAT_H
#define LINUXCOMPAT_H
#include <termios.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
int getch();
int kbhit();
#endif

