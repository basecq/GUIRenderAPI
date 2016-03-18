#include "LogFile.h"

CLogFile::CLogFile ( TCHAR *szFile )
{
	InitializeCriticalSection ( &m_cs );
	m_hFile = SIMPLEGUI_FOPEN ( szFile, _UI ( "a" ) );

	if ( !m_hFile )
		exit ( 1 );
}

CLogFile::~CLogFile ( void )
{
	if ( m_hFile ) 
		fclose ( m_hFile );

	DeleteCriticalSection ( &m_cs );
}

void CLogFile::Log ( const TCHAR *szType, const TCHAR *szString, va_list arglist )
{
	if ( !m_hFile )
		return;

	EnterCriticalSection ( &m_cs );

	TCHAR szStr [ MAX_PATH ];

	time_t now;
	time ( &now );

	SIMPLEGUI_FTIME ( szStr, 128, _UI ( "[%d.%m %H:%M:%S]" ), localtime ( &now ) );
	SIMPLEGUI_FPUTS ( szStr, m_hFile );

	if ( SIMPLEGUI_STRLEN ( szType ) > 0 )
		SIMPLEGUI_FPRINTF ( m_hFile, _UI ( " %s: " ), szType );
	else
		SIMPLEGUI_FPRINTF ( m_hFile, _UI ( ": " ) );

	SIMPLEGUI_VFPRINTF ( m_hFile, szString, arglist );
	SIMPLEGUI_FPRINTF ( m_hFile, _UI ( "\n" ) );
	fflush ( m_hFile );

	LeaveCriticalSection ( &m_cs );
}