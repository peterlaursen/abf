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
virtual void Play();
virtual void Stop();
virtual void IncreaseVolume();
virtual void DecreaseVolume();
};
#endif
