#include <audiere.h>
#include <speex/speex.h>
#include "database.h"
#include <libabf.h>
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
using namespace ABF;
bool Quit = false;
bool Previous = false;
bool Next = false;
bool Paused = false;
bool VolumeUp = false;
bool VolumeDown = false;
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
AbfDecoder AD;
AD.OpenFile((char*)Filename);
AD.ReadHeader();
unsigned short NumSections = AD.GetNumSections();
AudioDevicePtr Device(OpenDevice());
SampleFormat SF = SF_S16;
OutputStreamPtr Stream;
#ifdef WIN32
{
char ConTitle[255];
GetConsoleTitle(ConTitle, 255);
string Temp = ConTitle;
Temp += " - ";
Temp += AD.GetTitle();
SetConsoleTitle(Temp.c_str());
}
#endif
int* Array = new int[AD.GetNumSections()];
AD.GetPositions(Array);
int CurrentSection = 0;
int LastPosition = GetLastPosition((char*)AD.GetTitle());
if (LastPosition > 0) {
cout << "Last Position: " << LastPosition << endl;
for (int i = 0; i < NumSections; i++) {
if (LastPosition < Array[i]) {
CurrentSection = i-1;
break;
}
fseek(AD.GetFileHandle(), LastPosition, SEEK_SET);
}
} // End of if (LastPosition > 0)
float Volume = 1.0;
short Samples[320];
short Buffer[32000];
while (!Quit) {
if (AD.feof()) break;		
if (AD.ftell() > Array[CurrentSection+1]) CurrentSection += 1;
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
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
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
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
Previous = false;
}
LastPosition = AD.ftell();
for (int i = 0; i < 32000; i+=320) {
if (AD.feof()) break;
AD.Decode(Samples);
for (int j = 0; j < 320; j++) Buffer[i+j] = Samples[j];
}
Stream = Device->openBuffer(Buffer, 32000, 1, 16000, SF);
Stream->setVolume(Volume);
Stream->play();
while (Stream->isPlaying());
}
if (Quit) SaveLastPosition((char*)AD.GetTitle(), LastPosition);
delete[] Array;
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
