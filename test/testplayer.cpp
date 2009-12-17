#include <iostream>
#include <libabf.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <fcntl.h>
using namespace std;
using namespace ABF;
int DeviceID;
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
short Buffer[320];
// Try and set up the audio device
DeviceID = open("/dev/dsp", O_WRONLY);
int Format = AFMT_S16_NE;
ioctl(DeviceID, SNDCTL_DSP_SETFMT, &Format);
Format = 1;
ioctl(DeviceID, SNDCTL_DSP_CHANNELS, &Format);
Format = 16000;
ioctl(DeviceID, SNDCTL_DSP_SPEED, &Format);
while (1) {
AD.Decode(Buffer);
write(DeviceID, Buffer, sizeof(Buffer));
}
}


