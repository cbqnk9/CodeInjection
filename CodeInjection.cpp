#include "CodeInhjection.h"

int main(int argc, char* argv[])
{
	DWORD dwPID = 0;
	int getPID = 0;

	printf("PID :");
	scanf("%d", &getPID);
	dwPID = (DWORD)getPID;
	InjectCode(dwPID); // InjectCode함수 호출

	return 0;
}


DWORD WINAPI ThreadProc(LPVOID lParam)
{
	PTHREAD_PARAM pParam = (PTHREAD_PARAM)lParam;
	HMODULE hMod = NULL;
	FARPROC pFunc = NULL;

	hMod = ((PFLOADLIBRARYA)pParam->pFunc[0])(pParam->szBuf[0]);

	pFunc = (FARPROC)((PFGETPROCADDRESS)pParam->pFunc[1])(hMod, pParam->szBuf[1]);

	((PFMESSAGEBOXA)pFunc)(NULL, pParam->szBuf[2], pParam->szBuf[3], MB_OK);

	return 0;
}

void AfterFunc() {};

BOOL InjectCode(DWORD dwPID)
{
	HMODULE hMod = NULL;
	THREAD_PARAM param = { 0, };
	HANDLE hProcess = NULL;
	HANDLE hThread = NULL;
	LPVOID pRemoteBuf[2] = { 0, };
	DWORD dwSize = 0;

	hMod = GetModuleHandleA("kernel32.dll");

	// set THREAD_PARAM
	param.pFunc[0] = GetProcAddress(hMod, "LoadLibraryA");
	param.pFunc[1] = GetProcAddress(hMod, "GetProcAddress");
	printf("%x", param.pFunc);
	strcpy(param.szBuf[0], "user32.dll");
	strcpy(param.szBuf[1], "MessageBoxA");
	strcpy(param.szBuf[2], "Swing!!!");
	strcpy(param.szBuf[3], "CodeInjection");

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);

	// Allocation for THREAD_PARAM
	dwSize = sizeof(THREAD_PARAM);
	pRemoteBuf[0] = VirtualAllocEx(hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE);

	WriteProcessMemory(hProcess,pRemoteBuf[0],(LPVOID)&param,dwSize,NULL); 

	//Allocation for ThreadProc()
	dwSize = (DWORD)InjectCode - (DWORD)ThreadProc;
	pRemoteBuf[1] = VirtualAllocEx(hProcess,NULL,dwSize,MEM_COMMIT,PAGE_EXECUTE_READWRITE);

	WriteProcessMemory(hProcess,pRemoteBuf[1],(LPVOID)ThreadProc,dwSize,NULL);
			  
	hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pRemoteBuf[1],pRemoteBuf[0],0,NULL);

	WaitForSingleObject(hThread, INFINITE);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}