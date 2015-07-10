/* $Id$
$Copyright$

This contains an implementation of a database access layer for use in ABF.
It is tied specifically to the sqlite3 database.
*/
#ifndef DATABASE_H
#define DATABASE_H
#include <cstdio>
#include "sqlite3.h"
namespace ABF {
using std::FILE;
void CreateDatabase();
bool DBExists();
void SaveLastPosition(const char*, int);
int GetLastPosition(const char* Title);
void DeletePosition(const char*);
}
#endif
