/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Peter Laursen.

This contains the class definition for the OSS API in use on FreeBSD.
If your system supports another API, make an implementation which you derive from the AudioSystem class.
*/
#ifndef UNIXAUDIO_H
#define UNIXAUDIO_H
#include "../abfdecoder/abfdecoder.h"
#include "audiosystem.h"
#include <string>
namespace ABF {
using std::string;
class UnixAudio : public AudioSystem {
int Device = 0;
int Volume = 0;
int FrameSize = ABF_SAMPLING_RATE/50;
protected:
virtual void Init(ABF::AbfDecoder*);
public:
UnixAudio(string DevName = "");
~UnixAudio();
virtual void Play();
virtual void Stop();
virtual void IncreaseVolume();
virtual void DecreaseVolume();
};
}
#endif
