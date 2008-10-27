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
#endif
using namespace std;
using namespace audiere;
using namespace ABF;
bool Quit = false;
bool BookIsFinished = false;
bool Previous = false;
bool Next = false;
bool Paused = false;
bool FirstSection = false;
bool LastSection = false;
void Thread(void* Filename) {
AbfDecoder AD((char*)Filename);
bool IsValid = AD.Validate();
if (!IsValid) {
cout << "Error, not a valid ABF daisy AD.GetFileHandle()." << endl;
Quit = true;
return;
}
AD.ReadHeader();
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
while (!AD.feof() && !Quit) {
if (AD.ftell() > Array[CurrentSection+1]) CurrentSection += 1;
if (Paused) {
if (Stream->isPlaying()) Stream->stop();
continue;
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
Stream->play();
while (Stream->isPlaying());
}
if (Quit) SaveLastPosition(AD.GetTitle(), LastPosition);
else DeletePosition(AD.GetTitle());
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
