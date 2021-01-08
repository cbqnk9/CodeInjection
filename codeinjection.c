#include <stdio.h>
#include <Windows.h>

typedef UINT(WINAPI* WINEXEC)(LPCSTR, UINT);  //함수 포인터


typedef struct {
	char buf[15];
	WINEXEC pFunc;
}INJECT_DATA;

void WINAPI ThreadProc(INJECT_DATA* threadinject) {
	WINEXEC pFunc = threadinject->pFunc;
	((WINEXEC)pFunc)(threadinject->buf, SW_SHOWDEFAULT);
}

void AfterFunc() {};

void codeinjection(DWORD PID) {
	HMODULE hMod;
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	PVOID ThreadProc_Address = NULL;
	PVOID Inject_Data_Address = NULL;
	INJECT_DATA InjectData;

	//ThreadProc Size
	DWORD ThreadProc_Size = (DWORD)AfterFunc - (DWORD)ThreadProc;

	hMod = GetModuleHandleA("kernel32.dll");

	//Target Proccess Handle
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);

	//Target Process에 ThreadProc_Size 만큼 Memory Allocate, Address 반환
	ThreadProc_Address = VirtualAllocEx(hProcess, NULL, ThreadProc_Size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(hProcess, ThreadProc_Address, (LPVOID)ThreadProc, ThreadProc_Size, NULL);
	
	//messagebox
	sprintf(InjectData.buf, "messagebox.exe\0");

	//kernel32.dll 모듈의 WinExec 함수의 주소를 INJECT_DATA 구조체의 함수포인터로 입력
	InjectData.pFunc = (WINEXEC)GetProcAddress(GetModuleHandleA("kernel32.dll"), "WinExec");

	//print("%x", InjectData.pFunc)
	
	Inject_Data_Address = VirtualAllocEx(hProcess, NULL, sizeof(InjectData), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(hProcess, Inject_Data_Address, (LPVOID)&InjectData, sizeof(InjectData), NULL);

	hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProc_Address, Inject_Data_Address, CREATE_SUSPENDED, NULL);

	ResumeThread(hThread);
	WaitForSingleObject(hThread, INFINITE);
}

int main() {
	int getPID = 0;

	printf("PID :");
	scanf("%d", &getPID);

	codeinjection((DWORD)getPID);

	return 0;
}
