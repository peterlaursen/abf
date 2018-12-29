/* $Id$
Copyright (C) 2017, 2018, 2019 Peter Laursen.

We split our encoder out into more files.
*/
#ifndef ABFSECTION_H
#define ABFSECTION_H
#ifndef SHARED
#ifdef BUILD_DLL
#define SHARED __declspec(dllexport)
#else
#define SHARED
#endif
#endif
#include <opus/opus.h>
#include <stdio.h>
namespace ABF {
enum SHARED EncodingStatus {
Waiting,
Encoding,
Finished
};
class SHARED AbfSection {
int FrameSize = -1;
int SamplingRate = -1;
OpusEncoder* Encoder = nullptr;
FILE* fd = nullptr;
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
FILE* Getfd() const { return fd; }
const char* TempFile() { return FileTemplate; }
void Init(int Samp);
};
}
#endif

