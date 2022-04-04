// updater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "updater.h"
#include "UpdaterDlg.h"
#include "..\common\CommonDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CUpdaterApp

BEGIN_MESSAGE_MAP(CUpdaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CUpdaterApp construction

CUpdaterApp::CUpdaterApp()
{
}


// The one and only CUpdaterApp object

CUpdaterApp theApp;


// CUpdaterApp initialization

BOOL CUpdaterApp::InitInstance()
{
	CWinApp::InitInstance();
	
	CString cstrCmdLine(m_lpCmdLine);
	if(cstrCmdLine != APP_CMDLINE)
	{
		::MessageBox(NULL, _T("You are not allowed to run updater!"), _T("iLUpdater Error"), MB_OK | MB_ICONEXCLAMATION | MB_TOPMOST);
		return FALSE;
	}

	CUpdaterDlg dlg;
  dlg.Create(IDD_UPDATER_DIALOG, NULL);
  dlg.ShowWindow(SW_HIDE);
  dlg.UpdateWindow();
  m_pMainWnd = &dlg;

	//CUpdaterDlg dlg;
	//m_pMainWnd = &dlg;
	//dlg.DoModal();

	return FALSE;
}
