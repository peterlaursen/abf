#include <iostream>
#include <sstream>
#include "libabfmulti.h"
using namespace std;
using namespace ABFMulti;
int main(int argc, char* argv[]) {
AbfEncoder AE("peter.abf");
AE.SetNumParts(3);
for (int i = 0; i < 3; i++) {
AbfPart* A = AE.GetAbfPart(i);
ostringstream Name;
Name << "Hello-";
Name << i;
A->SetFileName(Name.str());
}

}

