// ToggleTop.h : main header file for the TOGGLETOP application
//

#if !defined(AFX_TOGGLETOP_H__0F6F63F9_5DB6_497A_8A6F_FFA86746C4D5__INCLUDED_)
#define AFX_TOGGLETOP_H__0F6F63F9_5DB6_497A_8A6F_FFA86746C4D5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

typedef struct _TOGGLETOP_INFO{
	struct _GENERAL{
		BOOL bAutoRun;
		BOOL bShowTip;
		BOOL bNoTray;
		BOOL bQueryEnd;
	} general;
	struct _HOTKEY{
		DWORD dwHotToggTop;
		DWORD dwHotShowBox;
	} hotkey;
	struct _CHECKVER{
		BOOL bAutoChkVer;
	} checkver;
} TOGGLETOP_INFO, *PTOGGLETOP_INFO;

#define WM_USER_NEWSESSION		WM_APP + 8049
/////////////////////////////////////////////////////////////////////////////
// CToggleTopApp:
// See ToggleTop.cpp for the implementation of this class
//

class CToggleTopApp : public CWinApp
{
public:
	static BOOL CALLBACK EnumWndProc(HWND hWnd, LPARAM lParam);
	BOOL WriteIniFile();
	BOOL ReadIniFile();
	CStdioFile m_sfProfile;
	CString m_strProfilePath;
	CString m_strAppPath;
	TOGGLETOP_INFO m_struIniInfo;
	BOOL InitData();
	CToggleTopApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToggleTopApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	CWnd m_wndInvisible; // parent of main wnd, for invisible the main window

	//{{AFX_MSG(CToggleTopApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOGGLETOP_H__0F6F63F9_5DB6_497A_8A6F_FFA86746C4D5__INCLUDED_)
