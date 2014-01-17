Important:

When building a C# program referencing a 32-bit DLL (as is the case for libabf.dll currently), you must specify the following command-line options for the csc.exe compiler:
csc /platform:x86 /r:libabf-csharp.dll example.cs

Otherwise, you get a format exception.
