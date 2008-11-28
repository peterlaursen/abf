#ifdef WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <libabf.h>
#include <libdaisy.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio> // for FILE
#include <cstdlib>
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libdaisy.lib")
#pragma comment(lib, "libspeexdsp_imp.lib")
#pragma comment(lib, "libspeex_static.lib")
using namespace ABF;
using namespace std;
using namespace audiere;
void Decode(Daisy&, AbfEncoder& AE);
void Encode(AbfEncoder& AE, char* Temp);
int main(int argc, char* argv[]) {
if (argc != 3) {
cout << "Usage: " << argv[0] << " <path to daisy book> <output file>" << endl;
return 1;
}
Daisy D(argv[1]);
if (!D.IsValid()) {
cout << "Error, not a valid daisy book." << endl;
return 1;
}
AbfEncoder AE(argv[2]);
string Meta = "dc:title";
string StrTitle = D.ExtractMetaInfo(Meta);
AE.SetTitle(StrTitle.c_str());
Meta = "dc:creator";
string StrAuthor = D.ExtractMetaInfo(Meta);
AE.SetAuthor(StrAuthor.c_str());
Meta = "ncc:totalTime";
string StrTime = D.ExtractMetaInfo(Meta);
AE.SetTime(StrTime.c_str());
AE.SetNumSections(D.GetNumSections());
AE.WriteHeader();
unsigned short File = 0;
while (D.OpenSmil()) {
AE.WriteSection();
++File;
cout << "Converting file " << File << " of " << D.GetNumSections() << endl;
Decode(D, AE);
}
cout << "The book has been converted successfully." << endl;
return 0;
}
void Decode(Daisy& D, AbfEncoder& AE) {
unsigned int Size = 4096, Processed = 8192;
short Resampled[8192];
string Filename = D.GetMP3FileName();
SampleSourcePtr Source = OpenSampleSource(Filename.c_str());
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[4096];
char* TempFile = tempnam(".\\", "ABFConv");
FILE* temp = fopen(TempFile, "wb");
while (1) {
unsigned int FramesRead = Source->read(4096, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
fwrite(Resampled, sizeof(short), Processed, temp);
}
speex_resampler_destroy(State);
fclose(temp);
Encode(AE, TempFile);
return;
}
void Encode(AbfEncoder& AE, char* TempFile) {
FILE* fin = fopen(TempFile, "rb");
// Initialize Speex.
short Input[320];
unsigned short BytesToRead;
while (!feof(fin)) {
fread(Input, 2, 320, fin);
AE.Encode(Input);
}
fclose(fin);
#ifdef WIN32
DeleteFile(TempFile);
#else
system("rm temp.raw");
#endif
}
