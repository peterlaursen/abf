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
Close();
}
string& DaisyBook::GetTag(bool FromNCC) {
if (FromNCC)
getline(Content, Tag, '>');
else getline(Smil, Tag, '>');
return Tag;
}
bool DaisyBook::GetMetadata() {
if (!IsValid) return false;
// Define the metadata we are looking for
string Metadata[3];
Metadata[0] = string("\"dc:title");
Metadata[1] = string("\"dc:creator");
Metadata[2] = string("\"ncc:setInfo");
for (int i = 0; i < 3; i++) {
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
else if (i == 2) {
printf("We have reached the setInfo.\nHere is the contents of the tag: %s\n", Tag.c_str());
string TmpString = Tag.substr(Position, Position2-Position);
Position = TmpString.find_last_of(" ");
Volumes = atoi(TmpString.substr(Position).c_str());

std::cout << TmpString << std::endl;

}
}
return true;
}

const string& DaisyBook::GetTitle() { return Title; }
const string& DaisyBook::GetAuthor() { return Author; }
const unsigned short DaisyBook::GetNumSections() { return AudioFiles.size(); }
const int DaisyBook::GetVolumes() { return Volumes; }
}
