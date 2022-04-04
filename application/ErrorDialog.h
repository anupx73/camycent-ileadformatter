#pragma once


// CErrorDialog dialog

class CErrorDialog : public CDialog
{
	DECLARE_DYNAMIC(CErrorDialog)

public:
	CErrorDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CErrorDialog();

// Dialog Data
	enum { IDD = IDD_ERROR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG* pMsg);
	DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnPaint();
  afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
  afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
  LRESULT OnNcHitTest(UINT uParam, LONG lParam);

private:
	CBitmap m_BmpImage[5];
	CSize m_SizBmpImage[5];
  CString m_cstrErrorText;
  CString m_cstrTitleText;
  void CloseErrorDialog();

public:
  void UpdateErrorText(CString cstrText, bool bShowAbout = false);
};
