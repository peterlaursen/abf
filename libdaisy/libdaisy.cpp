/* LIBDAISY.CPP, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This is the class implementation for LibDaisy.
*/
#include <iostream>
#include <fstream>
#include <string>
#include "libdaisy.h"
using namespace ABF;
using namespace std;
Daisy::Daisy(string Path, bool _Open): _Meta("none"), _Valid(false), _LastPosition(0) {
string Temp = Path;
char* c = (char*)Temp[Temp.length()];
if (c != FILE_SEP) _Path = Temp + FILE_SEP;
else
_Path = Path;
// Open the different file descriptors
if (_Open) Open();

}
Daisy::Daisy(const char* Path, bool _Open): _Meta("none"), _Valid(false), _LastPosition(0) {
string Temp = Path;
char* c = (char*)Temp[Temp.length()];
if (c != FILE_SEP) _Path = Temp + FILE_SEP;
else
_Path = Path;
if (_Open) Open();
}
bool Daisy::Open(bool _OpenSmil) {
// Catch the obvious problem first - are the files already open?
if (_Ncc.is_open() || _Smil.is_open()) return false;
// We now know that the files are not open, let's open them.
_Ncc.open((_Path + "ncc.html").c_str());
if (!_Ncc) {
cerr << "Error, cannot open daisy book." << endl;
return false;
}
_Valid = true;
if (!_OpenSmil) return true;
OpenSmil();
if (_Smil.is_open()) return true;
else {
return false;
}
}
// Close the files open.
void Daisy::Close() {
_Ncc.close();
_Smil.close();
}
Daisy::~Daisy() { 
Close(); 
}
void Daisy::Replace(string& Value) {
string Temp = Value;
int Erased=0;
for (int i = 0; i < Value.length(); i++) {
// I arrived at this value by doing an analysis of what character triggered the odd problem with the Danish characters not being displayed correctly.
	if (Value[i] == -61) {
if (Value[i+1] == -72)
Temp[i-Erased] = 'ø';
if (Value[i+1] == -90)
Temp[i-Erased] = 'æ';
Temp.erase((i-Erased)+1, 1);
Erased++;
}
}
Value = Temp;
}

string& Daisy::ExtractMetaInfo(string& Name) {
_Ncc.seekg(0, ios::beg);
string Line;
int Position = 0;
while (1) {
getline(_Ncc, Line);
if (Line.find("</head>") != string::npos) {
_Ncc.seekg(0, ios::beg);
_Meta = "notfound";
return _Meta;
}
int Position = Line.find("meta");
// If the word "meta" cannot be found, read the next line.
if (Position == string::npos) continue;
else {
Position = Line.find(Name);
if (Position == string::npos) continue;
else {
Position = Line.find("content=\"");
// The first quote character is after the content= thing. It is noted that if any spaces exist, this will actually give problems. We bypass the content=" part and start searching 10 characters in front of position.
int Position2 = Line.find('\"', Position+10);
// Get the substring. Again, we bypass content=". We only get the characters we need.
string Content = Line.substr(Position+9, Position2 - Position);
// Look for Danish characters and replace them.
Replace(Content);
// We need to look for scheme. If we find it, we erase it.
int Position3 = Content.find("scheme");
if (Position3 != string::npos) {
Content.erase(Position3);
_Ncc.seekg(0, ios::beg);
// Remove the last quote and a > character
_Meta = Content.substr(0, Content.length() - 2);
return _Meta;
}
Position3 = Content.find_last_of("\"");
if (Position3 != string::npos) {
Content.erase(Position3);
}
_Meta = Content;
return _Meta;
}
break;
}
}
}
int Daisy::FindBody() {
_Ncc.seekg(0, ios::beg);
string Line;
while (1) {
getline(_Ncc, Line);
if (Line.find("<body>") == string::npos) continue;
// We encountered end of file and didn't find what we were looking for
if (_Ncc.eof()) return -1;
// We have indeed found the body.
_LastPosition = _Ncc.tellg();
return _LastPosition;
}
}
string Daisy::FindSmil() {
static bool BodyFound = false;
_Ncc.seekg(_LastPosition, ios::beg);
string Line;
while (1) {
if (BodyFound) return "nomore";
getline(_Ncc, Line);
if (Line.find("</body>") != string::npos) BodyFound = true;
// Bypass page numbers
if (Line.find("<span") != string::npos) continue;
int Position = Line.find("href=\"");
if (Position == string::npos) continue;
// Bypass href="
Position += 6;
int Position2 = Line.find("#", Position);
if (Position2 == string::npos) return string("# notfound");
// Remove everything up to and including the " character
Line.erase(0, Position);
// Position 2 is invalid, search again
Position2 = Line.find("#");
// Remove everything after Position2
Line.erase(Position2);
// Return the absolute path
_LastPosition = _Ncc.tellg();
for (int i = 0; i < Line.length(); i++) Line[i] = tolower(Line[i]);
return _Path + Line;
}
}
bool Daisy::OpenSmil() {
// This contains the last position from where we had a smil file.
// We'll make this easier by locating the body first.
if (_LastPosition == 0)
_LastPosition = FindBody();
else {
_Ncc.seekg(_LastPosition, ios::beg);
}
string Filename = FindSmil();
if (Filename == "nomore" || Filename == "notfound") {
return false;
}
_Smil.close();
_Smil.clear();
_Smil.open(Filename.c_str());
if (!_Smil.is_open()) {
cout << "Smil file not found - the file name was " << Filename << endl;
return false;
}
 
_LastPosition = _Ncc.tellg();
// We need to set the file position to 0 to satisfy the ExtractMetaInfo function.
return true;
}
const char* Daisy::GetMP3FileName() {
string Line;
string MP3;
while (!_Smil.eof()) {
getline(_Smil, Line);
if (Line.find("<audio ") == string::npos) continue;
int Position = Line.find("\"");
Line.erase(0, Position+1);
Position = 0;
// This is just an audio file - we know it ends with a quote, therefore, we search again.
int Position2 = Line.find("\"", Position);
// We do not need to return the quote, we'll delete it alongside the rest of the string.
Line.erase(Position2);
for (int i =0; i < Line.length(); i++) Line[i] = tolower(Line[i]);
MP3 = Line;
}
_Meta = _Path + MP3;
#ifdef DEBUG
cout << "MP3 file is " << _Meta << endl;
#endif
return _Meta.c_str();
}
void Daisy::Validate() {
ifstream fin((_Path + "ncc.html").c_str());
if (fin.is_open()) _Valid = true;
else _Valid = false;
fin.close();
}
bool Daisy::IsValid() { return _Valid; }
string& Daisy::ExtractSectionTitle() { 
_SectionTitle = "notworking";
return _SectionTitle;
}
string& Daisy::GetPath() { return _Path; }
unsigned short Daisy::GetNumSections() {
int Last = _Ncc.tellg();
_Ncc.seekg(0, ios::beg);
_Ncc.seekg(FindBody(), ios::beg);
unsigned short NumSections = 0;
// This is a cheap trick: The ExtractSectionTitle() remains public, but it is not currently used in any of the programs.
string Line;
while (1) {
getline(_Ncc, Line);
if (Line.find("href") != string::npos) {
++NumSections;
}
if (Line.find("</body>") != string::npos) break;
}
_Ncc.seekg(Last, ios::beg);
_LastPosition = Last;
return NumSections;
}
