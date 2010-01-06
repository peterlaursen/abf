#ifndef UNIXAUDIO_H
#define UNIXAUDIO_H
#include <libabf.h>
class UnixAudio {
ABF::AbfDecoder* Decoder;
volatile bool IsPlaying;
int Device;
public:
UnixAudio();
~UnixAudio();
void Init(ABF::AbfDecoder* AD);
void Play();
void Stop();
bool isPlaying();
};
#endif
