#include <cstdio>
#include <cstring>
#include "libabf.h"
using namespace std;
namespace ABF {
AbfDecoder::AbfDecoder(char* Filename) {
Decoder = speex_decoder_init(&speex_wb_mode);
speex_bits_init(&Bits);
fin = fopen(Filename, "rb");
if (!fin) _IsOpen = false;
else _IsOpen = true;
}
bool AbfDecoder::Validate() {
char Buffer[4];
Buffer[3] = '\0';
fread(Buffer, 1, 3, fin);
if (strcmp(Buffer, "ABF") == 0) return true;
else return false;
}
void AbfDecoder::ReadHeader() {
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
Array = new int[NumSections];
for (unsigned short i = 0; i < NumSections; i++) {
fseek(fin, 2, SEEK_CUR);
fread(&Array[i], sizeof(int), 1, fin);
}
return;
}
int* AbfDecoder::GetSections() {
return Array;
}
void AbfDecoder::Decode(short* Output) {
unsigned short Bytes;
char Input[320];
fread(&Bytes, 2, 1, fin);
fread(Input, 1, Bytes, fin);
speex_bits_read_from(&Bits, Input, Bytes);
speex_decode_int(Decoder, &Bits, Output);
}
int AbfDecoder::ftell() { return std::ftell(fin); }
bool AbfDecoder::feof() { return std::feof(fin); }
char* AbfDecoder::GetTitle() { return Title; }
char* AbfDecoder::GetAuthor() { return Author; }
char* AbfDecoder::GetTime() { return Time; }
unsigned short AbfDecoder::GetNumSections() { return NumSections; }
FILE* AbfDecoder::GetFileHandle() { return fin; }
AbfDecoder::~AbfDecoder() {
	fclose();
	delete[] Title;
	delete[] Author;
	delete[] Time;
delete[] Array;
}
void AbfDecoder::fclose() { std::fclose(fin); }
AbfEncoder::AbfEncoder(char* Filename) {
fout = fopen(Filename, "wb+");
Encoder = speex_encoder_init(&speex_wb_mode);
speex_bits_init(&Bits);
}
AbfEncoder::~AbfEncoder() {
fclose(fout);
speex_encoder_destroy(Encoder);
speex_bits_destroy(&Bits);
}
void AbfEncoder::SetTitle(const char* Title) { _Title = Title; }
void AbfEncoder::SetAuthor(const char* Author) { _Author = Author; }
void AbfEncoder::SetTime(const char* Time) { _Time = Time; }
void AbfEncoder::SetNumSections(unsigned short NumSections) { _NumSections = NumSections; }
void AbfEncoder::WriteHeader() {
fseek(fout, 0, SEEK_SET);
fwrite("ABF", 1, 3, fout);
HeaderSize = 0;
fwrite(&HeaderSize, sizeof(short), 1, fout);
unsigned short Major = 1, Minor = 0;
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
HeaderSize = ftell(fout);
fseek(fout, 3, SEEK_SET);
fwrite(&HeaderSize, sizeof(short), 1, fout);
fseek(fout, HeaderSize, SEEK_SET);
int Size = 0;
for (unsigned short i = 0; i < _NumSections; i++) {
fwrite(&i, sizeof(short), 1, fout);
fwrite(&Size, sizeof(int), 1, fout);
}
}
void AbfEncoder::WriteSection() {
static int CurrentSection = 0;
int Position = ftell(fout);
fseek(fout, HeaderSize, SEEK_SET);
for (int i = 0; i < CurrentSection; i++) fseek(fout, 6, SEEK_CUR);
// We are now at the beginning of a section header.
fseek(fout, 2, SEEK_CUR);
fwrite(&Position, sizeof(int), 1, fout);
fseek(fout, Position, SEEK_SET);
CurrentSection += 1;
}
void AbfEncoder::Encode(short* Input) {
speex_bits_reset(&Bits);
speex_encode_int(Encoder, Input, &Bits);
char Output[200];
unsigned short Bytes = speex_bits_write(&Bits, Output, 200);
fwrite(&Bytes, sizeof(short), 1, fout);
fwrite(Output, sizeof(char), Bytes, fout);
}
}