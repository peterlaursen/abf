/* PLAYLIST.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This file contains the implementation of a rough playlist class.
*/
#include <iostream>
#include "playlist.h"
using namespace std;
void PlayList::Add(string Item) { Items.push_back(Item); ++NumberOfItems; }
void PlayList::Add(char* Item) {
string MyItem = Item;
Add(MyItem);
}
void PlayList::Remove(int CurrentBook) {
if (CurrentBook >= NumberOfItems) return;
Items.erase(Items.begin()+CurrentBook);
if (NumberOfItems > 0) --NumberOfItems;
if (NumberOfItems == 1) CurrentItem = 0;
}

bool PlayList::PreviousBook() {
if (CurrentItem == 0) return false;
--CurrentItem;
return true;
}
bool PlayList::NextBook() {
if (NumberOfItems == 1) {
CurrentItem = 0;
return false;
}

if (CurrentItem < NumberOfItems - 1) {
++CurrentItem;
return true;
}
if (CurrentItem == NumberOfItems - 1 || CurrentItem > NumberOfItems - 1) {
CurrentItem = NumberOfItems - 1;
return true;
}

return false;
}

int PlayList::GetCurrentBook() { return CurrentItem; }
char* PlayList::GetCurrentBookName() {
return (char*)Items[CurrentItem].c_str();
}
int PlayList::GetTotalItems() { return NumberOfItems; }
