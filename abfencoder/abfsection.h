/* $Id$
Copyright (C) 2017, 2018, 2019 Peter Laursen.

We split our encoder out into more files.
*/
#ifndef ABFSECTION_H
#define ABFSECTION_H
#include <opus/opus.h>
namespace ABF {
enum EncodingStatus {
Waiting,
Encoding,
Finished
};
class AbfSection {
int FrameSize = -1;
OpusEncoder* Encoder = nullptr;
int fd = 0;
short* TempBuffer = nullptr;
short TempBufferPosition = 0;
char FileTemplate[40]= {0};
char* FileBuffer = nullptr;
int FileBufferPosition = 0;
public:
AbfSection(unsigned short SamplingRate = 16000);
~AbfSection();
EncodingStatus Status = Waiting;
void Close();
void Encode(const short* Samples, int& Length);
int Getfd() const { return fd; }
const char* TempFile() { return FileTemplate; }
};
}
#endif

