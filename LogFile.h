#pragma once

#include "GUI\renderers\Define.h"
#include "GUI\unistr.h"

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

