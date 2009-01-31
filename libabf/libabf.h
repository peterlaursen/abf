#ifndef LIBABF_H
#define LIBABF_H
#include <cstdio>
#include <string>
#include <speex/speex.h>
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
class SHARED AbfDecoder {
int* Array;
	void* Decoder;
SpeexBits Bits;
FILE* fin;
bool _IsOpen;
char* Title;
char* Author;
char* Time;
unsigned short HeaderSize, Major, Minor, NumSections;
public:
AbfDecoder(char* Filename);
~AbfDecoder();
bool Validate();
bool IsOpen() { return _IsOpen; }
 char* GetTitle();
 char* GetAuthor();
 char* GetTime();
unsigned short GetNumSections();
unsigned short GetHeaderSize() { return HeaderSize; }
unsigned short GetMajor();
unsigned short GetMinor();
bool feof();
int ftell();
void fclose();
void ReadHeader();
int* GetSections();
void Decode(short* Output);
FILE* GetFileHandle();
};
class SHARED AbfEncoder {
void* Encoder;
SpeexBits Bits;
FILE* fout;
unsigned short HeaderSize;
string _Title, _Author, _Time;
unsigned short _NumSections;
public:
AbfEncoder(char* Filename);
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
SHARED char* DecodeToRawAudio(const char* Filename);
SHARED bool EncodeABF(AbfEncoder& AE, char* TempFile);
}
#endif
