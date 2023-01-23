#include <iostream>


#include "graphics_console.h"


#define UNICODE
#define _UNICODE


/*
Thanks to Ates Goral for this function
https://stackoverflow.com/a/440240
*/
std::wstring gen_random_wstr(const int len) {
    static const WCHAR alphanum[] =
        L"0123456789"
        L"abcdefghijklmnopqrstuvwxyz";
    std::wstring tmp_s;
    tmp_s.reserve(len);

    for (int i = 0; i < len; ++i) {
        tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    
    return tmp_s;
}

GraphicsConsole::GraphicsConsole(){

}

GraphicsConsole::~GraphicsConsole(){
	if (this->hProcessGraphics!=INVALID_HANDLE_VALUE){
		TerminateProcess(this->hProcessGraphics,0);
		CloseHandle(this->hProcessGraphics);
	}
}


int GraphicsConsole::initializeWindow(int width, int height){
	this->graphicArraySize = width*height;

	std::wstring randomWstr = gen_random_wstr(16);

	wcscpy(charInfoFileName, L"graphicsfile");

	std::wstring graphicConsoleArgumentsWstring = charInfoFileName;
	graphicConsoleArgumentsWstring += L" " + std::to_wstring(width) + L" " + std::to_wstring(height);

	WCHAR graphicConsoleArguments[sizeof(graphicConsoleArgumentsWstring)];

	wcscpy(graphicConsoleArguments, graphicConsoleArgumentsWstring.c_str());

	////////// INITIALIZE FILE MAPPING FOR GRAPHICS

	
	HANDLE hMapFile = CreateFileMappingW(
					INVALID_HANDLE_VALUE, // Use paging file
					NULL, // Default security
					PAGE_READWRITE,// Read-Write access
					0,// Allocate for all CHAR_INFO array (high-order DWORD)
					graphicArraySize*sizeof(CHAR_INFO),// Allocate for all CHAR_INFO array (low-order DWORD)
					charInfoFileName);// Name of mapping object
	
	if (hMapFile == NULL){
		std::cerr << "GraphicsConsole: CreateFileMappingW() Fail: " << GetLastError();
		return 0;
	}
	

	this->sharedGraphicsArray = (CHAR_INFO *) MapViewOfFile(hMapFile, // Handle to map object
					FILE_MAP_ALL_ACCESS, // Read/Write permission
					0,
					0, //Beggin at zero
					0); //Alloca

	if (sharedGraphicsArray == NULL){
		CloseHandle(hMapFile);
		std::cerr << "GraphicsConsole: MapViewOfFile() Fail: " << GetLastError();
		return 0;
	}

	////////// CREATE PIPE FOR SIGNALING WHEN TO REDRAW


	SECURITY_ATTRIBUTES saAttr;  // Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES); 
	saAttr.bInheritHandle = TRUE; 
	saAttr.lpSecurityDescriptor = NULL;

	if (! CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0)) {
		std::cerr << "GraphicsConsole: CreatePipe() Fail: " << GetLastError();
		return 0;
	}
	
   	if ( ! SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) ){
    	std::cerr << "GraphicsConsole: SetHandleInformation() Fail: " << GetLastError();
		return 0;
	}


	////////// CREATE GRAPHICS CONSOLE PROCESS


	//Structs used for process creation
	PROCESS_INFORMATION pi;
	ZeroMemory(&pi,sizeof(pi));

	STARTUPINFOW si;
	ZeroMemory(&si,sizeof(si));
	si.cb=sizeof(STARTUPINFO);
	si.hStdInput = g_hChildStd_IN_Rd; //Pass read end of pipe as std input.
	si.dwFlags |= STARTF_USESTDHANDLES;

	LPCWSTR graphicConsoleExecutableName = L"console_graphics_helper.exe";

	//Create graphics console process
	if(!CreateProcessW(graphicConsoleExecutableName, //Executable name
					   graphicConsoleArguments, //Arguments (shared memory name for graphics data)
					   NULL,//Default security descriptor
					   NULL,//No handle inheretance to thread
					   TRUE,//Inherit process handle 
					   CREATE_NEW_CONSOLE|CREATE_UNICODE_ENVIRONMENT, //New unicode console
					   NULL, //Use same enviorment as calling process
					   NULL, //Same current directory
					   &si, //Pass startup info
					   &pi))//Pass process info pointer. Data is written to pi
	{ 
		std::cerr << "GraphicsConsole: CreateProcess() Fail: " << GetLastError();
		return 0;
	}

	this->hProcessGraphics = pi.hProcess; //Wirte process handle

    this->startupInfo = si;

	this->width = width;
	this->height = height;

	CloseHandle(pi.hThread);

	return 1;
}

bool GraphicsConsole::updateConsole() { 

	//Writing any character on pipe causes a console screen buffer update

	DWORD dwWritten; 
	CHAR chBuf[1];
	chBuf[0] = 'x';
	BOOL bSuccess = FALSE;

	bSuccess = WriteFile(this->g_hChildStd_IN_Wr, chBuf, 1, &dwWritten, NULL);

	return bSuccess;
} 

int GraphicsConsole::getWidth(){
	return this->width;
}

int GraphicsConsole::getHeigth(){
	return this->height;
}

int GraphicsConsole::getGraphicArraySize(){
	return this->graphicArraySize;
}

bool GraphicsConsole::drawOnConsole(CHAR_INFO * graphicsArray){
	CopyMemory(this->sharedGraphicsArray, graphicsArray, this->graphicArraySize*sizeof(CHAR_INFO));

	return this->updateConsole();
}