#include "CGUI.h"

#define SCROLLBAR_SIZE 18

CScrollablePane::CScrollablePane ( CDialog *pDialog )
{
	m_pDialog = pDialog;
	m_bScrollVerShow = m_bScrollHorShow = true;
	ZeroMemory ( &m_pageSize, sizeof ( SIZE ) );

	m_pScrollbarHor = new CScrollBarHorizontal ( pDialog );

	// Assert if 'm_pScrollbarHor' is invalid
	assert ( m_pScrollbarHor &&
			 _UI ( "Error for creating horizontal scrollbar" ) );

	m_pScrollbarVer = new CScrollBarVertical ( pDialog );

	// Assert if 'm_pScrollbarVer' is invalid
	assert ( m_pScrollbarVer &&
			 _UI ( "Error for creating vertical scrollbar" ) );
}

CScrollablePane::~CScrollablePane ( void )
{
	SAFE_DELETE ( m_pScrollbarHor );
	SAFE_DELETE ( m_pScrollbarVer );
}

void CScrollablePane::OnMouseWheel ( int nDelta )
{
	if ( IsVerScrollbarNeeded () )
	{
		m_pScrollbarVer->Scroll ( nDelta * m_pScrollbarVer->GetStepSize () );
	}
	else if ( IsHorScrollbarNeeded () )
	{
		m_pScrollbarHor->Scroll ( nDelta * m_pScrollbarHor->GetStepSize () );
	}
}

void CScrollablePane::OnDraw ( void )
{
	if ( !m_pScrollbarHor ||
		 !m_pScrollbarVer )
		return;

	if ( IsHorScrollbarNeeded () )
		m_pScrollbarHor->Draw ();

	if ( IsVerScrollbarNeeded () )
		m_pScrollbarVer->Draw ();
}

bool CScrollablePane::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( m_pScrollbarHor && 
		 m_pScrollbarHor->HandleMouse ( uMsg, pos, wParam, lParam ) )
	{
		SetFocussedControl ();
		return true;
	}

	if ( m_pScrollbarVer &&
		 m_pScrollbarVer->HandleMouse ( uMsg, pos, wParam, lParam ) )
	{
		SetFocussedControl ();
		return true;
	}

	return false;
}

bool CScrollablePane::IsHorScrollbarNeeded ( void )
{
	return ( m_pScrollbarHor->IsThumbShowing () &&
			 m_bScrollVerShow );
}

bool CScrollablePane::IsVerScrollbarNeeded ( void )
{
	return ( m_pScrollbarVer->IsThumbShowing () &&
			 m_bScrollHorShow );
}

bool CScrollablePane::IsHorScrollbarShowing ( void )
{
	return m_bScrollHorShow;
}

bool CScrollablePane::IsVerScrollbarShowing ( void )
{
	return m_bScrollVerShow;
}

void CScrollablePane::ShowScrollHor ( bool bShow )
{
	m_bScrollHorShow = bShow;
}

void CScrollablePane::ShowScrollVer ( bool bShow )
{
	m_bScrollVerShow = bShow;
}

void CScrollablePane::AddControl ( CControl *pControl )
{
	m_pControl = pControl;
}

CControl *CScrollablePane::GetControl ( void )
{
	return m_pControl;
}

void CScrollablePane::RemoveControl ( void )
{
	m_pControl = NULL;
}

void CScrollablePane::SetFocussedControl ( void )
{
	// Check for a valid control
	if ( m_pControl )
	{
		// Make sure that the control is a window, and if it unfocused
		if ( m_pControl->GetType () == CControl::TYPE_WINDOW  &&
			 !m_pControl->HasFocus () )
		{
			// Give it focus
			m_pDialog->SetFocussedWindow ( ( CWindow* ) m_pControl );
		}
		// Otherwise, make sure the control has a parent, and if it unfocused
		else if ( m_pControl->GetParent () &&
				  !m_pControl->HasFocus () )
		{
			// Give it focus
			m_pControl->GetParent ()->RequestControlFocus ( m_pControl );
		}
	}
}

void CScrollablePane::OnClickLeave ( void )
{
	if ( m_pScrollbarVer )
		m_pScrollbarVer->OnClickLeave ();

	if ( m_pScrollbarHor )
		m_pScrollbarHor->OnClickLeave ();
}

bool CScrollablePane::OnClickEvent ( void )
{
	return ( m_pScrollbarHor &&
			 m_pScrollbarHor->OnClickEvent () ||
			 m_pScrollbarVer &&
			 m_pScrollbarVer->OnClickEvent () );
}

void CScrollablePane::OnFocusIn ( void )
{
	if ( m_pScrollbarVer )
		m_pScrollbarVer->OnFocusIn ();

	if ( m_pScrollbarHor )
		m_pScrollbarHor->OnFocusIn ();
}

void CScrollablePane::OnFocusOut ( void )
{
	if ( m_pScrollbarVer )
		m_pScrollbarVer->OnFocusOut ();

	if ( m_pScrollbarHor )
		m_pScrollbarHor->OnFocusOut ();
}

void CScrollablePane::OnMouseEnter ( void )
{
	if ( m_pScrollbarVer )
		m_pScrollbarVer->OnMouseEnter ();

	if ( m_pScrollbarHor )
		m_pScrollbarHor->OnMouseEnter ();
}

void CScrollablePane::OnMouseLeave ( void )
{
	if ( m_pScrollbarVer )
		m_pScrollbarVer->OnMouseLeave ();

	if ( m_pScrollbarHor )
		m_pScrollbarHor->OnMouseLeave ();
}

bool CScrollablePane::CanHaveFocus ( void )
{
	if ( !IsHorScrollbarNeeded () ||
		 !IsVerScrollbarNeeded () )
		return false;

	return ( m_pScrollbarHor &&
			 m_pScrollbarHor->CanHaveFocus () ||
			 m_pScrollbarVer &&
			 m_pScrollbarVer->CanHaveFocus () );
}

void CScrollablePane::SetPageSize ( int nSizeX, int nSizeY )
{
	m_pageSize.cx = nSizeX;
	m_pageSize.cy = nSizeY;
}

void CScrollablePane::SetTrackRange ( int nScrollHorValue, int nScrollVerValue )
{
	if ( !m_pScrollbarHor ||
		 !m_pScrollbarVer )
		return;

	if ( nScrollHorValue )
		m_pScrollbarHor->SetTrackRange ( 0, nScrollHorValue );
	
	if ( nScrollVerValue )
		m_pScrollbarVer->SetTrackRange ( 0, nScrollVerValue );
}

void CScrollablePane::UpdateScrollbars ( SControlRect rRect )
{
	if ( !m_pScrollbarHor ||
		 !m_pScrollbarVer )
		return;

	m_pScrollbarHor->SetPageSize ( m_pageSize.cx ? m_pageSize.cx : rRect.size.cx - ( IsVerScrollbarNeeded () ? SCROLLBAR_SIZE : 0 ) );
	m_pScrollbarHor->SetMinSize ( 0, 0 );
	m_pScrollbarHor->SetSize ( rRect.size.cx - ( IsVerScrollbarNeeded () ? SCROLLBAR_SIZE : 0 ), SCROLLBAR_SIZE );
	m_pScrollbarHor->SetPos ( rRect.pos.GetX (), rRect.pos.GetY () + rRect.size.cy - SCROLLBAR_SIZE );
	m_pScrollbarHor->UpdateRects ();

	m_pScrollbarVer->SetPageSize ( m_pageSize.cy ? m_pageSize.cy : rRect.size.cy - ( IsHorScrollbarNeeded () ? SCROLLBAR_SIZE : 0 ) );
	m_pScrollbarVer->SetMinSize ( 0, 0 );
	m_pScrollbarVer->SetSize ( SCROLLBAR_SIZE, rRect.size.cy - ( IsHorScrollbarNeeded () ? SCROLLBAR_SIZE : 0 ) );
	m_pScrollbarVer->SetPos ( rRect.pos.GetX () + rRect.size.cx - SCROLLBAR_SIZE, rRect.pos.GetY () );
	m_pScrollbarVer->UpdateRects ();
}

bool CScrollablePane::ContainsRect ( CPos pos )
{
	if ( !IsHorScrollbarNeeded () || 
		 !IsVerScrollbarNeeded () )
		return false;

	return ( m_pScrollbarHor->ContainsRect ( pos ) ||
			 m_pScrollbarVer->ContainsRect ( pos ) );
}

CScrollBarHorizontal *CScrollablePane::GetHorScrollbar ( void )
{
	return m_pScrollbarHor;
}

CScrollBarVertical *CScrollablePane::GetVerScrollbar ( void )
{
	return m_pScrollbarVer;
}