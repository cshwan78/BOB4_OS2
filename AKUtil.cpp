/*-----------------------------------------------------------------------------
* AKUtil.cpp
*-----------------------------------------------------------------------------
*
*-----------------------------------------------------------------------------
* All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#include "stdafx.h"
#include "AKUtil.h"

HANDLE advOpenProcess(DWORD dwPid, DWORD dwRights, BOOL RaisePrivilege)
{
	HANDLE hProc = NULL;

	// enable SeDebugPrivilege              
	//
	if (TRUE == RaisePrivilege)
	{
		if (TRUE != SetPrivilege(SE_DEBUG_NAME, TRUE))
		{
			_ASSERTE(!"SetPrivilege");
			return NULL;
		}
	}


	// open the process
	//
	hProc = OpenProcess(dwRights, FALSE, dwPid);
	if (hProc == NULL)
	{
		// º¸¾È ÇÁ·Î±×·¥µîÀÇ °æ¿ì OpenProcess() °¡ ½ÇÆÐÇÒ ¼ö ÀÖÀ½
		//
		//_ASSERTE(!"OpenProcess");
		return NULL;
	}

	// disable SeDebugPrivilege
	//
	if (TRUE == RaisePrivilege)
	{
		if (TRUE != SetPrivilege(SE_DEBUG_NAME, FALSE))
		{
			_ASSERTE(!"SetPrivilege");

			if (NULL != hProc)
			{
				CloseHandle(hProc); hProc = NULL;
			}
		}
	}

	return hProc;
}


/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
BOOL WriteToFileW(LPCWCH file_path, LPCWCH format, ...)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH)))
	{
		return FALSE;
	}

	HANDLE hFile = CreateFileW(
		file_path,
		GENERIC_WRITE,
		FILE_SHARE_READ,
		NULL,
		OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		RefStringW  rs = FormatErrorStringW(GetLastError());
		return FALSE;
	}

	// ÀÚ¿ø °ü¸® °´Ã¼ »ý¼º
	//
	SmrtHandle file_handle(hFile);

	int pos = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (pos != -1) // Test for failure
	{
		va_list args;
		DWORD dw = 0;
		WCHAR temp[5120] = { 0 };
		WCHAR* pszDestEnd = temp;
		size_t cbRemaining = sizeof(temp);


		va_start(args, format);
		if (TRUE != SUCCEEDED(StringCbVPrintfExW(
			pszDestEnd,
			cbRemaining,
			&pszDestEnd,
			&cbRemaining,
			0,
			format,
			args)))
		{
			_ASSERTE(!"StringCbVPrintf");
			va_end(args);
			return FALSE;
		}
		va_end(args);

		if (!SUCCEEDED(StringCbPrintfExW(
			pszDestEnd,
			cbRemaining,
			&pszDestEnd,
			&cbRemaining,
			0,
			L"%ws",
			L"\r\n")))
		{
			_ASSERTE(!"StringCbPrintfEx()");
			return FALSE;
		}
		WriteFile(hFile, temp, (DWORD)((DWORD_PTR)pszDestEnd - (DWORD_PTR)temp), &dw, NULL);
	}

	// SmrtHandle °´Ã¼°¡ hFile ¿¡ ´ëÇØ ¾Ë¾Æ¼­ Ã³¸®ÇØÁØ´Ù. :D
	return TRUE;            // for success
}

/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
BOOL WriteToFileW(HANDLE hFile, LPCWCH format, ...)
{
	int pos = SetFilePointer(hFile, 0, NULL, FILE_END);
	if (pos != -1) // Test for failure
	{
		va_list args;
		DWORD dw = 0;
		WCHAR temp[5120] = { 0 };
		WCHAR* pszDestEnd = temp;
		size_t cbRemaining = sizeof(temp);


		va_start(args, format);
		if (TRUE != SUCCEEDED(StringCbVPrintfExW(
			pszDestEnd,
			cbRemaining,
			&pszDestEnd,
			&cbRemaining,
			0,
			format,
			args)))
		{
			_ASSERTE(!"StringCbVPrintf");
			va_end(args);
			return FALSE;
		}
		va_end(args);

		if (!SUCCEEDED(StringCbPrintfExW(
			pszDestEnd,
			cbRemaining,
			&pszDestEnd,
			&cbRemaining,
			0,
			L"%ws",
			L"\r\n")))
		{
			_ASSERTE(!"StringCbPrintfEx()");
			return FALSE;
		}
		WriteFile(hFile, temp, (DWORD)((DWORD_PTR)pszDestEnd - (DWORD_PTR)temp), &dw, NULL);
	}

	return TRUE;            // for success
}


/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
BOOL IsFileExistA(const CHAR* lpszFilename)
{
	_ASSERTE(NULL != lpszFilename);
	_ASSERTE(TRUE != IsBadStringPtrA(lpszFilename, MAX_PATH));
	if ((NULL == lpszFilename) ||
		(TRUE == IsBadStringPtrA(lpszFilename, MAX_PATH)))
	{
		return FALSE;
	}

	std::wstring wstr = MbsToWcs(lpszFilename);
	return IsFileExistW(wstr.c_str());
}

/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
BOOL IsFileExistW(const WCHAR* lpszFilename)
{
	_ASSERTE(NULL != lpszFilename);
	_ASSERTE(TRUE != IsBadStringPtrW(lpszFilename, MAX_PATH));
	if ((NULL == lpszFilename) || (TRUE == IsBadStringPtrW(lpszFilename, MAX_PATH))) return FALSE;

	HANDLE hFile = CreateFileW(
		lpszFilename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return TRUE;
	}
	else
		return FALSE;
}

/** ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
WCHAR* MbsToWcs(IN const char* mbs)
{
	_ASSERTE(NULL != mbs);
	if (NULL == mbs) return NULL;

	int outLen = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbs, -1, NULL, 0);
	if (0 == outLen) return NULL;

	WCHAR* outWchar = (WCHAR*)malloc(outLen * (sizeof(WCHAR)));  // outLen contains NULL char 
	if (NULL == outWchar) return NULL;
	RtlZeroMemory(outWchar, outLen);

	if (0 == MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbs, -1, outWchar, outLen))
	{
		FreeAndNil(outWchar);
		return NULL;
	}

	return outWchar;
}

/** ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
CHAR* WcsToMbs(IN const WCHAR* wcs)
{
	_ASSERTE(NULL != wcs);
	if (NULL == wcs) return NULL;

	int outLen = WideCharToMultiByte(CP_ACP, 0, wcs, -1, NULL, 0, NULL, NULL);
	if (0 == outLen) return NULL;

	CHAR* outChar = (CHAR*)malloc(outLen * sizeof(CHAR));
	if (NULL == outChar) return NULL;
	RtlZeroMemory(outChar, outLen);

	if (0 == WideCharToMultiByte(CP_ACP, 0, wcs, -1, outChar, outLen, NULL, NULL))
	{
		FreeAndNil(outChar);
		return NULL;
	}

	return outChar;
}


/**     ---------------------------------------------------------------------------
\brief
http://support.microsoft.com/kb/131065/EN-US/
\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
BOOL
SetPrivilege(
LPCTSTR Privilege,      // Privilege to enable/disable
BOOL bEnablePrivilege   // TRUE to enable. FALSE to disable
)
{
	if (ISNTFAMILY(OsVersion()))
	{
		HANDLE hToken;
		if (OpenThreadToken(GetCurrentThread(),
			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken) != TRUE)
		{
			if (GetLastError() == ERROR_NO_TOKEN)
			{
				if (ImpersonateSelf(SecurityImpersonation) != TRUE)
				{
					_ASSERTE(!"Impersonate");
					return FALSE;
				}

				if (OpenThreadToken(
					GetCurrentThread(),
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					FALSE,
					&hToken) != TRUE)
				{
					_ASSERTE(!"OpenThreadToken");
					return FALSE;
				}
			}
			else
			{
				_ASSERTE(!"OpenThread");
				return FALSE;
			}
		}

		TOKEN_PRIVILEGES tp = { 0 };
		LUID luid = { 0 };
		DWORD cb = sizeof(TOKEN_PRIVILEGES);
		if (!LookupPrivilegeValue(NULL, Privilege, &luid))
		{
			_ASSERTE(!"Lookup");

			CloseHandle(hToken);
			return FALSE;
		}
		tp.PrivilegeCount = 1;
		tp.Privileges[0].Luid = luid;
		if (bEnablePrivilege) {
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
		}
		else {
			tp.Privileges[0].Attributes = 0;
		}
		AdjustTokenPrivileges(hToken, FALSE, &tp, cb, NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
		{
			_ASSERTE(!"Adjust");

			CloseHandle(hToken);
			return FALSE;
		}

		CloseHandle(hToken);
	}

	return TRUE;
}


/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
WORD OsVersion(void)
{
	OSVERSIONINFOEX osvi;
	BOOL bOsVersionInfoEx;

	// Try calling GetVersionEx using the OSVERSIONINFOEX structure.
	// If that fails, try using the OSVERSIONINFO structure.
	//
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO *)&osvi);
	if (bOsVersionInfoEx == FALSE)
	{
		osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
		if (!GetVersionEx((OSVERSIONINFO *)&osvi))
			return 0;
	}

	switch (osvi.dwPlatformId)
	{
		// Test for the Windows NT product family.
	case VER_PLATFORM_WIN32_NT:

		// Test for the specific product family.
		if (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
			return _WINLONGHORN;

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2)
			return _WIN2003;

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			return _WINXP;

		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			return _WIN2000;

		if (osvi.dwMajorVersion <= 4)
			return _WINNT;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			/*
			printf ("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
			printf("OSR2 " );
			*/
			return _WIN95;
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{

			// printf ("Microsoft Windows 98 ");

			if (osvi.szCSDVersion[1] == 'A') {
				return _WINSE;
				// printf("SE " );
			}

			return _WIN98;
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
		{
			return _WINME;
			/*
			printf ("Microsoft Windows Millennium Edition\n");
			*/
		}
		break;

	case VER_PLATFORM_WIN32s:

		/*
		printf ("Microsoft Win32s\n");
		*/
		break;
	}

	return 0;
}

/** ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
RefStringW FormatErrorStringW(DWORD errcode)
{
	WCHAR* msg = NULL;
	if (0 == FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		errcode,
		0,
		(WCHAR*)&msg,
		0,
		NULL))
	{
		msg = FormatStringW(
			L"FormatMessage(requested err code=0x%08x) failed, error code=0x%08x",
			errcode,
			GetLastError()
			);
	}

	RefStringW rs(msg);
	LocalFree(msg);
	return rs;
}


/** ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
RefStringW FormatStringW(LPCWCH format, ...)
{
	va_list args;
	va_start(args, format);
	WCHAR tmp[1024] = { 0 };

	if (!SUCCEEDED(StringCbVPrintfW(
		tmp,
		sizeof(tmp),
		format,
		args)))
	{
		_ASSERTE(!"StringCbVPrintfW");
		return NULL;
	}

	RefStringW rs(tmp);
	return (rs);
}


/**     ---------------------------------------------------------------------------
\brief

\param
\return
\code
\endcode
-----------------------------------------------------------------------------*/
int DumpMemory(FILE* stream, DWORD Length, BYTE* Buf)
{
	if ((0 < Length) && (NULL != Buf) && (TRUE != IsBadReadPtr(Buf, Length)))
	{
		_ftprintf(stream, _T("\n  00 01 02 03 04 05 06 07   08 09 0A 0B 0C 0D 0E 0F\n"));
		_ftprintf(stream, _T("  -- -- -- -- -- -- -- --   -- -- -- -- -- -- -- --\n"));

		TCHAR print_buf[128 * sizeof(TCHAR)] = { 0 };
		DWORD i = 0, x = 0;
		UCHAR*  Addr = Buf;
		TCHAR*  Pos = NULL;
		size_t  Remain = 0;
		for (;;)
		{
			if (i >= Length) break;

			// reset all
			//
			Pos = print_buf;
			Remain = sizeof(print_buf);

			// first 8 bytes
			//
			for (x = 0; x < 8; x++, i++)
			{
				if (!SUCCEEDED(StringCbPrintfEx(
					Pos,
					Remain,
					&Pos,
					&Remain,
					0,
					_T("%02X "),
					Addr[i])))
				{
					_ftprintf(stream, _T("StringCbPrintfEx() failed \n"));
					break;
				}
			}

			// insert space between first 8bytes and last 8 bytes.
			//
			if (!SUCCEEDED(StringCbPrintfEx(
				Pos,
				Remain,
				&Pos,
				&Remain,
				0,
				_T("%s"),
				_T("  "))))
			{
				_ftprintf(stream, _T("StringCbPrintfEx() failed \n"));
				break;
			}

			// last 8 bytes
			//
			for (x = 0; x < 8; x++, i++)
			{
				if (!SUCCEEDED(StringCbPrintfEx(
					Pos,
					Remain,
					&Pos,
					&Remain,
					0,
					_T("%02X "),
					Addr[i])))
				{
					_ftprintf(stream, _T("StringCbPrintfEx() failed \n"));
					break;
				}
			}

			// print string..
			//
			_ftprintf(stream, _T("  %s\n"), print_buf);
			memset(print_buf, 0x00, sizeof(print_buf));
		}

		_ftprintf(stream, _T("  %s\n\n"), print_buf);
		return 0;
	}

	_ftprintf(stream, _T("%s > invalid parameters \n"), __FUNCTION__);
	return -1;
}