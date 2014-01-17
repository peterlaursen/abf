/* $Id$

We have tried to build an example using our LibABF things.
*/
using System;
class Program {
static void Main(string[] Args) {
if (Args.Length != 1) {
Console.WriteLine("Provide us an audio book and we'll get some meta data.");
return;
}
AbfDecoder AD = new AbfDecoder(Args[0]);
Console.WriteLine("Title: {0}, Author: {1}, Time: {2}", AD.GetTitle(), AD.GetAuthor(), AD.GetTime());
}
}
