// CRCCheckerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CRCChecker.h"
#include "CRCCheckerDlg.h"
#include "..\common\Crc32Dynamic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCRCCheckerDlg dialog




CCRCCheckerDlg::CCRCCheckerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCRCCheckerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_FilePath = _T("");
}

void CCRCCheckerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PATH, m_EditPath);
	DDX_Control(pDX, IDC_EDIT_CRC, m_EditCrc);
}

BEGIN_MESSAGE_MAP(CCRCCheckerDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &CCRCCheckerDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CCRCCheckerDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CCRCCheckerDlg message handlers

BOOL CCRCCheckerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_EditCrc.SetReadOnly();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCRCCheckerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCRCCheckerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CCRCCheckerDlg::OnBnClickedButton1()
{
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, NULL, this);

	if(dlg.DoModal() == IDOK)
	{
		m_FilePath = dlg.GetPathName();
		m_EditPath.SetWindowText(m_FilePath);
		m_EditCrc.SetWindowText(_T(""));
	}
}

void CCRCCheckerDlg::OnBnClickedOk()
{
	if(m_FilePath.IsEmpty() == true)
		return;

	DWORD dwCrc32, dwErrorCode = NO_ERROR;
	CCrc32Dynamic *pobCrc32Dynamic = new CCrc32Dynamic;

	pobCrc32Dynamic->Init();
	dwErrorCode = pobCrc32Dynamic->FileCrc32Streams(m_FilePath, dwCrc32);
	pobCrc32Dynamic->Free();

	delete pobCrc32Dynamic;

	CString cstrCalculatedCRC;
	if(dwErrorCode == NO_ERROR)
	{
		cstrCalculatedCRC.Format(_T("0x%08x"), dwCrc32);
	}
	else
	{
		cstrCalculatedCRC.Format(_T("Error: [0x%08x]"), dwErrorCode);
	}

	m_EditCrc.SetWindowText(cstrCalculatedCRC);
}