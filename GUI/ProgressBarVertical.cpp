#include "CGUI.h"

#define PROGRESSBAR_ARROWCLICK_REPEAT 0.05
#define PROGRESSBAR_ARROWCLICK_START 0.33

//--------------------------------------------------------------------------------------
CProgressBarVertical::CProgressBarVertical ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_PROGRESSBARVERTICAL );

	m_fStep = 1.f;
	m_fValue = 0.f;
	m_fMax = 100.f;
}

//--------------------------------------------------------------------------------------
void CProgressBarVertical::Draw ( void )
{
	if ( !m_bVisible )
		return;

	CMouse *pMouse = m_pDialog->GetMouse ();
	assert ( pMouse && "Invalid mouse pointer" );

	if ( pMouse && 
		 m_bPressed && 
		 m_rBoundingBox.InControlArea ( pMouse->GetPos () ) && 
		 !m_timer.Running() )
	{
		if ( pMouse->GetPos ().GetX () > m_rProgress.pos.GetX () + m_rProgress.size.cx )
		{
			m_timer.Start ( PROGRESSBAR_ARROWCLICK_START );
			SetValue ( m_fValue + m_fStep );
		}
		else if ( pMouse->GetPos ().GetX () < m_rProgress.pos.GetX () + m_rProgress.size.cx )
		{
			m_timer.Start ( PROGRESSBAR_ARROWCLICK_START );
			SetValue ( m_fValue - m_fStep );
		}
	}

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
	m_pDialog->DrawBox ( m_rProgress, m_sControlColor.d3dColorBoxSel, m_sControlColor.d3dColorOutline, m_bAntAlias );
}

//--------------------------------------------------------------------------------------
bool CProgressBarVertical::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			if ( m_bPressed )
			{
				SetValue ( ValueFromPos ( pos.GetY () - m_rProgress.pos.GetY () ) );
				return true;
			}
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( m_rBoundingBox.InControlArea ( pos ) )
			{
				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pParent )
					m_pParent->RequestControlFocus ( this );

				m_timer.Start ( PROGRESSBAR_ARROWCLICK_START );

				if ( pos.GetY () > m_rProgress.pos.GetY () + m_rProgress.size.cy )
				{
					SetValue ( m_fValue + m_fStep );
					return true;
				}
				else if ( pos.GetY () < m_rProgress.pos.GetY () + m_rProgress.size.cy )
				{
					SetValue ( m_fValue - m_fStep );
					return true;
				}

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

				return true;
			}
			break;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------
float CProgressBarVertical::ValueFromPos ( int iY )
{
	float fValuePerPixel = ( float ) ( m_fMax / m_rBoundingBox.size.cy );
	return  ( 0.5 + fValuePerPixel * iY );
}

//--------------------------------------------------------------------------------------
void CProgressBarVertical::SetValue ( float fValue )
{
	// Clamp to range
	fValue = __max ( 0.f, fValue );
	fValue = __min ( m_fMax, fValue );

	if ( m_fValue == fValue )
		return;

	m_fValue = fValue;
}

//--------------------------------------------------------------------------------------
float CProgressBarVertical::GetValue ( void )
{
	return m_fValue;
}

//--------------------------------------------------------------------------------------
void CProgressBarVertical::SetMaxValue ( float fMax )
{
	m_fMax = fMax;
}

//--------------------------------------------------------------------------------------
float CProgressBarVertical::GetMaxValue ( void )
{
	return m_fMax;
}

//--------------------------------------------------------------------------------------
void CProgressBarVertical::SetStep ( float fStep )
{
	m_fStep = fStep;
}

//--------------------------------------------------------------------------------------
float CProgressBarVertical::GetStep ( void )
{
	return m_fStep;
}

//--------------------------------------------------------------------------------------
void CProgressBarVertical::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_rProgress = m_rBoundingBox;
	m_rProgress.size.cy = ( m_fValue * ( float ) ( m_rBoundingBox.size.cy ) / m_fMax );
}

//--------------------------------------------------------------------------------------
bool CProgressBarVertical::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.InControlArea ( pos ) ||
			 m_rProgress.InControlArea ( pos ) );
}
