// ToggleTopMainDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ToggleTop.h"
#include "ToggleTopMainDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CToggleTopApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CToggleTopMainDlg dialog

CToggleTopMainDlg::CToggleTopMainDlg(CWnd *pParent /*=NULL*/)
	: CDialog(CToggleTopMainDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CToggleTopMainDlg)
	m_bHelp = FALSE;
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(m_pFilterWnd, sizeof(HWND) * TT_FILTERWND_NUM);
	m_pwndHelp = NULL;
	ZeroMemory(&m_struNID, sizeof(NOTIFYICONDATA));
}

void CToggleTopMainDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CToggleTopMainDlg)
	DDX_Check(pDX, IDC_CHKBTN_HELP, m_bHelp);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CToggleTopMainDlg, CDialog)
//{{AFX_MSG_MAP(CToggleTopMainDlg)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDC_CHKBTN_HELP, OnHelp)
ON_BN_CLICKED(IDC_BTN_ABOUT, OnAbout)
ON_BN_CLICKED(IDC_BTN_CHECKVER, OnCheckVer)
ON_BN_CLICKED(IDC_BTN_SAVESETTING, OnSetting)
ON_COMMAND(ID_MENUITEM_EXIT, OnExit)
ON_COMMAND(ID_MENUITEM_ABOUT, OnAbout)
ON_COMMAND(ID_MENUITEM_SHOWHIDE, OnShowHide)
//}}AFX_MSG_MAP
ON_MESSAGE(WM_USER_TRAYNOTIFY, OnTrayNotify)
ON_MESSAGE(WM_USER_NEWSESSION, OnNewSession)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CToggleTopMainDlg message handlers
CBitmap bmpAbout;

BOOL CToggleTopMainDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 更改系统菜单
	CMenu *pSysMenu = GetSystemMenu(FALSE);

	if (pSysMenu != NULL)
	{
		// 修改 "退出" 系统菜单
		ASSERT((IDM_EXIT & 0xFFF0) == IDM_EXIT);
		ASSERT(IDM_EXIT < 0xF000);
		int nMenuCount = pSysMenu->GetMenuItemCount();
		if (nMenuCount > 0)
		{
			MENUITEMINFO menuinfo;
			menuinfo.cbSize = sizeof(menuinfo);
			menuinfo.fMask = MIIM_ID;
			menuinfo.wID = IDM_EXIT;

			::SetMenuItemInfo(pSysMenu->m_hMenu, SC_CLOSE, FALSE, &menuinfo);
			pSysMenu->SetDefaultItem(IDM_EXIT);
		}

		// Add "About..." menu item to system menu.
		// IDM_ABOUTBOX must be in the system command range.
		ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
		ASSERT(IDM_ABOUTBOX < 0xF000);

		CString strAddMenu;
		strAddMenu.LoadString(IDS_ABOUTBOX);
		if (!strAddMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAddMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);  // Set big icon
	SetIcon(m_hIcon, FALSE); // Set small icon

	// TODO: Add extra initialization here
	// 初始化任务栏图标数据
	m_struNID.cbSize = sizeof(NOTIFYICONDATA);
	m_struNID.hWnd = this->m_hWnd;
	m_struNID.uID = IDR_MAINFRAME;
	m_struNID.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	m_struNID.uCallbackMessage = WM_USER_TRAYNOTIFY;
	m_struNID.hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	_tcscpy(m_struNID.szTip, _T("ToggleTop"));

	// 初始化设置窗口
	m_dlgSetting.Create(m_dlgSetting.IDD, this);
	m_dlgSetting.m_pParentWnd = this;
	m_dlgSetting.InitSetting(&m_struNID);
	// 初始化设置窗口位置
	CWnd *pwndPlace = GetDlgItem(IDC_PLACEHOLDER);
	ASSERT_VALID(pwndPlace);
	CRect rect;
	pwndPlace->GetWindowRect(rect);
	ScreenToClient(rect);
	m_dlgSetting.SetWindowPos(NULL, rect.left, rect.top, 0, 0,
							  SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);

	// 初始化帮助窗口
	InitHelpWnd();

	// 初始化提示窗口
	//	m_wndTip.Create( this ); // 不在这里初始化因为设置了窗口Rgn后,无法再移动窗口

	GetFilterWnd();

	return TRUE; // return TRUE  unless you set the focus to a control
}

void CToggleTopMainDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if (SC_MAXIMIZE == nID)
		return; // 禁止最大化
	else if (SC_MINIMIZE == nID || SC_RESTORE == nID)
	{ // 改变最小化和恢复的标题栏动画
		CRect rcBig(0, 0, 0, 0);
		CRect rcSmall(0, 0, 112, 27); // 最小化窗口的大小，由 Spy++ 得到

		GetWindowRect(rcBig);
		CRect rcTmp;
		CWnd *pWnd = GetDesktopWindow();
		pWnd->GetWindowRect(rcTmp);
		rcSmall.OffsetRect(rcTmp.BottomRight() - rcSmall.BottomRight());

		if (SC_MINIMIZE == nID)
		{ // 最小化
			DrawAnimatedRects(m_hWnd, IDANI_CAPTION, rcBig, rcSmall);
			this->ShowWindow(SW_HIDE);

			return;
		}
		else
		{ // 恢复
			DrawAnimatedRects(m_hWnd, IDANI_CAPTION, rcSmall, rcBig);
			this->ShowWindow(SW_SHOW);

			return;
		}
	}
	else if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{ // 显示关于...
		OnAbout();
		return;
	}
	else if ((nID & 0xFFF0) == IDM_EXIT)
	{ // 退出应用程序
		OnExit();
		return;
	}

	CDialog::OnSysCommand(nID, lParam);
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CToggleTopMainDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CToggleTopMainDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

void CToggleTopMainDlg::OnHelp()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	m_pwndHelp->ShowWindow(m_bHelp ? SW_SHOW : SW_HIDE);
	m_dlgSetting.ShowWindow(m_bHelp ? SW_HIDE : SW_SHOW);
}

void CToggleTopMainDlg::OnAbout()
{
	// TODO: Add your control notification handler code here
	if (FALSE == ::IsWindow(m_dlgAbout.m_hWnd))
		m_dlgAbout.DoModal();
}

void CToggleTopMainDlg::OnCheckVer()
{
	// TODO: Add your control notification handler code here
}

void CToggleTopMainDlg::OnSetting()
{
	// TODO: Add your control notification handler code here
	if (m_dlgSetting.ApplySetting(&m_struNID))
		PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0L); // 最小化窗口
}

void CToggleTopMainDlg::OnCancel()
{
	// TODO: Add extra cleanup here
	UpdateData(FALSE);
	DWORD dwHotKey = theApp.m_struIniInfo.hotkey.dwHotToggTop;
	m_dlgSetting.m_hotToggTop.SetHotKey(LOWORD(dwHotKey), HIWORD(dwHotKey));
	dwHotKey = theApp.m_struIniInfo.hotkey.dwHotShowBox;
	m_dlgSetting.m_hotShowWnd.SetHotKey(LOWORD(dwHotKey), HIWORD(dwHotKey));

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0L);
}

void CToggleTopMainDlg::WinHelp(DWORD /*dwData*/, UINT /*nCmd*/)
{
	// TODO: Add your specialized code here and/or call the base class
	// 模拟用户点击"帮助"按钮
	HWND hwndCtrl = NULL;
	CWnd::GetDlgItem(IDC_CHKBTN_HELP, &hwndCtrl);
	VERIFY(hwndCtrl);

	::SendMessage(hwndCtrl, WM_LBUTTONDOWN, 0L, 0L);
	::PostMessage(hwndCtrl, WM_LBUTTONUP, 0L, 0L);
}

void CToggleTopMainDlg::OnExit()
{
	// TODO: Add your command handler code here
	if (theApp.m_struIniInfo.general.bQueryEnd)
		if (IDYES != AfxMessageBox(_T("确定要退出 ToggleTop 吗?"), MB_YESNO, MB_ICONQUESTION))
			return;

	// 删除任务栏图标
	Shell_NotifyIcon(NIM_DELETE, &m_struNID);

	// 注销系统热键
	UnregisterHotKey(m_hWnd, IDHOT_SETTOP);
	UnregisterHotKey(m_hWnd, IDHOT_SHOWBOX);

	CDialog::EndDialog(IDOK);
}

void CToggleTopMainDlg::OnShowHide()
{
	// TODO: Add your command handler code here
	if (IsWindowVisible())
	{
		if (GetActiveWindow() == this)
			PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0L);
		else
		{
			SetActiveWindow();
			SetForegroundWindow();
		}
	}
	else
		ShowMainWnd();
}

void CToggleTopMainDlg::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{
	UINT uID;		//发出该消息的图标的ID
	UINT uMouseMsg; //鼠标动作
	POINT point;

	uID = (UINT)wParam;
	uMouseMsg = (UINT)lParam;
	if (uID == IDR_MAINFRAME) // 如果是我们的图标
	{
		if (IsWindow(m_dlgAbout.m_hWnd))
		{ // 帮助窗口已打开, do nothing
			::SetActiveWindow(m_dlgAbout.m_hWnd);
			::SetForegroundWindow(m_dlgAbout.m_hWnd);
			return;
		}

		if (uMouseMsg == WM_RBUTTONDOWN)
		{ // 如果是单击右键

			GetCursorPos(&point); //取得鼠标位置

			// 取菜单资源
			CMenu mnuTop;
			mnuTop.LoadMenu(IDR_TRAYMENU);

			// 取子菜单
			CMenu *pPopup = mnuTop.GetSubMenu(0);
			ASSERT_VALID(pPopup);

			// 设置默认菜单
			SetMenuDefaultItem(pPopup->m_hMenu, 0, TRUE);

			// bring us to the front, else when user clicks
			// outside if menu the menu stays up
			SetForegroundWindow();

			// 显示菜单
			pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
								   point.x, point.y,
								   AfxGetMainWnd(), NULL);

			PostMessage(WM_NULL);

			// 弹出式菜单的命令会被 MFC 的消息路由机制自动处理
		}

		if (uMouseMsg == WM_LBUTTONDOWN)
		{ // 如果是单击左键
			OnShowHide();
		}
	}

	return;
}

LRESULT CToggleTopMainDlg::OnNewSession(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	ShowMainWnd();

	return WM_USER_NEWSESSION;
}

void CToggleTopMainDlg::InitHelpWnd()
{
	// 创建
	CCreateContext pContext;
	CWnd *pFrameWnd = this;
	pContext.m_pCurrentDoc = new CDocument;
	pContext.m_pNewViewClass = RUNTIME_CLASS(CHtmlView);
	m_pwndHelp = (CHtmlView *)((CFrameWnd *)pFrameWnd)->CreateView(&pContext);
	VERIFY(m_pwndHelp);

	// 初始化位置
	CWnd *pwndPlace = GetDlgItem(IDC_PLACEHOLDER);
	ASSERT_VALID(pwndPlace);
	CRect rcHelp;

	pwndPlace->GetWindowRect(rcHelp);
	rcHelp.bottom += 1; // +1 for looks better
	rcHelp.right += 1;

	// 大小修正
	CRect rcTemp, rcTemp2;
	GetWindowRect(rcTemp);
	GetClientRect(rcTemp2);
	ClientToScreen(rcTemp2);
	rcHelp -= rcTemp2.TopLeft() - rcTemp.TopLeft();

	m_pwndHelp->MoveWindow(rcHelp);
	m_pwndHelp->ShowWindow(SW_HIDE);

	// 初始化内容
	CString strHelp;
	strHelp.LoadString(IDS_HTMLHELP);
	m_pwndHelp->Navigate2(strHelp, navNoHistory);
}

LRESULT CToggleTopMainDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	// 正在显示提示时不作处理
	if (m_wndTip.m_bIsShowing)
		return CDialog::WindowProc(message, wParam, lParam);

	if (message == WM_HOTKEY)
	{
		DWORD dwHotKeyRev = theApp.m_struIniInfo.hotkey.dwHotToggTop;
		DWORD dwHotKey = MAKELPARAM(HIWORD(dwHotKeyRev), LOWORD(dwHotKeyRev));
		if ((wParam == IDHOT_SETTOP) && ((DWORD)lParam == dwHotKey))
		{
			CWnd *pwndCurr = GetForegroundWindow();
			CWnd *pwndTarg = pwndCurr->GetTopLevelParent();

			// 判断是否为要过滤的窗口
			BOOL bFound = FALSE;
			for (int i = 0; i < TT_FILTERWND_NUM; i++)
			{
				if (pwndTarg->m_hWnd == m_pFilterWnd[i] ||
					pwndCurr->m_hWnd == m_pFilterWnd[i])
				{
					bFound = TRUE;
					break;
				}
			}

			if (!bFound)
			{
				CString strTitle;
				pwndTarg->GetWindowText(strTitle);
				BOOL bTopMost = pwndTarg->GetExStyle() & WS_EX_TOPMOST;
				BOOL bRet = ::SetWindowPos(pwndTarg->m_hWnd,
										   (bTopMost ? HWND_NOTOPMOST : HWND_TOPMOST),
										   0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

				strTitle += "\n";
				if (bRet)
				{
					strTitle /* + */ = (bTopMost ? _T("取消最上层显示") : _T("最上层显示"));
					if (theApp.m_struIniInfo.general.bShowTip)
					{
						if (m_wndTip.m_hWnd)
							m_wndTip.DestroyWindow();

						m_wndTip.Create(this);
						m_wndTip.ShowFadeTip(pwndTarg, strTitle);
					}
				}
				else
				{
					strTitle += (bTopMost ? _T("DeTop faild!") : _T("SetTop faild!"));
					::MessageBox(pwndTarg->m_hWnd, strTitle, _T("ToggleTop"),
								 MB_OK | MB_ICONERROR);
				}
			}
		}
		else
		{
			dwHotKeyRev = theApp.m_struIniInfo.hotkey.dwHotShowBox;
			dwHotKey = MAKELPARAM(HIWORD(dwHotKeyRev), LOWORD(dwHotKeyRev));
			if ((wParam == IDHOT_SHOWBOX) && ((DWORD)lParam == dwHotKey))
				ShowMainWnd();
		}
	}

	return CDialog::WindowProc(message, wParam, lParam);
}

BOOL CToggleTopMainDlg::GetFilterWnd()
{
	// 过滤自身窗口和系统桌面
	int i = 0;
	m_pFilterWnd[i++] = NULL; // 过滤掉空句柄防止一些访问错误
	m_pFilterWnd[i++] = AfxGetMainWnd()->m_hWnd;
	m_pFilterWnd[i++] = ::FindWindow(_T("Progman"), _T("Program Manager"));
	m_pFilterWnd[i++] = ::FindWindow(_T("Shell_TrayWnd"), _T(""));
	m_pFilterWnd[i++] = ::FindWindow(_T("BaseBar"), _T("Quick Launch"));

	ASSERT(i == TT_FILTERWND_NUM); // 确保没有忘记同步宏定义

	return TRUE;
}

void CToggleTopMainDlg::ShowMainWnd()
{
	if (FALSE == IsWindowVisible())
		PostMessage(WM_SYSCOMMAND, SC_RESTORE, 0L);

	//	if( FALSE == ::IsWindow( m_dlgAbout.m_hWnd ) )
	//	{
	SetActiveWindow();
	SetForegroundWindow();
	//	}
}
