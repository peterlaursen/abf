/* SMILEXTRACT.H, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This file contains the interface to SmilExtract.
*/
#ifndef SMILEXTRACT_H
#define SMILEXTRACT_H
#include <string>
#include <iosfwd>
#include <fstream>
#include <vector>
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;
// Find the position of the string <body>
int FindBody(ifstream& fin);
// Get the SMIL file to open, then run it through to look for MP3 files.
string OpenSmil(ifstream& fin);
// Return list of MP3 files
string GetMP3(ifstream& fin);
#endif
