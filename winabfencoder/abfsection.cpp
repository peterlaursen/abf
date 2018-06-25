/* AbfSection.cpp
Copyright (C) 2017 Peter Laursen.

We split our encoder out into more files.
*/
#include "abfsection.h"
#include <opus/opus.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
using namespace std;
namespace ABF {
AbfSection::AbfSection() {
sprintf(FileTemplate, "/tmp/abfconvXXXXXX");
fd = mkstemp(FileTemplate);
#ifdef DEBUG
printf("Temporary file: %s, fd = %d\n", FileTemplate, fd);
#endif
int Error = 0;
Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);

if (Error != OPUS_OK) fprintf(stderr, "Error in creating our encoder!\n");
FileBuffer = new char[1024*1024];
}
AbfSection::~AbfSection() {
close(fd);
opus_encoder_destroy(Encoder);
delete[] FileBuffer;
FileBufferPosition = 0;
unlink(FileTemplate);
}
void AbfSection::Close() {
for (int i = TempBufferPosition; i<320; i++) TempBuffer[i]=0;
int Length=320;
TempBufferPosition = 0;
Encode(TempBuffer, Length);
if (FileBufferPosition > 0) {
size_t tmp = write(fd, FileBuffer, FileBufferPosition);
close(fd);
FileBufferPosition = 0;
}
return;
}
void AbfSection::Encode(const short* Input, int& Length) {
if (TempBufferPosition + Length >= 320) {
int Remaining = 0;
for (int i = TempBufferPosition, j=0; i < 320; i++, j++, Remaining++) TempBuffer[i]=Input[j];

unsigned char Output[200] = {0};
short Bytes = opus_encode(Encoder, TempBuffer, 320, Output, 200);
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
size_t tmp = write(fd, FileBuffer, FileBufferPosition);
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