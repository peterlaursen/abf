/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This file contains the definition of DSAudio - the audio system implementation available on Windows.
*/

#ifndef DSAUDIO_H
#define DSAUDIO_H
#include <windows.h>
#include <dsound.h>
#include "../libabf/libabf.h"
#include "audiosystem.h"
namespace ABF {
class DSAudio: public AudioSystem {
static int LastPosition;
HWND WindowHandle = 0;
IDirectSoundBuffer8* Buffer = 0;
IDirectSound8* Device = 0;
void SetupWindow();
HRESULT CreateBasicBuffer(IDirectSound8* lpDirectSound, IDirectSoundBuffer8** ppDsb8);
IDirectSoundBuffer8* GetBuffer() { return Buffer; }
IDirectSound8* GetDevice() { return Device; }
static int GetLastPosition() { return LastPosition; }
public:
DSAudio();
virtual ~DSAudio();

void Play();
void Stop();
void IncreaseVolume() {}
void DecreaseVolume() {}
};
}
#endif
