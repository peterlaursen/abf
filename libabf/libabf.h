/* $Id$
Copyright (C) 2010, 2011, 2012, 2013, 2014 Peter Laursen.

This is our interface to our ABF library. Currently, we focus on upgrading our format slightly, so our player and other things may catch up as time progresses.
For all the previous work on ABF, see /branches/libabf-1.0.
That library is no longer updated and this library is incompatible with it.
*/

#ifndef LIBABF_H
#define LIBABF_H
#include <cstdio>
#include <string>
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
using namespace std;
class SHARED AbfDecoder {
int* Array = nullptr;
OpusDecoder* Decoder = nullptr;
FILE* fin = nullptr;
bool _IsOpen = false;
bool _IsValid = false;
char* Title = nullptr;
char* Author = nullptr;
char* Time = nullptr;
unsigned short HeaderSize = 0, Major = 0, Minor = 0, NumSections = 0;
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
const unsigned short GetNumSections() const;
const unsigned short GetHeaderSize() const { return HeaderSize; }
const unsigned short GetMajor() { return Major; }
const unsigned short GetMinor() const { return Minor; }
bool feof() const;
int ftell() const;
void fclose();
int Seek(long offset, int whence);
const int* GetSections() const;
void Decode(short* Output);
/*
This function currently will not work since our format has changed quite a bit. I'll see what I can do about it.
*/
bool GoToPosition(const int Minutes);
};
class SHARED AbfEncoder {
OpusEncoder* Encoder = nullptr;
FILE* fout = nullptr;
unsigned short HeaderSize = 0;
string _Title, _Author, _Time;
unsigned short _NumSections = 0;
unsigned char* Buffer = nullptr;
// Have a max size constant defined here.
const int MaxBufferSize = (1024 * 1024) * 32;
int CurrentBufferPosition = 0;
public:
AbfEncoder(const char* Filename);
AbfEncoder();
void Initialize(const char*);
~AbfEncoder();
void SetTitle(const char* Title);
void SetAuthor(const char* Author);
void SetTime(const char* Time);
void SetNumSections(unsigned short NumSections);
void WriteHeader();
void WriteSection();
void Encode(const short* Input);
};
}
#endif
