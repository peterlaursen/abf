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
Daisy::Daisy(string Path, bool _Open) {
_Path = Path;
// Open the different file descriptors
if (_Open) Open();
// No meta info has been extracted yet.
_Meta = "none";
}
Daisy::Daisy(const char* Path, bool _Open) {
_Path = Path;
_Meta = "none";
if (_Open) Open();
}
bool Daisy::Open() {
// Catch the obvious problem first - are the files already open?
if (_Ncc.is_open() || _Smil.is_open()) return false;
// We now know that the files are not open, let's open them.
_Ncc.open((_Path + "\\" + "ncc.html").c_str());
if (!_Ncc) {
cerr << "Error, cannot open daisy book." << endl;
return false;
}
OpenFirstSmil();
if (_Smil.is_open()) return true;
else {
cout << "Cannot open first Smil file." << endl;
return false;
}
}
// Close the files open.
void Daisy::Close() {
_Ncc.close();
_Smil.close();
}
Daisy::~Daisy() { 
cout << "Closing files, cleaning up." << endl;
Close(); 
}
void Daisy::Replace(string& Value) {
string Temp = Value;
int Erased=0;
for (int i = 0; i < Value.length(); i++) {
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
int Position2 = Line.find('\"', Position+10);
string Content = Line.substr(Position+9, Position2 - Position);
Replace(Content);
int Position3 = Content.find("scheme");
if (Position3 != string::npos) {
Content.erase(Position3);
_Ncc.seekg(0, ios::beg);
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
string Line;
while (1) {
getline(_Ncc, Line);
if (Line.find("<body>") == string::npos) continue;
// We encountered end of file and didn't find what we were looking for
if (_Ncc.eof()) return -1;
// We have indeed found the body.
return _Ncc.tellg();
}
}
string Daisy::OpenSmil() {
string Line;
getline(_Ncc, Line);
int Position = Line.find("href=\"");
if (Position == string::npos) return string("notfound");
Position += 6;
int Position2 = Line.find("#", Position);
if (Position2 == string::npos) return string("# notfound");
Line.erase(0, Position);
Position2 = Line.find("#");
Line.erase(Position2);
return _Path + "\\" + Line;
}

void Daisy::OpenFirstSmil() {
// We'll make this easier by locating the body first.
FindBody();
string Filename = OpenSmil();
_Smil.open(Filename.c_str());
// We need to set the file position to 0 to satisfy the ExtractMetaInfo function.
_Ncc.seekg(0, ios::beg);
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
MP3 = Line;
}
return MP3.c_str();
}
