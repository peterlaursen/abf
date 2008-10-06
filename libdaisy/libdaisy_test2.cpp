#include <iostream>
#include <string>
#include "libdaisy.h"
using namespace std;
using namespace ABF;
int main(int argc, char* argv[]) {
if (argc != 2) {
cout << "Specify a path to a Daisy book." << endl;
return 1;
}
Daisy D(argv[1]);
if (!D.IsValid()) {
cout << "Isn't a valid daisy book." << endl;
return 2;
}
string Peter;
for (int i = 0; i < 2; i++) {
D.ExtractSectionTitle();
cout << D.GetSectionTitle() << endl;
}
return 0;
}
