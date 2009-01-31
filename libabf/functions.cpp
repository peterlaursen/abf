#ifdef WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "libabf.h"
#include <audiere.h>
#include <speex/speex.h>
#include <speex/speex_resampler.h>
#include <cstdio>
#include <iostream>
using namespace audiere;
using namespace std;
namespace ABF {
SHARED char* DecodeToRawAudio(const char* Filename) {
// Define the variable Processed to be twice as large as Size. This allows for upsampling the audio if such things are necessary.
// It would be more elegant to have a few constants for the array size.
unsigned int Size = 4096;
unsigned int Processed = 8192;
short Resampled[8192];
SampleSourcePtr Source = OpenSampleSource(Filename);
if (!Source) {
cerr << "Error, the file " << Filename << " does not exist." << endl;
return 0;
}
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[4096];
#ifdef WIN32
char* TempFile = tempnam(".\\", "ABFConv");
Â#else
char* TempFile = tempnam("./", "ABFConv");
#endif
FILE* temp = fopen(TempFile, "wb");
while (1) {
unsigned int FramesRead = Source->read(4096, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
fwrite(Resampled, sizeof(short), Processed, temp);
}
speex_resampler_destroy(State);
fclose(temp);
return TempFile;
}
SHARED bool EncodeABF(AbfEncoder& AE, char* TempFile) {
// Check our arguments
if (!TempFile) return false;
FILE* fin = fopen(TempFile, "rb");
if (!fin) return false;
short Input[320];
while (!feof(fin)) {
fread(Input, 2, 320, fin);
AE.Encode(Input);
}
fclose(fin);
#ifdef WIN32
DeleteFile(TempFile);
#else
unlink(TempFile);
#endif
return true;
}
} //  End of namespace
