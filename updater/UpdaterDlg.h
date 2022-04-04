// UpdaterDlg.h : header file
//

#pragma once


// CUpdaterDlg dialog
class CUpdaterDlg : public CDialog
{
// Construction
public:
	CUpdaterDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_UPDATER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	//HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	//afx_msg void OnPaint();
	//afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:
	CString m_ctrUpdateFolder;
	CString m_ctrUpdaterIni;
	CString m_ctrAppFolder;
	CStringArray m_cArrUpdateData;

public:
	void CreateUpdateTempFolder();
	bool CheckFileCRC(LPCTSTR chFileName, LPCTSTR chCRC);
	void SoftwareUpdateThreadFunction();
	bool GetFile(CString szUrl, CString szFileName);
	void DeleteDirectory(CString lpszDir);
	bool CheckForUpdate();
	bool HigherVersionFound(CString cstrRegVer, CString cstrGotVer);
	bool DownloadNewVersion();
	bool CheckNewVersionCRC();
	void InstallNewVersion();
	void RunApplication();
	void StopApplication();
  CString WaitForNewVersionWrite(CString csOldVer);
};

UINT SoftwareUpdateThread(LPVOID lParam);