// DlgSetting.cpp : implementation file
//

#include "stdafx.h"
#include "toggletop.h"
#include "ToggleTopMainDlg.h"
#include "DlgSetting.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CToggleTopApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CDlgSetting dialog

CDlgSetting::CDlgSetting(CWnd *pParent /*=NULL*/)
	: CDialog(CDlgSetting::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgSetting)
	m_bAutoChkVer = FALSE;
	m_bAutoRun = FALSE;
	m_bNoTray = FALSE;
	m_bQueryEnd = FALSE;
	m_bShowTip = FALSE;
	//}}AFX_DATA_INIT
}

void CDlgSetting::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgSetting)
	DDX_Control(pDX, IDC_HOTKEY_TOGGTOP, m_hotToggTop);
	DDX_Control(pDX, IDC_HOTKEY_SHOWBOX, m_hotShowWnd);
	DDX_Check(pDX, IDC_CHKBTN_AUTOCHKVER, m_bAutoChkVer);
	DDX_Check(pDX, IDC_CHKBTN_AUTORUN, m_bAutoRun);
	DDX_Check(pDX, IDC_CHKBTN_NOTRAY, m_bNoTray);
	DDX_Check(pDX, IDC_CHKBTN_QUERYEND, m_bQueryEnd);
	DDX_Check(pDX, IDC_CHKBTN_SHOWTIP, m_bShowTip);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgSetting, CDialog)
//{{AFX_MSG_MAP(CDlgSetting)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgSetting message handlers

void CDlgSetting::InitSetting(PNOTIFYICONDATA pNID)
{
	m_bAutoChkVer = theApp.m_struIniInfo.checkver.bAutoChkVer;

	m_bShowTip = theApp.m_struIniInfo.general.bShowTip;
	m_bQueryEnd = theApp.m_struIniInfo.general.bQueryEnd;

	m_bAutoRun = theApp.m_struIniInfo.general.bAutoRun;
	if (!SetAutoRun(m_bAutoRun))
	{
		ASSERT(0);
	}

	m_bNoTray = theApp.m_struIniInfo.general.bNoTray;
	if (!m_bNoTray)
	{
		if (!Shell_NotifyIcon(NIM_ADD, pNID))
			ASSERT(0);
	}

	DWORD dwHotKey = theApp.m_struIniInfo.hotkey.dwHotToggTop;
	if (!RegisterHotKey(pNID->hWnd, IDHOT_SETTOP,
						HIWORD(dwHotKey), LOWORD(dwHotKey)))
	{
		AfxMessageBox(_T("注册第一系统热键失败."), MB_OK | MB_ICONERROR);
	}
	else
		m_hotToggTop.SetHotKey(LOWORD(dwHotKey), HIWORD(dwHotKey));

	dwHotKey = theApp.m_struIniInfo.hotkey.dwHotShowBox;
	if (!RegisterHotKey(pNID->hWnd, IDHOT_SHOWBOX,
						HIWORD(dwHotKey), LOWORD(dwHotKey)))
	{
		AfxMessageBox(_T("注册第二系统热键失败."), MB_OK | MB_ICONERROR);
	}
	else
		m_hotShowWnd.SetHotKey(LOWORD(dwHotKey), HIWORD(dwHotKey));

	UpdateData(FALSE);
}

BOOL CDlgSetting::ApplySetting(PNOTIFYICONDATA pNID)
{
	UpdateData();

	// 校验数据合法性
	WORD wHotToggTopH, wHotToggTopL, wHotShowWndH, wHotShowWndL;
	m_hotToggTop.GetHotKey(wHotToggTopL, wHotToggTopH);
	DWORD dwHotToggTop = (DWORD)MAKELONG(wHotToggTopL, wHotToggTopH);
	m_hotShowWnd.GetHotKey(wHotShowWndL, wHotShowWndH);
	DWORD dwHotShowWnd = (DWORD)MAKELONG(wHotShowWndL, wHotShowWndH);

	if (dwHotToggTop == dwHotShowWnd)
	{
		AfxMessageBox(_T("热键不能重复."), MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	// 应用自动更新设置
	theApp.m_struIniInfo.checkver.bAutoChkVer = m_bAutoChkVer;

	// 应用一般设置
	theApp.m_struIniInfo.general.bShowTip = m_bShowTip;
	theApp.m_struIniInfo.general.bQueryEnd = m_bQueryEnd;

	if (theApp.m_struIniInfo.general.bAutoRun != m_bAutoRun)
	{
		if (!SetAutoRun(m_bAutoRun))
		{
			m_bAutoRun = theApp.m_struIniInfo.general.bAutoRun;
			UpdateData(FALSE);
			return FALSE;
		}

		theApp.m_struIniInfo.general.bAutoRun = m_bAutoRun;
	}

	if (theApp.m_struIniInfo.general.bNoTray != m_bNoTray)
	{
		if (!Shell_NotifyIcon((m_bNoTray ? NIM_DELETE : NIM_ADD), pNID))
		{
			AfxMessageBox(_T("设置任务栏图标失败."), MB_OK | MB_ICONERROR);
			m_bShowTip = theApp.m_struIniInfo.general.bNoTray;
			UpdateData(FALSE);
			return FALSE;
		}

		theApp.m_struIniInfo.general.bNoTray = m_bNoTray;
	}

	// 应用热键设置
	if ((dwHotToggTop != 0) &&
		(theApp.m_struIniInfo.hotkey.dwHotToggTop != dwHotToggTop))
	{
		if (!RegisterHotKey(pNID->hWnd, IDHOT_SETTOP,
							HIWORD(dwHotToggTop), LOWORD(dwHotToggTop)))
		{
			AfxMessageBox(_T("注册第一系统热键失败."), MB_OK | MB_ICONERROR);
			m_hotToggTop.SetHotKey(
				LOWORD(theApp.m_struIniInfo.hotkey.dwHotToggTop),
				HIWORD(theApp.m_struIniInfo.hotkey.dwHotToggTop));

			return FALSE;
		}

		theApp.m_struIniInfo.hotkey.dwHotToggTop = dwHotToggTop;
	}

	if ((dwHotShowWnd != 0) &&
		(theApp.m_struIniInfo.hotkey.dwHotShowBox != dwHotShowWnd))
	{
		if (!RegisterHotKey(pNID->hWnd, IDHOT_SHOWBOX,
							HIWORD(dwHotShowWnd), LOWORD(dwHotShowWnd)))
		{
			AfxMessageBox(_T("注册第二系统热键失败."), MB_OK | MB_ICONERROR);
			m_hotShowWnd.SetHotKey(
				LOWORD(theApp.m_struIniInfo.hotkey.dwHotShowBox),
				HIWORD(theApp.m_struIniInfo.hotkey.dwHotShowBox));

			return FALSE;
		}

		theApp.m_struIniInfo.hotkey.dwHotShowBox = dwHotShowWnd;
	}

	return TRUE;
}

BOOL CDlgSetting::SetAutoRun(BOOL bFlag)
{
	HKEY hKey = NULL;
	LRESULT lRet = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
								  _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run"), 0L, NULL,
								  REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, NULL,
								  &hKey, NULL);

	if (ERROR_SUCCESS != lRet)
	{
		AfxMessageBox(_T("访问注册表失败."), MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (bFlag)
	{
		lRet = RegSetValueEx(hKey, _T("ToggleTop"), 0L,
							 REG_SZ, (CONST BYTE *)(LPCTSTR)theApp.m_strAppPath,
							 theApp.m_strAppPath.GetLength() + sizeof(TCHAR));
	}
	else
	{
		/*lRet = */ RegDeleteValue(hKey, _T("ToggleTop"));
	}

	if (ERROR_SUCCESS != lRet)
	{
		AfxMessageBox(_T("写注册表失败."), MB_OK | MB_ICONERROR);
	}

	RegCloseKey(hKey);
	return (ERROR_SUCCESS == lRet);
}
