#include "CGUI.h"

CButton::CButton ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_BUTTON );
}

//--------------------------------------------------------------------------------------
void CButton::Draw ( void )
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

	std::string str = GetText ();
	m_pFont->CutString ( m_rBoundingBox.size.cx - 4, str );

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline );
	m_pDialog->DrawFont ( SControlRect ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx / 2, m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy / 2 ),
						  m_sControlColor.d3dColorFont, str.c_str (), D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y, m_pFont );
}

//--------------------------------------------------------------------------------------
bool CButton::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( m_rBoundingBox.InControlArea ( pos ) )
			{
				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pParent && !m_bHasFocus )
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

				if ( m_pParent  )
					m_pParent->ClearControlFocus ();

				// Button click
				if ( m_rBoundingBox.InControlArea ( pos ) )
					SendEvent ( EVENT_CONTROL_CLICKED, true );

				return true;
			}
			break;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
bool CButton::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_SPACE:
					m_bPressed = true;
					return true;
			}
		}

		case WM_KEYUP:
		{
			switch ( wParam )
			{
				case VK_SPACE:
					if ( m_bPressed )
					{
						m_bPressed = false;
						SendEvent ( EVENT_CONTROL_CLICKED, true );
					}
					return true;
			}
		}
	}

	return false;
}