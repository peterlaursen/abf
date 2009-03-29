/* PLAYLIST.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This contains a rough implementation of a playlist class.
*/
#ifndef PLAYLIST_H
#define PLAYLIST_H
#include <libabf.h>
#include "player.h"
#include <string>
#include <vector>
SHARED class PlayList {
std::vector<std::string> PlayList;
int NumberOfItems;
int CurrentItem;
public:
PlayList(): NumberOfItems = 0, CurrentItem = 0 {}
void Add(std::string);
void Add(std::string[]);
void Remove(int BookNumber = this->CurrentItem);
bool NextBook();
bool PreviousBook();
};
#endif

