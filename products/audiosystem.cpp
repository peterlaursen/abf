/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This contains the implementation of the AudioSystem class.
*/
#include "audiosystem.h"
#ifndef WIN32
#include "../libabf/libabf.h"
#include <string>
#ifndef PULSE
#include "unixaudio.h"
#else
#include "linuxaudio.h"
#endif
#else
#include "dsaudio.h"
#endif
namespace ABF {
using std::string;
AudioSystem* AudioSystem::AS = nullptr;
AudioSystem* AudioSystem::Create(AbfDecoder* AD, string DevName = "") {
#ifndef WIN32
#ifndef PULSE
AS = new UnixAudio();
#else
AS = new LinuxAudio();
#endif

#else
AS = new DSAudio();
#endif
AS->Init(AD);
return AS;
}
void AudioSystem::Init(AbfDecoder* _AD) { AD = _AD; }
}
