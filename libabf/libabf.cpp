/* LIBABF.CPP, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This file contains the implementation of both the encoder and decoder. We'll concentrate of the encoder first.
*/
#include <cstdio>
#include <string>
#include <speex/speex.h>
#include "libabf.h"
using namespace std;
namespace ABF {
AbfEncoder::AbfEncoder(): Author("none"), Title("none"), Time("not set"), NumSections(0), CurrentSection(0), fin(0) {
Encoder = speex_encoder_init(&speex_wb_mode);
speex_bits_init(&Bits);
}
AbfEncoder::AbfEncoder(const char* Filename, const char* Author, const char* Title, const char* Time, unsigned short NumSections): Author(Author), Title(Title), Time(Time), NumSections(NumSections) {
Encoder = speex_encoder_init(&speex_wb_mode);
speex_bits_init(&Bits);
fin = fopen(Filename, "wb+");
if (!fin) {
printf("ABFEncoder: Error, file not found.");
}
}
AbfEncoder::~AbfEncoder() {
	speex_encoder_destroy(Encoder);
	speex_bits_destroy(&Bits);
	fclose(fin);
}

string& AbfEncoder::GetAuthor() { return Author; }
string& AbfEncoder::GetTitle() { return Title; }
string& AbfEncoder::GetTime() { return Time; }
unsigned short AbfEncoder::GetNumSections() { return NumSections; }
FILE* AbfEncoder::GetFileHandle() { return fin; }
void AbfEncoder::SetAuthor(const char* _Author) { Author = string(_Author); }
void AbfEncoder::SetTitle(const char* _Title) { Title = string(_Title); }
void AbfEncoder::SetTime(const char* _Time) { Time = string(_Time); }
void AbfEncoder::SetNumSections(unsigned short _NumSections) { NumSections = _NumSections; }
bool AbfEncoder::OpenFile(const char* Filename) {
if (fin) fclose(fin);
fin = fopen(Filename, "wb+");
if (!fin) return false;
else return true;
}
void AbfEncoder::WriteHeader() {
fseek(fin, 0, SEEK_SET);
fwrite("ABF", 1, 3, fin);
unsigned short HeaderSize = 0;
fwrite(&HeaderSize, sizeof(short), 1, fin);
unsigned short Major = 1, Minor = 0;
// To make our life easier, we write this byte by byte.
HeaderSize += fwrite(&Major, 1, sizeof(short), fin);
HeaderSize += fwrite(&Minor, 1, sizeof(short), fin);
unsigned short Length;
Length = Title.length();
HeaderSize += fwrite(&Length, 1, sizeof(short), fin);
const char* Buffer = Title.c_str();
HeaderSize += fwrite(Buffer, 1, Length, fin);
Length = Author.length();
HeaderSize += fwrite(&Length, 1, sizeof(short), fin);
Buffer = Author.c_str();
HeaderSize += fwrite(Buffer, 1, Length, fin);
Length = Time.length();
HeaderSize += fwrite(&Length, 1, sizeof(short), fin);
Buffer = Time.c_str();
HeaderSize += fwrite(Buffer, 1, Length, fin);
HeaderSize += fwrite(&NumSections, 1, sizeof(short), fin);
fseek(fin, 3, SEEK_SET);
fwrite(&HeaderSize, sizeof(short), 1, fin);
fseek(fin, 0, SEEK_END);
int Position = 0;
for (unsigned short i = 0; i < NumSections; i++) {
fwrite(&i, sizeof(short), 1, fin);
fwrite(&Position, sizeof(int), 1, fin);
}

}
void AbfEncoder::WriteSection() {
int Position = ftell(fin);
fseek(fin, 3, SEEK_SET);
unsigned short HeaderSize;
fread(&HeaderSize, sizeof(short), 1, fin);
fseek(fin, HeaderSize, SEEK_CUR);
for (unsigned short i = 0; i <= CurrentSection; i++) {
fwrite(&i, sizeof(short), 1, fin);
fwrite(&Position, sizeof(int), 1, fin);
}
CurrentSection += 1;
fseek(fin, Position, SEEK_SET);
}
void AbfEncoder::Encode(short* Wave) {
speex_bits_reset(&Bits);
speex_encode_int(Encoder, Wave, &Bits);
char Output[400];
unsigned short Bytes = speex_bits_write(&Bits, Output, 400);
fwrite(&Bytes, sizeof(short), 1, fin);
fwrite(Output, sizeof(char), Bytes, fin);

}
bool AbfDecoder::OpenFile(const char* _Filename) {
	fin = fopen(_Filename, "rb");
	if (!fin) return false;
	else return true;
}
bool AbfDecoder::feof() { return std::feof(fin); }
int AbfDecoder::ftell() { return std::ftell(fin); }
const char* AbfDecoder::GetAuthor() { return Author; }
const char* AbfDecoder::GetTitle() { return Title; }
const char* AbfDecoder::GetTime() { return Time; }
unsigned short AbfDecoder::GetNumSections() { return NumSections; }
void AbfDecoder::ReadHeader() {
	fseek(fin, 0, SEEK_SET);
	char Buffer[4];
	fread(Buffer, 1, 3, fin);
	Buffer[3] = '\0';
	printf("I read this from the audio file: %s\n", Buffer);
	fread(&HeaderSize, sizeof(short), 1, fin);
	printf("Header Size: %d\n", HeaderSize);
	fread(&Major, sizeof(short), 1, fin);
	fread(&Minor, sizeof(short), 1, fin);
	unsigned short TitleLength;
	fread(&TitleLength, sizeof(short), 1, fin);
	Title = new char[TitleLength]+1;
	fread(Title, 1, TitleLength, fin);
Title[TitleLength] = '\0';
fread(&TitleLength, sizeof(short), 1, fin);
	Author = new char[TitleLength]+1;
fread(Author, 1, TitleLength, fin);
Author[TitleLength] = '\0';
fread(&TitleLength, sizeof(short), 1, fin);
Time = new char[TitleLength]+1;
fread(Time, 1, sizeof(short), fin);
Time[TitleLength] = '\0';
fread(&NumSections, sizeof(short), 1, fin);
}
// The array must be NumSections int big
void AbfDecoder::GetPositions(int* Array) {
	for (int i = 0; i < NumSections; i++) {
		fseek(fin, 2, SEEK_CUR);
		fread(&Array[i], sizeof(int), 1, fin);
	}
}
AbfDecoder::AbfDecoder() {
	Decoder = speex_decoder_init(&speex_wb_mode);
	speex_bits_init(&Bits);
}

AbfDecoder::~AbfDecoder() {
	delete[] Author;
	delete[] Title;
	delete[] Time;
	speex_decoder_destroy(Decoder);
	speex_bits_destroy(&Bits);
	fclose(fin);
}
FILE* AbfDecoder::GetFileHandle() { return fin; }
void AbfDecoder::Decode(short* Output) {
unsigned short BytesToRead;
char Buffer[200];
fread(&BytesToRead, sizeof(short), 1, fin);
fread(Buffer, 1, BytesToRead, fin);
speex_bits_read_from(&Bits, Buffer, BytesToRead);
speex_decode_int(Decoder, &Bits, Output);
return;
}
}
