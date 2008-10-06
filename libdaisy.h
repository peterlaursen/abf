/* LIBDAISY.H, Version 1.0.
Copyright (C) 2008 Peter Laursen.

This is the header file for LibDaisy.

*/
#ifndef LIBDAISY_H
#define LIBDAISY_H
// Define this for Win32
#ifdef WIN32
#define FILE_SEP "\\"
#ifdef BUILD_DLL
#define SHARED __declspec(dllexport)
#else
#define SHARED __declspec(dllimport)
#endif
#else
#define SHARED
#define FILE_SEP "/"
#endif
// Includes
#include <fstream>
#include <string>
#include <vector>
using std::ifstream;
using std::string;
using std::vector;
namespace ABF {
// This is the only class available in this namespace. If other classes (or formats) need parsing, they shall also be added in this namespace.
class SHARED Daisy {
// This is set for every daisy book
string _Path;
// Set the private members
ifstream _Ncc;
string _SectionTitle;
ifstream _Smil;
string _Meta; // A single string for a single meta information value.
// Whether the book is valid.
bool _Valid;
// We'll also need our private Replace() function from DaisyInfo.cpp, it is copied in here.
void Replace(string& Value);
// This function opens the first smil file it encounters. Speaking from a design standpoint, I am not too happy with this behaviour, but I'll change it in a short while.
string FindSmil();
// This function helps OpenSmil() and other functions.
int FindBody();
public:
// Let us add the constructors and destructors. Note that they are not implemented here.
// We overload the constructor. The reason behind this is that we may either want to use an already-existing string or a string we get from, say, command line.
// If the Open parameter is set to true, the different file descriptors are initialised to known values, otherwise, you manually have to call the Open function later.
Daisy(string Path, bool _Open = true);
Daisy(const char* Path, bool _Open = true);
// The destructor - takes care of cleaning up.
~Daisy();
// Validate whether this is a Daisy book.
void Validate();
// Return whether this is a valid Daisy book.
bool IsValid();
// The Open function returns false if the files are already open or another error occurs.
bool Open(bool _OpenSmil = false);
bool OpenSmil();
string& ExtractMetaInfo(string& Value);
// Get the current content of meta info string, may not be necessary.
string const GetMetaInfo() const { return _Meta; }
// Close the open file descriptors.
// The destructor internally calls the Close function, but it seemed wise to make it public anyway.
void Close();
// Let's get the MP3 file too.
const char* GetMP3FileName();
// ExtractSectionTitle gives you back the title of the next section in the book.
string& ExtractSectionTitle();
// This returns the section title.
};

// Close the namespace
}
#endif
