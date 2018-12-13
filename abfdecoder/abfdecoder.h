/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017 Peter Laursen.

This is our interface to our ABF library. Currently, we focus on upgrading our format slightly, so our player and other things may catch up as time progresses.
For all the previous work on ABF, see /branches/libabf-1.0.
That library is no longer updated and this library is incompatible with it.
*/
#ifndef ABFDECODER_H
#define ABFDECODER_H
#include <cstdio>
#include <string>
#include <vector>
#include <opus/opus.h>
#ifdef WIN32
#ifdef BUILD_DLL
#define SHARED __declspec(dllexport)
#else
#define SHARED __declspec(dllimport)
#endif
#else
#define SHARED
#endif
namespace ABF {
using std::FILE;
using std::string;
using std::vector;
class SHARED AbfDecoder {
int* Array = nullptr;
int* MinutePositions = nullptr;
OpusDecoder* Decoder = nullptr;
FILE* fin = nullptr;
bool _IsOpen = false;
bool _IsValid = false;
char* Title = nullptr;
char* Author = nullptr;
char* Time = nullptr;
unsigned short HeaderSize, Major, Minor, NumSections, NumMinutes;
int IndexTableStartPosition = 0;
void ReadHeader();
bool Validate();
public:
AbfDecoder(const char* Filename);
AbfDecoder();
~AbfDecoder();
void Initialize(const char* Filename);
void Reset();
bool IsValid() const { return _IsValid; }
bool IsOpen() const { return _IsOpen; }
const char* GetTitle() const;
const char* GetAuthor() const;
const char* GetTime() const;
unsigned short GetNumSections() const;
unsigned short GetHeaderSize() const { return HeaderSize; }
unsigned short GetMajor() { return Major; }
unsigned short GetMinor() const { return Minor; }
bool feof() const;
int ftell() const;
void fclose();
int Seek(long offset, int whence);
const int* GetSections() const;
void Decode(short* Output) const;
/*
This function only works with ABF 2.1, the absolutely latest format.
*/
int GetMinutes() const { return NumMinutes; }
const int* GetMinutePositions() { return MinutePositions; }
bool GoToPosition(const int Minutes);
int GetGain() const;
void SetGain(int NewGain);
int GetSamplingRate() const;
void SetSamplingRate(int SampleRate);
};
}
#endif
