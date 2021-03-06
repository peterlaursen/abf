/* $Id$
LIBDAISY, Version 2.0
Copyright (C) 2010, 2011, 2012, 2013, 2014 Peter Laursen.

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
#include <strings.h>
#endif
#include <string>
#include <vector>
#include <fstream>
#ifndef WIN32
#include <dirent.h>
#else
#include "scandir.h"
#endif

namespace ABF {
class SHARED DaisyBook {
std::string Path;
dirent** FileList; // List of files in path
int FileListLength;
int Volumes; // Daisy books may have multiple volumes
int CurrentVolume; // Current CD
std::string Tag; // The tag as read by GetTag
std::string Identification; // Identification retrieved from Daisy book
std::string Title, Author, Time, Language, CharSet; // The book's title, author, playing time, language and character set.
std::ifstream Smil, Content; // File descriptors for contents and a single smil file.
std::vector<std::string> AudioFiles; // List of audio file names.
bool IsValid;
public:
DaisyBook(char* Path);
~DaisyBook();
const std::string& GetTag(bool FromNCC=true);
// The below function extracts all the required data from the daisy book. This is the data required by ABF.
bool GetMetadata();
bool NextVolume(char* Path);
const std::string& GetTitle();
const std::string& GetAuthor();
std::string& GetPath();
const unsigned short GetNumSections();
const int GetVolumes();
int GetCurrentVolume();
const std::string& GetTotalTime();
const std::string& GetIdentification();
void GetAudioFiles(); // This function fills the vector with file strings
const char* GetSectionFile(int SectionNumber) { return AudioFiles[SectionNumber].c_str(); }
bool BookIsValid() { return IsValid; }

};
}
#endif
