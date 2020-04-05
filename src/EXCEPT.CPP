//******************************************************************
// except.cpp
//******************************************************************


#include "diablo.h"
#pragma hdrstop
#include <tchar.h>


//******************************************************************
// private
//******************************************************************
class CExcept {
public:
	CExcept();
	~CExcept();

private:
	// entry point where control comes on an unhandled exception
	static LONG WINAPI ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo);

	// variables used by the class
	static TCHAR m_szLogFileName[MAX_PATH];
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
};

TCHAR CExcept::m_szLogFileName[MAX_PATH];
LPTOP_LEVEL_EXCEPTION_FILTER CExcept::m_previousFilter;
static CExcept g_CExcept;


//******************************************************************
//******************************************************************
static void __cdecl tprintf(HANDLE hFile,LPCTSTR pszFmt,...) {
	va_list argptr;
	DWORD cbWritten;
	TCHAR szBuf[1024];
	va_start(argptr,pszFmt);
	int nChars = wvsprintf(szBuf,pszFmt,argptr);
	WriteFile(hFile,szBuf,nChars * sizeof(TCHAR),&cbWritten,0);
	va_end(argptr);
}


//******************************************************************
// Given a linear address,locates the module,section,and offset containing
// that address.
//
// Note: the szModule paramater buffer is an output buffer of length specified
// by the len parameter (in characters!)
//******************************************************************
static BOOL GetLogicalAddress(
	PVOID addr,
	PTSTR szModule,
	DWORD len,
	DWORD *pdwSection,
	DWORD *pdwOffset
) {
	MEMORY_BASIC_INFORMATION mbi;
	if (!VirtualQuery(addr,&mbi,sizeof(mbi)))
		return FALSE;

	DWORD hMod = (DWORD)mbi.AllocationBase;
	if (!GetModuleFileName((HMODULE)hMod,szModule,len))
		return FALSE;

	// Point to the DOS header in memory
	PIMAGE_DOS_HEADER pDosHdr = (PIMAGE_DOS_HEADER)hMod;

	// From the DOS header,find the NT (PE) header
	PIMAGE_NT_HEADERS pNtHdr = (PIMAGE_NT_HEADERS)(hMod + pDosHdr->e_lfanew);
	PIMAGE_SECTION_HEADER pSection = IMAGE_FIRST_SECTION(pNtHdr);
	DWORD rva = (DWORD)addr - hMod; // RVA is offset from module load address

	// Iterate through the section table,looking for the one that encompasses
	// the linear address.
	for (
		unsigned i = 0;
		i < pNtHdr->FileHeader.NumberOfSections;
		i++,pSection++
	) {
		DWORD sectionStart = pSection->VirtualAddress;
		DWORD sectionEnd = sectionStart
					+ max(pSection->SizeOfRawData,pSection->Misc.VirtualSize);

		// Is the address in this section???
		if ((rva >= sectionStart) && (rva <= sectionEnd)) {
			// Yes,address is in the section.	Calculate section and offset
			*pdwSection = i+1;
			*pdwOffset = rva - sectionStart;
			return TRUE;
		}
	}

	return FALSE;	// Should never get here!
}


//******************************************************************
//******************************************************************
static void IntelStackWalk(HANDLE hFile,PCONTEXT pContext) {
	tprintf(hFile,_T("\r\nCall stack:\r\n"));
	tprintf(hFile,_T("Address	Frame	 Logical addr	Module\r\n"));

	DWORD pc = pContext->Eip;
	PDWORD pFrame,pPrevFrame;
	pFrame = (PDWORD)pContext->Ebp;
	while (1) {
		TCHAR szModule[MAX_PATH] = _T("");
		DWORD section = 0,offset = 0;

		GetLogicalAddress((PVOID)pc,szModule,sizeof(szModule),&section,&offset);
		tprintf(hFile,_T("%08X	%08X	%04X:%08X %s\r\n"),
					pc,pFrame,section,offset,szModule);

		// precede to next higher frame on stack
		pc = pFrame[1];
		pPrevFrame = pFrame;
		pFrame = (PDWORD)pFrame[0];
		// Frame pointer must be aligned on a DWORD boundary
		if ((DWORD)pFrame & 3) break;
		if (pFrame <= pPrevFrame) break;

		// Can two DWORDs be read from the supposed frame address?
		if (IsBadWritePtr(pFrame,sizeof(PVOID)*2)) break;
	}
}


//******************************************************************
//******************************************************************
static LPTSTR GetExceptionString(DWORD dwCode) {
	#define EXCEPTION(x) case EXCEPTION_##x: return _T(#x);
	switch (dwCode) {
		EXCEPTION(ACCESS_VIOLATION)
		EXCEPTION(DATATYPE_MISALIGNMENT)
		EXCEPTION(BREAKPOINT)
		EXCEPTION(SINGLE_STEP)
		EXCEPTION(ARRAY_BOUNDS_EXCEEDED)
		EXCEPTION(FLT_DENORMAL_OPERAND)
		EXCEPTION(FLT_DIVIDE_BY_ZERO)
		EXCEPTION(FLT_INEXACT_RESULT)
		EXCEPTION(FLT_INVALID_OPERATION)
		EXCEPTION(FLT_OVERFLOW)
		EXCEPTION(FLT_STACK_CHECK)
		EXCEPTION(FLT_UNDERFLOW)
		EXCEPTION(INT_DIVIDE_BY_ZERO)
		EXCEPTION(INT_OVERFLOW)
		EXCEPTION(PRIV_INSTRUCTION)
		EXCEPTION(IN_PAGE_ERROR)
		EXCEPTION(ILLEGAL_INSTRUCTION)
		EXCEPTION(NONCONTINUABLE_EXCEPTION)
		EXCEPTION(STACK_OVERFLOW)
		EXCEPTION(INVALID_DISPOSITION)
		EXCEPTION(GUARD_PAGE)
		EXCEPTION(INVALID_HANDLE)
	}
	#undef EXCEPTION

	// If not one of the "known" exceptions, try to
	// get the string from NTDLL.DLL's message table.
	static TCHAR szBuf[512] = { 0 };
	FormatMessage(FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_FROM_HMODULE,
					GetModuleHandle(_T("NTDLL.DLL")),
					dwCode,0,szBuf,sizeof(szBuf),0);
	return szBuf;
}


//******************************************************************
//******************************************************************
static void GenerateExceptionReport(HANDLE hFile,PEXCEPTION_POINTERS pExceptionInfo) {
	PEXCEPTION_RECORD pExceptionRecord = pExceptionInfo->ExceptionRecord;

	// First print information about the type of fault
	tprintf(hFile,_T("Exception code: %08X %s\r\n"),
				pExceptionRecord->ExceptionCode,
				GetExceptionString(pExceptionRecord->ExceptionCode));

	// Now print information about where the fault occured
	TCHAR szFaultingModule[MAX_PATH];
	DWORD section,offset;
	GetLogicalAddress(pExceptionRecord->ExceptionAddress,
						szFaultingModule,
						sizeof(szFaultingModule),
						&section,&offset);

	tprintf(hFile,_T("Fault address:	%08X %02X:%08X %s\r\n"),
				pExceptionRecord->ExceptionAddress,
				section,offset,szFaultingModule);

	PCONTEXT pCtx = pExceptionInfo->ContextRecord;

	// Show the registers
	#ifdef _M_IX86	// Intel Only!
	tprintf(hFile,_T("\r\nRegisters:\r\n"));

	tprintf(hFile,_T("EAX:%08X\r\nEBX:%08X\r\nECX:%08X\r\nEDX:%08X\r\nESI:%08X\r\nEDI:%08X\r\n"),
			pCtx->Eax,pCtx->Ebx,pCtx->Ecx,pCtx->Edx,pCtx->Esi,pCtx->Edi);

	tprintf(hFile,_T("CS:EIP:%04X:%08X\r\n"),pCtx->SegCs,pCtx->Eip);
	tprintf(hFile,_T("SS:ESP:%04X:%08X	EBP:%08X\r\n"),
				pCtx->SegSs,pCtx->Esp,pCtx->Ebp);
	tprintf(hFile,_T("DS:%04X	ES:%04X	FS:%04X	GS:%04X\r\n"),
				pCtx->SegDs,pCtx->SegEs,pCtx->SegFs,pCtx->SegGs);
	tprintf(hFile,_T("Flags:%08X\r\n"),pCtx->EFlags);

	// Walk the stack using x86 specific code
	IntelStackWalk(hFile,pCtx);

	#endif

	tprintf(hFile,_T("\r\n"));
}


//******************************************************************
//******************************************************************
LONG WINAPI CExcept::ExceptionFilter(PEXCEPTION_POINTERS pExceptionInfo) {

	// try opening the error report file in the program directory
	// if we can't open the file, it may be because the app
	// is running from a CDROM drive -- try opening a file
	// with the same name in c:
	HANDLE hFile;
	for (int i = 0; i < 2; i++) {	
		hFile = CreateFile(
			m_szLogFileName,
			GENERIC_WRITE,
			0,
			0,
			OPEN_ALWAYS,
			FILE_FLAG_WRITE_THROUGH,
			0
		);
		if (hFile != INVALID_HANDLE_VALUE) break;

		// extract the file name
		LPTSTR lpFName = _tcsrchr(m_szLogFileName,_T('\\'));
		if (! lpFName) break;

		// create full path to file in C:
		TCHAR szTemp[MAX_PATH] = _T("c:\\");
		_tcscat(szTemp,lpFName);
		_tcscpy(m_szLogFileName,szTemp);
	}

	if (hFile != INVALID_HANDLE_VALUE) {
		SetFilePointer(hFile,0,0,FILE_END);
		GenerateExceptionReport(hFile,pExceptionInfo);
		CloseHandle(hFile);
	}

	if (m_previousFilter) return m_previousFilter(pExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}


//******************************************************************
//******************************************************************
CExcept::CExcept() {
	// Install the unhandled exception filter function
	m_previousFilter = SetUnhandledExceptionFilter(ExceptionFilter);

	// Figure out what the report file will be named,and store it away
	GetModuleFileName(0,m_szLogFileName,MAX_PATH);

	// replace .EXE with .ERR
	PTSTR pszDot = _tcsrchr(m_szLogFileName,_T('.'));
	if (pszDot) {
		pszDot++;
		if (_tcslen(pszDot) >= 3)
			_tcscpy(pszDot,_T("ERR"));
	}

	// delete any old exception reports
	DeleteFile(m_szLogFileName);
}


//******************************************************************
//******************************************************************
CExcept::~CExcept() {
	SetUnhandledExceptionFilter(m_previousFilter);
}
