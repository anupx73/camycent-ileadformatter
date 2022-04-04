#pragma once

class CRegistryOperation
{
public:
	static CString GetRegValue(HKEY hHeadKey, CString cstrPath, CString cstrValue);
	static void SetRegValue(HKEY hHeadKey, CString cstrPath, CString cstrValue, CString cstrData);
	static void SetRegBinaryValue(HKEY hHeadKey, CString cstrPath, CString cstrValue, int nValue);
	static int GetRegBinaryValue(HKEY hHeadKey, CString cstrPath, CString cstrValue);
};