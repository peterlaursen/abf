/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This contains the base class definition for an audio playback system.
It is used by the player to produce audio.
We have three different implementations in the tree:
unixaudio.cpp/unixaudio.h: Implements the OSS API that's used on FreeBSD.
linuxaudio.cpp/linuxaudio.h: Implements a Pulseaudio class
dsaudio.cpp/dsaudio.h: Implements the DirectSound API that's in use on Windows.

To implement a new playback system, derive from this class.
*/
#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H
#include "../libabf/libabf.h"
#include <string>
namespace ABF {
using std::string;
class AudioSystem {
private:
static AudioSystem* AS;
protected:
ABF::AbfDecoder* AD;
bool IsPlaying;
virtual void Init(ABF::AbfDecoder* AD);
public:
AudioSystem(string DevName = ""): AD(nullptr) {}
static AudioSystem* Create(ABF::AbfDecoder* AD, string DevName = "");
virtual ~AudioSystem() {}
virtual void Play() = 0;
virtual void Stop() = 0;
const bool isPlaying() const { return IsPlaying; }
virtual void DecreaseVolume() = 0;
virtual void IncreaseVolume() = 0;
ABF::AbfDecoder* GetDecoder() { return AD; }
};
}
#endif
