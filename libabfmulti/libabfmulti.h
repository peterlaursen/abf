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
ofstream FinalAudioBook;
public:
AbfEncoder(string FileName): FinalAudioBook(FileName) {}
~AbfEncoder() {
if (AbfParts != nullptr) delete[] AbfParts;
}
void SetNumParts(unsigned int Sections);
AbfPart* GetAbfPart(unsigned int PartNumber) { return &AbfParts[PartNumber]; }
};
}
#endif
