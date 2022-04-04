// iFormaterDlg.h : header file
//

#pragma once
#include "PictureEx.h"
#include "MySqlConn.h"
#include "iFormaterConvertDlg.h"
#include "..\common\commondef.h"

enum eLoginErrorType{
	ERROR_NOERROR,
	ERROR_BLANK,
	ERROR_INVALID_USER_PASS,
	ERROR_ACC_SUSPENDED,
	ERROR_ACC_DEACTIVATED,
	ERROR_CONNECTIVITY
};

enum eUserType{
	USER_NOUSER,
	USER_DEMO,
	USER_LICENSED
};

// CiFormaterDlg dialog
class CiFormaterDlg : public CDialog
{
public:
	CiFormaterDlg(CWnd* pParent = NULL);	// standard constructor
	~CiFormaterDlg();

	enum { IDD = IDD_IFORMATER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	DECLARE_MESSAGE_MAP()
public:
	HICON m_hIcon;
	CEdit m_Edit;
	CEdit m_Edit2;
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg LRESULT OnNcHitTest(UINT uParam, LONG lParam);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);
	void OnChangeUsername();
	void OnChangePassword();
	void ProcessLogin(bool bAutoLogin = false);
	CPictureEx m_LoggingIn;
	CBitmap m_BmpImg[5];
	CSize m_SizBmpImg[5];

	CFont m_UerPassFont;
	CFont m_LoginFont;
	//CFont m_VerFont;
	CFont m_ForgotPassFont;
  CFont m_ForgotPassUFont;

	void CreateRequiredFonts();

	std::vector<std::string> m_vCountry;
	std::vector<std::string> m_vUrl;
  std::vector<std::string> m_vBal;
	CString m_cstrUser;
	CString m_cstrPass;
	LRESULT OnDBAuthComplete(WPARAM wParam, LPARAM lParam);

	bool m_bLoginAreaPressed;
	CString m_cstrGIFLoader;
	bool LoadResFile(int ResID, LPCTSTR ResFileName);

	eLoginErrorType m_eLoginErr;

	bool m_bProcessingLogin;
	void LicensedUserValidationThreadFunction();

  //eUserType m_eUserType;
  void ValidateDemoUser(CString cstrUser, CString cstrPass);
  void ValidateLicensedUser(CString cstrUser, CString cstrPass);
  void OpenConverterDialog();
  void ProcessAutoLogin();

  CButton m_CheckBoxRemem;
  bool m_bHoverOnForgotPass;
};


UINT LicensedUserValidationThread(LPVOID lParam);
UINT UpdateCheckerThread(LPVOID lParam);