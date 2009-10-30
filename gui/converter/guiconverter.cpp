/* GUICONVERTER.CPP, Version 1.0.
Copyright (C) 2009 Peter Laursen.

This is a simple graphical user interface for our converter.

I am by no means a GUI designer, so I'll make it as simple as I possibly can.
*/
// This is managed code.
#pragma managed
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
using namespace System::Windows::Forms;
public ref class MyForm: public Form {
FolderBrowserDialog^ FB;
MainMenu^ MyMenu;
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
array<String^>^ Files = Directory::GetFiles(FB->SelectedPath, "*.mp3", SearchOption::AllDirectories);
if (Files->Length == 0) 
// Seek for ogg files
Files = Directory::GetFiles(FB->SelectedPath, "*.ogg", SearchOption::AllDirectories);
if (Files->Length == 0)
Files = Directory::GetFiles(FB->SelectedPath, "*.wav", SearchOption::AllDirectories);
if (Files->Length != 0)
MessageBox::Show("We have found some files.");
AbfEncoder AE("demo.abf");
AE.SetTitle("Test Audio Book");
AE.SetAuthor("Unknown");
AE.SetTime("Unknown");
AE.SetNumSections((unsigned short)Files->Length);
AE.WriteHeader();
this->Text += "- Converting Audio Book";
for each (String^ S in Files) {
String^ Convert = "Converting File ";
Convert += S;
MessageBox::Show(Convert, "Status");
ConvertToAscii(S, AE);
}

}
void ConvertToAscii(String^ S, AbfEncoder& AE) {
IntPtr ip = Marshal::StringToHGlobalAnsi(S);
const char* str = static_cast<const char*>(ip.ToPointer());
char* TempFile = DecodeToRawAudio(str);
Marshal::FreeHGlobal(ip);
AE.WriteSection();
EncodeABF(AE, TempFile);
}

public:
MyForm() {
SetupMenu();
this->Text = "ABF Graphical Converter";
this->TopLevel = true;
this->AutoSize = true;
}
};

[STAThread]
int WinMain(HINSTANCE ThisInstance, HINSTANCE, LPTSTR, int) {
Application::EnableVisualStyles();
Application::Run(gcnew MyForm());
return 0;
}
