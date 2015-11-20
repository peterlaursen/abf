/* $Id$

This file contains compatibility functions for FreeBSD.
*/

#include "compat.h"
#include <stdio.h>
#include <termios.h>
int getch() {
termios old, newt;
tcgetattr(STDIN_FILENO, &old);
newt = old;
newt.c_lflag &=~(ICANON|ECHO);
tcsetattr(STDIN_FILENO, TCSANOW, &newt);
char ch = getchar();  
tcsetattr(STDIN_FILENO, TCSANOW, &old);
return ch;
}
int kbhit() {
termios oldt, newt;
tcgetattr(STDIN_FILENO, &oldt);
newt = oldt;
newt.c_lflag &= ~(ICANON|ECHO);
tcsetattr(STDIN_FILENO, TCSANOW, &newt);
timeval tv;
fd_set rdfs;
tv.tv_sec = tv.tv_usec = 0;
FD_ZERO(&rdfs);
FD_SET(STDIN_FILENO, &rdfs);
select(STDIN_FILENO+1, &rdfs, 0, 0, &tv);
tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
return FD_ISSET(STDIN_FILENO, &rdfs);
}
/*int main() {
printf("Hello world.");
int ch = mygetch();
printf("You typed in %c\n", ch);
}


*/
