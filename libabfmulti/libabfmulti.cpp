/* $Id$
Copyright (C) 2016 Peter Laursen.

This file contains a short implementation of a very small function, just to test the abf parts vector.
*/
#include "libabfmulti.h"
#include <iostream>
#include <sstream>
namespace ABFMulti {
using std::ostringstream;
using std::cout;
using std::endl;
void AbfEncoder::SetNumParts(unsigned short NumSections) {
if (AbfParts != nullptr) {
	delete[] AbfParts; // Free the memory
	AbfParts = nullptr;
}
AbfParts = new AbfPart[NumSections];
for (int i = 0; i < NumSections; i++) {
cout << AbfParts[i].GetPartNumber() << " is this part's sequence." << endl;
}
}
}
