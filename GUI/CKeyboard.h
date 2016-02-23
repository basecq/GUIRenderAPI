#pragma once

#include "CGUI.h"

struct SKey
{
	SIMPLEGUI_CHAR m_vKey;
	int m_bDown;
	LPARAM m_lParam;

	SKey()
	{
		m_vKey = 0;
		m_bDown = false;
		m_lParam = 0;
	}
	SKey( SIMPLEGUI_CHAR vKey, bool bDown )
	{
		m_vKey = vKey;
		m_bDown = bDown;
		m_lParam = 0;
	}
	SKey( SIMPLEGUI_CHAR vKey, bool bDown, LPARAM lParam )
	{
		m_vKey = vKey;
		m_bDown = bDown;
		m_lParam = lParam;
	}
};

class CKeyboard
{
	CDialog *m_pDialog;
	SKey m_sKey;

public:
	CKeyboard(CDialog *Gui);

	bool HandleMessage( unsigned int uMsg, unsigned int wParam, long lParam );

	void SetKey( SKey sKey );
	SKey GetKey();
};