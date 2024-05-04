#include <iostream>
#include <Windows.h>
#include <string>
#include <thread>
#include <libloaderapi.h>

using namespace std;

void getProcId(const char* windowTitle, DWORD& procId)
{
	GetWindowThreadProcessId(FindWindow(NULL, windowTitle), &procId);
}

void error(const char* errTitle, const char* errMsg)
{
	MessageBox(NULL, errMsg, errTitle, NULL);
	exit(-1);
}

bool fileExists(string fileName)
{
	struct stat buffer;
	return (stat(fileName.c_str(), &buffer) == 0);
}



int main()
{
	DWORD procId = NULL;
	char dllPath[MAX_PATH];
	const char* dllName = "DMM.dll"; // Enter your DLL file name
	const char* windowTitle = "Untitled - Paint"; // Enter your program title

	if (!fileExists(dllName))
	{
		error("fileExists", "Failed");
	}

	if (GetFullPathName(dllName, MAX_PATH, dllPath, nullptr))
	{
		error("GetFullPathName", "Failed");
	}

	getProcId(windowTitle, procId);
	if (procId == NULL)
	{
		error("getProcId", "Failed");
	}

	HANDLE H_proc = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	if (!H_proc)
	{
		error("OpenProcess", "Failed");
	}

	void* allocatedMemory = VirtualAllocEx(H_proc, nullptr, MAX_PATH, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	if (!allocatedMemory)
	{
		error("VirtualAllocEx", "Failed");
	}
	if (!WriteProcessMemory(H_proc, nullptr, dllPath, MAX_PATH, nullptr))
	{
		error("WriteProcessMemory", "Failed");
	}

	HANDLE H_Thread = CreateRemoteThread(H_proc, nullptr, NULL, LPTHREAD_START_ROUTINE(LoadLibraryA), allocatedMemory, NULL, nullptr);
	if (!H_Thread)
	{
		error("H_Thread", "Failed");
	}

	CloseHandle(H_proc);
	VirtualFreeEx(H_proc, allocatedMemory, NULL, MEM_RELEASE);
	MessageBox(0, "Injected", "Injected", 0);
}
