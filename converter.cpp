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
void Decode(Daisy&, FILE*);
void Encode(FILE*);
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
int Offset = 0;
string StrTitle = D.ExtractMetaInfo(string("dc:title"));
unsigned short Length = StrTitle.length();
fwrite(&Length, 2, 1, fout);
char* Title = (char*)StrTitle.c_str();
fwrite(Title, 1, Length, fout);
string StrAuthor = D.ExtractMetaInfo(string("dc:creator"));
Length = StrAuthor.length();
fwrite(&Length, sizeof(short), 1, fout);
char* Author = (char*)StrAuthor.c_str();
fwrite(Author, 1, Length, fout);
string StrTime = D.ExtractMetaInfo(string("ncc:totalTime"));
Length = StrTime.length();
fwrite(&Length, sizeof(short), 1, fout);
char* Time = (char*)StrTime.c_str();
fwrite(Time, 1, Length, fout);
fwrite(&Sections, sizeof(short), 1, fout);
Offset = ftell(fout);
int Size = 0;
for (unsigned short i = 0; i < Sections; i++) {
fwrite(&i, sizeof(short), 1, fout);
fwrite(&Size, sizeof(int), 1, fout);
}

unsigned short Section = 0;
while (D.OpenSmil()) {
int Position = ftell(fout);
fseek(fout, Offset, SEEK_SET);
for (int i = 0; i < Section; i++) fseek(fout, 6, SEEK_CUR);
// We are now at the beginning of a section header.
fseek(fout, 2, SEEK_CUR);
fwrite(&Position, sizeof(int), 1, fout);
fseek(fout, Position, SEEK_SET);
Decode(D, fout);
++Section;
}
fclose(fout);
cout << "The book has been converted successfully." << endl;
return 0;
}
void Decode(Daisy& D, FILE* fout) {
unsigned int Size = 4096, Processed = 4096;
short Resampled[4096];
SampleSourcePtr Source = OpenSampleSource(D.GetMP3FileName());
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[4096];
FILE* temp = fopen("temp.raw", "wb");
while (1) {
unsigned int FramesRead = Source->read(4096, Buffer);
if (FramesRead <= 0) break;
speex_resampler_process_int(State, 0, Buffer, &FramesRead, Resampled, &Processed);
//Encode(Resampled, Processed, fout);
fwrite(Resampled, sizeof(short), Processed, temp);
}
speex_resampler_destroy(State);
fclose(temp);
Encode(fout);
return;
}
void Encode(FILE* fout) {
FILE* fin = fopen("temp.raw", "rb");
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
#ifdef WIN32
system("del temp.raw");
#else
system("rm temp.raw");
#endif
}
