#ifndef WINGUI_H
#define WINGUI_H

#include <windows.h>
//#include "BasicTypes.h"
//
//typedef HDC		M_GraphicPort;
//typedef HRGN	M_Region;
//extern HINSTANCE	ghDMRInstance;
//extern HWND	gDMRWin;

//For converting Win32 Messages to M_KeyModifiers
/*void M_GetCoordinates(long lParam, M_Point &thePoint);
void M_GetWheelMouseData(ulong wParam, M_KeyModifier &theModifiers, short &theDelta);


//For converting M_* objects to Win32 objects
void M_GetM_Point(M_Point &thePt, POINT &winPoint);
void M_GetNativePoint(POINT &thePt, M_Point &theM_Point);
void M_GetNativeColor(COLORREF &nativeColor, M_Color &theColor);
void M_GetM_Color(M_Color &theColor, COLORREF &nativeColor);
void M_GetM_Rect(M_Rect &theRect, RECT &winRect);
void M_GetNativeRect(RECT &winRect, M_Rect &theRect);
*/

//Call this when you think you have a windows error
//#LA defined this to do nothing for now
//extern void WindowsError(DWORD err, UInt32 location);


//Debug code
#define DEBUG_LEVEL 2

void _trace(char *dbg);
void _trace(char *dbg, int val);

#ifdef _DEBUG
	#if DEBUG_LEVEL == 5
		#define TRACE1(x)     (void(_trace(x)))
		#define TRACE1EX(x,y) (void(_trace(x,y)))
		#define TRACE2(x)     (void(_trace(x)))
		#define TRACE2EX(x,y) (void(_trace(x,y)))
		#define TRACE3(x)     (void(_trace(x)))
		#define TRACE3EX(x,y) (void(_trace(x,y)))
		#define TRACE4(x)     (void(_trace(x)))
		#define TRACE4EX(x,y) (void(_trace(x,y)))
		#define TRACE5(x)     (void(_trace(x)))
		#define TRACE5EX(x,y) (void(_trace(x,y)))
	#elif DEBUG_LEVEL == 4
		#define TRACE1(x)     (void(_trace(x)))
		#define TRACE1EX(x,y) (void(_trace(x,y)))
		#define TRACE2(x)     (void(_trace(x)))
		#define TRACE2EX(x,y) (void(_trace(x,y)))
		#define TRACE3(x)     (void(_trace(x)))
		#define TRACE3EX(x,y) (void(_trace(x,y)))
		#define TRACE4(x)     (void(_trace(x)))
		#define TRACE4EX(x,y) (void(_trace(x,y)))
		#define TRACE5(x)     (void(0))
		#define TRACE5EX(x,y) (void(0))
	#elif DEBUG_LEVEL == 3
		#define TRACE1(x)     (void(_trace(x)))
		#define TRACE1EX(x,y) (void(_trace(x,y)))
		#define TRACE2(x)     (void(_trace(x)))
		#define TRACE2EX(x,y) (void(_trace(x,y)))
		#define TRACE3(x)     (void(_trace(x)))
		#define TRACE3EX(x,y) (void(_trace(x,y)))
		#define TRACE4(x)     (void(0))
		#define TRACE4EX(x,y) (void(0))
		#define TRACE5(x)     (void(0))
		#define TRACE5EX(x,y) (void(0))
	#elif DEBUG_LEVEL == 2
		#define TRACE1(x)     (void(_trace(x)))
		#define TRACE1EX(x,y) (void(_trace(x,y)))
		#define TRACE2(x)     (void(_trace(x)))
		#define TRACE2EX(x,y) (void(_trace(x,y)))
		#define TRACE3(x)     (void(0))
		#define TRACE3EX(x,y) (void(0))
		#define TRACE4(x)     (void(0))
		#define TRACE4EX(x,y) (void(0))
		#define TRACE5(x)     (void(0))
		#define TRACE5EX(x,y) (void(0))
	#else
		#define TRACE1(x)     (void(_trace(x)))
		#define TRACE1EX(x,y) (void(_trace(x,y)))
		#define TRACE2(x)     (void(0))
		#define TRACE2EX(x,y) (void(0))
		#define TRACE3(x)     (void(0))
		#define TRACE3EX(x,y) (void(0))
		#define TRACE4(x)     (void(0))
		#define TRACE4EX(x,y) (void(0))
		#define TRACE5(x)     (void(0))
		#define TRACE5EX(x,y) (void(0))
	#endif
#else
	#define TRACE1(x)     (void(0))
	#define TRACE1EX(x,y) (void(0))
	#define TRACE2(x)     (void(0))
	#define TRACE2EX(x,y) (void(0))
	#define TRACE3(x)     (void(0))
	#define TRACE3EX(x,y) (void(0))
	#define TRACE4(x)     (void(0))
	#define TRACE4EX(x,y) (void(0))
	#define TRACE5(x)     (void(0))
	#define TRACE5EX(x,y) (void(0))
#endif


#ifdef _DEBUG
#define TRACE(x)     (void(_trace(x)))
#define TRACEEX(x,y) (void(_trace(x,y)))
#else
#define TRACE(x)     (void(0))
#define TRACEEX(x,y) (void(0))
#endif


////extern HINSTANCE	gApplicationInstance;
////extern HWND 		gWin32Window;
//
//
//#define	WM_APP_TRAY_NOTIFY    ( WM_APP + 10 )
//#define WM_APP_CUSTOM_GUI_MSG ( WM_APP + 11 )
//#define WM_BUCKETNETDATAIN	  ( WM_APP + 12 )
//#define WM_BUCKETNETDATAOUT	  ( WM_APP + 13 )
//#define WM_FLASHDEBUGINFO	  ( WM_APP + 14 )
//#define WM_VERSIONINFOREMOTE  ( WM_APP + 300 )
//#define WM_UPDATEPARAMEDITMSGCOUNT ( WM_APP + 400 )
//#define WM_UPDATEPARAMEDITVALUECOUNT ( WM_APP + 401 )
//#define WM_UPDATEBACKUPRESTORESTATUS (WM_APP + 402)
//#define WM_UPDATECONSTAT			 (WM_APP + 500)
//
//
//
//#ifdef _DEBUG
////LRESULT CALLBACK XD2DebugDlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
//#else
////LRESULT CALLBACK UserFlashProcedureDlgProc(HWND hWndDlg, UINT Msg, WPARAM wParam, LPARAM lParam);
//#endif
//
//void OpenFlashProcedureDialog(void);
//void DoneFlashProcedure(void);
//void M_UpdateFlashStatusBar(float percent, char *message, bool doStatusBar, bool doMessage, bool doMulletMsg);
//void M_PrintFlashDebugString(char *text);
//void CloseFlashStatusDialog(void);
//void OpenFlashStatusDialog(void);

void WindowsError(DWORD err, int location);

#endif //WINGUI_H
