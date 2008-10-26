#ifndef DATABASE_H
#define DATABASE_H
#include <cstdio>
#include "sqlite3.h"
using std::FILE;
void CreateDatabase();
bool DBExists();
void SaveLastPosition(char*, int);
int GetLastPosition(char* Title);
void DeletePosition(char*);

#endif
