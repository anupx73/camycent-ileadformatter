// CRCCheckerDlg.h : header file
//

#pragma once
#include "afxwin.h"


// CCRCCheckerDlg dialog
class CCRCCheckerDlg : public CDialog
{
// Construction
public:
	CCRCCheckerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_CRCCHECKER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
	CEdit m_EditPath;
	CEdit m_EditCrc;
	CString m_FilePath;
};
