#include <iostream>
#include <string>
#include <fstream>
#include "daisyinfo.h"
using namespace std;
void Show(ifstream& fin, const char* Text, const char* Name) {
string Value = ExtractInfo(fin, Name);
if (Value == "notfound") return;
cout << Text << Value << endl;
}
string ExtractInfo(ifstream& fin, string& Name) {
string Line;
int Position = 0;
string MetaString("meta");
while (1) {
getline(fin, Line);
if (Line.find("</head>") != string::npos) {
fin.seekg(0, ios::beg);
return string("notfound");
}
int Position = Line.find("meta");
if (Position == string::npos) continue;
else {
Position = Line.find(Name);
if (Position == string::npos) continue;
else {
Position = Line.find("content=\"");
int Position2 = Line.find('\"', Position+10);
string Content = Line.substr(Position+9, Position2 - Position);
int Position3 = Content.find("scheme");
if (Position3 != string::npos) {
Content.erase(Position3);
fin.seekg(0, ios::beg);
return Content.substr(0, Content.length() - 2);
}
return Content.substr(0, Content.length() - 4);
}
break;
}
}
}
string ExtractInfo(ifstream& fin, const char* Name) { return ExtractInfo(fin, string(Name)); }
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "Usage: " << argv[0] << " <path>" << endl << "where <path> is the directory containing your Daisy book." << endl;
return 1;
}
ifstream fin;
string Filename = argv[1];
Filename += "\\ncc.html";
fin.open(Filename.c_str());
if (!fin) {
cout << "File not found." << endl;
return 1;
}
// Here is some info from the daisy book.
Show(fin, "Contributors to this book: ", "dc:contributor");
Show(fin, "Author: ", "dc:creator");Show(fin, "Subject of Book: ", "dc:coverage");
Show(fin, "Date Published: ", "dc:date");
Show(fin, "Description: ", "dc:description");
Show(fin, "Format of Daisy Book: ", "dc:format");
Show(fin, "Format of Daisy Book: ", "ncc:format");
Show(fin, "Unique Identifier: ", "dc:identifier");
Show(fin, "Unique Identifier: ", "ncc:identifier");
Show(fin, "Language: ", "dc:language");
Show(fin, "Publisher: ", "dc:publisher");
Show(fin, "Related Resource: ", "dc:relation");
Show(fin, "Special Legal Issues: ", "dc:rights");
Show(fin, "Print Source: ", "dc:source");
Show(fin, "Subject: ", "dc:subject");
Show(fin, "Title: ", "dc:title");
Show(fin, "Daisy Book Type: ", "dc:type");
Show(fin, "XML file encoding: ", "ncc:charset");
Show(fin, "Number of Levels: ", "ncc:depth");
Show(fin, "Number of Files in this book: ", "ncc:files");
Show(fin, "Footnotes: ", "ncc:footnotes");
Show(fin, "Software that generated this daisy book: ", "ncc:generator");
Show(fin, "Size of Book (in kb): ", "ncc:kByteSize");
Show(fin, "Page Count: ", "ncc:maxPageNormal");
Show(fin, "Type of Daisy book: ", "ncc:multimediaType");
Show(fin, "Narrator: ", "ncc:narrator");
Show(fin, "Front Page(s): ", "ncc:pageFront");
Show(fin, "Front Page(s): ", "ncc:page-front");
Show(fin, "Normal pages occuring: ", "ncc:pageNormal");
Show(fin, "Normal pages occuring: ", "ncc:page-normal");
Show(fin, "Special Pages: ", "ncc:pageSpecial");
Show(fin, "Special Pages: ", "ncc:page-special");

}
