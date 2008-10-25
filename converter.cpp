#include <libdaisy.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <libabf.h>
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
void Decode(Daisy&, AbfEncoder&);
void Encode(char* TempFile, AbfEncoder&);
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
AbfEncoder AE;
string Meta = "dc:creator";
string Value = D.ExtractMetaInfo(Meta);
AE.SetAuthor(Value.c_str());
Meta = "dc:title";
Value = D.ExtractMetaInfo(Meta);
AE.SetTitle(Value.c_str());
Meta = "ncc:totalTime";
Value = D.ExtractMetaInfo(Meta);
AE.SetTime(Value.c_str());
AE.SetNumSections(D.GetNumSections());
AE.OpenFile(argv[2]);
AE.WriteHeader();
unsigned short Section = 0;
while (D.OpenSmil()) {
Decode(D, AE);
}
cout << "The book has been converted successfully." << endl;
return 0;
}
void Decode(Daisy& D, AbfEncoder& AE) {
unsigned int Size = 4096, Processed = 4096;
short Resampled[4096];
SampleSourcePtr Source = OpenSampleSource(D.GetMP3FileName());
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[4096];
#ifdef WIN32
char* TempFile = tempnam(".\\", "ABFConv");
#else
char* TempFile = tempnam("./", "ABFConv");
#endif
cout << "Temporary file name: " << TempFile << endl;
FILE* temp = fopen(TempFile, "wb");
while (1) {
unsigned int FramesRead = Source->read(4096, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
fwrite(Resampled, sizeof(short), Processed, temp);
}
speex_resampler_destroy(State);
fclose(temp);
Encode(TempFile, AE);
return;
}
void Encode(char* TempFile, AbfEncoder& AE) {
FILE* fin = fopen(TempFile, "rb");
short Input[320];
unsigned short BytesToRead;
AE.WriteSection();
while (!feof(fin)) {
fread(Input, sizeof(short), 320, fin);
AE.Encode(Input);
}
fclose(fin);
#ifdef WIN32
string Command = "del ";
Command += TempFile;
#else
string Command = "rm ";
Command += TempFile;
#endif
system(Command.c_str());
}
