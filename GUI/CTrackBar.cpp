#include "CGUI.h"

#define TRACKBAR_THUMBSIZE 10

CTrackBarHorizontal::CTrackBarHorizontal ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_TRACKBARHORIZONTAL );

	m_nMin = 0;
	m_nMax = 1;
	m_nValue = 1;
}

//--------------------------------------------------------------------------------------
void CTrackBarHorizontal::Draw ( void )
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
	rBox.pos.SetY ( rBox.pos.GetY () + m_rBoundingBox.size.cy / 2 - 2 );

	rBox.size.cy = 5;
	m_pDialog->DrawBox ( rBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
	
	rBox.size.cx = m_nButtonX;
	m_pDialog->DrawBox ( rBox, m_sControlColor.d3dColorBoxSel, m_sControlColor.d3dColorOutline, m_bAntAlias );

	m_pDialog->DrawBox ( m_rThumb, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline, m_bAntAlias );
}

//--------------------------------------------------------------------------------------
bool CTrackBarHorizontal::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
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

				m_nDragOffset = pos.GetX () - m_rThumb.pos.GetX ();

				if ( m_pParent )
					m_pParent->RequestControlFocus ( this );

				return true;
			}

			if ( m_rBoundingBox.InControlArea ( pos ) )
			{
				m_nDragOffset = 0;

				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pParent )
					m_pParent->RequestControlFocus ( this );

				if ( pos.GetX () > m_rThumb.pos.GetX () )
				{
					SetValue ( m_nValue + 1 );
					return true;
				}

				if ( pos.GetX () < m_rThumb.pos.GetX () )
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
				SetValue ( ValueFromPos ( pos.GetX () - m_rBoundingBox.pos.GetX () - m_nDragOffset ) );
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
bool CTrackBarHorizontal::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
int CTrackBarHorizontal::ValueFromPos ( int iX )
{
	float fValuePerPixel = ( float ) ( m_nMax - m_nMin ) / ( m_rBoundingBox.size.cx - TRACKBAR_THUMBSIZE );
	return ( int ) ( 0.5 + m_nMin + fValuePerPixel * iX );
}

//--------------------------------------------------------------------------------------
void CTrackBarHorizontal::SetRange ( int nMin, int nMax )
{
	m_nMin = nMin;
	m_nMax = nMax;

	SetValue ( m_nValue );
}

//--------------------------------------------------------------------------------------
void CTrackBarHorizontal::SetValue ( int nValue )
{
	// Clamp to range
	nValue = __max ( m_nMin, nValue );
	nValue = __min ( m_nMax, nValue );

	if ( nValue == m_nValue )
		return;

	SendEvent ( EVENT_CONTROL_SELECT, m_nValue = nValue );
}

//--------------------------------------------------------------------------------------
void CTrackBarHorizontal::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_rThumb = m_rBoundingBox;
	m_rThumb.pos.SetX ( m_rThumb.pos.GetX () + m_nButtonX );
	m_rThumb.size.cx = TRACKBAR_THUMBSIZE;

	m_nButtonX = ( int ) ( ( m_nValue - m_nMin ) * ( float ) ( m_rBoundingBox.size.cx - TRACKBAR_THUMBSIZE ) / ( m_nMax - m_nMin ) );
}

//--------------------------------------------------------------------------------------
bool CTrackBarHorizontal::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.InControlArea ( pos ) ||
			 m_rThumb.InControlArea ( pos ) );
}
