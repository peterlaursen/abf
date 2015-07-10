/* $Id$
$Copyright$

This contains the class definition for the OSS API in use on FreeBSD.
If your system supports another API, make an implementation which you derive from the AudioSystem class.
*/
#ifndef UNIXAUDIO_H
#define UNIXAUDIO_H
#include "../libabf/libabf.h"
#include "audiosystem.h"
namespace ABF {
class UnixAudio : public AudioSystem {
int Device;
int Volume;
public:
UnixAudio();

~UnixAudio();
virtual void Play();
virtual void Stop();
virtual void IncreaseVolume();
virtual void DecreaseVolume();
};
}
#endif
