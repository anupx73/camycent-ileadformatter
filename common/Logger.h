#pragma once
#include <string>

class CLogger
{
private:
	std::string m_strPath;
public:
	CLogger(void);
	~CLogger(void);
	//void LogEvent(CString EvntName, CString EvntVal = _T(""));
	//void LogEvent(CString EvntName, int EvntVal = 0);
	void LogEvent(std::string EvntName, std::string EvntVal = "");
	void LogEvent(std::string EvntName, int EvntVal = 0);
	CString GetLogTime();
	void GetLoggingPath();
};

extern CLogger g_Logger;
