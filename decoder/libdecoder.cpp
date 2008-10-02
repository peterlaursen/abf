#include <libdaisy.h>
#include <cstdio>
#include "libdecoder.h"
namespace ABF {
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
while (1) {
// To be continued from this point on.
}
return true;
}
} // End of namespaces
