#include <iostream>
#include <string>
#include <fstream>
using namespace std;
string ExtractInfo(ifstream& fin, string& Name) {
string Line;
int Position = 0;
string MetaString("meta");
while (1) {
getline(fin, Line);
if (Line.find("</head>") != string::npos) {
cout << "Data element not found." << Name;
fin.seekg(0, ios::beg);
return Name;
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
cout << "Usage: " << argv[0] << " <path>" << endl;
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
cout << "Author: " << ExtractInfo(fin, string("dc:creator")) << endl;
cout << "Title: " << ExtractInfo(fin, string("dc:title")) << endl;

cout << ExtractInfo(fin, string("dc:title")) << endl;
cout << "Publishing Organisation: " << ExtractInfo(fin, string("dc:publisher")) << endl;
cout << "Size of book in kilobytes: " << ExtractInfo(fin, "ncc:kByteSize") << endl;
cout << "Narrator: " << ExtractInfo(fin, "ncc:narrator") << endl;
cout << "Publishing Date: " << ExtractInfo(fin, "dc:date") << endl;
cout << "Language: " << ExtractInfo(fin, "dc:language");
}
