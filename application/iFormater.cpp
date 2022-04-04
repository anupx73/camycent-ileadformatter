// iFormater.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "iFormater.h"
#include "iFormaterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CiFormaterApp

BEGIN_MESSAGE_MAP(CiFormaterApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CiFormaterApp construction

CiFormaterApp::CiFormaterApp()
{
}


// The one and only CiFormaterApp object

CiFormaterApp theApp;


// CiFormaterApp initialization

BOOL CiFormaterApp::InitInstance()
{
	//Only one instance of the application
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, _T("{35D72DF6-E334-46f5-A0CD-E13F31AC7882-ILFORMATER}"));
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		//::MessageBox(NULL, _T("Application already running"), _T("iLFormater"), MB_OK | MB_ICONEXCLAMATION);
		::CloseHandle(hMutex);
		hMutex = NULL;
		return FALSE;
	}

	CWinApp::InitInstance();
	
	CiFormaterDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	::CloseHandle(hMutex);
	hMutex = NULL;

	return FALSE;
}
