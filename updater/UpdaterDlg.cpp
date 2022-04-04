// UpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include <wininet.h>
#include "updater.h"
#include "UpdaterDlg.h"
#include "TrayIcon.h"
#include "..\common\Registry.h"
#include "..\common\Logger.h"
#include "..\common\Crc32Dynamic.h"
#include "..\common\CommonDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DOWNLOAD_URL					_T("https://dl.dropboxusercontent.com/u/34761414/ilformater/")
#define UPDATE_INI_FILE				_T("updater.ini")
#define UPDATE_INI_CRC_FILE		_T("updater-crc.ini")

CUpdaterDlg *g_UpdateDlg = NULL;

CUpdaterDlg::CUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CUpdaterDlg::IDD, pParent)
{
	//m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_ctrUpdateFolder = _T("");
	m_ctrAppFolder = _T("");
	m_ctrUpdaterIni = _T("");
}

void CUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CUpdaterDlg, CDialog)
	//ON_WM_PAINT()
	//ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CUpdaterDlg message handlers

BOOL CUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//SetIcon(m_hIcon, TRUE);			// Set big icon
	//SetIcon(m_hIcon, FALSE);		// Set small icon
	
	g_UpdateDlg = this;

	g_Logger.GetLoggingPath();

	m_ctrAppFolder = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_APP_PATH);
	m_ctrAppFolder = m_ctrAppFolder + _T("\\");

	SoftwareUpdateThreadFunction();

	/*CWinThread *pThread = AfxBeginThread(SoftwareUpdateThread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	if(pThread != NULL)
	{
		pThread->ResumeThread();
	}*/

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//void CUpdaterDlg::OnPaint()
//{
//	CDialog::OnPaint();
//}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
//HCURSOR CUpdaterDlg::OnQueryDragIcon()
//{
//	return static_cast<HCURSOR>(m_hIcon);
//}

//{{Updater functions
void CUpdaterDlg::DeleteDirectory(CString cstrDir)
{
  //int len = _tcslen(lpszDir);
  //TCHAR *pszFrom = new TCHAR[len+2];
  //_tcscpy_s(pszFrom, len+2, lpszDir);
  //pszFrom[len] = 0;
  //pszFrom[len+1] = 0;
  //
  //SHFILEOPSTRUCT fileop;
  //fileop.hwnd   = NULL;    // no status display
  //fileop.wFunc  = FO_DELETE;  // delete operation
  //fileop.pFrom  = pszFrom;  // source file name as double null terminated string
  //fileop.pTo    = NULL;    // no destination needed
  //fileop.fFlags = FOF_NOCONFIRMATION|FOF_SILENT;  // do not prompt the user
  //
  //if(!noRecycleBin)
  //  fileop.fFlags |= FOF_ALLOWUNDO;

  //fileop.fAnyOperationsAborted = FALSE;
  //fileop.lpszProgressTitle     = NULL;
  //fileop.hNameMappings         = NULL;

  //int ret = SHFileOperation(&fileop);
  //delete [] pszFrom;  
  //return (ret == 0);

	WIN32_FIND_DATA info;
	int iPos = cstrDir.ReverseFind('\\');
	if((cstrDir.GetLength()-1) != iPos)
		cstrDir += _T("\\");
	CString cstrFile = cstrDir + _T("*.*");
	HANDLE hFile = FindFirstFile((LPCSTR)cstrFile, &info);
	while(FindNextFile(hFile, &info) == FALSE)
	{
		::DeleteFile(info.cFileName);
	}
}

bool CUpdaterDlg::GetFile(CString szUrl, CString szFileName) 
{
	HINTERNET hOpen, hConnect;
	DWORD dwSize, lpNumBytes;
	TCHAR szHead[] = _T("Accept: */*\r\n\r\n");
	VOID *szTemp[512];
	HANDLE hFile;

	hOpen = InternetOpen(_T("InetURL/1.0"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if( !(hConnect = InternetOpenUrl ( hOpen, (LPCTSTR)szUrl, szHead, _tcslen (szHead), INTERNET_FLAG_DONT_CACHE, 0)))
	{
		return false;
	}

	hFile = CreateFile((LPCTSTR)szFileName, GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == NULL)
	{
		return false;
	}

	while (1)
	{
		if(!InternetReadFile (hConnect, szTemp, 511,  &dwSize))
		{
			CloseHandle(hFile);
			return false;
		}
		if (!dwSize)
			break;  
		else
			WriteFile(hFile, szTemp, dwSize, &lpNumBytes, NULL);
	}

	CloseHandle(hFile);

	return true;
}

void CUpdaterDlg::StopApplication()
{
	cTray.ShowBalloon(_T("iLFormater"), _T("Applying iLFormater update. Please wait.."));

	HWND hApp = NULL;

	hApp = ::FindWindow(_T("#32770"), _T("iLFormater"));
	if(IsWindow(hApp) == TRUE)
	{
		DWORD dID = 0;
		::GetWindowThreadProcessId(hApp, &dID);
		HANDLE hProc = ::OpenProcess(PROCESS_TERMINATE, TRUE, dID);
		TerminateProcess(hProc, 0);
		CloseHandle(hProc);

		Sleep(500);
	}

	//HWND hTray = ::FindWindowEx(::FindWindow(_T("Shell_TrayWnd"), 0), 0, _T("TrayNotifyWnd"), 0);
	//HWND hSysPager = ::FindWindowEx(hTray, 0, _T("SysPager"), 0);
	//HWND hNotiArea = ::FindWindowEx(hSysPager, 0, 0, _T("Notification Area"));

	//::SendMessage(hNotiArea, WM_MOUSEMOVE, 0, 0);
}

bool CUpdaterDlg::CheckFileCRC(LPCTSTR chFileName, LPCTSTR chCRC)
{
	bool bReturn = false;
	DWORD dwCrc32, dwErrorCode = NO_ERROR;
	CCrc32Dynamic *pobCrc32Dynamic = new CCrc32Dynamic;

	pobCrc32Dynamic->Init();
	dwErrorCode = pobCrc32Dynamic->FileCrc32Streams(chFileName, dwCrc32);
	pobCrc32Dynamic->Free();

	delete pobCrc32Dynamic;

	CString cstrCalculatedCRC;
	if(dwErrorCode == NO_ERROR)
	{
		cstrCalculatedCRC.Format(_T("0x%08x"), dwCrc32);
		if(cstrCalculatedCRC.Compare(chCRC) == 0)
		{
			bReturn = true;
		}
		else
		{
			cstrCalculatedCRC.Format(_T("CRC match failed for file '%s'"), chFileName);
			CT2CA szLogTxt(cstrCalculatedCRC);
			std::string sLogTxt(szLogTxt);
			g_Logger.LogEvent("Updater", sLogTxt);
		}
	}
	else
	{
		cstrCalculatedCRC.Format(_T("Failed to calculate crc for file '%s' - Error: [0x%08x]"), chFileName, dwErrorCode);
		CT2CA szLogTxt(cstrCalculatedCRC);
		std::string sLogTxt(szLogTxt);
		g_Logger.LogEvent("Updater", sLogTxt);
	}

	return bReturn;
}

UINT SoftwareUpdateThread(LPVOID lParam)
{
	if(g_UpdateDlg != NULL)
	{
		g_UpdateDlg->SoftwareUpdateThreadFunction();
	}

	::PostMessage(g_UpdateDlg->m_hWnd, WM_CLOSE, 0, 0);

	return 0;
}

void CUpdaterDlg::SoftwareUpdateThreadFunction()
{
	CreateUpdateTempFolder();

	if(CheckForUpdate() == true)
	{
		if(DownloadNewVersion() == true)
		{
			if(CheckNewVersionCRC() == true)
			{
				StopApplication();
				InstallNewVersion();
				RunApplication();
			}
		}
	}
}

void CUpdaterDlg::CreateUpdateTempFolder()
{
	CString cstrVal = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("AppData"));
	cstrVal.Append(_T("\\iLFormater\\Updates\\"));
	//make sure '\\iLFormater' dir is already create. CLogger creating it from OnInitDialog, so no need to check here
	::CreateDirectory(cstrVal, NULL);
	m_ctrUpdateFolder = cstrVal;
}

bool CUpdaterDlg::CheckForUpdate()
{
	//Download ini file
	CString cstrURL = DOWNLOAD_URL;
	cstrURL = cstrURL + UPDATE_INI_FILE;
	CString cstrFile = m_ctrUpdateFolder + UPDATE_INI_FILE;
	m_ctrUpdaterIni = cstrFile;
	bool bGet = GetFile(cstrURL, cstrFile);
	if(bGet == false)
	{
		g_Logger.LogEvent("Updater", "Failed to download updater.ini file");
		return false;
	}
	//Check CRC of updater.ini file
	cstrURL = DOWNLOAD_URL;
	cstrURL = cstrURL + UPDATE_INI_CRC_FILE;
	cstrFile = m_ctrUpdateFolder + UPDATE_INI_CRC_FILE;
	bGet = GetFile(cstrURL, cstrFile);
	if(bGet == false)
	{
		g_Logger.LogEvent("Updater", "Failed to download updater-crc.ini file");
		return false;
	}
	Sleep(200);
	TCHAR chGotCRC[20] = _T("");
	GetPrivateProfileString(_T("updater"), _T("crc"), NULL, chGotCRC, 20, cstrFile);

	//Calculate the downloaded ini file crc
	cstrFile = m_ctrUpdateFolder + UPDATE_INI_FILE;
	bool bCRCCheck = CheckFileCRC((LPCTSTR)cstrFile, (LPCTSTR)chGotCRC);
	if(bCRCCheck == false)
	{
		return false;
	}
	
	//Read version updater.ini file
	TCHAR iniVer[10] = _T("");
	GetPrivateProfileString(_T("programinfo"), _T("version"), NULL, iniVer, 10, cstrFile);
	if(_tcscmp(iniVer, _T("")) == 0)
	{
		g_Logger.LogEvent("Updater", "Blank version found in updater.ini");
		return false;
	}
	//Read version from registry
  CString cstrVer = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_VER);
	if(HigherVersionFound(cstrVer, iniVer) == true)
	{
		CString cstrLogTxt;
		cstrLogTxt.Format(_T("Version: %s is available for download"), iniVer);
		CT2CA sLogTxt(cstrLogTxt);
		std::string strLogTxt(sLogTxt);
		g_Logger.LogEvent("Updater", strLogTxt);
		return true;
	}

	g_Logger.LogEvent("Updater", "No newer version is available for download");
	return false;
}

bool CUpdaterDlg::HigherVersionFound(CString cstrRegVer, CString cstrGotVer)
{
	bool bReturn = false;

	CString cstrRegVerMajor, cstrRegVerMinor, cstrRegVerRev;
	AfxExtractSubString(cstrRegVerMajor, cstrRegVer, 0, '.');
	AfxExtractSubString(cstrRegVerMinor, cstrRegVer, 1, '.');
	AfxExtractSubString(cstrRegVerRev, cstrRegVer, 2, '.');

	CString cstrGotVerMajor, cstrGotVerMinor, cstrGotVerRev;
	AfxExtractSubString(cstrGotVerMajor, cstrGotVer, 0, '.');
	AfxExtractSubString(cstrGotVerMinor, cstrGotVer, 1, '.');
	AfxExtractSubString(cstrGotVerRev, cstrGotVer, 2, '.');

	if((_ttoi(cstrGotVerMajor) > _ttoi(cstrRegVerMajor)) ||
			(_ttoi(cstrGotVerMinor) > _ttoi(cstrRegVerMinor)) ||
			(_ttoi(cstrGotVerRev) > _ttoi(cstrRegVerRev)))
	{
		bReturn = true;
	}

	return bReturn;
}

bool CUpdaterDlg::DownloadNewVersion()
{
	bool bUpdateNow = false;
	CString cstrAutoUpdate = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_AUTO_UPDATE);
  cstrAutoUpdate.MakeUpper();
	if(cstrAutoUpdate == _T("TRUE"))
	{
		bUpdateNow = true;
	}
	else
	{
		if(AfxMessageBox(_T("An update is available for iLFormater. Download now ?"), MB_YESNO | MB_ICONQUESTION | MB_TOPMOST) == IDYES)
			bUpdateNow = true;
	}

	if(bUpdateNow == false)
		return false;

	cTray.SetTrayIcon(this->m_hWnd, AfxGetInstanceHandle(), _T("iLFormater updater"));
	cTray.ShowBalloon(_T("iLFormater"), _T("Updating iLFormater. Please wait.."));

	bool bReturn = true;
	short iFileCount = 1;
	while(1)
	{
		TCHAR chFileNo[6] = _T("");
		_stprintf_s(chFileNo, _countof(chFileNo), _T("file%d"), iFileCount);
		TCHAR chData[50] = _T("");
		GetPrivateProfileString(_T("Updates"), chFileNo, NULL, chData, 50, m_ctrUpdaterIni);
		CString cstrData(chData);

		if(cstrData == _T(""))
			break;

		m_cArrUpdateData.Add(cstrData);

		CString chFileName, chFileCRC;
		AfxExtractSubString(chFileName, cstrData, 0, '|');
		AfxExtractSubString(chFileCRC, cstrData, 1, '|');
		
		if((chFileName.IsEmpty() == false) && (chFileCRC.IsEmpty() == false))
		{
			CString cstrURL = DOWNLOAD_URL;
			cstrURL = cstrURL + chFileName;
			CString cstrFile = m_ctrUpdateFolder + chFileName;
						
			bool bRet = GetFile(cstrURL, cstrFile);
			if(bRet == false)
			{
				CString cstrLogText = _T("Failed to download file: ") + cstrFile;
				CT2CA sLogText(cstrLogText);
				std::string strLogText(sLogText);
				g_Logger.LogEvent("Updater", strLogText);
				bReturn = false;
				break;
			}
		}
		iFileCount++;
	}
	
	if(iFileCount == 1)
	{
		cTray.ShowBalloon(_T("iLFormater"), _T("Failed to download iLFormater update. Try to update later."));
		bReturn = false;
	}

	return bReturn;
}

bool CUpdaterDlg::CheckNewVersionCRC()
{
	int iFileCount = m_cArrUpdateData.GetCount();
	if(iFileCount <= 0)
	{
		g_Logger.LogEvent("Updater", "No files downloaded. Quiting crc check !");
		return false;
	}

	for(int i=0; i<iFileCount; i++)
	{
		CString cstrData = m_cArrUpdateData.GetAt(i);
		CString cstrFileName = _T("");
		CString cstrCRC = _T("");
		AfxExtractSubString(cstrFileName, cstrData, 0, '|');
		AfxExtractSubString(cstrCRC, cstrData, 1, '|');

		CString cstrNewFile = m_ctrUpdateFolder + cstrFileName;
		bool bCRC = CheckFileCRC(cstrNewFile, cstrCRC);
		if(bCRC == false)
		{
			CString cstrLogText = _T("CRC checking failed for file: ") + cstrFileName;
			CT2CA sLogText(cstrLogText);
			std::string strLogText(sLogText);
			g_Logger.LogEvent("Updater", strLogText);
			return false;
		}
	}

	return true;
}

void CUpdaterDlg::InstallNewVersion()
{
	//CString cstTempFolder = m_ctrAppFolder + _T("\\CurrentVersion\\");
	//::CreateDirectory(cstTempFolder, NULL);
	//WIN32_FIND_DATA info;
	//CString cstrFile = m_ctrAppFolder + _T("*.*");
	//HANDLE hFile = FindFirstFile((LPCSTR)cstrFile, &info);
	//while(FindNextFile(hFile, &info) == FALSE)
	//{
	//	CString cstrFileTemp = cstTempFolder + info.cFileName;
	//	CString cstrFile = m_ctrAppFolder + info.cFileName;
	//	::CopyFile(cstrFile, cstrFileTemp, FALSE);
	//}

	int iFileCount = m_cArrUpdateData.GetCount();
	for(int i=0; i<iFileCount; i++)
	{
		CString cstrData = m_cArrUpdateData.GetAt(i);
		CString cstrFileName = _T("");
		AfxExtractSubString(cstrFileName, cstrData, 0, '|');

		CString cstrNewFile = m_ctrUpdateFolder + cstrFileName;
		CString cstrOldFile = m_ctrAppFolder + cstrFileName;

		::CopyFile(cstrNewFile, cstrOldFile, FALSE);
	}

	DeleteDirectory(m_ctrUpdateFolder);
}

void CUpdaterDlg::RunApplication()
{
  CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_UPDATESTAT, _T("1"));
	Sleep(1000);
  CString cstrOldVersion = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_VER);
	CString cstrApplication = m_ctrAppFolder + APP_EXE;
	ShellExecute(NULL, _T("open"), (LPCTSTR)cstrApplication,_T(""), (LPCTSTR)m_ctrAppFolder, SW_SHOW);
	CString cstrVersion = WaitForNewVersionWrite(cstrOldVersion);
	cstrVersion = _T("iLFormater updated to v") + cstrVersion;
	cTray.ShowBalloon(_T("iLFormater"), (LPCTSTR)cstrVersion);
  CRegistryOperation::SetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_UPDATESTAT, _T("0"));
	Sleep(10 * 1000);
}

CString CUpdaterDlg::WaitForNewVersionWrite(CString csOldVer)
{
  CString cstrNewVersion = _T("");

  DWORD dwStartTime = GetTickCount();
  DWORD dwEndTime = 0;
  while(1)
  {
    dwEndTime = GetTickCount();
    cstrNewVersion = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, APP_REGPATH, REG_VAL_VER);
    if((csOldVer != cstrNewVersion) || ((dwEndTime - dwStartTime) >= 1000*40))
    {
      break;
    }
    Sleep(400);
  }

  return cstrNewVersion;
}
