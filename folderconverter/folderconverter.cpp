/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This small program now attempts to read an Daisy book using Libdaisy20, resamples it using the Speex resampler and encodes it using Opus.
If everything goes well, this will be done all in memory so that the only file that's written is the Opus output file.
*/
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <thread>
#include <vector>
#include "../libdaisy20/scandir.h"
#include "../abfencoder/abfencoder.h"
#include <opus/opus.h>
#include <mpg123.h>
#include "../products/speex_resampler.h"
using namespace std;
using namespace ABF;
static const char* BookFileName = nullptr;
static int NumberOfFiles = 0;
static const char* CurrentFileName = nullptr;
static AbfEncoder* GlobalAE = nullptr;
vector<string> Paths;
void ConverterThread(AbfEncoder* AE) {
// Put everything here into a while loop...
//I hope it works.
while (true) {
AE->Lock();
int MyFile = -1;
for (unsigned short i = 0; i < AE->GetNumSections(); i++) {
if (AE->GetStatus(i) == Waiting) {
AE->SetStatus(i, Encoding);
MyFile = i;
CurrentFileName = Paths[i].c_str();
break;
}
}
AE->Unlock();
if (MyFile == -1) 
return; // Exit the thread

mpg123_handle* Mp3File;
long SamplingRate = 0;
int Channels = 0, Encoding = 0;
int err = MPG123_OK;
Mp3File = mpg123_new(NULL, &err);
mpg123_open(Mp3File, CurrentFileName);
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, AE->GetSamplingRate(), 10, 0);

// Let's try to create an encoder.
// Get a little information about our encoder
short* Buffer = new short[32768];
short* Resampled = new short[32768];
int ResampledSize= 32768;
int SamplesWritten = 0;
int Status = MPG123_OK;
printf("Working with file %s.\n", CurrentFileName);
const int FrameSize = AE->GetSamplingRate()/50;
short* TempEncoder = new short[FrameSize];
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 32768, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
int Temp = 0;
while (Temp < Processed) {
int EncSize = FrameSize;
if (Temp + FrameSize  > Processed) EncSize = Processed - Temp;
memcpy(TempEncoder, &Resampled[Temp], EncSize*2);
Temp += EncSize;
AE->Encode(MyFile, TempEncoder, EncSize);
}
} while (Status == MPG123_OK);
delete[] TempEncoder;
AE->Lock();
AE->CloseSection(MyFile);
AE->Unlock();
mpg123_close(Mp3File);
mpg123_delete(Mp3File);
delete[] Buffer;
delete[] Resampled;
Buffer = Resampled = nullptr;
speex_resampler_destroy(Resampler);
}
return;
}
int SelectMP3(const struct dirent* d) {
string cont(d->d_name);
int index = cont.find(string(".mp3"));
if (index == string::npos) return 0;
else return 1;
}
int main(int argc, char* argv[]) {
if (argc < 3) {
fprintf(stderr, "Error, need at least an input folder and an output file name.\n");
return (EXIT_FAILURE);
}
unsigned short GlobalSamplingRate = 16000;
if (argc == 4) {
string arg(argv[3]);
if (arg == "-h") GlobalSamplingRate = 24000;
if (arg == "-x") GlobalSamplingRate = 48000;
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
dirent** FileList;
int FileListLength = 0;
FileListLength = scandir(argv[1], &FileList, SelectMP3, alphasort);
if (FileListLength == 0) {
cerr << "No MP3 files found." << endl;
return -1;
}
for (int i = 0; i < FileListLength; i++) {
string P(argv[1]);
P += "\\";
P += FileList[i]->d_name;
Paths.push_back(P);
free(FileList[i]);
}
free(FileList);
AbfEncoder AE(argv[2], FileListLength, GlobalSamplingRate);
GlobalAE = &AE;
BookFileName = argv[2];
string title, author;
cout << "Title: ";
getline(cin, title);
cout << endl << "Author: ";
getline(cin, author);
AE.SetTitle(title.c_str());
AE.SetAuthor(author.c_str());

AE.WriteHeader();
AbfEncoder* PAE = &AE;
int NumThreads = std::thread::hardware_concurrency();
NumberOfFiles = FileListLength;
thread* ThreadHandle = new thread[NumThreads];
for (int i = 0; i < NumThreads; i++) {
ThreadHandle[i] = thread(ConverterThread, PAE);
}
for (int i = 0; i < NumThreads; i++) ThreadHandle[i].join();
delete[] ThreadHandle;
ConverterThread(&AE);
AE.Gather();
mpg123_exit();
return (EXIT_SUCCESS);
}
