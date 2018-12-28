/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019 Peter Laursen.

This is the main ABF library which is used for converting books to our ABF format.

Note that we remove the ABFDecoder from this library.
The library only supports encoding the latest formats.
Note furthermore that this is intended to work only on Unix platforms.

*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <pthread.h>
#include "abfencoder.h"
#include "abfsection.h"
#include <fcntl.h>
using namespace std;
namespace ABF {
AbfEncoder::AbfEncoder(const char* Filename, unsigned short NumSections, unsigned short SamplingRate): _NumSections(NumSections), SamplingRate(SamplingRate) {
Initialize(Filename);
}
void AbfEncoder::Initialize(const char* Filename) {

if (_NumSections > 0 && (SamplingRate == 16000 || SamplingRate == 24000 || SamplingRate == 48000)) {
AbfSections = new AbfSection[_NumSections];
for (int i = 0; i < _NumSections; i++) AbfSections[i].Init(SamplingRate);
}
fout = fopen(Filename, "wb+");
}
AbfEncoder::~AbfEncoder() {
Lock();
for (int i = 0; i < _NumSections; i++) {
AbfSections[i].Close();
}
Unlock();
// Let's write the last bits of the file so that we can finalize it before we close it.
IndexTableStartPosition = ftell(fout);
int StartPosition = HeaderSize - sizeof(unsigned short) - sizeof(int);
fseek(fout, StartPosition, SEEK_SET);
NumMinutes = MinutePositions.size();
fwrite(&NumMinutes, 1, sizeof(short), fout);
fwrite(&IndexTableStartPosition, 1, sizeof(int), fout);
fseek(fout, IndexTableStartPosition, SEEK_SET);
for (int i = 0; i < MinutePositions.size(); i++) fwrite(&MinutePositions[i], 1, sizeof(int), fout);
delete[] AbfSections;
fclose(fout);
}
void AbfEncoder::Lock() { 
mtx.lock();
}
void AbfEncoder::Unlock() {
mtx.unlock();
}
void AbfEncoder::SetTitle(const char* Title) { _Title = Title; }
void AbfEncoder::SetAuthor(const char* Author) { _Author = Author; }
const unsigned short AbfEncoder::GetSamplingRate() const { return SamplingRate; }
void AbfEncoder::WriteHeader() {
rewind(fout);
fwrite("ABF", 1, 3, fout);
HeaderSize = 0;
fwrite(&HeaderSize, sizeof(short), 1, fout);
unsigned short Major = 2, Minor = 2;
fwrite(&Major, sizeof(short), 1, fout);
fwrite(&Minor, sizeof(short), 1, fout);
unsigned short Length = _Title.length();
fwrite(&Length, sizeof(short), 1, fout);
fwrite(_Title.c_str(), 1, Length, fout);
Length = _Author.length();
fwrite(&Length, sizeof(short), 1, fout);
fwrite(_Author.c_str(), 1, Length, fout);
fwrite(&SamplingRate, 1, sizeof(unsigned short), fout);
fwrite(&_NumSections, sizeof(short), 1, fout);
fwrite(&NumMinutes, 1, sizeof(unsigned short), fout);
fwrite(&IndexTableStartPosition, 1, sizeof(int), fout);
HeaderSize = ftell(fout);
fseek(fout, 3, SEEK_SET);
fwrite(&HeaderSize, sizeof(short), 1, fout);
fseek(fout, HeaderSize, SEEK_SET);
int Size = 0;
for (unsigned short i = 0; i < _NumSections; i++) {
fwrite(&Size, sizeof(int), 1, fout);
}
}
void AbfEncoder::Encode(unsigned short NumSection, const short* Input, int& Length) {
AbfSections[NumSection].Encode(Input, Length);
}
void AbfEncoder::Gather() {
unsigned char FileBuffer[1024]={0};
for (int i = 0; i < _NumSections; i++) {
int CurPos = ftell(fout);
fseek(fout, HeaderSize, SEEK_SET);
for (int z = 0; z < i; z++) fseek(fout, 4, SEEK_CUR);
fwrite(&CurPos, sizeof(int), 1, fout);
fseek(fout, CurPos, SEEK_SET);
if (AbfSections[i].Status != Finished) continue;

int fd = open(AbfSections[i].TempFile(), O_RDONLY);
int status = 0;
do {
status = read(fd, &FileBuffer, 1024);
fwrite(&FileBuffer, status, 1, fout);
} while (status > 0);
}
fseek(fout, HeaderSize, SEEK_SET);
for (int i = 0; i < _NumSections; i++) fseek(fout, 4, SEEK_CUR);
int FramesRead = 0;
do {
if (FramesRead % 3000 == 0) MinutePositions.push_back(ftell(fout));
unsigned short FrameSize = 0;
unsigned char Data[200] = {0};
int _FrameSize = fread(&FrameSize, sizeof(short), 1, fout);
int _DataSize = fread(Data, FrameSize, 1, fout);
++FramesRead;
} while(!feof(fout));
}
void AbfEncoder::Cleanup() {
Lock();
for (int i = 0; i < _NumSections; i++) {
AbfSections[i].Close();
unlink(AbfSections[i].TempFile());
}
Unlock();
}
}
