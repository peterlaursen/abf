#ifndef LINUXCOMPAT_H
#define LINUXCOMPAT_H
#include <sys/termios.h>
#include <sys/types.h>
void cfmakesane(struct termios* t);
#endif

