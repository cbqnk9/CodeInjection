#include <stdio.h>
#include <Windows.h>

typedef UINT(WINAPI* WINEXEC)(LPCSTR, UINT);  //함수 포인터


//Thread Parameter
typedef struct _THREAD_PARAM
{
	FARPROC pFunc[2]; //LoadLibraryA(), GetProcAddress()
	char szBuf[4][128]; //"user32.dll", "MessageBoxA",
} THREAD_PARAM, * PTHREAD_PARAM;

// LoadLibraryA()
typedef HMODULE(WINAPI* PFLOADLIBRARYA)
(
	LPCSTR lpLibFileName
	);

//GetProcAddress()
typedef FARPROC(WINAPI* PFGETPROCADDRESS)
(
	HMODULE hModule,
	LPCSTR lpProcName
	);

//MessageBoxA()
typedef int (WINAPI* PFMESSAGEBOXA)
(
	HWND hWnd,
	LPCSTR lpText,
	LPCSTR lpCaption,
	UINT uType
	);

DWORD WINAPI ThreadProc(LPVOID lParam);
BOOL InjectCode(DWORD dwPID);