#include "Registry.h"

void CRegistryOperation::SetRegValue(HKEY hHeadKey, CString cstrPath, CString cstrValue, CString cstrData)
{
	HKEY hKey;
	TCHAR szData[255] = _T("");
	_tcscpy_s(szData, 254, (LPCTSTR)cstrData);

	::RegCreateKeyEx(hHeadKey, (LPCTSTR)cstrPath, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0);
	::RegSetValueEx(hKey, (LPCTSTR)cstrValue, 0, REG_SZ, (LPBYTE)szData, sizeof(TCHAR) * (_tcslen(szData) + 1));
	::RegCloseKey(hKey);
}

CString CRegistryOperation::GetRegValue(HKEY hHeadKey, CString cstrPath, CString cstrValue)
{
	HKEY hKey;
	TCHAR szBuff[255] = _T("");
	DWORD dwBufSize = 255;

	::RegOpenKeyEx(hHeadKey, (LPCTSTR)cstrPath, 0, KEY_READ, &hKey);
	::RegQueryValueEx(hKey, (LPCTSTR)cstrValue, NULL, 0, (LPBYTE)szBuff, &dwBufSize);
	::RegCloseKey(hKey);

	CString cstr(szBuff);

	return cstr;
}

void CRegistryOperation::SetRegBinaryValue(HKEY hHeadKey, CString cstrPath, CString cstrValue, int nValue)
{
	HKEY hKey;

	::RegCreateKeyEx(hHeadKey, (LPCTSTR)cstrPath, 0, 0, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, 0, &hKey, 0);
	::RegSetValueEx(hKey, (LPCTSTR)cstrValue, 0, REG_BINARY, (LPBYTE)&nValue, sizeof(nValue));
	::RegCloseKey(hKey);
}

int CRegistryOperation::GetRegBinaryValue(HKEY hHeadKey, CString cstrPath, CString cstrValue)
{
	int iValue = 0;
	DWORD dwSize = sizeof(iValue);
	HKEY hKey;

	::RegOpenKeyEx(hHeadKey, (LPCTSTR)cstrPath, 0, KEY_READ, &hKey);
	::RegQueryValueEx(hKey, (LPCTSTR)cstrValue, NULL, 0, (LPBYTE)&iValue, &dwSize);
	::RegCloseKey(hKey);

	return iValue;
}