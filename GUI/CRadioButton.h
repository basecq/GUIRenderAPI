#pragma once

#include "CGUI.h"

class CRadioButton : public CControl
{
public:
	CRadioButton ( CDialog *pDialog );
	~CRadioButton ( void );

	void SetGroup ( UINT uGroup )
	{
		m_uGroup = uGroup;
		m_mButtonID [ this ] [ m_uGroup ] = m_nCount;
	}

	UINT GetGroup ( void )
	{
		return m_uGroup;
	}

	void Draw ( void );

	bool GetChecked ( void )
	{
		return m_bChecked;
	}

	void SetChecked ( bool bChecked )
	{
		m_mID [ m_uGroup ] = m_mButtonID [ this ] [ m_uGroup ];
		SendEvent ( EVENT_CONTROL_SELECT, m_bChecked = bChecked );
	}

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );
private:
	
	UINT m_uGroup;
	SControlRect m_rText;
	bool m_bChecked;

	static UINT m_nCount;
	static std::map< CRadioButton*, std::map< UINT, UINT >> m_mButtonID;
	static std::map< UINT, UINT > m_mID;
};

