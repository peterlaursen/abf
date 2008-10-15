#include <libdaisy.h>
#include <audiere.h>
#include <speex/speex_resampler.h>
#include <speex/speex.h>
#include <io.h>
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
void Decode(Daisy&, FILE*);
void Encode(FILE*, char*);
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
FILE* fout = fopen(argv[2], "wb+");
unsigned short Sections = D.GetNumSections();
fwrite(&Sections, sizeof(short), 1, fout);
int Size = 0;
for (unsigned short i = 0; i < Sections; i++) {
fwrite(&i, sizeof(short), 1, fout);
fwrite(&Size, sizeof(int), 1, fout);
}

unsigned short Section = 0;
while (D.OpenSmil()) {
int Position = ftell(fout);
fseek(fout, 2, SEEK_SET);
for (int i = 0; i < Section; i++) fseek(fout, 6, SEEK_CUR);
// We are now at the beginning of a section header.
fseek(fout, 2, SEEK_CUR);
cout << "Am to write a section size at position " << ftell(fout) << endl;
fwrite(&Position, sizeof(int), 1, fout);
fseek(fout, Position, SEEK_SET);
Decode(D, fout);
++Section;
}
fclose(fout);
return 0;
}
void Decode(Daisy& D, FILE* fout) {
unsigned int Size = 320, Processed = 320;
short Resampled[320];
SampleSourcePtr Source = OpenSampleSource(D.GetMP3FileName());
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[320];
char* Filename = _mktemp("ConvertXXXX.raw");
FILE* temp = fopen(Filename, "wb");
while (1) {
unsigned int FramesRead = Source->read(320, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
fwrite(Resampled, sizeof(short), Processed, temp);
}
speex_resampler_destroy(State);
fclose(temp);
Encode(fout, Filename);
return;
}
void Encode(FILE* fout, char* TmpFile) {
FILE* fin = fopen(TmpFile, "rb");
// Initialize Speex.
void* Encoder = speex_encoder_init(&speex_wb_mode);
SpeexBits Bits;
speex_bits_init(&Bits);
int FrameSize;
speex_encoder_ctl(Encoder, SPEEX_GET_FRAME_SIZE, &FrameSize);
short Input[400];
unsigned short BytesToRead;
while (!feof(fin)) {
fread(Input, 2, 320, fin);
speex_bits_reset(&Bits);
speex_encode_int(Encoder, Input, &Bits);
char Output[400];
unsigned short Bytes = speex_bits_write(&Bits, Output, 400);
fwrite(&Bytes, sizeof(short), 1, fout);
fwrite(Output, sizeof(char), Bytes, fout);
}
speex_encoder_destroy(Encoder);
speex_bits_destroy(&Bits);
fclose(fin);
string Command;

#ifdef WIN32
Command = "del ";
Command += TmpFile;
#else
Command = "rm ";
Command += TmpFile;
#endif
system(Command.c_str());
}
