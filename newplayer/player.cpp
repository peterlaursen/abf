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
void* thread(void*) {
int kq = kqueue();
struct kevent kev;
EV_SET(&kev, 0, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT, 0, 0, 0);
char key;
while (key != 'q') {
kevent(kq, &kev, 1, &kev, 1, NULL);
key = getchar();
printf("Received event.\n");
}
PS = Quit;
return nullptr;
}

int main(int argc, char* argv[]) {
cfmakeraw(&term);
tcsetattr(0, TCSANOW, &term);;
AbfDecoder AD(argv[1]);

AudioSystem* Device = AudioSystem::Create(&AD);
pthread_t threadhandle;
pthread_create(&threadhandle, NULL, thread, NULL);
Device->Play();
pthread_join(threadhandle, NULL);
Device->Stop();

}
