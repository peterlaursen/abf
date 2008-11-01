#include <windows.h>
#include <libabf.h>
#include <speex/speex.h>
#include <speex/speex_resampler.h>
	#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <audiere.h>
using namespace ABF;
using namespace audiere;
using namespace std;
void Decode(const char* Filename, AbfEncoder& AE);
void Encode(AbfEncoder& AE, char* Tempfile);
int main(int argc, char* argv[]) {
string Path = argv[1];
Path += "\\";
ifstream fin((Path + "Index.txt").c_str());
string Line;
AbfEncoder AE(argv[2]);
getline(fin, Line);
AE.SetTitle((char*)Line.c_str());
getline(fin, Line);
AE.SetAuthor((char*)Line.c_str());
getline(fin, Line);
AE.SetTime((char*)Line.c_str());
unsigned short NumSections;
fin >> NumSections;
AE.SetNumSections(NumSections);
AE.WriteHeader();

getline(fin, Line);
cout << "Line: " << Line << endl;
string Filename;
while (!fin.eof()) {
AE.WriteSection();
getline(fin, Line);
Filename = Path + Line;
Decode(Filename.c_str(), AE);
}
fin.close();
return 0;
}
void Decode(const char* Filename, AbfEncoder& AE) {
unsigned int Size = 4096, Processed = 4096;
short Resampled[4096];
SampleSourcePtr Source = OpenSampleSource(Filename);
int SampleRate, NumChannels;
SampleFormat SF;
Source->getFormat(NumChannels, SampleRate, SF);
SpeexResamplerState* State = speex_resampler_init(1, SampleRate, 16000, 8, 0);
short Buffer[4096];
char* TempFile = tempnam(".\\", "ABFConv");
FILE* temp = fopen(TempFile, "wb");
cout << "Temporary File: " << TempFile << endl;
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

