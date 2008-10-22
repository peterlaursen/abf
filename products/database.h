#ifndef DATABASE_H
#define DATABASE_H
#include <cstdio>
#include "sqlite3.h"
using std::FILE;
void CreateDatabase();
bool DBExists();
void SaveLastPosition(FILE*, char*);
int GetLastPosition(char* Title);

#endif
