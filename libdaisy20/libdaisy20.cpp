/* LIBDAISY, Version 2.0
Copyright (C) 2010 Peter Laursen.

This is to be a replacement for the old LibDaisy library. It is written mainly to improve and (I hope) simplify getting information out of Daisy books.

This library is released under the same license as the rest of this package.
*/
#include "libdaisy20.h"
#include <cstdio>
#include <iostream>
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
Content.open((Path + "ncc.html").c_str());
if (!Content) IsValid = false;
else IsValid = true;
}
DaisyBook::~DaisyBook() {
if (Content.is_open()) Content.close();
if (Smil.is_open()) Smil.close();
}
string& DaisyBook::GetTag(bool FromNCC) {
if (FromNCC)
getline(Content, Tag, '>');
else getline(Smil, Tag, '>');
return Tag;
}
bool DaisyBook::GetMetadata() {
const int NumMetadata = 5;
if (!IsValid) return false;
// Define the metadata we are looking for
string Metadata[NumMetadata];
Metadata[0] = string("\"dc:title");
Metadata[1] = string("\"dc:creator");
Metadata[2] = string("\"ncc:totalTime");
Metadata[3] = string("\"dc:identifier");
Metadata[4] = string("\"ncc:setInfo");
for (int i = 0; i < NumMetadata; i++) {
// First, we'll seek for the title.
Content.seekg(0, ios::beg);
int Position = 0;
while ((Position = Tag.find(Metadata[i])) == string::npos) GetTag();
// We have found the requested meta data
int Position2 = Tag.find("t=\"", Position);
Position = Position2 + 3;
Position2 = Tag.find("\"", Position);
if (i == 0) Title = Tag.substr(Position, Position2 - Position);
else if (i == 1) Author = Tag.substr(Position, Position2 - Position);
else if (i == 2) Time = Tag.substr(Position, Position2-Position);
else if (i == 3) Identification = Tag.substr(Position, Position2-Position);
else if (i == 4) {
string TmpString = Tag.substr(Position, Position2-Position);
Position = TmpString.find_last_of(" ");
Volumes = atoi(TmpString.substr(Position).c_str());
CurrentVolume = atoi(TmpString.substr(0,TmpString.find_first_of(" ")).c_str());
}
}
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
Content.seekg(0, ios::beg);
Tag.clear();
GetTag();
while (Tag.find("<body") == string::npos && !Content.eof()) GetTag();
/*
We have now located our <body> tag and we have also gotten the following tag, which is usually a heading.
The search then goes on to search through the smil file and like the first version of the library extracts the name of the audio file it encounters first.
*/
string AudioFile;
while (Tag.find("</body") == string::npos) {
GetTag();
while (Tag.find("<a") == string::npos && Tag.find("</body") == string::npos) GetTag();
if (Tag.find("</body") != string::npos) break;
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
Smil.open((Path + AF).c_str());
Tag.clear();
while (Tag.find("<audio") == string::npos && !Smil.eof()) GetTag(false);
if (!Smil.eof()) {
Position = Tag.find("src=\"") + 5;
Position2 = Tag.find("\"", Position);
AudioFile = Path + Tag.substr(Position, Position2-Position);
Smil.close();
cout << "AudioFile is " << AudioFile.length() << " characters and the string contains " << AudioFile << endl;
AudioFiles.push_back(AudioFile);
}
}
for (int i = 0; i < 5; i++) cout << endl << "File " << i << " in vector: " << AudioFiles[i] << endl;
}
bool DaisyBook::NextVolume(char* Path) {
// In this function, we declare a second DaisyBook so that we can inspect the next volume in piece.
DaisyBook DB(Path);
if (!DB.GetMetadata()) {
cout << "Error, cannot find metadata." << endl;
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
cout << "Error, there's no ID in this audio book." << endl;
return false;
}
}
string& DaisyBook::GetPath() { return Path; }

}
