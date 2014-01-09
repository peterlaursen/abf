#ifndef DSAUDIO_H
#define DSAUDIO_H
#include <windows.h>
#include <dsound.h>
#include "../libabf/libabf-win.h"
#include "audiosystem.h"
class DSAudio: public AudioSystem {
static int LastPosition;
HWND WindowHandle;
IDirectSoundBuffer8* Buffer;
IDirectSound8* Device;
void SetupWindow();
HRESULT CreateBasicBuffer(IDirectSound8* lpDirectSound, IDirectSoundBuffer8** ppDsb8);
IDirectSoundBuffer8* GetBuffer() { return Buffer; }
ABF::AbfDecoder* GetDecoder() { return AD; }
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
#endif
