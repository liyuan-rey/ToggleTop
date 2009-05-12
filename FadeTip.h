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
	BOOL m_bIsShowing; // �Ƿ�������ʾ
	static COLORREF PixelAlpha (COLORREF srcPixel, COLORREF dstPixel, int percent);
	BOOL SetTipStyle(LPRECT lprcMargin, COLORREF clrBk, COLORREF clrText);
	BOOL ShowFadeTip(CWnd* pwndTarg, CString &strTip);
	virtual ~CFadeTip();
	enum { ID = 8050 };

	// Generated message map functions
protected:
	unsigned int			m_nFrame;		// �߿���
	unsigned int			m_nDelayTime;	// ��ʾ��ʾ�ӳ�ʱ��
	DWORD					m_dwStart;		// ��ʾ��ʾ��ʼʱ��
	DWORD					m_dwLast;		// ��ʾ���һ����ʾʱ��
	CFont					m_cFont;		// ��ʾ��ʾ������
	COLORREF				m_clrText;		// ��ʾ������ɫ
	COLORREF				m_clrBk;		// ��ʾ����ɫ
	COLORREF				m_clrFrame;		// ��ʾ�߿���ɫ
	CRect					m_rcMargin;		// ��ʾҳ�߾�
	CRect					m_rcText;		// ��ʾ���ֵ�����
	CString					m_strTip;		// ��ʾ����

	CBitmap					m_bmpSrc;		// ����ԭʼͼ���λͼ
	CBitmap					m_bmpDst;		// ���汾����ͼ���λͼ
	CBitmap					m_bmpTmp;		// ����͸������ʱλͼ

	int						m_iFadePercent; // ͸����
	
	COLORREF*				m_pclrFadeSrc;	// ԭʼͼ���������ɫ����
	COLORREF*				m_pclrFadeDst;	// ������ͼ���������ɫ����
	COLORREF*				m_pclrFadeTmp;	// ��ʱλͼ��������ɫ����

	//{{AFX_MSG(CFadeTip)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FADETIP_H__3D13E9FB_1492_4595_A074_A89312FC7947__INCLUDED_)
