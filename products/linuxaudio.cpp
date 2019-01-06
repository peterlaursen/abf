/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 Peter Laursen.

This file contains the class implementation of the Pulseaudio API that's in use on various Linux distributions.
If your system uses another audio system, derive from the AudioSystem class and make your own implementation.
*/
#include <iostream>
#include "linuxaudio.h"
#include "audiosystem.h"
#include "player.h"
#include <pulse/simple.h>
#include "../abfdecoder/abfdecoder.h"
#include <fcntl.h>
#include <string>
namespace ABF {
using std::string;
using std::cerr;
using std::endl;
LinuxAudio::LinuxAudio(string DevName) {
/*
Pulseaudio apparently requires the format to be set up in a slightly different way to OSS on FreeBSD.
We do it here.
*/
AudioFormat.format = PA_SAMPLE_S16LE;
AudioFormat.rate = AD->GetSamplingRate();
AudioFormat.channels = 1;
int Error = 0;
Device = pa_simple_new(NULL, "abfplayer", PA_STREAM_PLAYBACK, NULL, "ABF Audio Book", &AudioFormat, NULL, NULL, &Error); 
}
LinuxAudio::~LinuxAudio() {
pa_simple_free(Device); 
}
void LinuxAudio::Play() {
IsPlaying = true;
while (!AD->feof() && PS == Playing) {
int FrameSize = AD->GetSamplingRate()/50;
short Buffer[FrameSize];
AD->Decode(Buffer);
int Error = 0;
pa_simple_write(Device, Buffer, sizeof(Buffer), &Error);
}
}
void LinuxAudio::Stop() { IsPlaying = false; }
void LinuxAudio::IncreaseVolume() {
cerr << "IncreaseVolume not Implemented for Pulseaudio" << endl;
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
cerr << "DecreaseVolume not implemented for Pulseaudio." << endl;
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
