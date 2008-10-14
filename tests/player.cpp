#include <audiere.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio>
using namespace std;
using namespace audiere;
int main(int argc, char* argv[]) {
void* Decoder = speex_decoder_init(&speex_wb_mode);
SpeexBits Bits;
speex_bits_init(&Bits);
short Buffer[320];
short Buffer1[32000];
AudioDevicePtr Device(OpenDevice());
FILE* Book = fopen(argv[1], "rb");
if (!Book) {
cout << "The book was not found." << endl;
speex_bits_destroy(&Bits);
speex_decoder_destroy(Decoder);
return 1;
}
SampleFormat SF = SF_S16;
OutputStreamPtr Stream;
char Input[200];
unsigned short Bytes;
// We'll need to read our newly added headers.
unsigned short NumSections;
fread(&NumSections, sizeof(short), 1, Book);
int* Array = new int[NumSections];
for (int i = 0; i < NumSections; i++) {
fseek(Book, 2, SEEK_CUR);
fread(&Array[i], sizeof(int), 1, Book);
}
cout << "Type in the section you want to go to: " << endl;
int Section;
cin >> Section;
fseek(Book, Array[Section], SEEK_SET);
while (!feof(Book)) {
for (int i = 0; i < 32000; i+=320) {
if (feof(Book)) break;
fread(&Bytes, 2, 1, Book);
fread(Input, 1, Bytes, Book);
speex_bits_read_from(&Bits, Input, Bytes);
speex_decode_int(Decoder, &Bits, Buffer);
for (int j = 0; j < 320; j++) Buffer1[i+j] = Buffer[j];
}
Stream = Device->openBuffer(Buffer1, 32000, 1, 16000, SF);
Stream->play();
while (Stream->isPlaying());
}
speex_bits_destroy(&Bits);
speex_decoder_destroy(Decoder);
fclose(Book);
delete[] Array;
}
