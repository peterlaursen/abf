#ifndef DAISYINFO_H
#define DAISYINFO_H
#include <iosfwd>
#include <string>
#include <fstream>
using std::ifstream;
using std::string;
string ExtractInfo(ifstream& fin, string& Name);
string ExtractInfo(ifstream& fin, const char* Name);
void Show(ifstream& fin, const char* Text, const char* Name);
#endif
