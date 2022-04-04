#pragma once
#include "PictureEx.h"
#include "MySqlConn.h"
#include "USA.h"
#include "Aus.h"
#include "FormatterProcess.h"
#include "ErrorDialog.h"
#include "ReadOnlyEdit.h"

enum eConvertStatus{
  CONVERT_SUCESS,
  CONVERT_FAIL_FILE_FORMAT,
  CONVERT_FAIL_DB_ERROR,
  CONVERT_FAIL_NO_INTERNET,
  CONVERT_FAIL_INSUFFICIENT_BAL,
  CONVERT_FAIL_FILE_COUNT,
  CONVERT_FAIL_RECHARGE_ACC,
};

enum eHoverArea{
  AREA_NONE,
  AREA_ABOUT,
  AREA_HELP,
};


class CiFormaterConvertDlg : public CDialog
{
	DECLARE_DYNAMIC(CiFormaterConvertDlg)

public:
	CiFormaterConvertDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CiFormaterConvertDlg();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG *pMsg);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnNcHitTest(UINT uParam, LONG lParam);
  afx_msg void OnMouseMove(UINT nFlags, CPoint point);
  afx_msg BOOL OnSetCursor(CWnd *pWnd, UINT nHitTest, UINT message);
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);

public:
	CReadOnlyEdit m_Edit[6];
	CButton m_Check[5];
	CButton *m_CheckFilter[10];
  int m_iNextTop;
  int m_iNextBottom;
	CComboBox m_ComboCountry;
  CComboBox m_ComboURL;

	CPictureEx m_cLogo;
	CBitmap m_BmpImage[6];
	CSize m_SizBmpImage[6];

	CFont m_BodyTxtFont;
	CFont m_LinkFont;
  CFont m_LinkUFont;
	//CFont m_InfoTxtFont;
	CFont m_BottomTxtFont;

	void ProcessFormat();
	LRESULT OnConvertComplete(WPARAM wParam, LPARAM lParam);
	CPictureEx m_Processing;
	bool m_bProcessing;
	bool m_bLoadGettingStarted;
	short m_iFileAddedCount;
	CString m_csURL;
	void CreateRequiredFonts();
	CStringArray m_cArrCountry;
	CStringArray m_cArrCountryURL;
	afx_msg void OnCountryComboChanged();
  afx_msg void OnURLComboChanged();
	void ProcessFormatThreadFunction();
	std::string GenerateOutputFileName(CString cstrInputFile);
	void ResizeControls();
	void BitmapLoadFromResource();
	CStringArray m_carrInputFilePaths;
	bool m_bCompleteStatusShow;
	void ClearCompleteIcons();
  void SetErrorDialogVisible(bool bVisible);

private:
	std::vector <int> iCompleteStatus;
	std::vector <int> iCompleteIndex;

	int m_iTotalLeadsConverted;
	CRect m_rcToRefresh;

	void WriteTotalLeadsConvertedToRegistry();
	void ReadTotalLeadsConvertedFromRegistry();

	int ReverseNumber(int iNumber);

	CString m_cstrSelectedCountry;
	CString m_cstrSelectedURL;
	bool CheckForCountryAndURL();

  void SetComboBoxValueForCountry();
  void SetComboBoxValueForURL();

  CString m_cstrLeadsLeft;
  void UpdateLeadsBalance();

  eHoverArea m_eHoverArea;
  CErrorDialog *pErrorDlg;
  bool m_bErrorDialogOpened;
  void ShowFilterPanel(bool bShow);
  bool m_bFilterPanelOpened;
  std::string GetFilterState();
};


UINT ProcessFormatThread(LPVOID lParam);