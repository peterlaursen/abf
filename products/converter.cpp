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
#include "../libdaisy20/libdaisy20.h"
#include "../abfencoder/abfencoder.h"
#include <opus/opus.h>
#include <mpg123.h>
#include "speex_resampler.h"
using namespace ABF;
static const char* BookFileName = nullptr;
static int NumberOfFiles = 0;
static const char* CurrentFileName = nullptr;
static DaisyBook* Book = nullptr;
static AbfEncoder* GlobalAE = nullptr;
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
CurrentFileName = Book->GetSectionFile(MyFile);

mpg123_open(Mp3File, Book->GetSectionFile(MyFile));
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, 16000, 10, 0);

// Let's try to create an encoder.
// Get a little information about our encoder
short* Buffer = new short[32768];
short* Resampled = new short[32768];
int ResampledSize= 32768;
int SamplesWritten = 0;
int Status = MPG123_OK;

printf("Working with file %s.\n", Book->GetSectionFile(MyFile));
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 32768, &Decoded);
//printf("MP3 status: %d\n", Status);

unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
short TempEncoder[320] = {0};
int Temp = 0;
while (Temp < Processed) {
int EncSize = 320;
if (Temp + 320  > Processed) EncSize = Processed - Temp;
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
delete[] Buffer;
delete[] Resampled;
Buffer = Resampled = nullptr;
speex_resampler_destroy(Resampler);
}
return;
}
int main(int argc, char* argv[]) {
if (argc != 3) {
fprintf(stderr, "Error, need at least an input folder and an output file name.\n");
return (EXIT_FAILURE);
}

// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
DaisyBook D(argv[1]);
if (!D.BookIsValid()) {
fprintf(stderr, "No daisy Book. Exitting\n");
return (EXIT_FAILURE);
}
try {
D.GetMetadata();
D.GetAudioFiles();
} catch (string& E) {
fprintf(stderr, "Caught exception: %s\nWe exit because of this.\n", E.c_str());
return (EXIT_FAILURE);
}
printf("Sections: %d\n", D.GetNumSections());
AbfEncoder AE(argv[2], D.GetNumSections());
GlobalAE = &AE;
Book = &D;
BookFileName = argv[2];
AE.SetTitle(D.GetTitle().c_str());
AE.SetAuthor(D.GetAuthor().c_str());
AE.SetTime(D.GetTotalTime().c_str());
AE.WriteHeader();
AbfEncoder* PAE = &AE;
int NumThreads = std::thread::hardware_concurrency();
std::thread* ThreadHandle = new std::thread[NumThreads];
NumberOfFiles = D.GetNumSections();
for (int i = 0; i < NumThreads; i++) {
ThreadHandle[i] = std::thread(ConverterThread, PAE);
}
for (int i = 0; i < NumThreads; i++) ThreadHandle[i].join();
delete[] ThreadHandle;
AE.Gather();
mpg123_exit();
return (EXIT_SUCCESS);
}
