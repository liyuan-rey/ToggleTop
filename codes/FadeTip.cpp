// FadeTip.cpp : implementation file
//

#include "stdafx.h"
#include "toggletop.h"
#include "ToggleTopMainDlg.h"
#include "FadeTip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CToggleTopApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CFadeTip

CFadeTip::CFadeTip()
{
	// 初始化成员变量
	m_bIsShowing = FALSE;
	m_iFadePercent = 100; // 不透明
	m_dwStart = m_dwLast = 0;
	m_pclrFadeDst = NULL;
	m_pclrFadeSrc = NULL;
	m_pclrFadeTmp = NULL;

	// 设置默认提示风格
	// 默认使用系统标题栏字体
	NONCLIENTMETRICS metrics;
	metrics.cbSize = sizeof(NONCLIENTMETRICS);
	if (SystemParametersInfo(SPI_GETNONCLIENTMETRICS, 0, &metrics, FALSE))
		m_cFont.CreateFontIndirect(&metrics.lfCaptionFont);
	VERIFY(m_cFont.m_hObject);

	m_clrBk = RGB(252, 252, 228); // 淡黄色
	m_clrText = RGB(0, 0, 0);	 // 黑色
	m_clrFrame = RGB(0, 0, 0);	// 黑色

	m_nFrame = 2;

	m_nDelayTime = 1000; // 默认显示一秒
	m_rcText.SetRect(0, 0, 0, 0);
	m_rcMargin.SetRect(15, 15, 15, 15);
}

CFadeTip::~CFadeTip()
{
	m_cFont.DeleteObject();
}

BEGIN_MESSAGE_MAP(CFadeTip, CWnd)
//{{AFX_MSG_MAP(CFadeTip)
ON_WM_PAINT()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFadeTip message handlers

BOOL CFadeTip::Create(CWnd *pwndOwner)
{
	// TODO: Add your specialized code here and/or call the base class
	pwndOwner->AssertValid();

	/////////////////////////////////////////
	// copy from wincore.cpp, and modified
	LPTSTR lpszName = _T("FadeTipWnd");
	HINSTANCE hInst = AfxGetInstanceHandle();
	// see if the class already exists
	WNDCLASS wndcls;
	if (!::GetClassInfo(hInst, lpszName, &wndcls))
	{ // not registered, we need to register a new class
		wndcls.style = CS_DBLCLKS;
		wndcls.lpfnWndProc = ::DefWindowProc;
		wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
		wndcls.hInstance = hInst;
		wndcls.hIcon = NULL;
		wndcls.hCursor = NULL;
		wndcls.hbrBackground = NULL;
		wndcls.lpszMenuName = NULL;
		wndcls.lpszClassName = lpszName;
		if (!AfxRegisterClass(&wndcls))
		{
			AfxMessageBox(_T("注册提示窗口类失败."), MB_OK | MB_ICONERROR);

			// 禁用显示提示
			CWnd *pwndItem = (((CToggleTopMainDlg *)pwndOwner)->m_dlgSetting).GetDlgItem(IDC_CHKBTN_SHOWTIP);
			ASSERT_VALID(pwndItem);
			((CButton *)pwndItem)->SetCheck(0);
			pwndItem->EnableWindow(FALSE);
			theApp.m_struIniInfo.general.bShowTip = FALSE;

			return FALSE;
		}
	}
	// else // already registered, assert everything is good
	// NOTE: We have to trust that the hIcon, hbrBackground, and the
	//  hCursor are semantically the same, because sometimes Windows does
	//  some internal translation or copying of those handles before
	//  storing them in the internal WNDCLASS retrieved by GetClassInfo.
	//////////////// End of Copy ///////////////////

	BOOL bRet = CWnd::CreateEx(WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOPARENTNOTIFY,
							   wndcls.lpszClassName, _T(""), WS_POPUP /*WINDOW*/,
							   0, 0, 0, 0, NULL, NULL /*ID*/, NULL);
	if (!bRet)
	{
		AfxMessageBox(_T("创建提示窗口失败."), MB_OK | MB_ICONERROR);

		// 禁用显示提示
		CWnd *pwndItem = (((CToggleTopMainDlg *)pwndOwner)->m_dlgSetting).GetDlgItem(IDC_CHKBTN_SHOWTIP);
		ASSERT_VALID(pwndItem);
		((CButton *)pwndItem)->SetCheck(0);
		pwndItem->EnableWindow(FALSE);
		theApp.m_struIniInfo.general.bShowTip = FALSE;

		return FALSE;
	}

	SetOwner(pwndOwner);

	return TRUE;
}

void CFadeTip::OnPaint()
{
	CPaintDC paintDC(this); // device context for painting

	// TODO: Add your message handler code here
	// 绘制淡出提示窗口, 不超过每秒 30 帧
	DWORD dwCurr = GetTickCount();
	if (m_dwStart == 0)
	{
		m_dwStart = m_dwLast = dwCurr; // 初始化时间数据

		// 准备AlphaBlend的图象, 每次显示只做一次
		// 销毁先前的位图
		if (m_bmpSrc.m_hObject)
			m_bmpSrc.DeleteObject();

		if (m_bmpDst.m_hObject)
			m_bmpDst.DeleteObject();

		if (m_bmpTmp.m_hObject)
			m_bmpTmp.DeleteObject();

		CDC dcMem;
		if (!dcMem.CreateCompatibleDC(&paintDC))
		{
			return;
		}

		// 取当前屏幕图象
		CRect rcTmp = paintDC.m_ps.rcPaint;

		// 构造与绘制有关的设备无关位图
		BITMAPINFOHEADER bih;
		bih.biSize = sizeof(BITMAPINFOHEADER);
		bih.biWidth = rcTmp.Width();
		bih.biHeight = rcTmp.Height();
		bih.biPlanes = 1;
		bih.biBitCount = 32;
		bih.biCompression = BI_RGB;
		bih.biSizeImage = bih.biWidth * bih.biHeight;
		bih.biXPelsPerMeter = 0;
		bih.biYPelsPerMeter = 0;
		bih.biClrUsed = 0;
		bih.biClrImportant = 0;

		HBITMAP hmbpDib;
		// Create a DIB section and attach it to the source bitmap
		hmbpDib = CreateDIBSection(
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_pclrFadeSrc,
			NULL, NULL);
		if (hmbpDib == NULL || m_pclrFadeSrc == NULL)
		{
			return;
		}

		m_bmpSrc.Attach(hmbpDib);

		// Create a DIB section and attach it to the destination bitmap
		hmbpDib = CreateDIBSection(
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_pclrFadeDst,
			NULL, NULL);
		if (hmbpDib == NULL || m_pclrFadeDst == NULL)
		{
			return;
		}
		m_bmpDst.Attach(hmbpDib);

		// Create a DIB section and attach it to the temporary bitmap
		hmbpDib = CreateDIBSection(
			dcMem.m_hDC, (LPBITMAPINFO)&bih, DIB_RGB_COLORS, (void **)&m_pclrFadeTmp,
			NULL, NULL);
		if (hmbpDib == NULL || m_pclrFadeTmp == NULL)
		{
			return;
		}

		m_bmpTmp.Attach(hmbpDib);

		// get source image, representing the window below the popup menu
		CBitmap *pBmpOld = dcMem.SelectObject(&m_bmpSrc);
		dcMem.BitBlt(0, 0, bih.biWidth, bih.biHeight, &paintDC,
					 rcTmp.left, rcTmp.top, SRCCOPY);

		// copy it to the destination so that shadow will be ok
		memcpy(m_pclrFadeDst, m_pclrFadeSrc, sizeof(COLORREF) * bih.biWidth * bih.biHeight);
		dcMem.SelectObject(&m_bmpDst);

		// 绘制我们的显示图象
		// 绘制背景
		dcMem.FillSolidRect(0, 0, rcTmp.Width(), rcTmp.Height(), m_clrBk);

		HRGN hrgn = CreateRectRgn(0, 0, 0, 0);
		GetWindowRgn(hrgn);

		HBRUSH hbrush = CreateSolidBrush(RGB(212, 208, 200)); // 浅灰色
		::FrameRgn(dcMem, hrgn, hbrush, 3, 3);
		::DeleteObject(hbrush);

		hbrush = CreateSolidBrush(RGB(192, 192, 192)); // 灰色
		::FrameRgn(dcMem, hrgn, hbrush, 2, 2);
		::DeleteObject(hbrush);

		hbrush = CreateSolidBrush(RGB(0, 0, 0)); // 黑色
		::FrameRgn(dcMem, hrgn, hbrush, 1, 1);
		::DeleteObject(hbrush);

		::DeleteObject(hrgn);

		// 写提示
		CFont *pOldFont = dcMem.SelectObject(&m_cFont);
		dcMem.DrawText(m_strTip, m_rcText, DT_LEFT | DT_VCENTER);
		dcMem.SelectObject(pOldFont);

		dcMem.SelectObject(pBmpOld);
	}

	if (dwCurr - m_dwStart > m_nDelayTime)
	{ // 绘制时限已到
		ShowWindow(SW_HIDE);
		m_bIsShowing = FALSE;
		return;
	}
	else
	{ //  仍需继续绘制窗口
		DWORD dwDec = dwCurr - m_dwLast;
		if (dwDec == 0 || // 首次绘制
			dwDec >= 10)  // 或已超过规定时间(一秒一百帧)
		{				  // 绘制窗口
			int cx = paintDC.m_ps.rcPaint.right - paintDC.m_ps.rcPaint.left;
			int cy = paintDC.m_ps.rcPaint.bottom - paintDC.m_ps.rcPaint.top;

			CDC dcMem;
			if (!dcMem.CreateCompatibleDC(&paintDC))
			{
				return;
			}

			if (m_dwLast - m_dwStart > 500)
			{ // 半秒之后开始淡出
				// 计算透明度
				m_iFadePercent -= 2 * (dwDec / 10);

				COLORREF *src = m_pclrFadeSrc;
				COLORREF *dst = m_pclrFadeDst;
				COLORREF *tmp = m_pclrFadeTmp;

				CBitmap *pBmpOld = dcMem.SelectObject(&m_bmpTmp);
				for (int pixel = 0; pixel < cx * cy; pixel++)
				{
					*tmp++ = PixelAlpha(*src++, *dst++, 100 - m_iFadePercent);
				}

				paintDC.BitBlt(paintDC.m_ps.rcPaint.left, paintDC.m_ps.rcPaint.top, cx, cy,
							   &dcMem, 0, 0, SRCCOPY);

				dcMem.SelectObject(pBmpOld);
			}
			else
			{ // 显示我们的界面
				CBitmap *pBmpOld = dcMem.SelectObject(&m_bmpDst);
				paintDC.BitBlt(paintDC.m_ps.rcPaint.left, paintDC.m_ps.rcPaint.top, cx, cy,
							   &dcMem, 0, 0, SRCCOPY);

				dcMem.SelectObject(pBmpOld);
			}

			m_dwLast = dwCurr;
		}
		else // 不必实施一帧
		{
			Sleep(dwDec / 2); // 给处理器刘点时间
		}

		Invalidate();
	}

	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CFadeTip::ShowFadeTip(CWnd *pwndTarg, CString &strTip)
{
	/*	// 得到字体宽/高的尝试(失败)
	CDC ScreenDC;
	ScreenDC.CreateIC(_T("DISPLAY"), NULL, NULL, NULL);
	const int nLogDPIX = ScreenDC.GetDeviceCaps(LOGPIXELSX),
		nLogDPIY = ScreenDC.GetDeviceCaps(LOGPIXELSY);
//*/

	m_strTip = strTip;

	// 取得要显示的文本区域大小
	m_rcText.SetRect(0, 0, 0, 0); // 清除上一次计算区域
	CDC memDC;
	memDC.CreateCompatibleDC(NULL);
	CFont *pOldFont = memDC.SelectObject(&m_cFont);

	if (-1 == strTip.FindOneOf(_T("\r\n")))
	{ // 单行文本
		memDC.DrawText(strTip, m_rcText, DT_CALCRECT);
	}
	else
	{ // 多行文本
		// 取得最长一行的文字
		CString strTmp;
		int iMaxLength = 0;
		TCHAR *pszTip = new TCHAR[strTip.GetLength() + 1];
		_tcscpy(pszTip, LPCTSTR(strTip));
		TCHAR *pszToken = _tcstok(pszTip, _T("\r\n"));
		while (pszToken != NULL)
		{
			int iTmp = _tcslen(pszToken);
			if (iTmp > iMaxLength)
			{
				iMaxLength = iTmp;
				strTmp = pszToken;
			}

			pszToken = _tcstok(NULL, _T("\r\n"));
			;
		}
		delete pszTip;

		// 得到宽度
		memDC.DrawText(strTmp, m_rcText, DT_CALCRECT);
		// 得到高度
		memDC.DrawText(strTip, m_rcText, DT_CALCRECT);
	}

	memDC.SelectObject(pOldFont);

	// 设置提示的位置到父窗口中央
	CSize szTip(m_rcMargin.left + m_rcMargin.right + m_rcText.right,
				m_rcMargin.top + m_rcMargin.bottom + m_rcText.bottom);

	CRect rcClient;
	::GetClientRect(pwndTarg->m_hWnd, rcClient);

	POINT pt;
	pt.x = (rcClient.Width() - szTip.cx) / 2;
	pt.y = (rcClient.Height() - szTip.cy) / 2;

	SetWindowPos(&wndTopMost, pt.x, pt.y, szTip.cx, szTip.cy,
				 SWP_NOREDRAW | SWP_NOSENDCHANGING);

	// 文字区域加上页边距偏移量
	m_rcText.OffsetRect(m_rcMargin.TopLeft());

	// 将提示边角圆整
	HRGN rgnTip = ::CreateRoundRectRgn(0, 0, szTip.cx, szTip.cy, 12, 11);
	SetWindowRgn(rgnTip, FALSE);

	// 使用父窗口的客户区坐标
	SetParent(pwndTarg);

	m_iFadePercent = 100;
	m_dwStart = m_dwLast = 0;
	m_bIsShowing = TRUE;
	ShowWindow(SW_SHOW);

	return TRUE;
}

BOOL CFadeTip::SetTipStyle(LPRECT lprcMargin, COLORREF clrBk, COLORREF clrText)
{
	m_rcMargin = *lprcMargin;
	m_clrBk = clrBk;
	m_clrText = clrText;

	return TRUE;
}

// ==================================================================
//
// FUNCTION :  PixelAlpha ()
//
// * Description : Shades a color value with a specified percentage
//
// * Author : [Guillaume Nodet]
//
// * Returns : [COLORREF] - The result pixel
//
// * Function parameters :
// [srcPixel] - The source pixel
// [dstPixel] - The destination pixel
// [percent] -  Percentage (amount of shadow)
//
// ==================================================================
COLORREF CFadeTip::PixelAlpha(COLORREF srcPixel, COLORREF dstPixel, int percent)
{
	int ipercent = 100 - percent;
	COLORREF clrFinal = RGB((GetRValue(srcPixel) * percent + GetRValue(dstPixel) * ipercent) / 100,
							(GetGValue(srcPixel) * percent + GetGValue(dstPixel) * ipercent) / 100,
							(GetBValue(srcPixel) * percent + GetBValue(dstPixel) * ipercent) / 100);

	return (clrFinal);
}
