#include "CGUI.h"

CKeyboard::CKeyboard(CDialog *Gui)
{
	m_pDialog = Gui;
}

bool CKeyboard::HandleMessage( unsigned int uMsg, unsigned int wParam, long lParam )
{
	if( !m_pDialog->IsVisible()|| m_pDialog->GetMouse()->GetLeftButton() )
		return false;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
			SetKey ( SKey ( static_cast< SIMPLEGUI_CHAR >( wParam ), true, lParam ) );
			break;
		case WM_KEYUP:
			SetKey ( SKey ( static_cast< SIMPLEGUI_CHAR >( wParam ), false, lParam ) );
			break;
	}

	return true;//m_pDialog->MsgProc( GetKey() );
}

void CKeyboard::SetKey( SKey sKey )
{
	m_sKey = sKey;
}

SKey CKeyboard::GetKey()
{
	SKey sRet = m_sKey;
	SetKey( SKey( 0, false ) );
	return sRet;
}