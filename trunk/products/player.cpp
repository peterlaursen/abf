#include <AudioToolbox/AudioToolbox.h>
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
#include "compat.h"
#include <unistd.h>
#include <pthread.h>
#endif

using namespace std;
using namespace ABF;
bool Quit = false;
bool BookIsFinished = false;
bool Previous = false;
bool Next = false;
bool Paused = false;
bool FirstSection = false;
bool LastSection = false;
bool VolumeUp = false;
bool VolumeDown = 0;
void OutputCallback(void* AD, AudioQueueRef Queue, AudioQueueBufferRef Buffer) {
AbfDecoder* Decoder = (AbfDecoder*)AD;
short TempBuffer[320];
short* AudioData = (short*)Buffer->mAudioData;
for (int i = 0; i < 32000; i++) {
Decoder->Decode(TempBuffer);
for (int j = 0; j < 320; j++) AudioData[i+j] = TempBuffer[j];

}
}
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
AbfDecoder AD((char*)Filename);
// Initialize the Mac specific stuff if possible.
AudioQueueRef Stream;
AudioQueueBufferRef AQBuffer1, AQBuffer2, AQBuffer3;

AudioStreamBasicDescription StreamDesc;
StreamDesc.mSampleRate = 16000;
StreamDesc.mFormatID = kAudioFormatLinearPCM;
StreamDesc.mBytesPerPacket = 2;
StreamDesc.mFramesPerPacket = 1;
StreamDesc.mBytesPerFrame = 2;
StreamDesc.mChannelsPerFrame = 1;
StreamDesc.mBitsPerChannel = 16;
// Try to allocate the audio buffers.
OSStatus Status = AudioQueueNewOutput(&StreamDesc, OutputCallback, &AD, NULL, NULL, 0, &Stream);
if (Status < 0) {
cout << "An error must have occurred." << endl;
Quit = true;
return 0;
}
AudioQueueAllocateBuffer(Stream, 32000*sizeof(short), &AQBuffer1);
AudioQueueAllocateBuffer(Stream, 32000*sizeof(short), &AQBuffer2);
AudioQueueAllocateBuffer(Stream, 32000*sizeof(short), &AQBuffer3);

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
cout << "f - Go to first section" << endl;
cout << "l - go to last section" << endl;
cout << "x - Play, c or v - Pause" << endl;
cout << "b - Next section, z - Previous section" << endl;
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
AudioQueueEnqueueBuffer(Stream, AQBuffer1, 0, NULL);

AudioQueuePrime(Stream, 0, 0);

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
AudioQueuePause(Stream);

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
// Replace with something Mac specific
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
// Replace with something Mac specific
CurrentSection -= 1;
fseek(AD.GetFileHandle(), Array[CurrentSection], SEEK_SET);
Previous = false;
}
// This bit pre-buffers input and decodes the output
LastPosition = AD.ftell();
}
if (Quit) SaveLastPosition(AD.GetTitle(), LastPosition);
else DeletePosition(AD.GetTitle());
AudioQueueDispose(Stream, false);
}
void Input() {
char Key = getch(); 
if (Key == '<') VolumeDown = true;
if (Key == '>') VolumeUp = true;
if (Key == 'b') Next = true;
if (Key == 'v' || Key == 'c') Paused = true;
if (Key == 'x') Paused = false;
if (Key == 'f') FirstSection = true;
if (Key == 'l') LastSection = true;
if (Key == 'z') Previous = true;
if (Key == 'q') Quit = true;
}
int main(int argc, char* argv[]) {
#ifdef WIN32
SetConsoleTitle("ABF Player");
HANDLE ThreadID = (HANDLE)_beginthread(Thread, 0, argv[1]);
#else
pthread_t id;
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
#endif
return 0;
}
