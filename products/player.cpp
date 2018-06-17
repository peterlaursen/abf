/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018 Peter Laursen.

This is the main player for our PC platforms.
This contains code that interfaces with our libabf library, most specifically our AbfDecoder.
*/

#ifdef WIN32
#include "dsaudio.h"
#else
#include "unixaudio.h"
#include "compat.h"
#include <termios.h>
#include <glob.h>
#include <sys/types.h>
#ifdef FREEBSD
#include <sys/event.h>
#endif
#endif
#include "database.h"
#include "player.h"
#include "audiosystem.h"
#include <iostream>
#include <cstdio>
#include "../libabf/libabf.h"
#ifdef WIN32
#include <process.h>
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <pthread.h>
#endif
#ifdef GPIO
#include <libgpio.h>
#endif

using namespace std;
using namespace ABF;
AudioSystem* Device;
volatile PlayerStatus PS = Playing;
PlayList PL;
AbfDecoder* GlobalAD = nullptr;
#ifndef WIN32
termios oldt, newt;
#endif

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
if (AD.GetMajor() == 2 && AD.GetMinor() != 1) {
cout << "ABF " << AD.GetMajor() << "." << AD.GetMinor() << " does not support seeking." << endl;
cout << "You must re-encode with a supported encoder in order to get this ability." << endl;
return false;
}
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
// This function will (hopefully) allow people to jump to various time positions within an audio book.
cin.clear();
cout << endl << "Type in the position you want to go to in minutes: (1-" << AD.GetMinutes() << "): " << endl;
int Minutes = 0;
cin >> Minutes;
// Clear cin (this is a bad hack!). Turn our minutes into seconds
cin.ignore(10000, '\n');
if (Minutes > AD.GetMinutes()) {
cout << "Error, the book isn't that long." << endl;
#ifndef WIN32
tcsetattr(0, TCSANOW, &newt);
#endif
return false;
}
#ifndef WIN32
tcsetattr(0, TCSANOW, &newt);
#endif
return AD.GoToPosition(--Minutes);
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
cout << "a - Add book to playlist, r - Remove book from playlist." << endl;
cout << "f - Go to first section, l - go to last section" << endl;
cout << "x - Play, c or v - Pause" << endl;
cout << "b - Next section, z - Previous section" << endl;
cout << "B - Next book, Z = Previous book" << endl;
cout << "g - go to section, j - jump to time" << endl;
cout << "< - Volume Down, > - Volume Up, + - Increase gain, '-' - Decrease gain" << endl;
cout << "q - Quit" << endl;
cout << "Title: " << AD.GetTitle() << endl << "Author: " << AD.GetAuthor() << endl << "This book lasts " << AD.GetMinutes() << " minutes." << endl;
#ifdef WIN32
// Put this into a scope of its own so that it vanishes as soon as possible.
{
char ConTitle[255] = {0};
GetConsoleTitle(ConTitle, 255);
string Temp = ConTitle;
Temp += " - ";
Temp += AD.GetTitle();
SetConsoleTitle(Temp.c_str());
}
#endif
//short Buffer[320];
//short Buffer1[32000];
const int* Array = AD.GetSections();
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
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
cout << endl << "Go To Section: (1-" << AD.GetNumSections() << "), current section is " << CurrentSection + 1 << ": ";
unsigned short NewSection;
cin.clear();
cin >> NewSection;
NewSection -= 1;
if (NewSection >= AD.GetNumSections()) NewSection = AD.GetNumSections() - 1;
CurrentSection = NewSection;
AD.Seek(Array[CurrentSection], SEEK_SET);
#ifndef WIN32
tcsetattr(0, TCSANOW, &newt);
#endif
PS = Playing;
}
if (PS == GoTime) {
Device->Stop();
if (!JumpToTime(AD)) {
PS = Playing;
continue;
}
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
if (CurrentSection == AD.GetNumSections()) continue;
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
if (Array[i] < CurPos && Array[i+1] > CurPos) {
CurrentSection = i-1;
AD.Seek(Array[CurrentSection], SEEK_SET);
break;
}
}
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
#ifdef GPIO
gpio_handle_t devhandle = 0;
#endif
void Input() {
#ifdef GPIO
if (gpio_pin_get(devhandle, 13) == 0) PS = Quit;
#endif

#ifndef WIN32
char Key = getchar();
#else
char Key = getch(); 
#endif
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
// Try to add some keys for gain adjustments
if (Key == '+') {
int Gain = GlobalAD->GetGain();
Gain += 100;
GlobalAD->SetGain(Gain);
}
if (Key == '-') {
int Gain = GlobalAD->GetGain();
Gain -= 100;
GlobalAD->SetGain(Gain);
}
}
#ifdef WIN32
void ThreadFunc(void*) {
#else
void* ThreadFunc(void*) {
#endif
#ifdef FREEBSD
int kq = kqueue();
struct kevent kev;
EV_SET(&kev, 0, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT, 0, 0, 0);
#endif
while (PS != Quit && PS != BookIsFinished && PS != PreviousBook && PS != NextBook) {
#ifdef GPIO
if (gpio_pin_get(devhandle, 13) == 0) {
PS=Quit;
break;
}
#endif
#ifdef WIN32
if (kbhit()) Input();
Sleep(250);
#else
#ifdef FREEBSD

kevent(kq, &kev, 1, &kev, 1, NULL);
#endif
#ifdef LINUX
if (kbhit()) Input();
#endif
usleep(250);
#endif
}
#ifndef WIN32
return nullptr;
#endif
}
int main(int argc, char* argv[]) {
if (argc < 2) AddBookToPlaylist();
#ifndef WIN32
tcgetattr(0, &oldt);
cfmakeraw(&newt);
tcsetattr(0, TCSANOW, &newt);
glob_t g;
for (int i = 1; i < argc; i++) {
glob(argv[i], GLOB_BRACE|GLOB_TILDE, NULL, &g);
for (int i = 0; i < g.gl_pathc; i++) PL.Add(g.gl_pathv[i]);
}
globfree(&g);
#endif
#ifdef GPIO
devhandle = gpio_open(0);
gpio_pin_pullup(devhandle, 13);
#endif
// Open the audio device.
#ifdef WIN32
for (int i = 1; i < argc; i++) PL.Add(argv[i]);
#endif
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
#ifdef GPIO
gpio_close(devhandle);
#endif
if (PS != Quit)
cout << endl << "Playlist empty; exitting..." << endl;
// Ensure our shell character comes on its own line
cout << endl;
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
return 0;
}
