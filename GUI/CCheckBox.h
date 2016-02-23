#pragma once

#include "CGUI.h"

class CCheckBox : public CControl
{
public:
	CCheckBox ( CDialog *pDialog );

	bool GetChecked ( void )
	{
		return m_bChecked;
	}

	void SetChecked ( bool bChecked )
	{
		SendEvent ( EVENT_CONTROL_SELECT, m_bChecked = bChecked );
	}

	void Draw ( void );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

private:
	bool m_bChecked;
	SControlRect m_rText;
};