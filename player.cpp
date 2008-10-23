#include <audiere.h>
#include <speex/speex.h>
#include "database.h"
#include <iostream>
#include <cstdio>
#include <string>
#ifdef WIN32
#include <process.h>
#include <conio.h>
#include <windows.h>
#else
#include <pthread.h>
#include "compat.h"
#endif
using namespace std;
using namespace audiere;
bool Quit = false;
bool Previous = false;
bool Next = false;
bool Paused = false;
bool VolumeUp = false;
bool VolumeDown = false;
bool DetectHeader(FILE* Book) {
char Buffer[4];
Buffer[3] = '\0';
fread(Buffer, 1, 3, Book);
if (strcmp(Buffer, "ABF") != 0) {
cout << "Old header." << endl;
return false;
}
return true;
}
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
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

#ifdef WIN32
return;
#else
return 0;
#endif
}
SampleFormat SF = SF_S16;
OutputStreamPtr Stream;
char Input[200];
unsigned short Bytes;
// We'll need to read our newly added headers.
bool HeaderType = DetectHeader(Book);
unsigned short NumSections = 0;
char* Title = 0;
char* Author = 0;
char* Time = 0;
if (!HeaderType) {
cout << "WARNING: These audio books have been encoded using an older format of the headers. The player still supports this format, but it is strongly recommended that you re-encode your audio books." << endl;
fseek(Book, 0, SEEK_SET);
fread(&NumSections, sizeof(short), 1, Book);
}
else {
fseek(Book, 3, SEEK_SET);
unsigned short HeaderSize;
fread(&HeaderSize, sizeof(short), 1, Book);
unsigned short Major, Minor;
fread(&Major, sizeof(short), 1, Book);
fread(&Minor, sizeof(short), 1, Book);
unsigned short TitleLength = 0, AuthorLength = 0;
fread(&TitleLength, sizeof(short), 1, Book);
Title = new char[TitleLength] + 1;
fread(Title, 1, TitleLength, Book);
Title[TitleLength] = '\0';
fread(&AuthorLength, sizeof(short), 1, Book);
Author = new char[AuthorLength] + 1;
fread(Author, 1, AuthorLength, Book);
Author[AuthorLength] = '\0';
#ifdef WIN32
{
char ConTitle[255];
GetConsoleTitle(ConTitle, 255);
string Temp = ConTitle;
Temp += " - ";
Temp += Title;
SetConsoleTitle(Temp.c_str());
}
#endif
unsigned short TimeLength = 0;
fread(&TimeLength, sizeof(short), 1, Book);
Time = new char[TimeLength+1];
fread(Time, 1, TimeLength, Book);
Time[TimeLength] = '\0';
cout << "Author: " << Author << endl << "Title: " << Title << endl << "This book lasts " << Time << endl;
fread(&NumSections, sizeof(short), 1, Book);
}
int* Array = new int[NumSections];
for (int i = 0; i < NumSections; i++) {
fseek(Book, 2, SEEK_CUR);
fread(&Array[i], sizeof(int), 1, Book);
}
int CurrentSection = 0;
int LastPosition = GetLastPosition(Title);
if (LastPosition > 0) {
cout << "Last Position: " << LastPosition << endl;
for (int i = 0; i < NumSections; i++) {
if (LastPosition < Array[i]) {
CurrentSection = i-1;
break;
}
fseek(Book, LastPosition, SEEK_SET);
}
} // End of if (LastPosition > 0)
float Volume = 1.0;
while (!feof(Book) && !Quit) {
if (ftell(Book) > Array[CurrentSection+1]) CurrentSection += 1;
// Check the global Input parameters
if (Paused) {
if (Stream->isPlaying()) Stream->stop();
continue;
}
if (VolumeDown) {
if (Volume == 0.0f) {
VolumeDown = false;
continue;
}
Volume -= 0.1f;
VolumeDown = false;
}
if (VolumeUp) {
if (Volume >= 1.0) {
VolumeUp = false;
continue;
}
Volume += 0.1f;
VolumeUp = false;
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
Stream->setVolume(Volume);
Stream->play();
while (Stream->isPlaying());
}
if (Quit) SaveLastPosition(Book, Title);
else DeletePosition(Title);
speex_bits_destroy(&Bits);
speex_decoder_destroy(Decoder);
fclose(Book);
delete[] Array;
delete[] Author;
delete[] Title;
delete[] Time;
}
void Input() {
char Key = getch(); 
if (Key == 'b') Next = true;
if (Key == 'v' || Key == 'c') Paused = true;
if (Key == 'x') Paused = false;
if (Key == 'z') Previous = true;
if (Key == 'q') Quit = true;
if (Key == '<') VolumeDown = true;
if (Key == '>') VolumeUp = true;
}
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "You must provide me with a book to play." << endl;
return 1;
}
#ifdef WIN32
SetConsoleTitle("ABF Player");
HANDLE ThreadHandle = (HANDLE*)_beginthread(Thread, 0, argv[1]);
#else
pthread_t id;
pthread_create(&id, 0, Thread, argv[1]);
#endif
while (!Quit) Input();
#ifdef WIN32
WaitForSingleObject(ThreadHandle, INFINITE);
#else
pthread_join(id, 0);
#endif
return 0;
}
