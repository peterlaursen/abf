#include <iostream>
#include <cstdio>
#include <string>
#include "database.h"
using namespace std;
void SaveLastPosition(FILE* fin, char* Title) {
sqlite3* DB;
sqlite3_open("ABFConverter.db", &DB);
char Position[40];
itoa(ftell(fin), Position, 10);
string UpdateQuery = "update audiobooks set position=";
UpdateQuery += Position;
UpdateQuery += " where title='";
UpdateQuery+=Title;
UpdateQuery+="';";
cout << "Update query: " << UpdateQuery << endl;
sqlite3_exec(DB, UpdateQuery.c_str(), 0, 0, 0);
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
