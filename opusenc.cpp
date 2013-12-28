/* $Id$
Copyright (C) 2013, 2014 Peter Laursen.

This small program now attempts to read an Daisy book using Libdaisy20, resamples it using the Speex resampler and encodes it using Opus.
If everything goes well, this will be done all in memory so that the only file that's written is the Opus output file.
*/
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include "libdaisy20/libdaisy20.h"
#include <sys/stat.h>
#include <opus/opus.h>
#include <mpg123.h>
#include <speex/speex_resampler.h>
using namespace ABF;

int main(int argc, char* argv[]) {
if (argc != 2) {
printf("Error, need at least an input file name.");
return (EXIT_FAILURE);
}
// Let's open the file and get some information, later to be used for the Speex resampler.
mpg123_init();
mpg123_handle* Mp3File;
long SamplingRate = 0;
int Channels = 0, Encoding = 0;
int err = MPG123_OK;
int Error = 0;
OpusEncoder* Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);
DaisyBook D(argv[1]);
if (!D.BookIsValid()) printf("No daisy book.\n");
D.GetMetadata();
D.GetAudioFiles();
unsigned short Files = 0;
FILE* fout = fopen("output.opus", "wb");
while (Files < D.GetNumSections()) {

Mp3File = mpg123_new(NULL, &err);
mpg123_open(Mp3File, D.GetSectionFile(Files));
mpg123_getformat(Mp3File, &SamplingRate, &Channels, &Encoding);
SpeexResamplerState* Resampler = speex_resampler_init(1, SamplingRate, 16000, 10, 0);

// Let's try to create an encoder.
// Get a little information about our encoder
int Bitrate = 1;
opus_encoder_ctl(Encoder, OPUS_SET_COMPLEXITY(&Bitrate));

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
FILE* Memory = fmemopen(Membuf, 640, "w+b");
printf("Working with file %s.\n", D.GetSectionFile(Files));
do {
unsigned int Processed = ResampledSize;
size_t Decoded;
Status = mpg123_read(Mp3File, (unsigned char*)Buffer, 640, &Decoded);
//printf("MP3 status: %d\n", Status);
unsigned int TotalSamples = Decoded/2;
speex_resampler_process_int(Resampler, 0, Buffer, &TotalSamples, Resampled, &Processed);
int Written = fwrite(Resampled, sizeof(short), Processed, Memory);
/*
if (Written == 0) {
printf("Our position in the stream is %ld.\n", ftell(Memory));
//Written = fwrite(Resampled, sizeof(short), Processed, Memory);
}
*/
//printf("Wrote %d samples.\nSamplesWritten: %d, Sum: %d\nProcessed: %d\n", Written, SamplesWritten, Written+SamplesWritten, Processed);
if (SamplesWritten == 320) SamplesWritten = 0;
SamplesWritten += Written;
if (SamplesWritten >= 320) {
if (SamplesWritten > 320) {
//printf("Samples: %d, Written: %d.\n", SamplesWritten, Written);
SamplesWritten = 320;
}

unsigned char OutputBuffer[2048] = {0};
unsigned short Length = opus_encode(Encoder, Membuf, 320, OutputBuffer, 2048);
if (Length > 100) break;
//printf("First frame: %d bytes\n", Length);
fwrite(&Length,1,sizeof(unsigned short),fout);
fwrite(OutputBuffer, 1, Length, fout);
rewind(Memory);

if (Written < Processed) {
short* MyPointer = &Resampled[Written];
SamplesWritten = fwrite(MyPointer, sizeof(short), Processed-Written, Memory);
}
if (SamplesWritten > 320) SamplesWritten=0;
}
} while (Status == MPG123_OK);
++Files;
mpg123_close(Mp3File);
mpg123_delete(Mp3File);

printf("Our bitrate is %d\n", Bitrate);
speex_resampler_destroy(Resampler);
fclose(Memory);
delete [] Membuf;
}
mpg123_exit();
fclose(fout);
opus_encoder_destroy(Encoder); 

}

