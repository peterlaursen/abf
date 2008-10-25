#ifndef LIBABF_H
#define LIBABF_H
#include <cstdio>
#include <speex/speex.h>
#include <speex/speex_resampler.h>
#include <string>
namespace ABF {
using std::FILE;
using std::string;
class AbfEncoder {
int CurrentSection;
int NumSections;
FILE* fin;
void* Encoder;
SpeexBits Bits;
string Author, Title, Time;
public:
AbfEncoder();
AbfEncoder(const char* Filename, const char* Author, const char* Title, const char* Time, unsigned short NumSections);
~AbfEncoder();
void WriteHeader();
void WriteSection();
unsigned short GetNumSections();
string& GetAuthor();
string& GetTitle();
string& GetTime();
void SetAuthor(const char* Author);
void SetTitle(const char* Title);
void SetTime(const char* Time);
void SetNumSections(unsigned short Num);
bool OpenFile(const char* Filename);
FILE* GetFileHandle();
void Encode(short* WaveBuffer);
};
class AbfDecoder {
unsigned short HeaderSize, Major, Minor;
char* Author;
char* Title;
char* Time;
unsigned short NumSections;
SpeexBits Bits;
void* Decoder;
FILE* fin;
public:
AbfDecoder();
~AbfDecoder();
void ReadHeader();
void GetPositions(int* Array);
bool feof();
int ftell();
void Decode(short* Output);
bool OpenFile(const char*);
unsigned short GetNumSections();
const char* GetTitle();
const char* GetAuthor();
const char* GetTime();
FILE* GetFileHandle();

};
}
#endif
