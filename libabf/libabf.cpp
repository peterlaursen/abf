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
printf("Header Size: %d, Major: %d, Minor: %d\n", HeaderSize, Major, Minor);
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
printf("I am to read %d bytes.\n", Bytes);
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

}
