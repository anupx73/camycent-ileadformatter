#include "Logger.h"
#include "Registry.h"
#include <fstream>

CLogger g_Logger;

CLogger::CLogger(void)
{
	m_strPath = "";
}

CLogger::~CLogger(void)
{
}

void CLogger::GetLoggingPath()
{
	CString cstrVal = CRegistryOperation::GetRegValue(HKEY_CURRENT_USER, _T("Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"), _T("AppData"));	
	
	cstrVal.Append(_T("\\WinCPPAuthener"));
	::CreateDirectory(cstrVal, NULL);
	cstrVal.Append(_T("\\Log.txt"));

	CT2CA szPath(cstrVal);
	m_strPath = szPath;
}

//void CLogger::LogEvent(CString EvntName, CString EvntVal)
//{
//	CString log = GetLogTime();
//	log.Append(EvntName);
//	log.Append(_T(" ------ "));
//	log.Append(EvntVal);
//	//Logging
//	std::ofstream logger;
//	logger.open(m_strPath.c_str() , std::ofstream::app); //casting std::string to char *
//	logger<<log<<std::endl;
//	logger.close();
//}
//void CLogger::LogEvent(CString EvntName, int EvntVal)
//{
//	CString log = GetLogTime();
//	log.Append(EvntName);
//	log.Append(_T(" ------ "));
//	//Logging
//	std::ofstream logger;
//	logger.open(m_strPath.c_str() , std::ofstream::app); //casting std::string to char *
//	logger<<log<<EvntVal<<std::endl;
//	logger.close();
//	
//}
void CLogger::LogEvent(std::string EvntName , std::string EvntVal)
{
	CString log = GetLogTime();
	CT2CA szlog(log); 
	std::string logstr(szlog);
	logstr.append(EvntName);
	logstr.append(" ------ ");
	logstr.append(EvntVal); 
	// Loging	
	std::ofstream logger; 
	logger.open(m_strPath.c_str() , std::ofstream::app); //casting std::string to char *
	logger<<logstr<<std::endl;
	logger.close();

}
void CLogger::LogEvent(std::string EvntName , int EvntVal)
{
	CString log = GetLogTime();
	CT2CA szlog(log);  
	std::string logstr(szlog);
	logstr.append(EvntName); 
	logstr.append(" ------ ");
	// Loging
	std::ofstream logger;   
	logger.open(m_strPath.c_str() , std::ofstream::app); //casting std::string to char *
	logger<<logstr<<EvntVal<<std::endl;
	logger.close();
}

CString CLogger::GetLogTime()
{
	CString cstrLog; 
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	WORD day		= sysTime.wDay;
	WORD month	= sysTime.wMonth;
	WORD year		= sysTime.wYear; 
	WORD hour		= sysTime.wHour; 
	WORD min		= sysTime.wMinute; 
	WORD sec		= sysTime.wSecond;

	cstrLog.Format(_T("%02u/%02u/%u %02u:%02u:%02u ------ "), month, day, year, hour, min, sec);

	return cstrLog;
}
