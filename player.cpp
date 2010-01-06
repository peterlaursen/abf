#include "dsaudio.h"
#include "database.h"
#include "player.h"
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
DSAudio* Device;
volatile PlayerStatus PS = Playing;
PlayList PL;
void AddBookToPlaylist() {
#ifndef WIN32
nocbreak();
echo();
#endif

string NewBook;
cout << "Type in the book to add." << endl;
getline(cin, NewBook);
PL.Add(NewBook);
#ifndef WIN32
cbreak();
noecho();
#endif
}
void RemoveBookFromPlaylist() {
PL.Remove(PL.GetCurrentBook());
if (PL.GetTotalItems() == 0) return;
}

bool JumpToTime(AbfDecoder& AD) {
// This function will (hopefully) allow people to jump to various time positions within an audio book.
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
// Clear cin (this is a bad hack!). Turn our minutes into seconds
cin.ignore(10000, '\n');
return AD.GoToPosition(Minutes);
}
#ifdef WIN32
void Thread(void* Filename) {
#else
void* Thread(void* Filename) {
#endif
char* Temp = (char*)Filename;
AbfDecoder AD(Temp);
Device->Init(&AD);
bool IsValid = AD.IsValid();
if (!IsValid) {
cout << "Error, not a valid ABF book." << endl;
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
if (AD.ftell() > Array[CurrentSection+1]) CurrentSection += 1;
// The rest of this loop processes key presses.
/*if (PS == VolumeDown) {
if (Volume == 0.0f) {
PS = Playing;
continue;
}
Volume -= 0.1f;
PS = Playing;
}
if (PS == VolumeUp) {
if (Volume >= 1.0f) {
PS = Playing;
continue;
}
Volume += 0.1f;
PS = Playing;
}
*/
if (PS == Paused) {
if (Device->isPlaying()) Device->Stop();
continue;
}
if (PS == GoToSection) {
Device->Stop();
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
if (PS == AddBook) {
Device->Stop();
AddBookToPlaylist();
PS = Playing;
}
if (PS == RemoveBook) {
Device->Stop();
RemoveBookFromPlaylist();
if (PL.GetTotalItems() >= 2 && PL.GetCurrentBook() + 1 <= PL.GetTotalItems() - 1) PS = PreviousBook;
else PS = NextBook;
break;
}
if (PS == Next) {
if (CurrentSection >= AD.GetNumSections() - 1) {
PS = Playing;
CurrentSection = AD.GetNumSections()-1;
continue;
}
Device->Stop();
CurrentSection += 1;
AD.Seek(Array[CurrentSection], SEEK_SET);
PS = Playing;
}
if (PS == Previous) {
if (CurrentSection == 0) {
PS = Playing;
continue;
}
if (CurrentSection >= AD.GetNumSections()) CurrentSection = AD.GetNumSections()-1;
Device->Stop();
CurrentSection -= 1;
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
// Wait until the playback is finished, then go run the loop again
}
if (PS == Quit || PS == PreviousBook || PS == NextBook) SaveLastPosition(AD.GetTitle(), LastPosition);
else DeletePosition(AD.GetTitle());
}
void Input() {
char Key = getch(); 
if (Key == 'a') PS = AddBook;
if (Key == 'r') PS = RemoveBook;
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
void ThreadFunc(void*) {
cout << "In Thread Function." << endl;
while (PS != Quit && PS != BookIsFinished && PS != PreviousBook && PS != NextBook) {
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
}
int main(int argc, char* argv[]) {
#ifndef WIN32
initscr();
cbreak();
noecho();
#endif
if (argc < 2) AddBookToPlaylist();
// Open the audio device.
Device = new DSAudio();
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
#else
ThreadType id;
pthread_create(&id, 0, Thread, Filename);
#endif
ThreadType Thread2 = (ThreadType)_beginthread(ThreadFunc, 0, NULL);

#ifdef WIN32
WaitForSingleObject(ThreadID, INFINITE);
#else
pthread_join(id, 0);
nocbreak();
echo();
endwin();
#endif
}
return 0;
}
