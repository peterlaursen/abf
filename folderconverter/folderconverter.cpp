/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This small program attempts to read a folder of MP3 files.
They are to be converted into our ABF audio book, so we are a little picky about the format.

*/
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include "../abfencoder/abfencoder.h"
#include <opus/opus.h>
#include <mpg123.h>
#include "../products/speex_resampler.h"
#include <glob.h>
using namespace std;
using namespace ABF;
static const char* BookFileName = nullptr;
static int NumberOfFiles = 0;
static const char* CurrentFileName = nullptr;
static glob_t Paths;
static AbfEncoder* GlobalAE = nullptr;
void Cleanup(int Signal) {
unlink(BookFileName);
GlobalAE->Cleanup();
exit(Signal);
}
void ConvertInfo(int Signal) {
cerr << "Book consists of " << NumberOfFiles << " sections - working with file " << CurrentFileName << endl;
}
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
CurrentFileName = Paths.gl_pathv[MyFile];
mpg123_open(Mp3File, Paths.gl_pathv[MyFile]);
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, AE->GetSamplingRate(), 10, 0);
// Let's try to create an encoder.
// Get a little information about our encoder
short *Buffer = new short[32768] ;
short* Resampled = new short[32768] ;
int ResampledSize=32768;
int SamplesWritten = 0;
int Status = MPG123_OK;
cout << "Working with file " << Paths.gl_pathv[MyFile] << endl;
do {
unsigned int Processed = ResampledSize;
size_t Decoded = 0;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 32768, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
int FrameSize = AE->GetSamplingRate()/50;
short TempEncoder[FrameSize];
int Temp = 0;
while (Temp < Processed) {
int EncSize = FrameSize;
if (Temp + FrameSize > Processed) EncSize = Processed - Temp;
memcpy(TempEncoder, &Resampled[Temp], EncSize*2);
Temp += EncSize;
AE->Encode(MyFile, TempEncoder, EncSize);
}
} while (Status == MPG123_OK);
AE->Lock();
AE->CloseSection(MyFile);
AE->Unlock();
mpg123_close(Mp3File);
mpg123_delete(Mp3File);
speex_resampler_destroy(Resampler);
delete[] Resampled;
delete[] Buffer;
Buffer = Resampled = nullptr;
}
}
int main(int argc, char* argv[]) {
if (argc != 3 || argc != 4) {
cerr << "Error, need at least an input folder and an output file name." << endl;
cout << "Alternatively, pass '-h' or '-x' as the last argument to encode in higher (24kHz) or extreme (48kHz)." << endl;
return (EXIT_FAILURE);
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
string Title, Author;
char Pattern[4096] = {0};
int PatternLength = sprintf(Pattern, "%s/*.mp3", argv[1]);
Pattern[PatternLength] = '\0';
try {
cout << "Title: ";
getline(cin, Title);
cout << endl << "Author: ";
getline(cin, Author);
glob(Pattern, 0, NULL, &Paths);
} catch (string& E) {
cerr << "Caught exception: %s\nWe exit because of this.\n" << E.c_str() << endl;
globfree(&Paths);
return (EXIT_FAILURE);
}
signal(SIGINT, &Cleanup);
#ifdef FREEBSD
signal(SIGINFO, &ConvertInfo);
#endif
BookFileName = argv[2];
unsigned int GlobalSamplingRate = 16000;
if (argc == 4) {
string arg(argv[3]);
if (arg == "-h") GlobalSamplingRate = 24000;
else if (arg == "-x") GlobalSamplingRate = 48000;
}
AbfEncoder AE(argv[2], (unsigned short)Paths.gl_pathc, GlobalSamplingRate);
GlobalAE = &AE;
AE.SetTitle(Title.c_str());
AE.SetAuthor(Author.c_str());
AE.WriteHeader();
AbfEncoder* PAE = &AE;
int NumThreads = thread::hardware_concurrency();
thread ThreadHandle[NumThreads];
NumberOfFiles = Paths.gl_pathc;
for (int i = 0; i < NumThreads; i++) {
ThreadHandle[i] = thread(ConverterThread, PAE);
}
for (int i = 0; i < NumThreads; i++) ThreadHandle[i].join();
AE.Gather();
mpg123_exit();
globfree(&Paths);
return (EXIT_SUCCESS);
}
