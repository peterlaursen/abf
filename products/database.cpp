#include <sstream>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstdlib>
#include "database.h"
using namespace std;
static bool Initialized = false;
static string DBName;
void Init() {
#ifdef WIN32
DBName += getenv("HOMEDRIVE");
char* Home = getenv("HOMEPATH");
DBName += Home;
DBName += "\\.abfplayer.db";
#else
DBName += getenv("HOME");
DBName += "/.abfplayer.db";
#endif
cout << "Database location: " << DBName << endl;
if (!DBExists()) CreateDatabase();
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
void SaveLastPosition(char* Title, int LastPosition) {
if (!Initialized) {
Init();
}
sqlite3* DB;
if (sqlite3_open_v2(DBName.c_str(), &DB, SQLITE_OPEN_READWRITE, 0) != SQLITE_OK) {
cout << "An error occurred opening the database: " << sqlite3_errmsg(DB) << endl;
return;}

const char* Position;
ostringstream os;
os << LastPosition;
string Temp = os.str();
Position = Temp.c_str();
// Detect if the book has already been stored in the database. This might be done in a different way, but this way is the easiest.
string Exists = "select * from audiobooks where title = '";
Exists += Title;
Exists += "';";
// Pass the Found pointer, which is pointing to MyFound, into the callback function. If the callback is called, the MyFound variable will be true.
bool MyFound = false;
bool* Found = &MyFound;
sqlite3_exec(DB, Exists.c_str(), Callback, Found, 0);
cout << Exists << endl;
char* Error;
if (MyFound) {
cout << "The title was found." << endl;
string UpdateQuery = "update audiobooks set lastposition = ";
UpdateQuery += Position;
UpdateQuery += " where title = '";
UpdateQuery += Title;
UpdateQuery += "';";
sqlite3_exec(DB, UpdateQuery.c_str(),0,0, &Error);
cout << UpdateQuery << endl;
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
sqlite3_exec(DB, Query.c_str(), 0, 0, &Error);
sqlite3_free(Error);
cout << "Query: " << Query << endl << "Hopefully, everything's saved by now." << endl;
sqlite3_close(DB);
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
bool DBExists() {
FILE* fin = fopen(DBName.c_str(), "rb");
if (!fin) return false;
fclose(fin);
return true;
}
void CreateDatabase() {
sqlite3_initialize();
sqlite3* DB;
sqlite3_open_v2(DBName.c_str(), &DB, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, 0);
sqlite3_exec(DB, "create table audiobooks(title varchar(255) not null, lastposition int not null);", 0, 0, 0);
sqlite3_close(DB);
}
void DeletePosition(char* Title) {
string Query = "delete from audiobooks where title = '";
Query += Title;
Query += "';";
sqlite3* DB;
sqlite3_open(DBName.c_str(), &DB);
sqlite3_exec(DB, Query.c_str(), 0, 0, 0);
sqlite3_close(DB);
return;
}
