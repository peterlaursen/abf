/* $Id$
Copyright (C) 2008, 2009, 2010, 2011, 2012, 2013, 2014, 2015 Peter Laursen.

This contains a rough implementation of a playlist class.
*/
#ifndef PLAYLIST_H
#define PLAYLIST_H
#include "../abfdecoder/abfdecoder.h"
#include "player.h"
#include <string>
#include <vector>
namespace ABF {
class PlayList {
std::vector<std::string> Items;
int NumberOfItems;
int CurrentItem;
public:
PlayList(): NumberOfItems(0), CurrentItem(0) {}
void Add(std::string);
void Add(char*);
void Remove(int BookNumber = 0);
bool NextBook();
bool PreviousBook();
int GetCurrentBook();
char* GetCurrentBookName();
int GetTotalItems();

};
}
#endif

