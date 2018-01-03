#if !defined(AFX_AboutDlg_H__AFB27DDC_EBC5_4387_87C6_B24E9EE34CD2__INCLUDED_)
#define AFX_AboutDlg_H__AFB27DDC_EBC5_4387_87C6_B24E9EE34CD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AboutDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
//	void AnimateHide();
//	void AnimateShow();
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	public:
	virtual int DoModal();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_iModal;
	//{{AFX_MSG(CAboutDlg)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AboutDlg_H__AFB27DDC_EBC5_4387_87C6_B24E9EE34CD2__INCLUDED_)
