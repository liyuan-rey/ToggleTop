#if !defined(AFX_FADETIP_H__3D13E9FB_1492_4595_A074_A89312FC7947__INCLUDED_)
#define AFX_FADETIP_H__3D13E9FB_1492_4595_A074_A89312FC7947__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FadeTip.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFadeTip window

class CFadeTip : public CWnd
{
// Construction
public:
	CFadeTip();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFadeTip)
	public:
	virtual BOOL Create( CWnd* pwndOwner );
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_bIsShowing; // 是否正在显示
	static COLORREF PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent);
	BOOL SetTipStyle(LPRECT lprcMargin, COLORREF clrBk, COLORREF clrText);
	BOOL ShowFadeTip(CWnd* pwndTarg, CString &strTip);
	virtual ~CFadeTip();
	enum { ID = 8050 };

	// Generated message map functions
protected:
	unsigned int			m_nFrame;		// 边框厚度
	unsigned int			m_nDelayTime;	// 提示显示延迟时间
	DWORD					m_dwStart;		// 提示显示开始时间
	DWORD					m_dwLast;		// 提示最后一次显示时间
	CFont					m_cFont;		// 显示提示的字体
	COLORREF				m_clrText;		// 提示文字颜色
	COLORREF				m_clrBk;		// 提示背景色
	COLORREF				m_clrFrame;		// 提示边框颜色
	CRect					m_rcMargin;		// 提示页边距
	CRect					m_rcText;		// 显示文字的区域
	CString					m_strTip;		// 提示文字

	CBitmap					m_bmpSrc;		// 保存原始图象的位图
	CBitmap					m_bmpDst;		// 保存本窗体图象的位图
	CBitmap					m_bmpTmp;		// 用于透明的临时位图

	int						m_iFadePercent; // 透明度
	
	COLORREF*				m_pclrFadeSrc;	// 原始图象的象素颜色数组
	COLORREF*				m_pclrFadeDst;	// 本窗体图象的象素颜色数组
	COLORREF*				m_pclrFadeTmp;	// 临时位图的象素颜色数组

	//{{AFX_MSG(CFadeTip)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FADETIP_H__3D13E9FB_1492_4595_A074_A89312FC7947__INCLUDED_)
