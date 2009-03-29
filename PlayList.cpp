/* PLAYLIST.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This file contains the implementation of a rough playlist class.
*/

#include "playlist.h"
using namespace std;
void PlayList::Add(string Item) { Items.push_back(Item); ++NumberOfItems; }
void PlayList::Add(char* Item) {
string MyItem = Item;
Add(MyItem);
}
void PlayList::Remove(int CurrentBook) {
if (CurrentBook < NumberOfItems)
return;
Items.erase(Items.begin()+CurrentBook);
--NumberOfItems;
}

bool PlayList::PreviousBook() {
if (CurrentItem == 0) return false;
--CurrentItem;
return true;
}
bool PlayList::NextBook() {
if (CurrentItem > NumberOfItems - 1) return false;
++CurrentItem;
return true;
}

int PlayList::GetCurrentBook() { return CurrentItem; }
char* PlayList::GetCurrentBookName() {
string Book = Items[CurrentItem];
return (char*)Book.c_str();
}
int PlayList::GetTotalItems() { return NumberOfItems; }
