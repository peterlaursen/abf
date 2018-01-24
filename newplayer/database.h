/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This contains an implementation of a database access layer for use in ABF.
It is tied specifically to the sqlite3 database.
*/
#ifndef DATABASE_H
#define DATABASE_H
#include <cstdio>
#include <sqlite3.h>
namespace ABF {
using std::FILE;
class Database {
void Init();
public:
void CreateDatabase();
bool DBExists();
void SaveLastPosition(const char*, int);
int GetLastPosition(const char* Title);
void DeletePosition(const char*);
};
}
#endif
