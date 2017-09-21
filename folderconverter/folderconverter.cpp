/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This small program attempts to read a folder of MP3 files.
They are to be converted into our ABF audio book, so we are a little picky about the format.

*/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "../abfencoder/abfencoder.h"
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <opus/opus.h>
#include <mpg123.h>
#include "../products/speex_resampler.h"
#include <iostream>
#include <glob.h>
using namespace std;
using namespace ABF;
static const char* BookFileName;
static int NumberOfFiles = 0;
static const char* CurrentFileName = nullptr;
static glob_t Paths;
void Cleanup(int Signal) {
unlink(BookFileName);
exit(Signal);
}
void ConvertInfo(int Signal) {
printf("Book consists of %d sections - working with file %s\n", NumberOfFiles, CurrentFileName);
}

void* ConverterThread(void* ThreadAE) {
// Put everything here into a while loop...
//I hope it works.
while (true) {
AbfEncoder* AE = (AbfEncoder*)ThreadAE;
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
return nullptr; // Exit the thread

mpg123_handle* Mp3File;
long SamplingRate = 0;
int Channels = 0, Encoding = 0;
int err = MPG123_OK;
Mp3File = mpg123_new(NULL, &err);
CurrentFileName = Paths.gl_pathv[MyFile];

mpg123_open(Mp3File, Paths.gl_pathv[MyFile]);
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, 16000, 10, 0);

// Let's try to create an encoder.
// Get a little information about our encoder
short Buffer[320] = {0};
short Resampled[640] = {0};
int ResampledSize=640;
int SamplesWritten = 0;
int Status = MPG123_OK;

printf("Working with file %s.\n", Paths.gl_pathv[MyFile]);
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 640, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
int Temp = Processed;
AE->Encode(MyFile, Resampled, Temp);
} while (Status == MPG123_OK);
AE->Lock();
AE->CloseSection(MyFile);
AE->Unlock();
mpg123_close(Mp3File);
mpg123_delete(Mp3File);
speex_resampler_destroy(Resampler);
}
return nullptr;
}
int main(int argc, char* argv[]) {
if (argc != 3) {
fprintf(stderr, "Error, need at least an input folder and an output file name.\n");
return (EXIT_FAILURE);
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
string Title, Author, Time;
char Pattern[4096] = {0};
sprintf(Pattern, "%s/*.mp3", argv[1]);
try {
printf("Title: ");
getline(cin, Title);
printf("\nAuthor: ");
getline(cin, Author);
printf("\nTime: ");
getline(cin, Time);
glob(Pattern, 0, NULL, &Paths);

} catch (string& E) {
fprintf(stderr, "Caught exception: %s\nWe exit because of this.\n", E.c_str());
globfree(&Paths);
return (EXIT_FAILURE);
}
signal(SIGINT, &Cleanup);
#ifdef FREEBSD
signal(SIGINFO, &ConvertInfo);
#endif
BookFileName = argv[2];
AbfEncoder AE(argv[2], (unsigned short)Paths.gl_pathc);
AE.SetTitle(Title.c_str());
AE.SetAuthor(Author.c_str());
AE.SetTime(Time.c_str());
AE.WriteHeader();
void* PAE = &AE;
pthread_t ThreadHandle[4];
NumberOfFiles = Paths.gl_pathc;
for (int i = 0; i < 4; i++) {
pthread_create(&ThreadHandle[i], NULL, ConverterThread, PAE);
}
for (int i = 0; i < 4; i++) pthread_join(ThreadHandle[i], 0);
AE.Gather();
mpg123_exit();
globfree(&Paths);
return (EXIT_SUCCESS);
}
