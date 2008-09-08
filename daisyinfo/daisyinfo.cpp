#include <iostream>
#include <string>
#include <fstream>
#include "daisyinfo.h"
using namespace std;
void Replace(string& Value) {
string Temp = Value;
int Erased=0;
for (int i = 0; i < Value.length(); i++) {
if (Value[i] == -61) {
if (Value[i+1] == -72)
Temp[i-Erased] = 'ø';
if (Value[i+1] == -90)
Temp[i-Erased] = 'æ';
Temp.erase((i-Erased)+1, 1);
Erased++;
}
}
Value = Temp;
}
void Show(ifstream& fin, const char* Text, const char* Name) {
string Value = ExtractInfo(fin, Name);
if (Value == "notfound") return;
cout << Text << Value << endl;
}
string ExtractInfo(ifstream& fin, string& Name) {
string Line;
int Position = 0;
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
Replace(Content);
int Position3 = Content.find("scheme");
if (Position3 != string::npos) {
Content.erase(Position3);
fin.seekg(0, ios::beg);
return Content.substr(0, Content.length() - 2);
}
#ifdef WIN32
return Content.substr(0, Content.length() - 4);
#else
return Content.substr(0, Content.length() - 5);
#endif
}
break;
}
}
}
string ExtractInfo(ifstream& fin, const char* Name) { 
string MyString(Name);
return ExtractInfo(fin, MyString);
}
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "Usage: " << argv[0] << " <path>" << endl << "where <path> is the directory containing your Daisy book." << endl;
return 1;
}
ifstream fin;
string Filename = argv[1];
#ifdef WIN32
Filename += "\\ncc.html";
#else
Filename += "/ncc.html";
#endif
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
Show(fin, "Producer's Notes: ", "ncc:prodNotes");
Show(fin, "Producer: ", "ncc:producer");
Show(fin, "Production Date: ", "ncc:producedDate");
Show(fin, "Revision Number: ", "ncc:revision");
Show(fin, "Revision Date: ", "ncc:revisionDate");
Show(fin, "Current Set: ", "ncc:setInfo");
Show(fin, "Current Set: ", "ncc:setinfo");
Show(fin, "Sidebars", "ncc:sidebars");
Show(fin, "Date the print source was published: ", "ncc:sourceDate");
Show(fin, "Source Edition: ", "ncc:sourceEdition");
Show(fin, "Publisher of the Print source: ", "ncc:sourcePublisher");
Show(fin, "Rights held over print source: ", "ncc:sourceRights");
Show(fin, "Title of print source: ", "ncc:sourceTitle");
Show(fin, "Navigation points: ", "ncc:tocItems");
Show(fin, "Navigation points: ", "ncc:tocitems");
Show(fin, "Navigation points: ", "ncc:TOCitems");
Show(fin, "Length of Daisy book: ", "ncc:totalTime");
Show(fin, "Length of Daisy book: ", "ncc:totaltime");

}
