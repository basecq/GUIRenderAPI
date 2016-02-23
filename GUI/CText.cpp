#include "CGUI.h"

CLabel::CLabel ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_LABEL );
	m_dwAlign = 0;
}

CLabel::~CLabel ( void )
{}

void CLabel::Draw ( void )
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

	m_pDialog->DrawFont ( SControlRect ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx / 2, m_rBoundingBox.pos.GetY () ),
						  m_sControlColor.d3dColorFont, GetText (), m_dwAlign, m_pFont );
}

bool CLabel::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
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

				if ( m_pParent )
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

bool CLabel::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

void CLabel::UpdateRects ( void )
{

	CControl::UpdateRects ();
}

void CLabel::SetAlign ( DWORD dwAlign )
{
	m_dwAlign = dwAlign;
}

void CLabel::SetWidth ( int nWidth )
{
	if ( !m_pFont )
		return;

	SIZE size;
	m_pFont->GetTextExtent ( GetText (), &size );
	m_rBoundingBox.size.cx = m_size.cx = min ( nWidth, size.cx );
}

void CLabel::SetHeight ( int nHeight )
{
	if ( !m_pFont )
		return;

	SIZE size;
	m_pFont->GetTextExtent ( GetText (), &size );
	m_rBoundingBox.size.cy = m_size.cy = min ( nHeight, size.cy );
}

void CLabel::SetSize ( SIZE size )
{
	SetSize ( size.cx, size.cy );
}

void CLabel::SetSize ( int nWidth, int nHeight )
{
	SetWidth ( nWidth );
	SetHeight ( nHeight );
}