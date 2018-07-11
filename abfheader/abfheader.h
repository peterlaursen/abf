/* $Id$
Copyright (C) 2018 Peter Laursen.

This folder contains a re-implementation of the ABF Header.
We try to make this into a class instead of a struct.
Since this means breaking API changes, this is presently placed here.
*/
#pragma once
#include <string>
#include <iosfwd>
#include <vector>
using std::string;]
using std::vector;
namespace ABF {
class ABFHeader {
const string ABFIdentifier("ABF");
unsigned short HeaderSize = 0;
unsigned short Major = 0;
unsigned short Minor = 0;
string Title;
string Author;
string Time;
unsigned short NumSections = 0;
unsigned short NumMinutes = 0;
int IndexTablePosition = 0;
vector<int> MinutePositions;
vector<int> ArrayPositions;

