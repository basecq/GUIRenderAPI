#pragma once

#include "renderers\Define.h"
#include "unistr.h"

class CLogFile
{
public:
	CLogFile ( TCHAR *szFile);
	~CLogFile ( void );

	void Log ( const TCHAR *szType, const TCHAR *szString, va_list arglist );

private:
	FILE *m_hFile;
	CRITICAL_SECTION m_cs;
};

