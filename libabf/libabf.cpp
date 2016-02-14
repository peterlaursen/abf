/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This is the main ABF library which is used for converting books to our ABF format.
It is also used for decoding the format.

Note that this library uses ABF 2.0 only.
For the previous library utilizing Speex for its encoding and decoding, see /code/branches/abf-1.0.
*/

#include <cstdio>
#include <cstring>
#include "libabf.h"
#ifdef WIN32
#ifdef BUILD_DLL
#pragma comment(lib, "opus.lib")
#pragma comment(lib, "celt.lib")
#pragma comment(lib, "silk_common.lib")
#pragma comment(lib, "silk_float.lib")
#pragma comment(lib, "silk_fixed.lib")
#endif
#endif
using namespace std;
namespace ABF {
int SHARED AbfDecoder::Seek(long offset, int whence) { return fseek(fin, offset, whence); }
AbfDecoder::AbfDecoder(): fin(nullptr), _IsOpen(false) {}
AbfDecoder::AbfDecoder(const char* Filename) {
Initialize(Filename);
}
void AbfDecoder::Initialize(const char* Filename) {
if (!fin) Reset();
fin = fopen(Filename, "rb");
if (!fin) _IsOpen = false;
else _IsOpen = true;
if (Validate()) ReadHeader();
int Error = 0;
Decoder = opus_decoder_create(16000, 1, &Error);
if (Error != OPUS_OK) {
fprintf(stderr, "Something went wrong in creating our decoder!\n");
::fclose(fin);
_IsOpen = false;
_IsValid = false;
}
}
bool AbfDecoder::Validate() {
rewind(fin);
char Buffer[4];
Buffer[3] = '\0';
fread(Buffer, 1, 3, fin);
unsigned short HeaderSize, Major, Minor;
fread(&HeaderSize, sizeof(short), 1, fin);
fread(&Major, sizeof(short), 1, fin);
fread(&Minor, sizeof(short), 1, fin);

if (strcmp(Buffer, "ABF") == 0 && HeaderSize > 0 && Major == 2 && Minor >= 0) _IsValid = true;
else {
if (Major == 1) {
fprintf(stderr, "This file was encoded with an older version of libabf. We do not support that version of the format anymore. Please re-convert your ABF book.\n");
}
_IsValid = false;
}
return _IsValid;
}
void AbfDecoder::ReadHeader() {
::fseek(fin, 3, SEEK_SET);
fread(&HeaderSize, sizeof(short), 1, fin);
fread(&Major, sizeof(short), 1, fin);
fread(&Minor, sizeof(short), 1, fin);
unsigned short TitleLength;
fread(&TitleLength, sizeof(short), 1, fin);
Title = new char[TitleLength+1];
Title[TitleLength] = '\0';
fread(Title, 1, TitleLength, fin);
fread(&TitleLength, sizeof(short), 1, fin);
Author = new char[TitleLength+1];
Author[TitleLength] = '\0';
fread(Author, 1, TitleLength, fin);
fread(&TitleLength, sizeof(short), 1, fin);
Time = new char[TitleLength+1];
fread(Time, 1, TitleLength, fin);
Time[TitleLength] = '\0';
fread(&NumSections, sizeof(short), 1, fin);
if (Major == 2 && Minor == 1) {
fread(&NumMinutes, 1, sizeof(short), fin);
fread(&IndexTableStartPosition, 1, sizeof(int), fin);
MinutePositions = new int[NumMinutes];
int CurrentPosition = ftell();
fseek(fin, IndexTableStartPosition, SEEK_SET);
for (int i = 0; i < NumMinutes; i++) fread(&MinutePositions[i], 1, sizeof(int), fin);
fseek(fin, CurrentPosition, SEEK_SET);
}
Array = new int[NumSections];
for (unsigned short i = 0; i < NumSections; i++) {
fread(&Array[i], sizeof(int), 1, fin);
}
return;
}
const int* AbfDecoder::GetSections() const {
return Array;
}
void AbfDecoder::Decode(short* Output) {
unsigned short Bytes;
unsigned char Input[320] = {0};
fread(&Bytes, 2, 1, fin);
int BytesRead = fread(Input, 1, Bytes, fin);
if (feof()) return;

int Error = opus_decode(Decoder, Input, BytesRead, Output, 320, 0);
if (Error < 0 && Error != OPUS_OK) {
fprintf(stderr, "Error decoding Opus frame.\n");
}
}
int AbfDecoder::ftell() const { return std::ftell(fin); }
bool AbfDecoder::feof() const { 
if (Major == 2 && Minor >= 1)
return (ftell() >= IndexTableStartPosition)?true:false; else return std::feof(fin);
}

const char* AbfDecoder::GetTitle() const { return Title; }
const char* AbfDecoder::GetAuthor() const { return Author; }
const char* AbfDecoder::GetTime() const { return Time; }
const unsigned short AbfDecoder::GetNumSections() const { return NumSections; }
bool AbfDecoder::GoToPosition(int minutes) { 

fseek(fin, MinutePositions[minutes], SEEK_SET);
return true;
 }

/* This function will not currently work - we therefore uncomment it until we can look at it more closely.

bool AbfDecoder::GoToPosition(int Minutes) {
// If input is wrong, bail out
if (Minutes < 0) return false;
// Get the header size + the size of the sections array.
int HeaderSize = GetHeaderSize() + (6*GetNumSections());
// Convert the minutes into seconds.
Minutes *= 60;
// Convert this into frames. 50 frames per second.
Minutes *= 50;
int LastPosition = ftell();
// Get the file size
Seek(0, SEEK_END);
long FileSize = this->ftell();
Seek(HeaderSize, SEEK_SET);
unsigned short FrameSize = 72;
long SearchPosition = (Minutes*FrameSize) + HeaderSize;
if (SearchPosition > FileSize) {
fprintf(stderr, "The book isn't that long.\n");
Seek(LastPosition, SEEK_SET);
return false;
}
Seek(SearchPosition, SEEK_SET);
return true;
}
*/

AbfDecoder::~AbfDecoder() {
Reset();
}
void AbfDecoder::Reset() {
if (_IsOpen) {
fclose();
_IsOpen = false;
_IsValid = false;
}
if (Major == 2 && Minor == 1) {
delete[] MinutePositions;
MinutePositions = nullptr;
}

opus_decoder_destroy(Decoder);
delete[] Title;
Title = nullptr;
delete[] Author;
Author = nullptr;
delete[] Time;
Time = nullptr;
delete[] Array;
Array = nullptr;
}
void AbfDecoder::fclose() { std::fclose(fin); fin = nullptr; }
AbfEncoder::AbfEncoder(): fout(nullptr) {}

AbfEncoder::AbfEncoder(const char* Filename) {
Initialize(Filename);
}
void AbfEncoder::Initialize(const char* Filename) {
Buffer = new unsigned char[MaxBufferSize];
memset(Buffer, '\0', MaxBufferSize);
fout = fopen(Filename, "wb+");
int Error = 0;
Encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &Error);
if (Error != OPUS_OK) {
fprintf(stderr, "Error in creating our encoder!\n");
delete[] Buffer;
return;
}

}
AbfEncoder::~AbfEncoder() {
if (CurrentBufferPosition > 0)
fwrite(Buffer, CurrentBufferPosition, 1, fout);
// Let's write the last bits of the file so that we can finalize it before we close it.
IndexTableStartPosition = ftell(fout);
int StartPosition = HeaderSize - sizeof(unsigned short) - sizeof(int);
fseek(fout, StartPosition, SEEK_SET);
NumMinutes = MinutePositions.size();
fwrite(&NumMinutes, 1, sizeof(short), fout);
fwrite(&IndexTableStartPosition, 1, sizeof(int), fout);
fseek(fout, IndexTableStartPosition, SEEK_SET);
for (int i = 0; i < MinutePositions.size(); i++) fwrite(&MinutePositions[i], 1, sizeof(int), fout);
fclose(fout);
opus_encoder_destroy(Encoder);
delete[] Buffer;
Buffer = nullptr;
CurrentBufferPosition = 0;
}
void AbfEncoder::SetTitle(const char* Title) { _Title = Title; }
void AbfEncoder::SetAuthor(const char* Author) { _Author = Author; }
void AbfEncoder::SetTime(const char* Time) { _Time = Time; }
void AbfEncoder::SetNumSections(unsigned short NumSections) { _NumSections = NumSections; }
void AbfEncoder::WriteHeader() {
rewind(fout);
fwrite("ABF", 1, 3, fout);
HeaderSize = 0;
fwrite(&HeaderSize, sizeof(short), 1, fout);
unsigned short Major = 2, Minor = 1;
fwrite(&Major, sizeof(short), 1, fout);
fwrite(&Minor, sizeof(short), 1, fout);
unsigned short Length = _Title.length();
fwrite(&Length, sizeof(short), 1, fout);
fwrite(_Title.c_str(), 1, Length, fout);
Length = _Author.length();
fwrite(&Length, sizeof(short), 1, fout);
fwrite(_Author.c_str(), 1, Length, fout);
Length = _Time.length();
fwrite(&Length, sizeof(short), 1, fout);
fwrite(_Time.c_str(), 1, Length, fout);
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
void AbfEncoder::WriteSection() {
static int CurrentSection = 0;
if (CurrentBufferPosition > 0) {
fwrite(Buffer, CurrentBufferPosition, 1, fout);
memset(Buffer, '\0', MaxBufferSize);
CurrentBufferPosition = 0;
}
int Position = ftell(fout);
fseek(fout, HeaderSize, SEEK_SET);
for (int i = 0; i < CurrentSection; i++) fseek(fout, 4, SEEK_CUR);
// We are now at the beginning of a section header.
fwrite(&Position, sizeof(int), 1, fout);
fseek(fout, Position, SEEK_SET);
CurrentSection += 1;
}
void AbfEncoder::Encode(const short* Input) {
unsigned char Output[200] = {0};
short Bytes = opus_encode(Encoder, Input, 320, Output, 200);
if (CurrentBufferPosition + Bytes >= MaxBufferSize - 1) {
fwrite(Buffer, CurrentBufferPosition, 1, fout);
memset(Buffer, '\0', MaxBufferSize);
CurrentBufferPosition = 0;
}
memcpy(&Buffer[CurrentBufferPosition], &Bytes, sizeof(short));
CurrentBufferPosition += sizeof(short);
memcpy(&Buffer[CurrentBufferPosition], Output, Bytes);
CurrentBufferPosition += Bytes;
++FramesEncoded;
if (FramesEncoded % 3000 == 0) {
int Position = ftell(fout)+CurrentBufferPosition;
MinutePositions.push_back(Position);
}
}
}
