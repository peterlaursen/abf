/* $Id$
Copyright (C) 2013, 2014 Peter Laursen.

This small program now attempts to read an Daisy book using Libdaisy20, resamples it using the Speex resampler and encodes it using Opus.
If everything goes well, this will be done all in memory so that the only file that's written is the Opus output file.
*/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#ifndef WIN32
#include "../libabf/libabf.h"
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#else
#include "../libabf/libabf-win.h"
#endif
#include <sys/stat.h>
#include <opus/opus.h>
#include <mpg123.h>
#include "speex_resampler.h"
#include <iostream>

using namespace ABF;
void MemEncode(AbfEncoder& AE, short* Input, const unsigned int& Processed) {
static short MemoryBuffer[320] = {0};
static int LastPos = 0;
if (LastPos + Processed >= 320) {
int Remaining = 0;
for (int i = LastPos, j = 0; i < 320; i++, j++) {
MemoryBuffer[i] = Input[j];
++Remaining;
}
AE.Encode(MemoryBuffer);
LastPos = 0;
for (int i = 0; i < Processed-Remaining; i++) {
MemoryBuffer[i]=Input[Remaining+i];
++LastPos;
}
}
else {
for (int i = LastPos, j=0; j < Processed; i++,j++) {
MemoryBuffer[i]=Input[j];
LastPos++;
}
}
}
#ifndef WIN32
static const char* BookFileName;
void Cleanup(int Signal) {
unlink(BookFileName);
exit(Signal);
}
#endif
int main(int argc, char* argv[]) {
if (argc != 3) {
printf("Error, need at least an input folder and an output file name.\n");
return (EXIT_FAILURE);
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
mpg123_handle* Mp3File;
long SamplingRate = 0;
int Channels = 0, Encoding = 0;
int err = MPG123_OK;
dirent** FileList = NULL;
int ListLength = scandir(argv[1], &FileList, NULL, alphasort);
cout << "We found " << ListLength << " files." << endl;
#ifndef WIN32
signal(SIGINT, &Cleanup);
BookFileName = argv[2];
#endif
string Title, Author, Time;
cout << endl << "Book Title: ";
getline(cin, Title);
cout << endl << "Author: ";
getline(cin, Author);
cout << endl << "Book Duration: ";
getline(cin, Time);

AbfEncoder AE(argv[2]);
AE.SetTitle(Title.c_str());
AE.SetAuthor(Author.c_str());
AE.SetTime(Time.c_str());
AE.SetNumSections(ListLength-2);
AE.WriteHeader();
chdir(argv[1]);
unsigned short Files = 2;
while (Files < ListLength) {

Mp3File = mpg123_new(NULL, &err);
AE.WriteSection();
mpg123_open(Mp3File, FileList[Files]->d_name);
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, 16000, 10, 0);

// Let's try to create an encoder.
// Get a little information about our encoder
short Buffer[320] = {0};
short Resampled[640] = {0};
int ResampledSize=640;
short* Membuf = new short[320];
if (!Membuf) {
printf("Cannot allocate memory.\n");
return 1;
}

int SamplesWritten = 0;
int Status = MPG123_OK;

printf("Working with file %s.\n", FileList[Files]->d_name);
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 640, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
MemEncode(AE, Resampled, Processed);
} while (Status == MPG123_OK);
++Files;
mpg123_close(Mp3File);
mpg123_delete(Mp3File);

speex_resampler_destroy(Resampler);
delete [] Membuf;
}
mpg123_exit();
for (int i = 0; i < ListLength; i++) {
free(FileList[i]);
}
free(FileList);

}

