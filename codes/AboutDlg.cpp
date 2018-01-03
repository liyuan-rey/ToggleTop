// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include <afxpriv.h>
#include "ToggleTop.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// about dialog

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
//{{AFX_MSG_MAP(CAboutDlg)
ON_WM_LBUTTONDOWN()
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AboutDlg message handlers
void CAboutDlg::OnOK()
{
	//AnimateHide();
	EndModalLoop(m_nModalResult);
}

void CAboutDlg::OnCancel()
{
	//AnimateHide();
	EndModalLoop(m_nModalResult);
}

void CAboutDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	// AnimateHide();
	EndModalLoop(m_nModalResult);

	CDialog::OnLButtonDown(nFlags, point);
}

#ifndef _AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e) \
	do                      \
	{                       \
		e->Delete();        \
	} while (0)
#else //!_AFX_OLD_EXCEPTIONS
#define DELETE_EXCEPTION(e)
#endif //_AFX_OLD_EXCEPTIONS

int CAboutDlg::DoModal()
{
	// can be constructed with a resource template or InitModalIndirect
	ASSERT(m_lpszTemplateName != NULL || m_hDialogTemplate != NULL ||
		   m_lpDialogTemplate != NULL);

	// load resource as necessary
	LPCDLGTEMPLATE lpDialogTemplate = m_lpDialogTemplate;
	HGLOBAL hDialogTemplate = m_hDialogTemplate;
	HINSTANCE hInst = AfxGetResourceHandle();
	if (m_lpszTemplateName != NULL)
	{
		hInst = AfxFindResourceHandle(m_lpszTemplateName, RT_DIALOG);
		HRSRC hResource = ::FindResource(hInst, m_lpszTemplateName, RT_DIALOG);
		hDialogTemplate = LoadResource(hInst, hResource);
	}
	if (hDialogTemplate != NULL)
		lpDialogTemplate = (LPCDLGTEMPLATE)LockResource(hDialogTemplate);

	// return -1 in case of failure to load the dialog template resource
	if (lpDialogTemplate == NULL)
		return -1;

	// disable parent (before creating dialog)
	HWND hWndParent = PreModal();
	AfxUnhookWindowCreate();
	BOOL bEnableParent = FALSE;
	if (hWndParent != NULL && ::IsWindowEnabled(hWndParent))
	{
		::EnableWindow(hWndParent, FALSE);
		bEnableParent = TRUE;
	}

	TRY
	{
		// create modeless dialog
		AfxHookWindowCreate(this);
		if (CreateDlgIndirect(lpDialogTemplate,
							  CWnd::FromHandle(hWndParent), hInst))
		{
			if (m_nFlags & WF_CONTINUEMODAL)
			{
				// 渐现效果
				::AnimateWindow(m_hWnd, 300, AW_BLEND);

				// enter modal loop
				DWORD dwFlags = MLF_SHOWONIDLE;
				if (GetStyle() & DS_NOIDLEMSG)
					dwFlags |= MLF_NOIDLEMSG;
				VERIFY(RunModalLoop(dwFlags) == m_nModalResult);
			}

			// 渐隐效果
			::AnimateWindow(m_hWnd, 300, AW_HIDE | AW_BLEND);

			// hide the window before enabling the parent, etc.
			if (m_hWnd != NULL)
				SetWindowPos(NULL, 0, 0, 0, 0, SWP_HIDEWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
		}
	}
	CATCH_ALL(e)
	{
		DELETE_EXCEPTION(e);
		m_nModalResult = -1;
	}
	END_CATCH_ALL

	if (bEnableParent)
		::EnableWindow(hWndParent, TRUE);
	if (hWndParent != NULL && ::GetActiveWindow() == m_hWnd)
		::SetActiveWindow(hWndParent);

	// destroy modal window
	DestroyWindow();
	PostModal();

	// unlock/free resources as necessary
	if (m_lpszTemplateName != NULL || m_hDialogTemplate != NULL)
		UnlockResource(hDialogTemplate);
	if (m_lpszTemplateName != NULL)
		FreeResource(hDialogTemplate);

	return m_nModalResult;
}

void CAboutDlg::OnPaint()
{
	CDialog::OnPaint();

	CPaintDC paintDC(this); // device context for painting

	// TODO: 绘制程序的标题
}
