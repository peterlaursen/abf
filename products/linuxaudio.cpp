/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This file contains the class implementation of the Pulseaudio API that's in use on various Linux distributions.
If your system uses another audio system, derive from the AudioSystem class and make your own implementation.
*/
#include <stdio.h>
#include "linuxaudio.h"
#include "audiosystem.h"
#include "player.h"
#include <pulse/simple.h>
#include "../libabf/libabf.h"
#include <fcntl.h>
#include <unistd.h>
namespace ABF {
LinuxAudio::LinuxAudio() {
/*
Pulseaudio apparently requires the format to be set up in a slightly different way to OSS on FreeBSD.
We do it here.
*/
AudioFormat.format = PA_SAMPLE_S16LE;
AudioFormat.rate = 16000;
AudioFormat.channels = 1;
int Error = 0;
Device = pa_simple_new(NULL, "abfplayer", PA_STREAM_PLAYBACK, NULL, "ABF Audio Book", &Format, NULL, NULL, &Error); 
}
LinuxAudio::~LinuxAudio() { pa_simple_free(Device); }
void LinuxAudio::Play() {
IsPlaying = true;
while (!AD->feof() && PS == Playing) {
short Buffer[320];
AD->Decode(Buffer);
int Error = 0;
pa_simple_write(Device, Buffer, sizeof(Buffer), Error);
pa_simple_drain(Device, Error);

}
}
void LinuxAudio::Stop() { IsPlaying = false; }
void LinuxAudio::IncreaseVolume() {
printf("IncreaseVolume not Implemented for Pulseaudio.\n");

/*
ioctl(Device, SNDCTL_DSP_GETPLAYVOL, &Volume);
int LeftVolume = Volume & 0x7f;
int RightVolume = (Volume >> 8) & 0x7f;
if (LeftVolume == 100) return;
LeftVolume = RightVolume += 5;
Volume = LeftVolume|(RightVolume<<8);
ioctl(Device, SNDCTL_DSP_SETPLAYVOL, &Volume);
*/
}
void LinuxAudio::DecreaseVolume() {
printf("DecreaseVolume not implemented for Pulseaudio.\n");

/*
ioctl(Device, SNDCTL_DSP_GETPLAYVOL, &Volume);
int LeftVolume = 0, RightVolume = 0;
LeftVolume = Volume & 0x7f;
RightVolume = Volume >> 8 & 0x7f;
if (LeftVolume == 0) return;
LeftVolume = RightVolume -= 5;
Volume = LeftVolume|(RightVolume<<8);
ioctl(Device, SNDCTL_DSP_SETPLAYVOL, &Volume);
*/
}
}
