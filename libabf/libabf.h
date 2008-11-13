#ifndef LIBABF_H
#define LIBABF_H
#include <cstdio>
#include <string>
#include <speex/speex.h>
namespace ABF {
using std::FILE;
using std::string;
class AbfDecoder {
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
unsigned short GetHeaderSize();
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
class AbfEncoder {
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
}
#endif