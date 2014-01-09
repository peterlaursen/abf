#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H
#ifndef WIN32
#include "../libabf/libabf.h"
#else
#include "../libabf/libabf-win.h"
#endif
class AudioSystem {
private:
static AudioSystem* AS;
protected:
ABF::AbfDecoder* AD;
bool IsPlaying;
virtual void Init(ABF::AbfDecoder* AD);
public:
AudioSystem(): AD(nullptr) {}
static AudioSystem* Create(ABF::AbfDecoder* AD);
virtual ~AudioSystem() {}
virtual void Play() = 0;
virtual void Stop() = 0;
bool isPlaying() { return IsPlaying; }
virtual void DecreaseVolume() = 0;
virtual void IncreaseVolume() = 0;
ABF::AbfDecoder* GetDecoder() { return AD; }

};
#endif
