#include "CGUI.h"

UINT CRadioButton::m_nCount = 0;
std::map<UINT, UINT> CRadioButton::m_mID;
std::map<CRadioButton*, std::map<UINT, UINT>> CRadioButton::m_mButtonID;

CRadioButton::CRadioButton ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_RADIOBUTTON );

	m_uGroup = 0;
	m_nCount++;
}

CRadioButton::~CRadioButton ( void )
{
	m_mButtonID.erase ( this );
	m_mID.erase ( m_uGroup );

	m_nCount--;
}

void CRadioButton::Draw ( void )
{
	if ( !m_bVisible )
		return;

	if ( !m_bEnabledStateColor )
		m_eState = SControlColor::STATE_NORMAL;
	else if ( !m_bEnabled )
		m_eState = SControlColor::STATE_DISABLED;
	else if ( m_bPressed )
		m_eState = SControlColor::STATE_PRESSED;
	else if ( m_bMouseOver )
		m_eState = SControlColor::STATE_MOUSE_OVER;
	else
		m_eState = SControlColor::STATE_NORMAL;

	CPos pos = m_rBoundingBox.pos;
	SIZE size = m_rText.size;
	size.cx = size.cy;

	m_pDialog->DrawCircle ( SControlRect ( pos, size ), m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline, m_bAntAlias );

	SIMPLEGUI_STRING str ( GetText () );
	m_pFont->CutString ( m_rText.size.cx, str );
	m_pDialog->DrawFont ( SControlRect ( m_rText.pos.GetX (), m_rText.pos.GetY () + ( m_rBoundingBox.size.cy / 2 ) ),
						  m_sControlColor.d3dColorFont, str.c_str (), D3DFONT_CENTERED_Y, m_pFont );

	if ( m_mButtonID [ this ] [ m_uGroup ] == m_mID [ m_uGroup ] &&
		 m_bChecked )
	{
		size.cx = size.cy -= 4;
		m_pDialog->DrawCircle ( SControlRect ( pos + 2, size ), m_sControlColor.d3dColorShape, 0, m_bAntAlias );
	}
}

bool CRadioButton::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( ContainsRect ( pos ) )
			{
				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pParent )
					m_pParent->RequestControlFocus ( this );

				return true;
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			if ( m_bPressed )
			{
				m_bPressed = false;

				// Button click
				if ( ContainsRect ( pos ) )
					SetChecked ( true );

				if ( m_pParent )
					m_pParent->ClearControlFocus ();

				return true;
			}

			break;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
bool CRadioButton::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	/// ...
	return false;
}

//--------------------------------------------------------------------------------------
void CRadioButton::UpdateRects ( void )
{
	CControl::UpdateRects ();

	if ( m_pFont )
	{
		SIZE size;
		m_pFont->GetTextExtent ( GetText (), &size );

		if ( m_rBoundingBox.size.cx > size.cx )
			m_size.cx = size.cx + 20;
	}

	m_rText = m_rBoundingBox;
	m_rText.pos.SetX ( m_rText.pos.GetX () + 20 );
	m_rText.size.cx -= 20; m_rText.size.cy = 15;
}

//--------------------------------------------------------------------------------------
bool CRadioButton::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.InControlArea ( pos ) ||
			 m_rText.InControlArea ( pos ) );
}