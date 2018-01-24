/* $Id$
Copyright (C) 2018 Peter Laursen.

This is a new player, built from the ground up to be slightly more efficient than the original player.
*/
#include "../libabf/libabf.h"
#include "audiosystem.h"
#include "unixaudio.h"
#include "player.h"
#include <sys/types.h>
#include <sys/event.h>
#include <pthread.h>
#include <termios.h>
using namespace std;
using namespace ABF;
static termios term;
volatile PlayerStatus PS = Playing;
void HandleInput(char key, ThreadState& TS) {
termios myterm;
tcgetattr(0, &myterm);
cfmakesane(&myterm);
if (key == 'p') {
int MaxNum = TS.AD.GetNumSections();
tcsetattr(0, TCSANOW, &myterm);
int number = 0;
printf("\nType in the section (1-%d): \n", TS.AD.GetNumSections());
scanf("%d", &number);
if (number > 0 && number <= MaxNum) TS.AD.Seek(TS.AD.GetSections()[--number], SEEK_SET);
getchar(); // Remove a newline
tcsetattr(0, TCSANOW, &term);
}
}
void* thread(void* _ABF) {
ThreadState* TS = (ThreadState*)_ABF;
printf("Title: %s\n", TS->AD.GetTitle());
int kq = kqueue();
struct kevent kev;
EV_SET(&kev, 0, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT, 0, 0, 0);
char key;
do {
kevent(kq, &kev, 1, &kev, 1, NULL);
key = getchar();
printf("Received event. Key: %d, %c\n", key, key);
HandleInput(key, *TS);
} while (key != 'q');
PS = Quit;
return nullptr;
}

int main(int argc, char* argv[]) {
if (argc != 2) {
fprintf(stderr, "Usage: %s <filename> ...\n", argv[0]);
return 1;
}
termios oldterm;
tcgetattr(0, &oldterm);
tcgetattr(0, &term);
term.c_lflag &= ~ICANON;
term.c_lflag&=~ECHO;
tcsetattr(0, TCSANOW, &term);
AbfDecoder AD(argv[1]);
AudioSystem* Device = AudioSystem::Create(&AD);
ThreadState TS {AD, *Device};
ThreadType threadhandle;
pthread_create(&threadhandle, NULL, thread, &TS);
Device->Play();
pthread_join(threadhandle, NULL);
Device->Stop();
}
