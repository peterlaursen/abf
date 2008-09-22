/* LIBDAISY.H, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This is the header file for LibDaisy.

*/
#ifndef LIBDAISY_H
#define LIBDAISY_H
// Includes
#include <fstream>
#include <string>
#include <vector>
using std::ifstream;
using std::string;
using std::vector;
namespace ABF {
// This is the only class available in this namespace. If other classes (or formats) need parsing, they shall also be added in this namespace.
class Daisy {
// This is set for every daisy book
string _Path;
// Set the private members
ifstream _Ncc;
ifstream _Mp3;
ifstream _Smil;
string _Meta; // A single string for a single meta information value.

public:
// Let us add the constructors and destructors. Note that they are not implemented here.
// We overload the constructor. The reason behind this is that we may either want to use an already-existing string or a string we get from, say, command line.
// If the Open parameter is set to true, the different file descriptors are initialised to known values.
Daisy(string Path, bool Open = false);
Daisy(const char* Path, bool Open = false);
// The destructor internally calls the Close function, but it seemed wise to make it public anyway.
~Daisy();
// The Open function returns false if the files are already open or another error occurs.
bool Open();
string& ExtractMetaInfo(string Value);
string& ExtractMetaInfo(string& Value);
// Get the current content of meta info string
string& GetMetaInfo() const { return _Meta; }
// Close the open file descriptors.
void Close();
};
#endif
