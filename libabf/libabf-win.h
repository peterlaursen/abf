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
//using std::FILE;
//using std::string;
class SHARED AbfDecoder {
int* Array;
OpusDecoder* Decoder;
FILE* fin;
bool _IsOpen;
bool _IsValid;
char* Title;
char* Author;
char* Time;
unsigned short HeaderSize, Major, Minor, NumSections;
void ReadHeader();
bool Validate();
public:
AbfDecoder(char* Filename);
AbfDecoder();
~AbfDecoder();
void Initialize(char* Filename);
void Reset();
bool IsValid() const { return _IsValid; }
bool IsOpen() const { return _IsOpen; }
const char* GetTitle() const;
const char* GetAuthor() const;
const char* GetTime() const;
const unsigned short GetNumSections() const;
const unsigned short GetHeaderSize() const { return HeaderSize; }
const unsigned short GetMajor();
const unsigned short GetMinor() const;
bool feof();
int ftell();
void fclose();
int Seek(long offset, int whence);
int* GetSections();
void Decode(short* Output);
/*
This function currently will not work since our format has changed quite a bit. I'll see what I can do about it.
*/
bool GoToPosition(int Minutes);
};
class SHARED AbfEncoder {
OpusEncoder* Encoder;
FILE* fout;
unsigned short HeaderSize;
string _Title, _Author, _Time;
unsigned short _NumSections;
public:
AbfEncoder(char* Filename);
AbfEncoder();
void Initialize(char*);
~AbfEncoder();
void SetTitle(const char* Title);
void SetAuthor(const char* Author);
void SetTime(const char* Time);
void SetNumSections(unsigned short NumSections);
void WriteHeader();
void WriteSection();
void Encode(short* Input);
};
// Functions common to most of the programs developed so far...
// DecodeToRawAudio
//char* DecodeToRawAudio(const char* Filename);
//bool EncodeABF(AbfEncoder& AE, char* TempFile);
}
#endif
