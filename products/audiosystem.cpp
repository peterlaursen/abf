/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This contains the implementation of the AudioSystem class.
*/
#include "audiosystem.h"
#include <string>
#include "dsaudio.h"
namespace ABF {
using std::string;
AudioSystem* AudioSystem::AS = nullptr;
AudioSystem* AudioSystem::Create(AbfDecoder* AD, string DevName) {
AS = new DSAudio();
AS->Init(AD);
return AS;
}
void AudioSystem::Init(AbfDecoder* _AD) { AD = _AD; }
}
