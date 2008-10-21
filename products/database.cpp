#include <iostream>
#include <cstdio>
#include <string>
#include "database.h"
using namespace std;
int Callback(void* Type, int NumRows, char** Results, char** Columns) {
bool* Found = (bool*)Type;
*Found = true;
return 0;
}
void SaveLastPosition(FILE* fin, char* Title) {
sqlite3* DB;
sqlite3_open("ABFConverter.db", &DB);
char Position[40];
itoa(ftell(fin), Position, 10);
string UpdateQuery = "select * from audiobooks where title = '";
UpdateQuery += Title;
UpdateQuery += "';";
bool MyFound = false;
bool* Found = &MyFound;
sqlite3_exec(DB, UpdateQuery.c_str(), Callback, Found, 0);
cout << boolalpha << MyFound << endl;
if (MyFound) {
cout << "The title already existed, updated position." << endl;
return;
}
		string Query = "insert into audiobooks values('";
Query += Title;
Query += "', ";
Query += Position;
Query += ");";
cout << Query << endl;
char* Error;
sqlite3_exec(DB, Query.c_str(), 0, 0, &Error);
sqlite3_free(Error);
sqlite3_close(DB);
sqlite3_shutdown();
}
