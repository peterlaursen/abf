#include <audiere.h>
#include <speex/speex.h>
#include "database.h"
#include <iostream>
#include <cstdio>
#include <libabf.h>
#ifdef WIN32
#include <process.h>
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#include <curses.h>
#endif

using namespace std;
using namespace audiere;
using namespace ABF;
// Status variables:
bool Quit = false;
bool BookIsFinished = false;
// Variables to do with actions
bool Previous = false;
bool Next = false;
bool Paused = false;
bool FirstSection = false;
bool LastSection = false;
bool VolumeUp = false;
bool VolumeDown = 0;
bool GoToSection = false;
bool JumpTime = false;
bool JumpToTime(AbfDecoder& AD) {
// This function will (hopefully) allow people to jump to various time positions within an audio book.
int HeaderSize = AD.GetHeaderSize() + (6*AD.GetNumSections());
cin.clear();
cout << endl << "Type in the position you want to go to in minutes: " << endl;
int Minutes;
#ifndef WIN32
nocbreak();
echo();
#endif
cin >> Minutes;
#ifndef WIN32
cbreak();
noecho();
#endif
if (Minutes < 0) return false;
// Clear cin (this is a bad hack!). Turn our minutes into seconds
cin.ignore(10000, '\n');
Minutes *= 60;
// Convert this into frames. 50 frames per second.
Minutes *= 50;
int LastPosition = AD.ftell();
fseek(AD.GetFileHandle(), HeaderSize, SEEK_SET);
FILE* Handle = AD.GetFileHandle();
unsigned short FrameSize;
for (int i = 0; i < Minutes; i++) {
if (fread(&FrameSize, sizeof(short), 1, Handle) <= 0) {
cout << "Cannot read from file, returning false." << endl;
fseek(Handle, LastPosition, SEEK_SET);
return false;
}
fseek(Handle, FrameSize, SEEK_CUR);
}
return true;
}
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
AbfDecoder AD((char*)Filename);
bool IsValid = AD.Validate();
if (!IsValid) {
cout << "Error, not a valid ABF daisy AD.GetFileHandle()." << endl;
Quit = true;
#ifdef WIN32
return;
#else
return 0;
#endif
}
AD.ReadHeader();
cout << "Commands you can use in the player: " << endl;
cout << "Key - Description" << endl;
cout << "f - Go to first section, l - go to last section" << endl;
cout << "x - Play, c or v - Pause" << endl;
cout << "b - Next section, z - Previous section" << endl;
cout << "g - go to section, j - jump to time" << endl;
cout << "< - Volume Down, > - Volume Up" << endl;
cout << "q - Quit" << endl;
cout << "Title: " << AD.GetTitle() << endl << "Author: " << AD.GetAuthor() << endl << "This book lasts " << AD.GetTime() << endl;
#ifdef WIN32
// Put this into a scope of its own so that it vanishes as soon as possible.
{
char ConTitle[255];
GetConsoleTitle(ConTitle, 255);
string Temp = ConTitle;
Temp += " - ";
Temp += AD.GetTitle();
SetConsoleTitle(Temp.c_str());
}
#endif
short Buffer[320];
short Buffer1[32000];
AudioDevicePtr Device(OpenDevice());
SampleFormat SF = SF_S16;
OutputStreamPtr Stream;
int* Array = AD.GetSections();
int CurrentSection = 0;
int LastPosition = GetLastPosition(AD.GetTitle());
if (LastPosition > 0) {
fseek(AD.GetFileHandle(), LastPosition, SEEK_SET);
// Set CurrentSection to the correct section
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] > LastPosition) {
CurrentSection = i-1;
break;
}
}
}
float Volume = 1.0f;
while (!AD.feof() && !Quit) {
// Ensure that CurrentSection is up-to-date
if (AD.ftell() > Array[CurrentSection+1]) CurrentSection += 1;
// The rest of this loop processes key presses.
if (VolumeDown) {
if (Volume == 0.0f) {
VolumeDown = false;
continue;
}
Volume -= 0.1f;
VolumeDown = false;
}
if (VolumeUp) {
if (Volume >= 1.0f) {
VolumeUp = false;
continue;
}
Volume += 0.1f;
VolumeUp = false;
}
if (Paused) {
if (Stream->isPlaying()) Stream->stop();
continue;
}
if (GoToSection) {
Stream->stop();
#ifndef WIN32
echo();
nocbreak();
#endif
cout << "Go To Section: (1-" << AD.GetNumSections() << "): ";
unsigned short NewSection;
cin.clear();
cin >> NewSection;
#ifndef WIN32
cbreak();
noecho();
#endif
--NewSection;
if (NewSection >= AD.GetNumSections()) NewSection = AD.GetNumSections() - 1;
CurrentSection = NewSection;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
GoToSection = false;
}
if (JumpTime) {
Stream->stop();
if (!JumpToTime(AD)) continue;
// Set current section
int Position = AD.ftell();
cout << "File position: " << AD.ftell() << endl;
for (int i = 0; i <= AD.GetNumSections(); i++) {
if (Array[i] > Position) {
CurrentSection = i;
break;
}
}
JumpTime = false;
cout << endl << "The current section is  " << CurrentSection << endl;
}
if (FirstSection) {
CurrentSection = 0;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
FirstSection = false;
}
if (LastSection) {
CurrentSection = AD.GetNumSections()-1;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
LastSection = false;
}

if (Next) {
if (CurrentSection >= AD.GetNumSections() - 1) {
Next = false;
CurrentSection = AD.GetNumSections()-1;
continue;
}
Stream->stop();
CurrentSection += 1;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
Next = false;
}
if (Previous) {
if (CurrentSection == 0) {
Previous = false;
continue;
}
if (CurrentSection >= AD.GetNumSections()) CurrentSection = AD.GetNumSections()-1;
Stream->stop();
CurrentSection -= 1;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
Previous = false;
}
// This bit pre-buffers input and decodes the output
LastPosition = ftell(AD.GetFileHandle());
for (int i = 0; i < 32000; i+=320) {
if (AD.feof()) {
BookIsFinished = true;
break;
}
AD.Decode(Buffer);
for (int j = 0; j < 320; j++) Buffer1[i+j] = Buffer[j];
}
Stream = Device->openBuffer(Buffer1, 32000, 1, 16000, SF);
Stream->setVolume(Volume);
Stream->play();
// Wait until the playback is finished, then go run the loop again
while (Stream->isPlaying());
}
if (Quit) SaveLastPosition(AD.GetTitle(), LastPosition);
else DeletePosition(AD.GetTitle());
}
void Input() {
char Key = getch(); 
if (Key == '<') VolumeDown = true;
if (Key == '>') VolumeUp = true;
if (Key == 'g') GoToSection = true;
if (Key == 'j') JumpTime = true;
if (Key == 'b') Next = true;
if (Key == 'v' || Key == 'c') Paused = true;
if (Key == 'x') Paused = false;
if (Key == 'f') FirstSection = true;
if (Key == 'l') LastSection = true;
if (Key == 'z') Previous = true;
if (Key == 'q') Quit = true;
}
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "You must specify an audio book to play." << endl;
return 1;
}
#ifndef WIN32
initscr();
cbreak();
noecho();
#endif
#ifdef WIN32
SetConsoleTitle("ABF Player");
HANDLE ThreadID = (HANDLE)_beginthread(Thread, 0, argv[1]);
#else
pthread* id;
pthread_create(&id, 0, Thread, argv[1]);

#endif
while (!Quit && !BookIsFinished) {
#ifdef WIN32
if (kbhit()) Input();
#else
Input();
#endif
#ifdef WIN32
Sleep(250);
#else
usleep(250);
#endif
}
#ifdef WIN32
WaitForSingleObject(ThreadID, INFINITE);
#else
pthread_join(id, 0);
nocbreak();
echo();
endwin();
#endif
return 0;
}
