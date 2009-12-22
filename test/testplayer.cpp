#include <iostream>
#include <libabf.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
#include <pthread.h>
#include "compat.h"
using namespace std;
using namespace ABF;
int DeviceID;
volatile bool Quit = false;
void* SoundSystem(void* AD) {
AbfDecoder ad = *(AbfDecoder*)AD;
short Buffer[320];
// Try and set up the audio device
DeviceID = open("/dev/dsp", O_WRONLY);
int Volume = 100|100 << 8;
ioctl(DeviceID, SNDCTL_DSP_SETPLAYVOL, &Volume);
int Format = AFMT_S16_NE;
ioctl(DeviceID, SNDCTL_DSP_SETFMT, &Format);
Format = 1;
ioctl(DeviceID, SNDCTL_DSP_CHANNELS, &Format);
Format = 16000;
ioctl(DeviceID, SNDCTL_DSP_SPEED, &Format);
while (!ad.feof() && !Quit) {
ad.Decode(Buffer);
write(DeviceID, Buffer, sizeof(Buffer));
}
close(DeviceID);
return 0;
}

int main(int argc, char* argv[]) {
if (argc != 2) {
cerr << "Error, we need an audio book." << endl;
return 1;
}
AbfDecoder AD(argv[1]);
if (!AD.IsValid()) {
cerr << "Error, not valid book." << endl;
return 1;
}
pthread* Thread;
int ThreadID = pthread_create(&Thread, 0, SoundSystem, &AD);
char ch = 0;
int* Sections = AD.GetSections();
int CurrentSection = 0;
while (1) {
if (AD.ftell() > Sections[CurrentSection]) ++CurrentSection;
if (!kbhit()) continue;
printf("I got a keypress.\n");
ch = getch();
if (ch == 'q') break;
if (ch == 'b') {
++CurrentSection;
AD.Seek(Sections[CurrentSection], SEEK_SET);
printf("CurrentSection is now %d\n", CurrentSection);
}
if (ch == '<') {
int Volume = 0;;
ioctl(DeviceID, SNDCTL_DSP_GETPLAYVOL, &Volume);
int LeftVolume = Volume & 0x7f;
int RightVolume = Volume >> 8 & 0x7f;
if (LeftVolume == 0) continue;
LeftVolume = RightVolume -= 5;
Volume = LeftVolume|(RightVolume<<8);
ioctl(DeviceID, SNDCTL_DSP_SETPLAYVOL, &Volume);
}
if (ch == '>') {
int Volume = 0;
int LeftVolume = 0, RightVolume = 0;
ioctl(DeviceID, SNDCTL_DSP_GETPLAYVOL, &Volume);
LeftVolume = Volume & 0x7f;
if (LeftVolume == 100) continue;
LeftVolume = RightVolume = LeftVolume + 5;
Volume = LeftVolume | (RightVolume << 8);
ioctl(DeviceID, SNDCTL_DSP_SETPLAYVOL, &Volume);
}
}
Quit = true;
return 0;
}


