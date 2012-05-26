#ifndef DSAUDIO_H
#define DSAUDIO_H
#include <windows.h>
#include <dsound.h>
#include <libabf.h>
class DSAudio {
static int LastPosition;
HWND WindowHandle;
ABF::AbfDecoder* Decoder;
volatile bool IsPlaying;
IDirectSoundBuffer8* Buffer;
IDirectSound8* Device;
void SetupWindow();
HRESULT CreateBasicBuffer(IDirectSound8* lpDirectSound, IDirectSoundBuffer8** ppDsb8);
public:
DSAudio();
~DSAudio();
void Init(ABF::AbfDecoder* AD);
void Play();
void Stop();
bool isPlaying();
IDirectSoundBuffer8* GetBuffer() { return Buffer; }
ABF::AbfDecoder* GetDecoder() { return Decoder; }
IDirectSound8* GetDevice() { return Device; }
static int GetLastPosition() { return LastPosition; }
};
#endif
