/* GUICONVERTER.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This is a simple graphical user interface for our converter.

I am by no means a GUI designer, so I'll make it as simple as I possibly can.
*/
// This is managed code.
#pragma managed
// Add the libraries as pragma directives
#pragma comment(lib, "audiere.lib")
#pragma comment(lib, "libabf.lib")
#pragma comment(lib, "libspeex.lib")
#pragma comment(lib, "libspeexdsp.lib")
#pragma comment(lib, "user32.lib")
#using <mscorlib.dll>
#include <tchar.h>
#using <System.dll>
#using <System.Windows.Forms.dll>
#include <windows.h>
#include <libabf.h>
using namespace ABF;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::IO;
using namespace System::ComponentModel;
using namespace System::Threading;
using namespace System::Windows::Forms;
public ref class MyForm: public Form {
array<String^>^ Files;
FolderBrowserDialog^ FB;
SaveFileDialog^ FSD;
MainMenu^ MyMenu;
TextBox^ TB;
bool Quit;
void SetupMenu() {
MyMenu = gcnew MainMenu();
MyMenu->MenuItems->Add(gcnew MenuItem("&Convert"));
MyMenu->MenuItems[0]->MenuItems->Add(gcnew MenuItem("&Daisy Book..."));
MyMenu->MenuItems[0]->MenuItems[0]->Click += gcnew EventHandler(this, &MyForm::ConvertDaisyBook);
MyMenu->MenuItems[0]->MenuItems->Add(gcnew MenuItem("&Folder..."));
MyMenu->MenuItems[0]->MenuItems[1]->Click += gcnew EventHandler(this, &MyForm::ConvertFolder);
MyMenu->MenuItems->Add(gcnew MenuItem("&Help"));
MyMenu->MenuItems[1]->MenuItems->Add(gcnew MenuItem("&About GUI Converter"));
MyMenu->MenuItems[1]->MenuItems[0]->Click += gcnew EventHandler(this, &MyForm::AboutApplication);
this->Menu = MyMenu;
}
// Event handlers
void AboutApplication(Object^ O, EventArgs^ E) {
MessageBox::Show("GUI Converter\r\n.NET Graphical User Interface\r\nCopyright (C) 2009 Peter Laursen.\r\n\r\nThis is a graphical application that should make it easier to convert ABF audio books if you are a Windows user.", "About GUICONVERTER");
}
void ConvertDaisyBook(Object^ O, EventArgs^ E) {
this->FB = gcnew FolderBrowserDialog();
FB->Description = "Select DAISY book to Convert.";
FB->RootFolder = Environment::SpecialFolder::MyComputer;
FB->ShowNewFolderButton = false;
if (FB->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
else MessageBox::Show("A path was selected.");

}
void ConvertFolder(Object^ O, EventArgs^ E) {
FB = gcnew FolderBrowserDialog();
FB->Description = "Select folder to convert. Note that subfolders are included.";
FB->RootFolder = Environment::SpecialFolder::MyComputer;
FB->ShowNewFolderButton = false;
if (FB->ShowDialog() != System::Windows::Forms::DialogResult::OK) return;
// Search the specified directory for MP3 files.
Files = Directory::GetFiles(FB->SelectedPath, "*.mp3", SearchOption::AllDirectories);
if (Files->Length == 0) 
// Seek for ogg files
Files = Directory::GetFiles(FB->SelectedPath, "*.ogg", SearchOption::AllDirectories);
if (Files->Length == 0)
Files = Directory::GetFiles(FB->SelectedPath, "*.wav", SearchOption::AllDirectories);
if (Files->Length != 0)
MessageBox::Show("We have found some files.");
Array::Sort(Files);
Thread^ MyThread = gcnew Thread(gcnew ThreadStart(this, &MyForm::MyThread));
MyThread->Start();
}
void MyThread() {
String^ OldTitle = this->Text;
	this->Text += "- Converting Audio Book";
try {
FSD = gcnew SaveFileDialog();
FSD->Filter = "ABF Audio Books (*.abf)|*.abf";
FSD->Title = "Choose where your audio book should be saved";
FSD->InitialDirectory = Environment::GetFolderPath(Environment::SpecialFolder::Desktop);
FSD->ShowDialog(this);
String^ Filename = FSD->FileName;
IntPtr ABFFileName = Marshal::StringToHGlobalAnsi(Filename);
char* ABFBookFileName = (char*)ABFFileName.ToPointer();
AbfEncoder AE(ABFBookFileName);
AE.SetTitle("Test Audio Book");
AE.SetAuthor("Unknown");
AE.SetTime("Unknown");
AE.SetNumSections((unsigned short)Files->Length);
AE.WriteHeader();
this->MyMenu->MenuItems[0]->Enabled = false;
int FilesConverted = 1;
String^ TBText;
this->Controls[0]->Enabled = true;
for each (String^ S in Files) {
	if (Quit) 		return;
	TBText = "Converting File " + FilesConverted + " of " + Files->Length;
this->Controls[0]->Text = TBText;
	if (!ConvertToAscii(S, AE)) break;
++FilesConverted;
}
Marshal::FreeHGlobal(ABFFileName);
} catch (Exception^ E) {
MessageBox::Show(E->Message, "Exception.");
}


MessageBox::Show("The audio book has been successfully converted.", "Conversion Finished!");
this->Controls[0]->Hide();
this->Text = OldTitle;
this->MyMenu->MenuItems[0]->Enabled = true;
}

bool ConvertToAscii(String^ S, AbfEncoder& AE) {
IntPtr ip = Marshal::StringToHGlobalAnsi(S);
const char* str = (const char*)ip.ToPointer();
char* TempFile = DecodeToRawAudio(str);
if (!TempFile) {
MessageBox::Show("Error, for some unknown reason, that file cannot be converted.", "Error, aborting conversion!");
return false;
}
AE.WriteSection();
EncodeABF(AE, TempFile);
Marshal::FreeHGlobal(ip);
return true;
}
void MyFormClosing(Object^ Sender, FormClosingEventArgs^ E) {
Quit = true;
}
public:
MyForm() {
Quit = false;
TB = gcnew TextBox();
TB->AutoSize = true;
TB->ReadOnly = true;
TB->Enabled = false;

this->Controls->Add(TB);
SetupMenu();
this->Text = "ABF Graphical Converter";
this->TopLevel = true;
this->AutoSize = true;
this->FormClosing += gcnew FormClosingEventHandler(this, &MyForm::MyFormClosing);
}
};

[STAThread]
int WinMain(HINSTANCE ThisInstance, HINSTANCE, LPTSTR, int) {
Application::EnableVisualStyles();
Application::Run(gcnew MyForm());
return 0;
}
