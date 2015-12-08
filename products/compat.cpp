/* $Id$

This file contains compatibility functions for FreeBSD.
*/

#include "compat.h"
#include <stdio.h>
#include <termios.h>
int getch() {
return getchar();
}
int kbhit() {
timeval tv;
fd_set rdfs;
tv.tv_sec = tv.tv_usec = 0;
FD_ZERO(&rdfs);
FD_SET(STDIN_FILENO, &rdfs);
select(STDIN_FILENO+1, &rdfs, 0, 0, &tv);
return FD_ISSET(STDIN_FILENO, &rdfs);
}
/*int main() {
printf("Hello world.");
int ch = mygetch();
printf("You typed in %c\n", ch);
}


*/
