/* $Id$
Copyright (C) 2013, 2014 Peter Laursen.

This small program now attempts to read an Daisy book using Libdaisy20, resamples it using the Speex resampler and encodes it using Opus.
If everything goes well, this will be done all in memory so that the only file that's written is the Opus output file.
*/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "libdaisy20/libdaisy20.h"
#ifndef WIN32
#include "libabf/libabf.h"
#else
#include "libabf/libabf-win.h"
#endif
#include <sys/stat.h>
#include <opus/opus.h>
#include <mpg123.h>
#include "products/speex_resampler.h"
using namespace ABF;

int main(int argc, char* argv[]) {
if (argc != 3) {
printf("Error, need at least an input folder and an output file name.");
return (EXIT_FAILURE);
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
mpg123_handle* Mp3File;
long SamplingRate = 0;
int Channels = 0, Encoding = 0;
int err = MPG123_OK;
DaisyBook D(argv[1]);
if (!D.BookIsValid()) printf("No daisy book.\n");
D.GetMetadata();
D.GetAudioFiles();
AbfEncoder AE(argv[2]);
AE.SetTitle(D.GetTitle().c_str());
AE.SetAuthor(D.GetAuthor().c_str());
AE.SetTime(D.GetTotalTime().c_str());
AE.SetNumSections(D.GetNumSections());
AE.WriteHeader();
unsigned short Files = 0;
while (Files < D.GetNumSections()) {
Mp3File = mpg123_new(NULL, &err);
AE.WriteSection();
mpg123_open(Mp3File, D.GetSectionFile(Files));
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, 16000, 10, 0);
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

printf("Working with file %s.\n", D.GetSectionFile(Files));
FILE* Memory = fopen("memory", "wb+");
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 640, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
fwrite(Resampled, sizeof(short), Processed, Memory);

} while (Status == MPG123_OK);
rewind(Memory);
while (!feof(Memory)) {
int Samples = fread(Membuf, sizeof(short), 320, Memory);
AE.Encode(Membuf);
}
fclose(Memory);
++Files;
mpg123_close(Mp3File);
mpg123_delete(Mp3File);

speex_resampler_destroy(Resampler);
delete [] Membuf;

}
mpg123_exit();
}

