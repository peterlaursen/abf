/* $Id$
LIBDAISY, Version 2.0
Copyright (C) 2010 Peter Laursen.

This is to be a replacement for the old LibDaisy library. It is written mainly to improve and (I hope) simplify getting information out of Daisy books.

This library is released under the same license as the rest of this package.
*/
#include "libdaisy20.h"
#include <cstdio>
#include <iostream>
#include <cstdlib>
#include <iconv.h>
#ifdef FREEBSD
#include <sys/param.h>
#endif
using namespace std;
namespace ABF {
DaisyBook::DaisyBook(char* SpecifiedPath): Path(SpecifiedPath), Volumes(0) {
int Length = Path.length() - 1;
if (Path[Length] != '/' || Path[Length] != '\\')
#ifdef WIN32
Path += '\\';
#else
Path += '/';
#endif
FileListLength = scandir(SpecifiedPath, &FileList, NULL, alphasort);
Content.open((Path + "ncc.html").c_str());
if (!Content) IsValid = false;
else IsValid = true;
}
DaisyBook::~DaisyBook() {
if (Content.is_open()) Content.close();
if (Smil.is_open()) Smil.close();
for (int i = 0; i < FileListLength; i++) free(FileList[i]);
free(FileList);

}
const string& DaisyBook::GetTag(bool FromNCC) {
if (FromNCC) {
if (Content.eof()) Tag="EOF";
else getline(Content, Tag, '>');
}
else {
if (Smil.eof()) Tag="EOF";
getline(Smil, Tag, '>');
}
return Tag;
}
bool DaisyBook::GetMetadata() {
if (!IsValid) return false;
// Define the metadata we are looking for
const int NumMetadata = 7;
const string Metadata[NumMetadata] = { string("\"dc:title"),
string("\"dc:creator"),
string("\"ncc:totalTime"),
string("\"dc:identifier"),
string("\"ncc:setInfo"), string("\"dc:language"), string("\"ncc:charset"), };
for (int i = 0; i < NumMetadata; i++) {
// First, we'll seek for the title.
Content.clear();
Content.seekg(0, ios::beg);
int Position = 0;
#ifdef DEBUG
cout << "Looking for metadata " << Metadata[i] << endl;
#endif
while ((Position = Tag.find(Metadata[i])) == string::npos && !Content.eof()) 
GetTag();
if (Content.eof()) continue;

// We have found the requested meta data
int Position2 = Tag.find("t=\"", Position);
Position = Position2 + 3;
Position2 = Tag.find("\"", Position);
if (i == 0) Title = Tag.substr(Position, Position2 - Position);
else if (i == 1) Author = Tag.substr(Position, Position2 - Position);
else if (i == 2) Time = Tag.substr(Position, Position2-Position);
else if (i == 3) Identification = Tag.substr(Position, Position2-Position);
else if (i == 4) {
if (Content.eof()) {
Volumes = 1;
CurrentVolume = 1;
break;
}

string TmpString = Tag.substr(Position, Position2-Position);
Position = TmpString.find_last_of(" ");
Volumes = atoi(TmpString.substr(Position).c_str());
CurrentVolume = atoi(TmpString.substr(0,TmpString.find_first_of(" ")).c_str());
}
else if (i == 5) {
#ifdef DEBUG
cout << "Language: " << Tag.substr(Position, Position2 - Position) << endl;
#endif
Language = Tag.substr(Position, Position2 - Position);
}
else if (i == 6) {
#ifdef DEBUG
cout << "Character set: " << Tag.substr(Position, Position2-Position) << endl;
#endif
CharSet = Tag.substr(Position, Position2 - Position);
}

}
/* Right, we have now finished getting our metadata.
It has been decided that we convert all our character inputs to UTF-8.
This is done only for the title and the author, but that is also important enough.
*/

iconv_t Iconv = iconv_open("utf-8", CharSet.c_str());
if (Iconv == (iconv_t)-1) {
cerr << "Could not open our iconv converter: " << endl;
}
char* TempTitle = (char*)Title.c_str();
int TempTitleLength = Title.length()+1;
char* TempAuthor = (char*)Author.c_str();
#ifdef DEBUG
cout << "TempTitle: " << TempTitle << ", Author: " << TempAuthor << endl;
#endif
int TempAuthorLength = Author.length()+1;
char* DestBuffer = new char[TempTitleLength*2];
char* Dst = DestBuffer;
#ifdef FREEBSD
#if __FreeBSD__ < 10
char* src = nullptr;
#endif
#if __FreeBSD__ == 10
#if __FreeBSD_version < 1001514 
const char* src = TempTitle;
#else
char* src = TempTitle;
#endif
#endif
#if __FreeBSD__ >= 11
#if __FreeBSD_version >= 1100069
char* src = TempTitle;
#else
const char* src = TempTitle;
#endif
#endif
#else /* Not defined FREEBSD */
#ifdef WIN32
const char* src = TempTitle;
#else /* Everything but FreeBSD and WIndows, apparently */
char* src = TempTitle;
#endif
#endif
size_t SrcLeft = TempTitleLength;
size_t DstLeft = TempTitleLength*2;
iconv(Iconv, &src, &SrcLeft,&Dst, &DstLeft);
#ifdef DEBUG
cout << "Remaining characters: " << DstLeft << endl;
#endif
*Dst='\0';
Title = string(DestBuffer);
#ifdef DEBUG
cout << "Title: " << Title << endl;
#endif
delete[] DestBuffer;
Dst = 0;
DestBuffer = 0;
DestBuffer = new char[TempAuthorLength*2];
Dst = DestBuffer;
src = TempAuthor;
SrcLeft = TempAuthorLength;
DstLeft = TempAuthorLength*2;
iconv(Iconv, &src, &SrcLeft, &Dst, &DstLeft);
#ifdef DEBUG
cout << "Characters left: " << DstLeft << endl;

#endif
*Dst='\0';
Author = string(DestBuffer);
delete[] DestBuffer;
Dst = nullptr;
DestBuffer = nullptr;

#ifdef DEBUG
cout << "Author: " << Author << endl;
#endif
iconv_close(Iconv);
return true;
}
const string& DaisyBook::GetTitle() { return Title; }
const string& DaisyBook::GetAuthor() { return Author; }
const unsigned short DaisyBook::GetNumSections() { return AudioFiles.size(); }
const int DaisyBook::GetVolumes() { return Volumes; }
int DaisyBook::GetCurrentVolume() { return CurrentVolume; }
const string& DaisyBook::GetTotalTime() { return Time; }
const string& DaisyBook::GetIdentification() { return Identification; }
void DaisyBook::GetAudioFiles() {
Content.clear();
Content.seekg(0, ios::beg);
Tag.clear();
GetTag();

while (Tag.find("<body") == string::npos && !Content.eof()) GetTag();
/*
We have now located our <body> tag and we have also gotten the following tag, which is usually a heading.
The search then goes on to search through the smil file and like the first version of the library extracts the name of the audio file it encounters first.
*/
string AudioFile;
while (Tag.find("</body") == string::npos && !Content.eof()) {
GetTag();
if (Tag.find("<span") != string::npos) {
while (Tag.find("</span>") == string::npos) GetTag();
continue;
}
 
while (Tag.find("<h") == string::npos && Tag.find("</body") == 
string::npos) GetTag();
if (Tag.find("</body") != string::npos) break;
// Get the tag after our heading
GetTag();
// Check whether we're on the correct disk
if (Volumes > 1) {
int MyPos;
if ((MyPos = Tag.find("rel=")) != string::npos) {
MyPos += 5;
int MyPos2 = Tag.find(" ", MyPos);
int Test = atoi(Tag.substr(MyPos, MyPos2-MyPos).c_str());
if (Test < CurrentVolume) continue;
if (Test > CurrentVolume) break;
}
}

// Bypass the string 'href="'
int Position = Tag.find("href=") + 6;
int Position2 = Tag.find("#");
string AF = Tag.substr(Position, Position2-Position);
for (int i = 0; i < FileListLength; i++) {
#ifdef WIN32
int Comparison = istrcmp(AF.c_str(), FileList[i]->d_name);
#else
int Comparison = strcasecmp(AF.c_str(), FileList[i]->d_name);
#endif
if (Comparison == 0) {
AF = FileList[i]->d_name;
break;
}
}

Smil.open((Path + AF).c_str());
if (!Smil) {
throw string("Error, could not open the SMIL file " + Path+AF+".");
IsValid = false;
}

Tag.clear();
while (Tag.find("<audio") == string::npos && !Smil.eof()) GetTag(false);
if (!Smil.eof()) {
Position = Tag.find("src=\"") + 5;
Position2 = Tag.find("\"", Position);
string TempFile = Tag.substr(Position, Position2 - Position);
for (int i = 0; i < FileListLength; i++) {
#ifdef WIN32
int Comparison = istrcmp(TempFile.c_str(), FileList[i]->d_name);
#else
int Comparison = strcasecmp(TempFile.c_str(), FileList[i]->d_name);
#endif
if (Comparison == 0) {
AudioFile = Path + FileList[i]->d_name;
break;
}
}

Smil.close();
AudioFiles.push_back(AudioFile);

}
}
}
bool DaisyBook::NextVolume(char* Path) {
// In this function, we declare a second DaisyBook so that we can inspect the next volume in piece.
DaisyBook DB(Path);
if (!DB.GetMetadata()) {
throw string("Error, cannot find metadata.");
IsValid = false;
return false;
}
string NewID = DB.GetIdentification();
if (Identification == NewID) {
Path = (char*)DB.GetPath().c_str();
Content.close();
Content.open("ncc.html");
GetAudioFiles();
return true;
}
else {
throw string("Error, there is no Content ID in this audio book.");
return false;
}
}
string& DaisyBook::GetPath() { return Path; }

}
