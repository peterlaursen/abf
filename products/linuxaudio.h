/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 Peter Laursen.

This contains the class definition for the Pulseaudio API in use on various Linux distributions.
If your system supports another API, make an implementation which you derive from the AudioSystem class.
*/
#ifndef LinuxAudio_H
#define LinuxAudio_H
#include <pulse/simple.h>
#include <string>
#include "../abfdecoder/abfdecoder.h"
#include "audiosystem.h"
namespace ABF {
using std::string;
class LinuxAudio : public AudioSystem {
pa_simple* Device;
pa_sample_spec AudioFormat;
int Volume;
virtual void Init(AbfDecoder* _AD);
public:
LinuxAudio(string DevName = "");
~LinuxAudio();
virtual void Play();
virtual void Stop();
virtual void IncreaseVolume();
virtual void DecreaseVolume();
};
}
#endif
