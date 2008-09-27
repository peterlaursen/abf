#include "libdaisy.h"
#include <iostream>
#include <cstdio>
using namespace ABF;using namespace std;

int main(int argc, char* argv[]) {
Daisy D(argv[1], false);
D.Open();
while (D.OpenSmil()) {
const char* Value = D.GetMP3FileName();
if (Value == "notfound") break;

cout << Value << endl;
FILE* File = fopen(Value, "rb");
if (!File) cout << "File not found.";
else {
fclose(File);
cout << Value << " opened successfully." << endl;
}
}
}


