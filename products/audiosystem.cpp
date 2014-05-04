#include "audiosystem.h"
#ifndef WIN32
#include "../libabf/libabf.h"
#include "unixaudio.h"
#else
#include "../libabf/libabf-win.h"
#include "dsaudio.h"
#endif
using namespace ABF;
AudioSystem* AudioSystem::AS = nullptr;
AudioSystem* AudioSystem::Create(AbfDecoder* AD) {
#ifndef WIN32
AS = new UnixAudio();
#else
AS = new DSAudio();
#endif
AS->Init(AD);
return AS;
}
void AudioSystem::Init(AbfDecoder* _AD) { AD = _AD; }

