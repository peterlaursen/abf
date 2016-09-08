/* $Id$
Copyright (C) 2016 Peter Laursen.

This contains an attempt at a multithreaded implementation.
I am not at all sure that this will work, given my limited exposure to C++11 threads. I will try, however.
*/
#ifndef LIBABFMULTI_H
#define LIBABFMULTI_H
#include <vector>
#include <fstream>
#include <string>
#include "abfpart.h"
namespace ABFMulti {
using std::vector;
using std::string;
using std::ofstream;
using std::ios_base;
class AbfEncoder {
AbfPart* AbfParts = nullptr;
unsigned short NumSections = 0;
const string AbfIdentifier = "ABF";
const unsigned short Major = 2;
const unsigned short Minor = 1;
string _Title, _Author, _Time;
ofstream FinalAudioBook;
public:
AbfEncoder(string FileName): FinalAudioBook(FileName) {}
~AbfEncoder() {
if (AbfParts != nullptr) delete[] AbfParts;
}
void SetTitle(const string& Title) { _Title = Title; }
void SetAuthor(const string& Author) { _Author = Author; }
void SetTime(const string& Time) { _Time = Time; }
void SetNumParts(unsigned short Sections);
AbfPart* GetAbfPart(unsigned int PartNumber) { return &AbfParts[PartNumber]; }
};
}
#endif
