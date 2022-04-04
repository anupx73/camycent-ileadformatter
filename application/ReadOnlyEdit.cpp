// ReadOnlyEdit.cpp : implementation file
//

#include "stdafx.h"
#include "iFormater.h"
#include "ReadOnlyEdit.h"

// CReadOnlyEdit

IMPLEMENT_DYNAMIC(CReadOnlyEdit, CEdit)

CReadOnlyEdit::CReadOnlyEdit()
{
	m_crBackGnd = RGB(255,255,255);
	m_crText = RGB(0,0,0);
}

CReadOnlyEdit::~CReadOnlyEdit()
{
	//delete brush
	if (m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
}

BEGIN_MESSAGE_MAP(CReadOnlyEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CReadOnlyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{
	pDC->SetTextColor(m_crText);
	pDC->SetBkColor(m_crBackGnd);

	//return the brush used for background this sets control background
	return m_brBackGnd;
}

void CReadOnlyEdit::SetBackColor(COLORREF rgb)
{
	m_crBackGnd = rgb;

	//free brush
	if(m_brBackGnd.GetSafeHandle())
		m_brBackGnd.DeleteObject();
	//set brush to new color
	m_brBackGnd.CreateSolidBrush(rgb);

	//redraw
	Invalidate(TRUE);
}

void CReadOnlyEdit::SetTextColor(COLORREF rgb)
{
	//set text color ref
	m_crText = rgb;

	//redraw
	Invalidate(TRUE);
}
