#include <stdio.h>
#include "unixaudio.h"
#include "player.h"
#include <sys/ioctl.h>
#include <libabf.h>
#include <sys/soundcard.h>
#include <fcntl.h>
UnixAudio::UnixAudio(): IsPlaying(false), Decoder(0) {
Device = open("/dev/dsp", O_WRONLY);
int Format = AFMT_S16_NE;
ioctl(Device, SNDCTL_DSP_SETFMT, &Format);
Format = 1;
ioctl(Device, SNDCTL_DSP_CHANNELS, &Format);
Format = 16000;
ioctl(Device, SNDCTL_DSP_SPEED, &Format);
}
bool UnixAudio::isPlaying() { return IsPlaying; }
void UnixAudio::Init(ABF::AbfDecoder* AD) { Decoder = AD; }
UnixAudio::~UnixAudio() { close(Device); }
void UnixAudio::Play() {
IsPlaying = true;
while (!Decoder->feof() && PS == Playing) {
short Buffer[320];
Decoder->Decode(Buffer);
write(Device, Buffer, sizeof(Buffer));
}
}
void UnixAudio::Stop() { IsPlaying = false; }
void UnixAudio::IncreaseVolume() {
ioctl(Device, SNDCTL_DSP_GETPLAYVOL, &Volume);
int LeftVolume = Volume & 0x7f;
int RightVolume = (Volume >> 8) & 0x7f;
if (LeftVolume == 100) return;
LeftVolume = RightVolume += 5;
Volume = LeftVolume|(RightVolume<<8);
ioctl(Device, SNDCTL_DSP_SETPLAYVOL, &Volume);
}
void UnixAudio::DecreaseVolume() {
ioctl(Device, SNDCTL_DSP_GETPLAYVOL, &Volume);
int LeftVolume = 0, RightVolume = 0;
LeftVolume = Volume & 0x7f;
RightVolume = Volume >> 8 & 0x7f;

if (LeftVolume == 0) return;
LeftVolume = RightVolume -= 5;

Volume = LeftVolume|(RightVolume<<8);
ioctl(Device, SNDCTL_DSP_SETPLAYVOL, &Volume);
}
