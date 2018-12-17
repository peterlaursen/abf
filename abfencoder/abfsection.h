/* $id$
Copyright (C) 2017 Peter Laursen.

We split our encoder out into more files.
*/
#ifndef ABFSECTION_H
#define ABFSECTION_H
#include <opus/opus.h>
namespace ABF {
const int SAMPLING_RATE = 16000;
enum EncodingStatus {
Waiting,
Encoding,
Finished
};
class AbfSection {
const int FrameSize = SAMPLING_RATE/50;
OpusEncoder* Encoder = nullptr;
int fd = 0;
short* TempBuffer = nullptr;
short TempBufferPosition = 0;
char FileTemplate[40]= {0};
char* FileBuffer = nullptr;
int FileBufferPosition = 0;
public:
AbfSection();
~AbfSection();
EncodingStatus Status = Waiting;
void Close();
void Encode(const short* Samples, int& Length);
int Getfd() const { return fd; }
const char* TempFile() { return FileTemplate; }

};
}
#endif

