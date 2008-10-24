#include "compat.h"
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
static struct termios oldt, newt;
static int peek_character = -1;
int getch() {
int ch;
tcgetattr( STDIN_FILENO, &oldt );
newt = oldt;
newt.c_lflag &= ~( ICANON | ECHO );
tcsetattr( STDIN_FILENO, TCSANOW, &newt );
ch = getchar();
tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
return ch;
}
int kbhit() {
    tcgetattr(0,&oldt);
oldt = newt;
    newt.c_lflag &= ~ICANON;
    newt.c_lflag &= ~ECHO;
    newt.c_lflag &= ~ISIG;
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(0, TCSANOW, &newt);
unsigned char ch;
int nread;
    if (peek_character != -1) {
return 1;
}
    newt.c_cc[VMIN]=0;
    tcsetattr(0, TCSANOW, &newt);
    nread = read(0,&ch,1);
    newt.c_cc[VMIN]=1;
    tcsetattr(0, TCSANOW, &newt);
    if(nread == 1)
    {
        peek_character = ch;
        return 1;
    }
    return 0;
}
