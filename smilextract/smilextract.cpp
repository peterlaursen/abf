/* SMILEXTRACT.CPP, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This file contains the implementation of the functions found in "smilextract.h"
*/
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "smilextract.h"
using namespace std;
int FindBody(ifstream& fin) {
// We are quite certain that this is a handle to ncc.html.
string Line;
while (1) {
getline(fin, Line);
if (Line.find("<body>") == string::npos) continue;
// We encountered end of file and didn't find what we were looking for
if (fin.eof()) return -1;
// We have indeed found the body.
return fin.tellg();
}
}
string OpenSmil(ifstream& fin) {
// We need to call this function to advance the position of the file pointer.
string Line;
getline(fin, Line);
int Position = Line.find("href=\"");
if (Position == string::npos) return string("notfound");
Position += 6;
int Position2 = Line.find("#", Position);
if (Position2 == string::npos) return string("# notfound");
Line.erase(0, Position);
Position2 = Line.find("#");
Line.erase(Position2);
return Line;
}
string GetMP3(ifstream& fin) {
// Note that this is to be the smil file, not the ncc.html file.
if (!fin) {
cout << "File cannot be opened" << endl;
return "notopened";
}
string Line;
string MP3;
while (!fin.eof()) {
getline(fin, Line);
if (Line.find("<audio ") == string::npos) continue;
int Position = Line.find("\"");
Line.erase(0, Position+1);
Position = 0;
// This is just an audio file - we know it ends with a quote, therefore, we search again.
int Position2 = Line.find("\"", Position);
Line.erase(Position2);
// We do not need to return the quote, we'll delete it alongside the rest of the string.
MP3 = Line;
}
cout << MP3;
return MP3;
}
int main(int argc, char* argv[]) {
string Filename = argv[1];
Filename += "\\ncc.html";
ifstream fin(Filename.c_str());
cout << "The Position of the <body> element is " << FindBody(fin) << endl;
string Smil(argv[1]);
Smil += "\\";
Smil += OpenSmil(fin);
cout << "Trying to open " << Smil << endl;
cout << "The first SMIL file in this book is " << Smil << endl;
ifstream SmilFile(Smil.c_str());
string File = GetMP3(SmilFile);
cout << "The MP3 file is " << File << endl;
cout << "The second SMIL file is " << OpenSmil(fin) << endl;
fin.close();
}