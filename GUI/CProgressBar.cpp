#include "CGUI.h"

#define PROGRESSBAR_ARROWCLICK_REPEAT 0.05
#define PROGRESSBAR_ARROWCLICK_START 0.33

//--------------------------------------------------------------------------------------
CProgressBarHorizontal::CProgressBarHorizontal ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_PROGRESSBARHORIZONTAL );

	m_fStep = 1.f;
	m_fValue = 0.f;
	m_fMax = 100.f;
}

//--------------------------------------------------------------------------------------
void CProgressBarHorizontal::Draw ( void )
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
			m_timer.Start ( PROGRESSBAR_ARROWCLICK_REPEAT );
			SetValue ( m_fValue + m_fStep );
		}
		else if ( pMouse->GetPos ().GetX () < m_rProgress.pos.GetX () + m_rProgress.size.cx )
		{
			m_timer.Start ( PROGRESSBAR_ARROWCLICK_REPEAT );
			SetValue ( m_fValue - m_fStep );
		}
	}

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
	m_pDialog->DrawBox ( m_rProgress, m_sControlColor.d3dColorBoxSel, m_sControlColor.d3dColorOutline, m_bAntAlias );
}

//--------------------------------------------------------------------------------------
bool CProgressBarHorizontal::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			if ( m_bPressed )
			{
				SetValue ( ValueFromPos ( pos.GetX () - m_rProgress.pos.GetX () ) );
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

				if ( pos.GetX () > m_rProgress.pos.GetX () + m_rProgress.size.cx )
				{
					SetValue ( m_fValue + m_fStep );
					return true;
				}

				if ( pos.GetX () < m_rProgress.pos.GetX () + m_rProgress.size.cx )
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
float CProgressBarHorizontal::ValueFromPos ( int iX )
{
	float fValuePerPixel = ( float ) ( m_fMax / m_rBoundingBox.size.cx );
	return  ( 0.5 + fValuePerPixel * iX );
}

//--------------------------------------------------------------------------------------
void CProgressBarHorizontal::SetValue ( float fValue )
{
	// Clamp to range
	fValue = __max ( 0.f, fValue );
	fValue = __min ( m_fMax, fValue );

	if ( m_fValue == fValue )
		return;

	m_fValue = fValue;
}

//--------------------------------------------------------------------------------------
float CProgressBarHorizontal::GetValue ( void )
{
	return m_fValue;
}

//--------------------------------------------------------------------------------------
void CProgressBarHorizontal::SetMaxValue ( float fMax )
{
	m_fMax = fMax;
}

//--------------------------------------------------------------------------------------
float CProgressBarHorizontal::GetMaxValue ( void )
{
	return m_fMax;
}

//--------------------------------------------------------------------------------------
void CProgressBarHorizontal::SetStep ( float fStep )
{
	m_fStep = fStep;
}

//--------------------------------------------------------------------------------------
float CProgressBarHorizontal::GetStep ( void )
{
	return m_fStep;
}

//--------------------------------------------------------------------------------------
void CProgressBarHorizontal::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_rProgress = m_rBoundingBox;
	m_rProgress.size.cx = ( m_fValue * ( float ) ( m_rBoundingBox.size.cx ) / m_fMax );
}

//--------------------------------------------------------------------------------------
bool CProgressBarHorizontal::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_rBoundingBox.InControlArea ( pos ) ||
			 m_rProgress.InControlArea ( pos ) );
}
