/* LIBDAISY, Version 2.0
Copyright (C) 2010 Peter Laursen.

This is to be a replacement for the old LibDaisy library. It is written mainly to improve and (I hope) simplify getting information out of Daisy books.

This library is released under the same license as the rest of this package.
*/
#ifndef LIBDAISY20_H
#define LIBDAISY20_H
#ifdef WIN32
#ifdef BUILD_DLL
#define SHARED __declspec(dllexport)
#else
#define SHARED __declspec(dllimport)
#endif
#else
#define SHARED
#endif
#include <string>
#include <vector>
#include <fstream>
namespace ABF {
class SHARED DaisyBook {
std::string Path;
int Volumes; // Daisy books may have multiple volumes
int CurrentVolume;
std::string Tag;
std::string Identification;
std::string Title, Author, Time;
std::ifstream Smil, Content;
std::vector<std::string> AudioFiles;
bool IsValid;
public:
DaisyBook(char* Path);
~DaisyBook();
std::string& GetTag(bool FromNCC=true);
// The below function extracts all the required data from the daisy book. This is the data required by ABF.
bool GetMetadata();
const std::string& GetTitle();
const std::string& GetAuthor();
const unsigned short GetNumSections();
const int GetVolumes();
int GetCurrentVolume();
const std::string& GetTotalTime();
const std::string& GetIdentification();
};
}
#endif
