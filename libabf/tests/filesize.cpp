#include <cstdio>
using namespace std;
int main()  {
printf("File size: %d\n", sizeof(FILE*));

FILE** numfiles = nullptr;
numfiles = new FILE*[10];
printf("Size of numfiles: %d\n", sizeof(numfiles));

numfiles[0] = fopen("file1.pem", "w");
fprintf(numfiles[0], "Hello.");
fclose(numfiles[0]);
delete[] numfiles;
}
