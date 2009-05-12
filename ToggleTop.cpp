// ToggleTop.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ToggleTop.h"
#include "ToggleTopMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CToggleTopApp

BEGIN_MESSAGE_MAP(CToggleTopApp, CWinApp)
//{{AFX_MSG_MAP(CToggleTopApp)
// NOTE - the ClassWizard will add and remove mapping macros here.
//    DO NOT EDIT what you see in these blocks of generated code!
//}}AFX_MSG
ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToggleTopApp construction

CToggleTopApp::CToggleTopApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CToggleTopApp object

CToggleTopApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CToggleTopApp initialization

BOOL CToggleTopApp::InitInstance()
{
  try{

	// ��ֹӦ�ó������μ���
	BOOL bAlreadyRunning;
	
	::CreateMutex( NULL, FALSE, // FALSE so we need NOT free
		_T("ToggleTop-BD87060C-CF27-4b0f-A6A5-163B6280A9FC"));
	
	// what changes for the alternative solutions
	// is the UID in the above call
	// which will be replaced by a call on
	// createExclusionName
	
	bAlreadyRunning = ( ::GetLastError() == ERROR_ALREADY_EXISTS || 
							::GetLastError() == ERROR_ACCESS_DENIED);
	// The call fails with ERROR_ACCESS_DENIED if the Mutex was 
	// created in a different users session because of passing
	// NULL for the SECURITY_ATTRIBUTES on Mutex creation);
	
	if ( bAlreadyRunning )
	{ 
		HWND hOther = NULL;
		EnumWindows( EnumWndProc, (LPARAM)&hOther );
		
		if ( hOther == NULL )
			ASSERT( 0 ); // ���û���ҵ�ǰһ����, Ӧ�������������...
		
		return FALSE; // terminates the creation
	}
	
	// ��ʼ�� Socket
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	
	// ��ʼ�� OLE
	AfxEnableControlContainer();
	
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// ��ʼ������
	if( !InitData() )
		return FALSE;

	// ȡ����ʾ��������ť
	// Create invisible window so we can have a icon and text
	// for the taskbar's window button
	CWnd *pWnd = NULL;
	if (!::IsWindow(m_wndInvisible.m_hWnd))
	{
		LPCTSTR pstrOwnerClass = AfxRegisterWndClass(0);
		if (
			m_wndInvisible.CreateEx( 0, pstrOwnerClass, _T(""), WS_POPUP,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, 0)
		   )
		{
			pWnd = &m_wndInvisible;
		}

		// If it is not created, leave it NULL, it will apear in the taskbar: Big deal, will never happen
	}

	// �����ڹ���
	CToggleTopMainDlg dlgMain( pWnd );
	m_pMainWnd = &dlgMain;
	dlgMain.DoModal();
	
	// ����������Ϣ
	WriteIniFile();
	
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;

  }catch(...){
	DWORD dwErr = GetLastError();
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
		);
	// Process any inserts in lpMsgBuf.
	CString strInfo;
	strInfo.Format( _T("����δԤ�ڵĴ���, ������Ϣ:\n\nID: 0x%x (%d)\n����: %s\n\nToggleTop �˳�!"),
					dwErr, dwErr, lpMsgBuf );

	// Display the string.
	AfxMessageBox( strInfo, MB_OK | MB_ICONERROR );
	// Free the buffer.
	LocalFree( lpMsgBuf );

	return FALSE;
  }
}

BOOL CToggleTopApp::InitData()
{
	// ��ʼ�������ļ�·��
	m_strProfilePath = m_pszHelpFilePath;
	m_strProfilePath.Delete( m_strProfilePath.GetLength() - 4, 4 );
	m_strAppPath = m_strProfilePath;
	m_strProfilePath += _T(".ini");
	m_strAppPath += _T(".exe");
	
	
	// װ��Ĭ������
	ZeroMemory( &m_struIniInfo, sizeof(TOGGLETOP_INFO) );
	m_struIniInfo.general.bAutoRun = TRUE;
	m_struIniInfo.general.bShowTip = TRUE;
	m_struIniInfo.general.bQueryEnd = TRUE;
	m_struIniInfo.hotkey.dwHotToggTop = VK_F11;
	m_struIniInfo.hotkey.dwHotShowBox = (DWORD)MAKELONG( VK_F11, HOTKEYF_CONTROL );
	
	// ��ʼ������
	BOOL bRet = m_sfProfile.Open( m_strProfilePath,
		CFile::modeReadWrite | CFile::shareDenyWrite );
	if( bRet )
	{// �ļ��򿪳ɹ�
		m_sfProfile.Close();
		// ��ȡ��ʼ������
		if( !ReadIniFile() )
		{
			AfxMessageBox( _T("��ȡ�����ļ�ʧ��."), MB_OK | MB_ICONERROR );
			return FALSE;
		}
	}
	else
	{// �ļ���ʧ��
		bRet = m_sfProfile.Open( m_strProfilePath,
			CFile::modeReadWrite | CFile::shareDenyWrite | CFile::modeCreate );
		
		if( !bRet )
		{// �ļ�����ʧ��
			AfxMessageBox( _T("�����ļ�����ʧ��."), MB_OK | MB_ICONERROR );
			return FALSE;
		}
		
		m_sfProfile.SeekToBegin();
		m_sfProfile.WriteString( _T(";\n; ToggleTop config file, please do NOT edit.\n;\n\n") );
		m_sfProfile.Close();
		
		// �ļ������ɹ�, д��Ĭ������
		if( !WriteIniFile() )
		{
			AfxMessageBox( _T("д�����ļ�ʧ��."), MB_OK | MB_ICONERROR );
			return FALSE;
		}
	}
	
	return TRUE;
}

BOOL CToggleTopApp::ReadIniFile()
{
	m_struIniInfo.general.bAutoRun = 
		GetPrivateProfileInt( _T("General"), _T("AutoRun"), TRUE, m_strProfilePath );
	m_struIniInfo.general.bShowTip =
		GetPrivateProfileInt( _T("General"), _T("ShowTip"), TRUE, m_strProfilePath );
	m_struIniInfo.general.bNoTray =
		GetPrivateProfileInt( _T("General"), _T("NoTray"), FALSE, m_strProfilePath );
	m_struIniInfo.general.bQueryEnd =
		GetPrivateProfileInt( _T("General"), _T("QueryEnd"), TRUE, m_strProfilePath );
	
	m_struIniInfo.hotkey.dwHotToggTop = 
		GetPrivateProfileInt( _T("Hotkey"), _T("ToggTop"), VK_F11, m_strProfilePath );
	m_struIniInfo.hotkey.dwHotShowBox =
		GetPrivateProfileInt( _T("Hotkey"), _T("ShowBox"),
		(DWORD)MAKELONG( VK_F11, HOTKEYF_CONTROL ), m_strProfilePath );
	
	m_struIniInfo.checkver.bAutoChkVer = 
		GetPrivateProfileInt( _T("CheckVer"), _T("AutoChkVer"), FALSE, m_strProfilePath );
	
	// ���ݺϷ���У��
	if(m_struIniInfo.hotkey.dwHotToggTop == m_struIniInfo.hotkey.dwHotShowBox)
	{
		m_struIniInfo.hotkey.dwHotToggTop = VK_F11;
		m_struIniInfo.hotkey.dwHotShowBox = (DWORD)MAKELONG( VK_F11, HOTKEYF_CONTROL );

		AfxMessageBox( _T("�����ļ����ݴ���, �������Զ��޸�.") );
	}

	return TRUE;
}

BOOL CToggleTopApp::WriteIniFile()
{
	CString strTmp;
	strTmp.Format( _T("%d"), m_struIniInfo.general.bAutoRun );
	WritePrivateProfileString( _T("General"), _T("AutoRun"), strTmp, m_strProfilePath );
	strTmp.Format( _T("%d"), m_struIniInfo.general.bShowTip );
	WritePrivateProfileString( _T("General"), _T("ShowTip"), strTmp, m_strProfilePath );
	strTmp.Format( _T("%d"), m_struIniInfo.general.bNoTray );
	WritePrivateProfileString( _T("General"), _T("NoTray"), strTmp, m_strProfilePath );
	strTmp.Format( _T("%d"), m_struIniInfo.general.bQueryEnd );
	WritePrivateProfileString( _T("General"), _T("QueryEnd"), strTmp, m_strProfilePath );
	
	strTmp.Format( _T("%d"), m_struIniInfo.hotkey.dwHotToggTop );
	WritePrivateProfileString( _T("Hotkey"), _T("ToggTop"), strTmp, m_strProfilePath );
	strTmp.Format( _T("%d"), m_struIniInfo.hotkey.dwHotShowBox );
	WritePrivateProfileString( _T("Hotkey"), _T("ShowBox"), strTmp, m_strProfilePath );
	
	strTmp.Format( _T("%d"), m_struIniInfo.checkver.bAutoChkVer );
	WritePrivateProfileString( _T("CheckVer"), _T("AutoChkVer"), strTmp, m_strProfilePath );
	
	return TRUE;
}

BOOL CALLBACK CToggleTopApp::EnumWndProc(HWND hWnd, LPARAM lParam)
{
	DWORD dwRet;
	LRESULT lRet = ::SendMessageTimeout(hWnd,
		WM_USER_NEWSESSION,
		0, 0, 
		SMTO_BLOCK | SMTO_ABORTIFHUNG,
		200,
		&dwRet);
	
	if(lRet == 0)
        return TRUE; // ignore this and continue
	
	if(dwRet == WM_USER_NEWSESSION)
	{ // found it
		HWND * target = (HWND *)lParam;
		*target = hWnd;
		return FALSE; // stop search
	}
	
	return TRUE; // continue search
}
