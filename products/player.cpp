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
#include "../abfdecoder/abfdecoder.h"
#include <thread>
#ifdef WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#endif
#ifdef GPIO
#include <libgpio.h>
#endif
#include <string>
using namespace std;
using namespace ABF;
AudioSystem* Device = nullptr;
string DevName = "";
volatile PlayerStatus PS = Playing;
PlayList PL;
AbfDecoder* GlobalAD = nullptr;
#ifndef WIN32
termios oldt, newt;
#endif

void AddBookToPlaylist() {
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
string NewBook;
cout << "Type in the book to add." << endl;
getline(cin, NewBook);
#ifdef WIN32
PL.Add(NewBook);
#else
if (access(NewBook.c_str(), F_OK)) cerr << "Book file does not exist." << endl;
else PL.Add(NewBook);
#endif
#ifndef WIN32
tcsetattr(0, TCSAFLUSH|TCSANOW, &newt);
#endif
}
void RemoveBookFromPlaylist() {
PL.Remove(PL.GetCurrentBook());
if (PL.GetTotalItems() == 0) return;
}

bool JumpToTime(AbfDecoder& AD) {
if (AD.GetMajor() == 2 && AD.GetMinor() != 1) {
cerr << "ABF " << AD.GetMajor() << "." << AD.GetMinor() << " does not support seeking." << endl;
cerr << "You must re-encode with a supported encoder in order to get this ability." << endl;
return false;
}
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
// This function will (hopefully) allow people to jump to various time positions within an audio book.
const int* Positions = AD.GetMinutePositions();
int CurrentMinute = 0;
for (int i = 0; i < AD.GetMinutes(); i++) {
if (Positions[i] >= AD.ftell()) {
CurrentMinute = i;
break;
}
}

cout << endl << "Type in the position you want to go to in minutes: (1-" << AD.GetMinutes() << "), current minute is " << CurrentMinute << ": " << endl;
int Minutes = 0;
cin >> Minutes;
if (Minutes > AD.GetMinutes()) {
cout << "Error, the book isn't that long." << endl;
#ifndef WIN32
tcsetattr(0, TCSANOW|TCSAFLUSH, &newt);
#endif
return false;
}
#ifndef WIN32
tcsetattr(0, TCSANOW|TCSAFLUSH, &newt);
#endif
return AD.GoToPosition(--Minutes);
}
void Thread(char* Filename) {
AbfDecoder AD(Filename);
GlobalAD = &AD;
Device = AudioSystem::Create(GlobalAD, DevName);

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
int LastPosition = GetLastPosition(AD.GetTitle());
if (LastPosition > 0) {
AD.Seek(LastPosition, SEEK_SET);
}
while (!AD.feof() && PS != Quit) {
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
tcsetattr(0, TCSANOW|TCSAFLUSH, &oldt);
#endif
unsigned short MaxSections = AD.GetNumSections()-1;
int CurrentSection = -1;
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] >= AD.ftell()) {
CurrentSection = i - 1;
break;
}
}
if (CurrentSection == -1) CurrentSection = MaxSections;
cout << endl << "Go To Section: (0-" << MaxSections << "), current section is " << CurrentSection << ":" << endl;
unsigned short NewSection;
cin >> NewSection;
if (NewSection >= MaxSections) NewSection = MaxSections;
CurrentSection = NewSection;
AD.Seek(Array[CurrentSection], SEEK_SET);
#ifndef WIN32
tcsetattr(0, TCSANOW|TCSAFLUSH, &newt);
cin.clear();
#endif
PS = Playing;
continue;
}
if (PS == GoTime) {
Device->Stop();
if (!JumpToTime(AD)) {
PS = Playing;
cin.clear();
continue;
}
PS = Playing;
continue;
}
if (PS == FirstSection) {
AD.Seek(Array[0], SEEK_SET);
PS = Playing;
continue;
}
if (PS == LastSection) {
Device->Stop();
AD.Seek(Array[AD.GetNumSections()-1], SEEK_SET);
PS = Playing;
continue;
}
if (PS == NextBook || PS == PreviousBook) break;
if (PS == Next) {
// Get current position
Device->Stop();
int CurPos = AD.ftell();
if (CurPos >= Array[AD.GetNumSections()-1]) {
PS = Playing;
continue;
}
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] > CurPos) {
AD.Seek(Array[i], SEEK_SET);
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
Device->Stop();
if (AD.ftell() < Array[1]) {
PS = Playing;
continue;
}
// Get current position
int CurPos = AD.ftell();
if (CurPos >= Array[AD.GetNumSections()-1]) {
AD.Seek(Array[AD.GetNumSections()-2], SEEK_SET);
PS = Playing;
continue;
}
for (int i = 0; i < AD.GetNumSections(); i++) {
if (Array[i] >= CurPos) {
AD.Seek(Array[i-2], SEEK_SET);
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
// In order to ensure our input is not repeated unnecessarily if you press the G or J key, we return if PlayerState is  equal to GoTime or GoToSection.
// If we don't do this, it seems our first few characters were skipped from the input.
// What happened in reality was that the Input() function was called again, then only afterwards relinquishing the input.
if (PS == GoToSection || PS == GoTime) return;
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
void ThreadFunc() {
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
#elif defined(LINUX)
if (kbhit())
#endif
Input();
usleep(250);
#endif
}
}
int main(int argc, char* argv[]) {
#ifndef WIN32
tcgetattr(0, &oldt);
cfmakeraw(&newt);
tcsetattr(0, TCSANOW, &newt);
#endif
int Start = 1;
if (argc >= 2) {
if (!strcmp(argv[1], "-d")) {
Start = 3;
DevName = argv[2];
}
}
if (argc == Start) AddBookToPlaylist();
#ifndef WIN32
glob_t g;
bool Globbed = false;
for (int i = Start; i < argc; i++) {
glob(argv[i], GLOB_BRACE|GLOB_TILDE, NULL, &g);
Globbed = true;
for (int i = 0; i < g.gl_pathc; i++) PL.Add(g.gl_pathv[i]);
}
if (Globbed) globfree(&g);
#endif
#ifdef GPIO
devhandle = gpio_open(0);
gpio_pin_pullup(devhandle, 13);
#endif
#ifdef WIN32
for (int i = Start; i < argc; i++) PL.Add(argv[i]);
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
#endif
thread Threads[2];
Threads[0] = thread(Thread, Filename);
Threads[1] = thread(ThreadFunc);
Threads[0].join();
Threads[1].join();
}
delete Device;
#ifdef GPIO
gpio_close(devhandle);
#endif
if (PS != Quit)
cout << endl << "Playlist empty; exitting..." << endl;
// Ensure our shell character comes on its own line
#ifndef WIN32
tcsetattr(0, TCSANOW, &oldt);
#endif
return 0;
}
