#ifndef UNIXAUDIO_H
#define UNIXAUDIO_H
#include "../libabf/libabf.h"
#include "audiosystem.h"
class UnixAudio : public AudioSystem {
int Device;
int Volume;
public:
UnixAudio();
~UnixAudio();
void Play();
void Stop();
bool isPlaying();
void IncreaseVolume();
void DecreaseVolume();
};
#endif
