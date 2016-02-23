#pragma once

class CVar;

#include "CGUI.h"

class CVar
{
	double m_dValue;
	SIMPLEGUI_STRING m_sString;

	typedef SIMPLEGUI_STRING ( __cdecl * tAction )( const SIMPLEGUI_CHAR * pszArgs, CControl * pElement );
	tAction m_pAction;
public:
	CVar( double dValue );
	CVar( float fValue );
	CVar( int iValue );
	CVar( bool bValue );
	CVar( SIMPLEGUI_STRING sString );
	CVar( tAction pAction );

	void SetDouble( double dValue );
	void SetFloat( float fValue );
	void SetInteger( int iValue );
	void SetBoolean( bool bValue );
	void SetString( SIMPLEGUI_STRING sString );
	void SetAction( tAction pAction );

	double GetDouble();
	float GetFloat();
	int GetInteger();
	bool GetBoolean();
	SIMPLEGUI_STRING GetString();
	SIMPLEGUI_STRING GetActionString( const SIMPLEGUI_CHAR * pszString, CControl * pElement = 0 );
	tAction GetAction();
};

class CCommand
{
public:
};