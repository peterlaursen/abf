/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014 Peter Laursen.

This is the main player for our PC platforms.
This contains code that interfaces with our libabf library, most specifically our AbfDecoder.
*/

#ifdef WIN32
#include "dsaudio.h"
#else
#include "unixaudio.h"
#include "compat.h"
#endif
#include "database.h"
#include "player.h"
#include "audiosystem.h"
#include <iostream>
#include <cstdio>
#ifndef WIN32
#include "../libabf/libabf.h"
#else
#include "../libabf/libabf-win.h"
#endif

#ifdef WIN32
#include <process.h>
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
using namespace std;
using namespace ABF;
AudioSystem* Device;
volatile PlayerStatus PS = Playing;
PlayList PL;
AbfDecoder* GlobalAD;
void AddBookToPlaylist() {

string NewBook;
cout << "Type in the book to add." << endl;
getline(cin, NewBook);
PL.Add(NewBook);
}
void RemoveBookFromPlaylist() {
PL.Remove(PL.GetCurrentBook());
if (PL.GetTotalItems() == 0) return;
}

bool JumpToTime(AbfDecoder& AD) {
cout << "Currently not implemented for the new ABF format." << endl;

/*
// This function will (hopefully) allow people to jump to various time positions within an audio book.
cin.clear();
cout << endl << "Type in the position you want to go to in minutes: " << endl;
int Minutes;
cin >> Minutes;
// Clear cin (this is a bad hack!). Turn our minutes into seconds
cin.ignore(10000, '\n');
return AD.GoToPosition(Minutes);
*/
return false;
}
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
char* Temp = (char*)Filename;
AbfDecoder AD(Temp);
GlobalAD = &AD;
Device = AudioSystem::Create(GlobalAD);
bool IsValid = AD.IsValid();
if (!IsValid) {
cerr << "Error, not a valid ABF book." << endl;
PS = Quit;
#ifdef WIN32
return;
#else
return 0;
#endif
}
cout << "Commands you can use in the player: " << endl;
cout << "Key - Description" << endl;
cout << "f - Go to first section, l - go to last section" << endl;
cout << "x - Play, c or v - Pause" << endl;
cout << "b - Next section, z - Previous section" << endl;
cout << "B - Next book, Z = Previous book" << endl;
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
int* Array = AD.GetSections();
int CurrentSection = 0;
int LastPosition = GetLastPosition(AD.GetTitle());
if (LastPosition > 0) {
AD.Seek(LastPosition, SEEK_SET);
// Set CurrentSection to the correct section
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] > LastPosition) {
CurrentSection = i-1;
break;
}
}
}
float Volume = 1.0f;
while (!AD.feof() && PS != Quit) {
// Ensure that CurrentSection is up-to-date
if (AD.ftell() > Array[CurrentSection] && AD.ftell() 
> Array[CurrentSection + 1]) 
CurrentSection += 1;
// The rest of this loop processes key presses.
if (PS == VolumeDown) {
Device->DecreaseVolume();
PS = Playing;
continue;
}
if (PS == VolumeUp) {
Device->IncreaseVolume();
PS = Playing;
continue;
}

if (PS == Paused) {
if (Device->isPlaying()) Device->Stop();
continue;
}
if (PS == GoToSection) {
Device->Stop();
cout << endl << "Go To Section: (1-" << AD.GetNumSections() << "), current section is " << CurrentSection + 1 << ": ";
unsigned short NewSection;
cin.clear();
cin >> NewSection;
NewSection -= 1;
if (NewSection >= AD.GetNumSections()) NewSection = AD.GetNumSections() - 1;
CurrentSection = NewSection;
AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
}
if (PS == GoTime) {
Device->Stop();
if (!JumpToTime(AD)) continue;
// Set current section
int Position = AD.ftell();
for (int i = 0; i <= AD.GetNumSections(); i++) {
if (Array[i] > Position) {
CurrentSection = i - 1;
break;
}
}
PS = Playing;
}
if (PS == FirstSection) {
CurrentSection = 0;
AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
}
if (PS == LastSection) {
CurrentSection = AD.GetNumSections()-1;
AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
}
if (PS == NextBook || PS == PreviousBook) break;

if (PS == Next) {
// Get current position
int CurPos = AD.ftell();
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] > CurPos) {
AD.Seek(Array[i], SEEK_SET);
CurrentSection = i;
break;
}
}
PS = Playing;
/* Uncomment this code for now...

// Check that we don't get an overflow.
if (CurrentSection >= AD.GetNumSections() - 1) {
PS = Playing;
CurrentSection = AD.GetNumSections()-1;
continue;
}
Device->Stop();
++CurrentSection;
AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
*/
continue;
}
if (PS == Previous) {
// Prevent us from going to -1
if (CurrentSection == 0) {
PS = Playing;
continue;
}
if (CurrentSection >= AD.GetNumSections()) CurrentSection = AD.GetNumSections()-1;
Device->Stop();
// Get current position
int CurPos = AD.ftell();
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] > CurPos) {
AD.Seek(Array[i], SEEK_SET);
CurrentSection = i-1;
break;
}
}

AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
}
// This bit pre-buffers input and decodes the output
LastPosition = AD.ftell();
if (AD.feof()) {
PS = BookIsFinished;
break;
}
Device->Play();
LastPosition = AD.ftell();
// Wait until the playback is finished, then go run the loop again
}
if (PS == Quit || PS == PreviousBook || PS == NextBook) 
SaveLastPosition(AD.GetTitle(), AD.ftell()); // Store the position 2 seconds before. In this version, we store the exact position instead - but we must reimplement the two second pause again. We'll have to see how we might do that properly.
else DeletePosition(AD.GetTitle());
if (GlobalAD->feof()) PS = BookIsFinished;
#ifndef WIN32
return nullptr;
#endif
}
void Input() {
char Key = getch(); 
if (Key == 'a') {
PS = AddBook;
Device->Stop();
AddBookToPlaylist();
PS = Playing;
}
else if (Key == 'r') {
PS = RemoveBook;
Device->Stop();
RemoveBookFromPlaylist();
if (PL.GetTotalItems() >= 2 && PL.GetCurrentBook() + 1 <= PL.GetTotalItems() - 1) PS = PreviousBook;
else PS = NextBook;
}
if (Key == '<') PS = VolumeDown;
if (Key == '>') PS = VolumeUp;
if (Key == 'g') PS = GoToSection;
if (Key == 'j') PS = GoTime;
if (Key == 'b') PS = Next;
if (Key == 'v' || Key == 'c') PS = Paused;
if (Key == 'x') PS = Playing;
if (Key == 'f') PS = FirstSection;
if (Key == 'l') PS = LastSection;
if (Key == 'z') PS = Previous;
if (Key == 'B') PS = NextBook;
if (Key == 'Z') PS = PreviousBook;
if (Key == 'q') PS = Quit;
}
#ifdef WIN32
void ThreadFunc(void*) {
#else
void* ThreadFunc(void*) {
#endif

while (PS != Quit && PS != BookIsFinished && PS != PreviousBook && PS != NextBook) {
if (kbhit()) Input();
#ifdef WIN32
Sleep(250);
#else
usleep(250);
#endif
}
#ifndef WIN32
return nullptr;
#endif
}
int main(int argc, char* argv[]) {
if (argc < 2) AddBookToPlaylist();
// Open the audio device.
for (int i = 1; i < argc; i++) PL.Add(argv[i]);
char* Filename;
while (PL.GetCurrentBook() < PL.GetTotalItems()){
if (PS == Quit) break;
if (PS == PreviousBook) {
if (!PL.PreviousBook()) break;
PS = Playing;
}
if (PS == NextBook) {
if (PL.GetTotalItems() == 0) break;

if (!PL.NextBook()) break;
PS = Playing;
}
if (PS == BookIsFinished) {
if (PL.GetTotalItems() == 1) break;
PS = NextBook;
continue;
}

Filename = PL.GetCurrentBookName();
#ifdef WIN32
SetConsoleTitle("ABF Player");
ThreadType ThreadID = (ThreadType)_beginthread(Thread, 0, Filename);
ThreadType Thread2 = (ThreadType)_beginthread(ThreadFunc, 0, NULL);
#else
ThreadType id, Thread2;
pthread_create(&id, 0, Thread, Filename);
pthread_create(&Thread2, 0, ThreadFunc, 0);
#endif

#ifdef WIN32
WaitForSingleObject(ThreadID, INFINITE);
#else
pthread_join(id, 0);
#endif
}
delete Device;
if (PS != Quit)
cout << endl << "Playlist empty; exitting..." << endl;
// Ensure our shell character comes on its own line
cout << endl;

return 0;
}
