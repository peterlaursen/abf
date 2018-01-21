/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Peter Laursen.

This contains the implementation of the AudioSystem class.
*/
#include "audiosystem.h"
#include "../libabf/libabf.h"
#include "unixaudio.h"
namespace ABF {
AudioSystem* AudioSystem::AS = nullptr;
AudioSystem* AudioSystem::Create(const AbfDecoder* AD) {
AS = new UnixAudio();
AS->Init(AD);
return AS;
}
void AudioSystem::Init(const AbfDecoder* _AD) { AD = _AD; }
}
