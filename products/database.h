#ifndef DATABASE_H
#define DATABASE_H
#include <cstdio>
#include "sqlite3.h"
using std::FILE;
void CreateDatabase();
bool DBExists();
void SaveLastPosition(const char*, int);
int GetLastPosition(const char* Title);
void DeletePosition(const char*);

#endif
