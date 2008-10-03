#include <libdaisy.h>
#include <cstdio>
#include "libdecoder.h"
#include <audiere.h>
namespace ABF {
using namespace audiere;
Decoder::Decoder(Daisy& _Book, const char* Filename): _Daisy(_Book) {
_Output = fopen(Filename, "wb");
if (!_Output) _Open = false;
else _Open = true;
}
bool Decoder::Decode() {
// We assume that the daisy book is open and that no other smil file functions have been called.
while (_Daisy.OpenSmil()) {
// Get me the name of the MP3 file
const char* Filename = _Daisy.GetMP3FileName();
// Open the audio file we have just been given.
SampleSource* Source = OpenSampleSource(Filename);
if (!Source) return false;
const int ReadSize = 4096;
int Channels, SampleRate;
SampleFormat SF;
Source->getFormat(Channels, SampleRate, SF);
int FrameSize = GetSampleSize(SF) * Channels;
printf("Frame size: %d\r\n", FrameSize);
int FramesRead = 0;
short Buffer[ReadSize];
while (1) {
int FramesRead = Source->read(sizeof(Buffer) / 2, Buffer);
if (FramesRead <= 0) {
printf("Finished decoding %s\r\n", Filename);
break;
}
fwrite(Buffer, sizeof(short), ReadSize, _Output);
}
}
return true;
}
} // End of namespaces
