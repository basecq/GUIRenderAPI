#include "CGUI.h"

#define TRACKBAR_THUMBSIZE 10

CTrackBarVertical::CTrackBarVertical ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_TRACKBARVERTICAL );

	m_nMin = 0;
	m_nMax = 1;
	m_nValue = 1;
}

//--------------------------------------------------------------------------------------
void CTrackBarVertical::Draw ( void )
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

	SControlRect rBox = m_rBoundingBox;
	rBox.pos.SetX ( rBox.pos.GetX () + m_rBoundingBox.size.cx / 2 - 2 );

	rBox.size.cx = 5;
	m_pDialog->DrawBox ( rBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
	rBox.size.cy = m_nButtonY;
	m_pDialog->DrawBox ( rBox, m_sControlColor.d3dColorBoxSel, m_sControlColor.d3dColorOutline, m_bAntAlias );

	m_pDialog->DrawBox ( m_rThumb, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline, m_bAntAlias );
}

//--------------------------------------------------------------------------------------
bool CTrackBarVertical::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( m_rThumb.InControlArea ( pos ) )
			{
				// Pressed while inside the control
				m_bPressed = true;

				m_nDragOffset = pos.GetY () - m_rThumb.pos.GetY ();

				if ( m_pParent && !m_bHasFocus )
					m_pParent->RequestControlFocus ( this );

				return true;
			}

			if ( m_rBoundingBox.InControlArea ( pos ) )
			{
				m_nDragOffset = 0;

				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pParent && !m_bHasFocus )
					m_pParent->RequestControlFocus ( this );

				if ( pos.GetY () > m_rThumb.pos.GetY () )
				{
					SetValue ( m_nValue + 1 );
					return true;
				}

				if ( pos.GetY () < m_rThumb.pos.GetY () )
				{
					SetValue ( m_nValue - 1 );
					return true;
				}
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			if ( m_bPressed )
			{
				m_bPressed = false;
				SendEvent ( EVENT_CONTROL_SELECT, m_nValue );

				if ( m_pParent )
					m_pParent->ClearControlFocus ();

				return true;
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			if ( m_bPressed )
			{
				SetValue ( ValueFromPos ( pos.GetY () - m_rBoundingBox.pos.GetY () - m_nDragOffset ) );
				return true;
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			int nScrollAmount = int ( ( short ) HIWORD ( wParam ) ) / WHEEL_DELTA;
			SetValue ( m_nValue - nScrollAmount );
			return true;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
bool CTrackBarVertical::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_HOME:
					SetValue ( m_nMin );
					return true;

				case VK_END:
					SetValue ( m_nMax );
					return true;

				case VK_LEFT:
				case VK_DOWN:
					SetValue ( m_nValue - 1 );
					return true;

				case VK_RIGHT:
				case VK_UP:
					SetValue ( m_nValue + 1 );
					return true;

				case VK_NEXT:
					SetValue ( m_nValue - ( 10 > ( m_nMax - m_nMin ) / 10 ? 10 : ( m_nMax - m_nMin ) / 10 ) );
					return true;

				case VK_PRIOR:
					SetValue ( m_nValue + ( 10 > ( m_nMax - m_nMin ) / 10 ? 10 : ( m_nMax - m_nMin ) / 10 ) );
					return true;
			}
			break;
		}
	}
}

//--------------------------------------------------------------------------------------
int CTrackBarVertical::ValueFromPos ( int iY )
{
	float fValuePerPixel = ( float ) ( m_nMax - m_nMin ) / ( m_rBoundingBox.size.cy - TRACKBAR_THUMBSIZE );
	return ( int ) ( 0.5 + m_nMin + fValuePerPixel * iY );
}

//--------------------------------------------------------------------------------------
void CTrackBarVertical::SetRange ( int nMin, int nMax )
{
	m_nMin = nMin;
	m_nMax = nMax;

	SetValue ( m_nValue );
}

//--------------------------------------------------------------------------------------
void CTrackBarVertical::SetValue ( int nValue )
{
	// Clamp to range
	nValue = __max ( m_nMin, nValue );
	nValue = __min ( m_nMax, nValue );

	if ( nValue == m_nValue )
		return;

	SendEvent ( EVENT_CONTROL_SELECT, m_nValue = nValue );
}

//--------------------------------------------------------------------------------------
void CTrackBarVertical::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_nButtonY = ( int ) ( ( m_nValue - m_nMin ) * ( float ) ( m_rBoundingBox.size.cy - TRACKBAR_THUMBSIZE ) / ( m_nMax - m_nMin ) );
	m_rThumb = m_rBoundingBox;
	m_rThumb.pos.SetY ( m_rThumb.pos.GetY () + m_nButtonY );
	m_rThumb.size.cy = TRACKBAR_THUMBSIZE;
}

//--------------------------------------------------------------------------------------
bool CTrackBarVertical::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.InControlArea ( pos ) ||
			 m_rThumb.InControlArea ( pos ) );
}
