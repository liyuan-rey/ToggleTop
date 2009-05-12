// ToggleTopMainDlg.h : header file
//

#if !defined(AFX_TOGGLETOPMAINDLG_H__B788AE00_A51F_47A2_AB3E_E80F53EFB403__INCLUDED_)
#define AFX_TOGGLETOPMAINDLG_H__B788AE00_A51F_47A2_AB3E_E80F53EFB403__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DlgSetting.h"
#include "FadeTip.h"
#include "AboutDlg.h"

#define WM_USER_TRAYNOTIFY		WM_APP + 8048	// 自定义消息 ID
#define TT_FILTERWND_NUM		5				// 过滤窗口数
/////////////////////////////////////////////////////////////////////////////
// CToggleTopMainDlg dialog

class CToggleTopMainDlg : public CDialog
{
// Construction
public:
	CToggleTopMainDlg(CWnd* pParent = NULL);	// standard constructor

	CHtmlView* m_pwndHelp;
	CDlgSetting m_dlgSetting;
	CAboutDlg m_dlgAbout;

	NOTIFYICONDATA m_struNID;

// Dialog Data
	//{{AFX_DATA(CToggleTopMainDlg)
	enum { IDD = IDD_TOGGLETOP_DIALOG };
	BOOL	m_bHelp;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CToggleTopMainDlg)
	public:
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void ShowMainWnd();
	BOOL GetFilterWnd();
	HICON m_hIcon;
	HWND m_pFilterWnd[TT_FILTERWND_NUM];
	CFadeTip m_wndTip;

	// Generated message map functions
	//{{AFX_MSG(CToggleTopMainDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnHelp();
	afx_msg void OnAbout();
	afx_msg void OnCheckVer();
	virtual void OnOK(){};
	virtual void OnCancel();
	afx_msg void OnSetting();
	afx_msg void OnExit();
	afx_msg void OnShowHide();
	//}}AFX_MSG
	afx_msg void OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewSession(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
private:
	void InitHelpWnd();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TOGGLETOPMAINDLG_H__B788AE00_A51F_47A2_AB3E_E80F53EFB403__INCLUDED_)
