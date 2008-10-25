#include <audiere.h>
#include <speex/speex.h>
#include <iostream>
#include <cstdio>
#ifdef WIN32
#include <process.h>
#include <conio.h>
#include <windows.h>
#endif
using namespace std;
using namespace audiere;
bool Quit = false;
bool BookIsFinished = false;
bool Previous = false;
bool Next = false;
bool Paused = false;
bool FirstSection = false;
bool LastSection = false;
void Thread(void* Filename) {
void* Decoder = speex_decoder_init(&speex_wb_mode);
SpeexBits Bits;
speex_bits_init(&Bits);
short Buffer[320];
short Buffer1[32000];
AudioDevicePtr Device(OpenDevice());
FILE* Book = fopen((char*)Filename, "rb");
if (!Book) {
cout << "The book was not found." << endl;
speex_bits_destroy(&Bits);
speex_decoder_destroy(Decoder);
return;
}
SampleFormat SF = SF_S16;
OutputStreamPtr Stream;
char Input[200];
unsigned short Bytes;
// We'll need to read our newly added headers.
unsigned short TitleLength;
fread(&TitleLength, 2, 1, Book);
char* Title = new char[TitleLength+1];
Title[TitleLength] = '\0';
fread(Title, 1, TitleLength, Book);
cout << "Book Title: " << Title << endl;
fread(&TitleLength, sizeof(short), 1, Book);
char* Author = new char[TitleLength+1];
Author[TitleLength] = '\0';
fread(Author, 1, TitleLength, Book);
cout << "Author: " << Author << endl;
fread(&TitleLength, sizeof(short), 1, Book);
char* Time = new char[TitleLength+1];
fread(Time, 1, TitleLength, Book);
Time[TitleLength] = '\0';
cout << Time << endl;
unsigned short NumSections;
fread(&NumSections, sizeof(short), 1, Book);
cout << NumSections << endl;
int* Array = new int[NumSections];
for (int i = 0; i < NumSections; i++) {
fseek(Book, 2, SEEK_CUR);
fread(&Array[i], sizeof(int), 1, Book);
}
int CurrentSection = 0;
while (!feof(Book) && !Quit) {
if (ftell(Book) > Array[CurrentSection+1]) CurrentSection += 1;
if (Paused) {
Stream->stop();
continue;
}
if (FirstSection) {
CurrentSection = 0;
fseek(Book, Array[CurrentSection], SEEK_SET);
FirstSection = false;
}
if (LastSection) {
CurrentSection = NumSections-1;
fseek(Book, Array[CurrentSection], SEEK_SET);
LastSection = false;
}

if (Next) {
if (CurrentSection >= NumSections - 1) {
Next = false;
CurrentSection = NumSections-1;
continue;
}
Stream->stop();
CurrentSection += 1;
fseek(Book, Array[CurrentSection], SEEK_SET);
Next = false;
}
if (Previous) {
if (CurrentSection == 0) {
Previous = false;
continue;
}
if (CurrentSection >= NumSections) CurrentSection = NumSections-1;
Stream->stop();
CurrentSection -= 1;
fseek(Book, Array[CurrentSection], SEEK_SET);
Previous = false;
}

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
delete[] Title;
delete[] Author;
delete[] Time;
}
void Input() {
char Key = getch(); 
if (Key == 'b') Next = true;
if (Key == 'v' || Key == 'c') Paused = true;
if (Key == 'x') Paused = false;
if (Key == 'f') FirstSection = true;
if (Key == 'l') LastSection = true;
if (Key == 'z') Previous = true;
if (Key == 'q') Quit = true;
}
int main(int argc, char* argv[]) {
SetConsoleTitle("ABF Player");
HANDLE ThreadID = (HANDLE)_beginthread(Thread, 0, argv[1]);
while (!Quit && !BookIsFinished) {
if (kbhit()) Input();
Sleep(250);
}
WaitForSingleObject(ThreadID, INFINITE);
return 0;
}
