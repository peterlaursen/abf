/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Peter Laursen.

This contains the base class definition for an audio playback system.
It is used by the player to produce audio.
We have one implementation in this folder, as this is a BSD player:
unixaudio.cpp/unixaudio.h: Implements the OSS API that's used on FreeBSD.
To implement a new playback system, derive from this class and use the virtual functions.
*/
#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H
#include "../libabf/libabf.h"
namespace ABF {
class AudioSystem {
private:
static AudioSystem* AS;
protected:
const ABF::AbfDecoder* AD;
bool IsPlaying;
virtual void Init(const ABF::AbfDecoder* AD);
public:
AudioSystem(): AD(nullptr), IsPlaying(false) {}
static AudioSystem* Create(const ABF::AbfDecoder* AD);
virtual ~AudioSystem() {}
virtual void Play() = 0;
virtual void Stop() = 0;
const bool isPlaying() const { return IsPlaying; }
virtual void DecreaseVolume() = 0;
virtual void IncreaseVolume() = 0;
const ABF::AbfDecoder* GetDecoder() { return AD; }
};
}
#endif
