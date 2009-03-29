/* PLAYLIST.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This file contains the implementation of a rough playlist class.
*/

#include "playlist.h"
using namespace std;
void PlayList::Add(string Item) { Items.push_back(Item); ++TotalItems; }
void PlayList::Add(char* Item) {
string MyItem = Item;
Add(MyItem);
}
bool Remove(int CurrentBook) {
if (CurrentBook < TotalItems)
return false;
Items.remove(CurrentBook);
}

bool PlayList::PreviousBook() {
if (CurrentItem == 0) return false;
--CurrentItem;
return true;
}
bool PlayList::NextBook() {
if (CurrentItem > TotalItems - 1) return false;
++CurrentItem;
return true;
}

