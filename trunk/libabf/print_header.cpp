#include <iostream>
#include "libabf.h"
using namespace ABF;
using namespace std;
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "Error, no book provided." << endl;
return 1;
}
AbfDecoder AD(argv[1]);
AD.Validate();
AD.ReadHeader();
cout << AD.GetTitle() << endl << AD.GetAuthor() << endl << AD.GetTime() << endl << AD.GetNumSections() << endl;
}
