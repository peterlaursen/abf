/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This file contains the class implementation of the OSS API that's in use on FreeBSD.
If your system uses another audio system, derive from the AudioSystem class and make your own implementation.
*/
#include <stdio.h>
#include "unixaudio.h"
#include "audiosystem.h"
#include "player.h"
#include <sys/ioctl.h>
#include "../libabf/libabf.h"
#include <sys/soundcard.h>
#include <fcntl.h>
#include <unistd.h>
namespace ABF {
UnixAudio::UnixAudio() {
#ifdef BLUETOOTH
Device = open("/dev/dspbt", O_WRONLY);
#else
Device = open("/dev/dsp", O_WRONLY);
#endif
int Format = AFMT_S16_NE;
ioctl(Device, SNDCTL_DSP_SETFMT, &Format);
Format = 1;
ioctl(Device, SNDCTL_DSP_CHANNELS, &Format);
Format = 16000;
ioctl(Device, SNDCTL_DSP_SPEED, &Format);
}
UnixAudio::~UnixAudio() { close(Device); }
void UnixAudio::Play() {
IsPlaying = true;
while (!AD->feof() && PS == Playing) {
short Buffer[320];
AD->Decode(Buffer);
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
}
