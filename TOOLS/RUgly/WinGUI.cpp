#include <windows.h>
#include <cstdio>
#include <string.h>
#include <winbase.h>
#include "WinGUI.h"

void _trace(char *dbg)
{
	char output[384];

	if(strlen(dbg) > 256)
		dbg[255] = 0;

	sprintf(output, "TRACE: %i - %i - %s\r\n", GetCurrentThreadId(), GetTickCount(), dbg);
	OutputDebugString(output);
}

void _trace(char *dbg, int val)
{
	char output[384];

	if(strlen(dbg) > 256)
		dbg[255] = 0;

	sprintf(output, "TRACE: %i - %i - %s %i\r\n", GetCurrentThreadId(), GetTickCount(), dbg, val);
	OutputDebugString(output);
}


void WindowsError(DWORD err, int location)
#ifdef _DEBUG
{
	if(err)
	{
		char errMsg[256];
		sprintf(errMsg, "General Error, Please report to Mackie Tech Support (%i,%i)", err, location);
		MessageBox(NULL, errMsg, "General Error", MB_OK);
	}
}
#else
{}
#endif
