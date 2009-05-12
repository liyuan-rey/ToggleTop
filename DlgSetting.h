#if !defined(AFX_DLGSETTING_H__136E4145_A3F6_4294_BDCD_919CAFA7B947__INCLUDED_)
#define AFX_DLGSETTING_H__136E4145_A3F6_4294_BDCD_919CAFA7B947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgSetting.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgSetting dialog

class CDlgSetting : public CDialog
{
// Construction
public:
	BOOL ApplySetting(PNOTIFYICONDATA pNID);
	CDlgSetting(CWnd* pParent = NULL);   // standard constructor

	friend class CToggleTopMainDlg;

// Dialog Data
	//{{AFX_DATA(CDlgSetting)
	enum { IDD = IDD_SETTING };
	CHotKeyCtrl	m_hotToggTop;
	CHotKeyCtrl	m_hotShowWnd;
	BOOL	m_bAutoChkVer;
	BOOL	m_bAutoRun;
	BOOL	m_bNoTray;
	BOOL	m_bQueryEnd;
	BOOL	m_bShowTip;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgSetting)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitSetting(PNOTIFYICONDATA pNID);
	BOOL SetAutoRun(BOOL bFlag);

	// Generated message map functions
	//{{AFX_MSG(CDlgSetting)
	virtual void OnOK(){};
	virtual void OnCancel(){};
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGSETTING_H__136E4145_A3F6_4294_BDCD_919CAFA7B947__INCLUDED_)
