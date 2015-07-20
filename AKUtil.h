/*-----------------------------------------------------------------------------
* AKUtil.h
*-----------------------------------------------------------------------------
*
*-----------------------------------------------------------------------------
* All rights reserved by somma (fixbrain@gmail.com, unsorted@msn.com)
**---------------------------------------------------------------------------*/
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <string>


// Window ÇÃ·§ÆûÄÚµå
//
#define OSTYPE_WIN_95           0x0400
#define OSTYPE_WIN_98           0x0410
#define OSTYPE_WIN_SE           0x0410
#define OSTYPE_WIN_ME           0x0490

#define OSTYPE_WIN_NT           0x0400

#define OSTYPE_WIN_2000         0x0500
#define OSTYPE_WIN_XP           0x0501
#define OSTYPE_WIN_2003         0x0502
#define OSTYPE_WIN_LONGHORN     0x0600


/// ÆíÀÇ»ó ÀçÁ¤ÀÇ
// windows 95 family °è¿­Àº 0x000F ¸¶½ºÅ©Àû¿ë

#define         _WIN95                  0x0001
#define         _WIN98                  0x0002
#define         _WINSE                  0x0004
#define         _WINME                  0x0008

// windows nt family °è¿­Àº 0x00F0 ¸¶½ºÅ©Àû¿ë

#define         _WINNT                  0x0010
#define         _WIN2000                0x0020
#define         _WINXP                  0x0040
#define         _WIN2003                0x0080
#define         _WINLONGHORN    0x00c0

#define         MASK_WIN95              0x000F
#define         MASK_WINNT              0x00F0

/// Windows NT/2000/XP/2003/LONGHORN ÆÐ¹Ð¸®ÀÎÁö¸¦ È®ÀÎÇÏ´Â ¸ÅÅ©·Î
//
//  < 2006.03.31 ³ë¿ëÈ¯ >
//  ISNTFAMILY(GetWinVersion()) ÇüÅÂ·Î »ç¿ëÇÏ¸é µÇ´Âµ¥ 
//      Àý´ë·Î 
//              if ( ISNTFAMILY(GetWinVersion()) == TRUE ) 
//              {
//                      // for ntfamily code 
//                      //
//              }
//      ÇüÅÂ·Î »ç¿ëÇÏ¸é ¾ÈµÈ´Ù. 
//      TRUE ´Â windef.h (¶Ç´Â ´Ù¸¥ µ¥¼­..) ´Ü¼øÈ÷ 1 ·Î Á¤ÀÇµÇ¾îÀÖÀ» »ÓÀÌ¹Ç·Î
//      nt family ÀÎ °æ¿ì¶ó ÇÏ´õ¶óµµ TRUE ¸¦ ¸®ÅÏÇÒ ¼ö ¾øÀ¸¹Ç·Î ¹ö±×¸¦ ¸¸µé¼ö ÀÖ´Ù.
//      
//              if ( ISNTFAMILY(GetWinVersion()  ) 
//              {
//                      // for ntfamily code 
//                      //
//              }
//
//      ¶Ç´Â 
// 
//              if ( ISNTFAMILY(GetWinVersion() != 0 ) 
//              {
//                      // for ntfamily code 
//                      //
//              }
// ÇüÅÂ·Î »ç¿ëÇØ¾ß ÇÑ´Ù. 
//
#define ISNTFAMILY(x)   (x & MASK_WINNT)

/// Windows 95/98/SE/ME ÆÐ¹Ð¸®ÀÎÁö¸¦ È®ÀÎÇÏ´Â ¸ÅÅ©·Î
#define IS95FAMILY(x)   (x & MASK_WIN95)



// ÁÖÀÇ 
// new ¸¦ ÅëÇØ¼­ »ý¼ºÇÏ¸é ¸Þ¸ð¸® ¸¯ »ý±è, ²À Áö¿ªº¯¼ö·Î¸¸ »ç¿ëÇÒ °Í
// 
// RefStringW °¡ °¡º¯ ÀÎÀÚ·Î »ç¿ëµÇ´Â °æ¿ì ¹Ýµå½Ã WCHAR* ·Î ¸í½ÃÀûÀ¸·Î 
// Å¸ÀÔ Ä³½ºÆÃÀ» ÇØ ÁÖ¾î¾ß ÇÔ. (¹¬½ÃÀûÀ¸·Î ¾îÄÉ ÇØ¾ß º¯È¯ÀÌ µÇ´Â°ÇÁö.. -__-)
//
class RefStringW
{
public:
	RefStringW(){};
	RefStringW(WCHAR* null_terminated_string)
	{
		if (NULL == null_terminated_string){ m_str = L""; }
		else{ m_str = null_terminated_string; }
	}
	~RefStringW(){}
	RefStringW(const RefStringW& rhs){ if (this->m_str != rhs.m_str) this->m_str = rhs.m_str; }
	RefStringW& operator = (const RefStringW& rhs)
	{
		if (this != &rhs){ this->m_str.clear(); this->m_str = rhs.m_str; }
		return *this;
	}
	operator wchar_t* () const { return (wchar_t*)m_str.c_str(); }
private:
	std::wstring m_str;
};


class UnCopyable
{
protected:
	UnCopyable(){};
	~UnCopyable(){};
private:
	UnCopyable(const UnCopyable& rhs);
	UnCopyable& operator= (const UnCopyable& rhs);
};


// handle wrapper
//
class SmrtHandle : public UnCopyable
{
public:
	SmrtHandle(HANDLE handle) : m_handle(handle) {}
	~SmrtHandle(){ CloseHandle(m_handle); m_handle = INVALID_HANDLE_VALUE; }
protected:
private:
	HANDLE m_handle;
};

// simple smart pointer
//
template <typename T>
class SmrtPtr : public UnCopyable
{
public:
	SmrtPtr(T& Ptr) : m_ptr(Ptr) {};
	~SmrtPtr()
	{
		if (NULL != m_ptr)
		{
			free(m_ptr); m_ptr = NULL;
		}
	};
private:
	T& m_ptr;
};



#define FreeAndNil(p)     do { if (NULL != p) { free(p); p = NULL;} } while(p != NULL);



HANDLE advOpenProcess(DWORD dwPid, DWORD dwRights, BOOL RaisePrivilege);
BOOL WriteToFileW(LPCWCH file_path, LPCWCH format, ...);
BOOL WriteToFileW(HANDLE hFile, LPCWCH format, ...);
BOOL IsFileExistA(const CHAR* lpszFilename);
BOOL IsFileExistW(const WCHAR* lpszFilename);
WCHAR* MbsToWcs(IN const char* mbs);
CHAR* WcsToMbs(IN const WCHAR* wcs);
WORD OsVersion(void);
BOOL SetPrivilege(LPCTSTR Privilege, BOOL bEnablePrivilege);
RefStringW FormatErrorStringW(DWORD errcode);
RefStringW FormatStringW(LPCWCH format, ...);
int DumpMemory(FILE* stream, DWORD Length, BYTE* Buf);

#ifdef UNICODE
#define IsFileExist         IsFileExistW
#else
#define IsFileExist     IsFileExistA
#endif//UNICODE
