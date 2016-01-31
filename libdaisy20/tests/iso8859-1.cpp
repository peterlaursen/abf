#include <iostream>
#include "../libdaisy20.h"
using namespace std;
using namespace ABF;
int main() {
DaisyBook D("iso8859-1");
D.GetMetadata();
cout << "Title: " << D.GetTitle() << endl << "Author: " << D.GetAuthor() << endl;
return(0);
}

