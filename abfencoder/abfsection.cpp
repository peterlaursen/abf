/* $Id$
Copyright (C) 2017, 2018, 2019 Peter Laursen.

We split our encoder out into more files.
*/
#include "abfsection.h"
#include <opus/opus.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <windows.h>
using namespace std;
namespace ABF {
AbfSection::AbfSection() {
GetTempFileName(".", "ABFConv", 0, FileTemplate); 
fd = fopen(FileTemplate, "wb+");
FileBuffer = new char[1024*1024];
#ifdef DEBUG
printf("Temporary file: %s\n", FileTemplate);
#endif
}
void AbfSection::Init(int Samp) {
SamplingRate = Samp;
FrameSize = SamplingRate/50;
TempBuffer = new  short[FrameSize];
int Error = 0;
Encoder = opus_encoder_create(SamplingRate, 1, OPUS_APPLICATION_VOIP, &Error);
if (Error != OPUS_OK) {
fprintf(stderr, "Error in creating our encoder!\n");
delete[] TempBuffer;
TempBuffer = nullptr;
return;
}
}
AbfSection::~AbfSection() {
fclose(fd);
opus_encoder_destroy(Encoder);
delete[] FileBuffer;
FileBufferPosition = 0;
DeleteFile(FileTemplate);
delete[] TempBuffer;
}
void AbfSection::Close() {
for (int i = TempBufferPosition; i<FrameSize; i++) TempBuffer[i]=0;
int Length=FrameSize;
TempBufferPosition = 0;
Encode(TempBuffer, Length);
if (FileBufferPosition > 0) {
size_t tmp = fwrite(FileBuffer, FileBufferPosition, 1, fd);
fclose(fd);
FileBufferPosition = 0;
}
return;
}
void AbfSection::Encode(const short* Input, int& Length) {
if (TempBufferPosition + Length >= FrameSize) {
int Remaining = 0;
for (int i = TempBufferPosition, j=0; i < FrameSize; i++, j++, Remaining++) TempBuffer[i]=Input[j];

unsigned char Output[400] = {0};
short Bytes = opus_encode(Encoder, TempBuffer, FrameSize, Output, 400);
if (Bytes < 0) {
fprintf(stderr, "Opus error: %d\n%s\n", Bytes, opus_strerror(Bytes));
return;
}
TempBufferPosition = 0;
if (FileBufferPosition + Bytes + 2 < 1024*1024) {
memcpy(&FileBuffer[FileBufferPosition], &Bytes, 2);
FileBufferPosition += 2;
memcpy(&FileBuffer[FileBufferPosition], Output, Bytes);
FileBufferPosition += Bytes;
}
else {
size_t tmp = fwrite(FileBuffer, FileBufferPosition, 1, fd);
FileBufferPosition = 0;
memset(FileBuffer, 0, 1024*1024);
memcpy(&FileBuffer[FileBufferPosition], &Bytes, 2);
FileBufferPosition += 2;
memcpy(&FileBuffer[FileBufferPosition], Output, Bytes);
FileBufferPosition += Bytes;
}
for (int i = Remaining, j=0; i < Length; i++, j++, TempBufferPosition++) TempBuffer[j]=Input[i];
}
else {
for (int i = TempBufferPosition, j=0; j<Length; i++, j++, TempBufferPosition++) TempBuffer[i]=Input[j];
}
}
}
