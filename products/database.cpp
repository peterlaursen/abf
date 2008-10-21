#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include "database.h"
using namespace std;
static bool Initialized = false;
static string DBName;
void Init() {
DBName = getenv("HOMEDRIVE");
char* Home = getenv("HOMEPATH");
DBName += Home;
DBName += "\\.abfplayer.db";
Initialized = true;
}
int Callback(void* Type, int NumRows, char** Results, char** Columns) {
bool* Found = (bool*)Type;
*Found = true;
return 0;
}
int Callback2(void* Type, int NumRows, char** Results, char** Columns) {
// We assume that this is the pointer holding the integer.
int LastPosition = atoi(Results[0]);
int* p_lastposition = (int*)Type;
 *p_lastposition = LastPosition;
return 0;
}
void SaveLastPosition(FILE* fin, char* Title) {
if (!Initialized) {
Init();
}
sqlite3* DB;
sqlite3_open(DBName.c_str(), &DB);
char Position[40];
itoa(ftell(fin), Position, 10);
// Detect if the book has already been stored in the database. This might be done in a different way, but this way is the easiest.
string Exists = "select * from audiobooks where title = '";
Exists += Title;
Exists += "';";
bool MyFound = false;
bool* Found = &MyFound;
sqlite3_exec(DB, Exists.c_str(), Callback, Found, 0);
char* Error;
if (MyFound) {
string UpdateQuery = "update audiobooks set lastposition = ";
UpdateQuery += Position;
UpdateQuery += " where title = '";
UpdateQuery += Title;
UpdateQuery += "';";
sqlite3_exec(DB, UpdateQuery.c_str(),0,0, &Error);
if (Error) {
cout << "An error must have occurred: " << Error << endl;
sqlite3_free(Error);
}
return;
}
		string Query = "insert into audiobooks values('";
Query += Title;
Query += "', ";
Query += Position;
Query += ");";
cout << Query << endl;
sqlite3_exec(DB, Query.c_str(), 0, 0, &Error);
sqlite3_free(Error);
sqlite3_close(DB);
sqlite3_shutdown();
}
int GetLastPosition(char* Title) {
string Query = "select lastposition from audiobooks where title = '";
Query += Title;
Query += "';";
int LastPosition = 0;
int* p_lastposition = &LastPosition;
sqlite3* DB;
if (!Initialized) Init();

sqlite3_open(DBName.c_str(), &DB);
sqlite3_exec(DB, Query.c_str(), Callback2, p_lastposition, 0);
return LastPosition;
}
