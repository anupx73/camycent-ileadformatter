#pragma once


// CReadOnlyEdit

class CReadOnlyEdit : public CEdit
{
	DECLARE_DYNAMIC(CReadOnlyEdit)

public:
	CReadOnlyEdit();
  virtual ~CReadOnlyEdit();

protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
  DECLARE_MESSAGE_MAP()

private:
	COLORREF m_crText;
	COLORREF m_crBackGnd;
	CBrush m_brBackGnd;

public:
	void SetTextColor(COLORREF rgb);
	void SetBackColor(COLORREF rgb);
};


