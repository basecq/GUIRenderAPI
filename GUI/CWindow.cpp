#include "CGUI.h"

//--------------------------------------------------------------------------------------
CWindow::CWindow ( CDialog *pDialog ) : m_pFocussedControl ( NULL ), m_bDragging ( false ), m_bCloseButtonEnabled ( true ),
m_iTitleBarSize ( 26 ),
m_bMovable ( true ),
m_bSizable ( true ),
m_bShowScrollbar ( true )
{
	m_eWindowArea = CLEAR;
	m_nDragX = m_nDragY = 0;
	SetControl ( pDialog, TYPE_WINDOW );

	m_sControlColor.d3dColorWindowTitle = D3DCOLOR_RGBA ( 21, 75, 138, 255 );
	m_sControlColor.d3dCOlorWindowBack = D3DCOLOR_RGBA ( 60, 60, 60, 255 );
	m_sControlColor.d3dColorBoxSel = D3DCOLOR_RGBA ( 50, 50, 50, 255 );

	m_sControlColor.d3dColorBox [ SControlColor::STATE_NORMAL ] = D3DCOLOR_RGBA ( 180, 0, 0, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_MOUSE_OVER ] = D3DCOLOR_XRGB ( 255, 0, 0, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ] = D3DCOLOR_XRGB ( 100, 0, 0, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_DISABLED ] = D3DCOLOR_XRGB ( 180, 180, 180, 255 );

	ZeroMemory ( &m_maxControlSize, sizeof ( SIZE ) );

	m_pScrollbar = new CScrollablePane ( pDialog );
	if ( !m_pScrollbar )
		MessageBox ( 0, _UI ( "CWindow::CWindow: Error for creating CScrollBarHorizontal" ), _UI ( "GUIAPI.asi" ), 0 );

	m_pScrollbar->AddControl ( this );
}

//--------------------------------------------------------------------------------------
CWindow::~CWindow ( void )
{
	SAFE_DELETE ( m_pFocussedControl );
	SAFE_DELETE ( m_pControlMouseOver );

	SAFE_DELETE ( m_pScrollbar );

	RemoveAllControls ();
}

//--------------------------------------------------------------------------------------
void CWindow::Draw ( void )
{
	if ( !m_bVisible )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	if ( !m_bEnabledStateColor )
		m_eState = SControlColor::STATE_NORMAL;
	else if ( !m_bEnabled )
		m_eState = SControlColor::STATE_DISABLED;
	else if ( m_bPressed )
		m_eState = SControlColor::STATE_PRESSED;
	else if ( m_bMouseOver &&  m_eWindowArea == CLEAR )
		m_eState = SControlColor::STATE_MOUSE_OVER;
	else
		m_eState = SControlColor::STATE_NORMAL;

	RECT old;
	m_pDialog->GetDevice ()->GetScissorRect ( &old );

	SControlRect rRect = m_rBoundingBox;
	rRect.pos.SetX ( rRect.pos.GetX () - 2 );
	rRect.pos.SetY ( rRect.pos.GetY () - 2 );
	rRect.size.cx = rRect.size.cx + 4;
	rRect.size.cy = rRect.size.cy + 4;
	SetScissor ( m_pDialog->GetDevice (), rRect.GetRect () );

	if ( !m_bMaximized )
	{
		// Draw background
		D3DCOLOR d3dColor = m_sControlColor.d3dCOlorWindowBack;
		if ( m_bHasFocus )
			d3dColor = m_sControlColor.d3dColorBoxSel;

		m_pDialog->DrawBox ( rRect, D3DCOLOR_RGBA ( 0, 0, 0, 210 ), 0, m_bAntAlias );
		m_pDialog->DrawBox ( m_rBoundingBox, D3DCOLOR_RGBA ( 10, 10, 10, 150 ), 0, m_bAntAlias );
	}

	// Draw title bar
	m_pDialog->DrawBox ( m_rTitle, m_sControlColor.d3dColorWindowTitle, m_sControlColor.d3dColorOutline, m_bAntAlias );
	m_pFont->Print ( m_pos.GetX () + 3, m_pos.GetY () + m_iTitleBarSize / 2, m_sControlColor.d3dColorFont, GetText (), D3DFONT_CENTERED_Y );

	// Draw button close
	if ( m_bCloseButtonEnabled )
	{
		D3DCOLOR d3dColorButton = m_sControlColor.d3dColorBox [ m_bPressed ? SControlColor::STATE_PRESSED : SControlColor::STATE_NORMAL ];
		if ( m_rButton.InControlArea ( m_pDialog->GetMouse ()->GetPos () ) )
			d3dColorButton = m_sControlColor.d3dColorBox [ m_eState ];

		m_pDialog->DrawBox ( m_rButton, d3dColorButton, m_sControlColor.d3dColorOutline );

		CD3DRender *pRender = m_pDialog->GetRenderer ();
		if ( pRender )
		{
			pRender->D3DLine ( m_pos.GetX () + m_rBoundingBox.size.cx - 20, m_pos.GetY () + 4, m_pos.GetX () + m_rBoundingBox.size.cx - 11.2, m_pos.GetY () + 12.2, 0xFFFFFFFF, true );
			pRender->D3DLine ( m_pos.GetX () + m_rBoundingBox.size.cx - 12, m_pos.GetY () + 4, m_pos.GetX () + m_rBoundingBox.size.cx - 20, m_pos.GetY () + 12, 0xFFFFFFFF, true );
		}
	}

	if ( !m_bMaximized )
	{
		bool bControlChangedStats = false;
		ZeroMemory ( &m_maxControlSize, sizeof ( SIZE ) );

		m_pScrollbar->OnDraw ();

		// Draw all controls
		for ( auto control : m_vControls )
		{
			if ( control )
			{
				control->LinkPos ( m_pos - ( m_bShowScrollbar ? CPos ( pScrollbarHor->GetTrackPos (), pScrollbarVer->GetTrackPos () ) : CPos () ) );

				if ( control->IsSizing () ||
					 control->IsMoving () )
				{
					bControlChangedStats = true;
				}

				SIZE size = control->GetSize ();
				CPos *pos = control->GetUpdatedPos ();

				if ( pos->GetX () + size.cx > m_rBoundingBox.pos.GetX () &&
					 pos->GetY () + size.cy > m_rBoundingBox.pos.GetY () + m_iTitleBarSize &&
					 pos->GetX () < m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx &&
					 pos->GetY () < m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy )
				{
					SControlRect rRect = m_rBoundingBox;
					rRect.size.cx = rRect.size.cx - ( m_pScrollbar->IsVerScrollbarNeeded () ? pScrollbarVer->GetWidth () : 0 );
					rRect.size.cy = rRect.size.cy - ( m_pScrollbar->IsHorScrollbarNeeded () ? pScrollbarHor->GetHeight () : 0 );

					control->BeginRenderRect ( rRect );
					control->Draw ();
					control->EndRenderRect ();
				}

				m_maxControlSize.cx = max ( m_maxControlSize.cx, pos->GetX () + size.cx );
				m_maxControlSize.cy = max ( m_maxControlSize.cy, pos->GetY () + size.cy );
			}
		}

		// Check that the control is changed position or size, or if
		// window is changed size
		if ( bControlChangedStats  )
		{
			UpdateScrollbars ();
		}
	}

	SetScissor ( m_pDialog->GetDevice (), old );
}

//--------------------------------------------------------------------------------------
void CWindow::AddControl ( CControl *pControl )
{
	if ( !pControl )
		return;

	pControl->SetPos ( *pControl->GetPos () + CPos ( 0, m_iTitleBarSize ) );
	pControl->SetParent ( this );

	m_vControls.push_back ( pControl );
}

//--------------------------------------------------------------------------------------
void CWindow::RemoveControl ( CControl *pControl )
{
	// Assert if 'pControl' is invalid
	assert ( pControl &&
			 _UI ( "Invalid 'pControl' pointer" ) );

	std::vector<CControl*>::iterator iter = std::find ( m_vControls.begin (), m_vControls.end (), pControl );
	if ( iter == m_vControls.end () )
	{
		assert ( iter != m_vControls.end () &&
				 _UI ( "The specified CControl for parameter 'pControl' is not attached to this CWindow." ) );
		return;
	}
	else
	{
		m_vControls.erase ( iter );
		SAFE_DELETE ( pControl );
	}
}

//--------------------------------------------------------------------------------------
void CWindow::RemoveAllControls ( void )
{
	for ( size_t i = 0; i < m_vControls.size (); i++ )
		SAFE_DELETE ( m_vControls [ i ] );

	m_vControls.clear ();
}

//--------------------------------------------------------------------------------------
void CWindow::RequestControlFocus ( CControl *pControl )
{
	if ( m_pFocussedControl != pControl )
	{
		if ( pControl &&
			 !pControl->CanHaveFocus () )
			return;

		if ( m_pFocussedControl )
			m_pFocussedControl->OnFocusOut ();

		if ( pControl )
			pControl->OnFocusIn ();

		m_pFocussedControl = pControl;
	}

	if ( pControl )
		BringControlToTop ( pControl );
}

//--------------------------------------------------------------------------------------
CControl *CWindow::GetFocussedControl ( void )
{
	return m_pFocussedControl;
}

//--------------------------------------------------------------------------------------
void CWindow::ClearControlFocus ( void )
{
	if ( m_pFocussedControl )
	{
		m_pFocussedControl->OnFocusOut ();
		m_pFocussedControl = NULL;
	}
}

//--------------------------------------------------------------------------------------
void CWindow::BringControlToTop ( CControl *pControl )
{
	std::vector<CControl*>::iterator iter = std::find ( m_vControls.begin (), m_vControls.end (), pControl );
	if ( iter == m_vControls.end () )
	{
		assert ( iter != m_vControls.end () &&
				 _UI ( "The specified CControl for parameter 'pControl' is not attached to this CWindow." ) );
		return;
	}
	else
	{
		m_vControls.erase ( iter );
		m_vControls.insert ( m_vControls.end (), pControl );
	}

	// Make sure the window has focus, otherwise give it focus.
	if ( !m_bHasFocus )
		m_pDialog->SetFocussedWindow ( this );
}

//--------------------------------------------------------------------------------------
CControl *CWindow::GetNextControl ( CControl *pControl )
{
	size_t size = m_vControls.size ();
	for ( size_t i = 0; i < size; i++ )
	{
		if ( m_vControls [ i ] )
		{
			if ( m_vControls [ i ] == pControl && i + 1 < size )
				return m_vControls [ i + 1 ];
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
CControl *CWindow::GetPrevControl ( CControl *pControl )
{
	for ( size_t i = 0; i < m_vControls.size (); i++ )
	{
		if ( m_vControls [ i ] )
		{
			if ( m_vControls [ i ] == pControl && int ( i - 1 ) > 0 )
				return m_vControls [ i - 1 ];
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
CControl *CWindow::GetControlByText ( const SIMPLEGUI_CHAR *pszText )
{
	for ( auto &control : m_vControls )
	{
		if ( control &&
			 control->GetText () &&
			 pszText )
		{
			if ( !SIMPLEGUI_STRCMP ( control->GetText (), pszText ) )
				return control;
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
CControl *CWindow::GetControlAtArea ( CPos pos )
{
	for ( std::vector<CControl*>::iterator iter = m_vControls.end (); iter != m_vControls.begin (); )
	{
		iter--;
		if ( ( *iter )->ContainsRect ( pos ) )
			return ( *iter );
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
bool CWindow::IsSizing ( void )
{
	return ( ( m_eWindowArea != CLEAR &&
			 m_bSizable ) &&
			 CControl::IsSizing () );
}

//--------------------------------------------------------------------------------------
void CWindow::OnClickLeave ( void )
{
	m_bPressed = m_bDragging = false;
	m_eWindowArea = CLEAR;

	if ( m_pScrollbar )
		m_pScrollbar->OnClickLeave ();
}

//--------------------------------------------------------------------------------------
bool CWindow::OnClickEvent ( void )
{
	return ( m_bPressed ||
			 m_bDragging ||
			 m_eWindowArea != CLEAR ||
			 ( m_pScrollbar && m_pScrollbar->OnClickEvent () ) );
}

//--------------------------------------------------------------------------------------
void CWindow::OnFocusIn ( void )
{
	CControl::OnFocusIn ();

	if ( m_pScrollbar )
		m_pScrollbar->OnFocusIn ();
}

//--------------------------------------------------------------------------------------
void CWindow::OnFocusOut ( void )
{
	CControl::OnFocusOut ();

	if ( m_pScrollbar )
		m_pScrollbar->OnFocusOut ();
}

//--------------------------------------------------------------------------------------
void CWindow::OnMouseEnter ( void )
{
	CControl::OnMouseEnter ();

	if ( m_pScrollbar )
		m_pScrollbar->OnMouseEnter ();
}

//--------------------------------------------------------------------------------------
void CWindow::OnMouseLeave ( void )
{
	CControl::OnMouseLeave ();

	if ( m_pScrollbar )
		m_pScrollbar->OnMouseLeave ();

	if ( m_pControlMouseOver )
	{
		m_pControlMouseOver->OnMouseLeave ();
		m_pControlMouseOver = NULL;
	}
}

//--------------------------------------------------------------------------------------
bool CWindow::CanHaveFocus ( void )
{
	return ( CControl::CanHaveFocus () ||
			 ( m_pScrollbar && 
			 m_pScrollbar->CanHaveFocus () ) );
}

//--------------------------------------------------------------------------------------
void CWindow::SetAlwaysOnTop ( bool bEnable )
{
	m_bOnTop = bEnable;
};

//--------------------------------------------------------------------------------------
bool CWindow::GetAlwaysOnTop ( void )
{
	return m_bOnTop;
}

//--------------------------------------------------------------------------------------
void CWindow::SetMovable ( bool bEnabled )
{
	m_bMovable = bEnabled;
}

//--------------------------------------------------------------------------------------
bool CWindow::GetMovable ( void )
{
	return m_bMovable;
}

//--------------------------------------------------------------------------------------
void CWindow::SetSizable ( bool bEnabled )
{
	m_bSizable = bEnabled;
}

//--------------------------------------------------------------------------------------
bool CWindow::GetSizable ( void )
{
	return m_bSizable;
}

//--------------------------------------------------------------------------------------
void CWindow::SetMaximized ( bool bMinimize )
{
	m_bMaximized;
}

//--------------------------------------------------------------------------------------
bool CWindow::GetMaximized ( void )
{
	return m_bMaximized;
}

//--------------------------------------------------------------------------------------
void CWindow::SetSize ( SIZE size )
{
	m_realSize = m_size = size;

	SetSize ( size.cx, size.cy );
	UpdateScrollbars ();
}

//--------------------------------------------------------------------------------------
void CWindow::SetSize ( int iWidth, int iHeight )
{
	m_realSize.cx = iWidth;
	m_realSize.cy = iHeight;

	SetWidth ( iWidth );
	SetHeight ( iHeight );
}

//--------------------------------------------------------------------------------------
SIZE CWindow::GetRealSize ( void )
{
	return m_realSize;
}

//--------------------------------------------------------------------------------------
void CWindow::ScrollPage ( int nDelta )
{
	if ( !m_pScrollbar )
		return;

	m_pScrollbar->OnMouseWheel ( nDelta );
}

//--------------------------------------------------------------------------------------
int CWindow::GetTitleBarHeight ( void )
{
	return m_iTitleBarSize;
}

//--------------------------------------------------------------------------------------
void CWindow::SetCloseButton ( bool bEnabled )
{
	m_bCloseButtonEnabled = bEnabled;
}

void CWindow::ShowScrollbars ( bool bShow )
{
	m_bShowScrollbar = bShow;
	m_pScrollbar->ShowScrollHor ( bShow );
	m_pScrollbar->ShowScrollVer ( bShow );
}

//--------------------------------------------------------------------------------------
void CWindow::OnMouseMove ( CControl *pControl, UINT uMsg )
{
	if ( !( GetAsyncKeyState ( VK_LBUTTON ) && 
		 pControl ) &&
		 uMsg == WM_MOUSEMOVE )
	{
		// If the mouse is still over the same control, nothing needs to be done
		if ( pControl == m_pControlMouseOver )
			return;

		// Handle mouse leaving the old control
		if ( m_pControlMouseOver )
			m_pControlMouseOver->OnMouseLeave ();

		// Handle mouse entering the new control
		m_pControlMouseOver = pControl;
		if ( pControl != NULL )
			m_pControlMouseOver->OnMouseEnter ();
	}
}

//--------------------------------------------------------------------------------------
bool CWindow::ControlMessages ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CControl::CanHaveFocus () ||
		 m_eWindowArea != CLEAR ||
		 m_pScrollbar->ContainsRect ( pos ) ||
		 !CControl::ContainsRect ( pos ) /*||
		 m_rTitle.InControlArea ( pos )*/ )
	{
		return false;
	}

	switch ( uMsg )
	{ 
		// Keyboard messages
		case WM_KEYDOWN:        
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			// If a control is in focus, and it's enabled, then give
			// it the first chance at handling the message.
			if ( m_pFocussedControl &&
				 m_pFocussedControl->IsEnabled () )
			{
				if ( m_pFocussedControl->HandleKeyboard ( uMsg, wParam, lParam ) )
					return true;
			}
			break;
		}

		// Mouse messages
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_XBUTTONDOWN:
		case WM_XBUTTONUP:
		case WM_LBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_XBUTTONDBLCLK:
		case WM_MOUSEWHEEL:
		{
			// it the first chance at handling the message.
			if ( m_pFocussedControl &&
				 m_pFocussedControl->IsEnabled () )
			{
				if ( m_pFocussedControl->HandleMouse ( uMsg, pos, wParam, lParam ) )
					return true;
			}

			if ( m_pControlMouseOver && 
				 uMsg == WM_MOUSEWHEEL )
			{
				int zDelta = int ( ( short ) HIWORD ( wParam ) ) / WHEEL_DELTA;
				ScrollPage ( -zDelta );
				return true;
			}

			// Not yet handled, see if the mouse is over any controls
			CControl* pControl = GetControlAtArea ( pos );
			if ( pControl != NULL && pControl->IsEnabled () )
			{
				if ( pControl->HandleMouse ( uMsg, pos, wParam, lParam ) )
					return true;
			}
			else
			{
				// which had focus it just lost it
				if ( uMsg == WM_LBUTTONDOWN &&
					 m_pFocussedControl )
				{
					m_pFocussedControl->OnFocusOut ();
					m_pFocussedControl = NULL;
				}
			}

			OnMouseMove ( pControl, uMsg );
		}
		break;
	}

	return false;
}

//--------------------------------------------------------------------------------------
bool CWindow::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	if ( !CControl::CanHaveFocus () ||
		 !m_pScrollbar )
		return false;
	
	bool bInArea = false;
	for ( size_t i = E_WINDOW_AREA::TOP_LEFT; i < E_WINDOW_AREA::BOTTOM + 1; i++ )
	{
		SControlRect *rRect = GetWindowRect ( ( E_WINDOW_AREA ) i );
		if ( rRect &&
			 rRect->InControlArea ( pos ) )
		{
			bInArea = true;
		}
	}

	// Check if mouse is over window boundaries
	if ( !bInArea )
	{
		// Let the scroll bar handle it first.
		if ( m_pScrollbar->HandleMouse ( uMsg, pos, wParam, lParam ) )
			return true;
	}
	
	switch ( uMsg )
	{
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONDOWN:
		{
			if ( m_rButton.InControlArea ( pos ) &&
				 m_bCloseButtonEnabled )
			{
				// Pressed while inside the control
				m_bPressed = true;

				if ( m_pDialog && !m_bHasFocus )
					m_pDialog->SetFocussedWindow ( this );

				return true;
			}

			if ( m_bSizable )
			{
				for ( size_t i = 1; i < 9; i++ )
				{
					SControlRect *rRect = GetWindowRect ( ( E_WINDOW_AREA ) i );
					if ( rRect && 
						 rRect->InControlArea ( pos ) )
					{
						m_eWindowArea = ( E_WINDOW_AREA ) i;

						if ( m_eWindowArea == TOP ||
							 m_eWindowArea == TOP_LEFT ||
							 m_eWindowArea == TOP_RIGHT )
						{
							m_nDragY = m_pos.GetY () - pos.GetY ();

							if ( m_eWindowArea == TOP )
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::S_RESIZE );
							else if ( m_eWindowArea == TOP_LEFT )
							{
								m_nDragX = m_pos.GetX () - pos.GetX ();
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::NE_RESIZE );
							}
							else if ( m_eWindowArea == TOP_RIGHT )
							{
								m_nDragX = m_pos.GetX () + m_rBoundingBox.size.cx - pos.GetX ();
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::SE_RESIZE );
							}
						}
						else if ( m_eWindowArea == LEFT ||
								  m_eWindowArea == LEFT_BOTTOM )
						{
							m_nDragX = m_pos.GetX () - pos.GetX ();

							if ( m_eWindowArea == LEFT )
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::E_RESIZE );
							else if ( m_eWindowArea == LEFT_BOTTOM )
							{
								m_nDragY = m_pos.GetY () + m_rBoundingBox.size.cy - pos.GetY ();
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::SE_RESIZE );
							}
						}
						else if ( m_eWindowArea == RIGHT ||
								  m_eWindowArea == RIGHT_BOTTOM )
						{
							m_nDragX = m_pos.GetX () + m_rBoundingBox.size.cx - pos.GetX ();

							if ( m_eWindowArea == RIGHT )
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::E_RESIZE );
							else if ( m_eWindowArea == RIGHT_BOTTOM )
							{
								m_nDragY = m_pos.GetY () + m_rBoundingBox.size.cy - pos.GetY ();
								m_pDialog->GetMouse ()->SetCursorType ( CMouse::NE_RESIZE );
							}
						}
						else if ( m_eWindowArea == BOTTOM )
						{
							m_pDialog->GetMouse ()->SetCursorType ( CMouse::N_RESIZE );
							m_nDragY = m_pos.GetY () + m_rBoundingBox.size.cy - pos.GetY ();
						}

						if ( m_pDialog && !m_bHasFocus )
							m_pDialog->SetFocussedWindow ( this );

						return true;
					}
				}
			}

			if ( m_rTitle.InControlArea ( pos ) )
			{
				m_pControlMouseOver = NULL;
				m_pFocussedControl = NULL;

				if ( m_pDialog->GetMouse ()->GetLeftButton () == 2 )
					m_bMaximized = !m_bMaximized;

				if ( m_bMovable )
				{
					m_bDragging = true;
					m_posDif = m_pos - pos;
				}

				if ( m_pDialog && !m_bHasFocus )
					m_pDialog->SetFocussedWindow ( this );

				return true;
			}

			if ( m_rBoundingBox.InControlArea ( pos ) && !m_bMaximized )
			{
				if ( m_pDialog && !m_bHasFocus )
					m_pDialog->SetFocussedWindow ( this );

				return true;
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			m_bDragging = false;
			m_eWindowArea = CLEAR;
			m_size = m_rBoundingBox.size;

			m_pControlMouseOver = NULL;
			m_pFocussedControl = NULL;

			if ( m_bPressed )
			{
				m_bPressed = false;

				// Button click
				if ( m_rButton.InControlArea ( pos ) )
				{
					SendEvent ( EVENT_CONTROL_CLICKED, true );
					SetVisible ( false );

					if ( m_pDialog )
						m_pDialog->ClearFocussedWindow ();
				}

				return true;
			}

			break;
		}

		case WM_MOUSEMOVE:
		{
			if ( m_bSizable )
			{
				if ( m_bMouseOver &&
					 m_eWindowArea == CLEAR &&
					 !m_rButton.InControlArea ( pos ) )
				{
					for ( size_t i = 1; i < 9; i++ )
					{
						SControlRect *rRect = GetWindowRect ( ( E_WINDOW_AREA ) i );
						if ( rRect && 
							 rRect->InControlArea ( pos ) )
						{
							switch ( i )
							{
								case TOP_LEFT:
								case RIGHT_BOTTOM:
									m_pDialog->GetMouse ()->SetCursorType ( CMouse::NE_RESIZE );
									break;

								case LEFT_BOTTOM:
								case TOP_RIGHT:
									m_pDialog->GetMouse ()->SetCursorType ( CMouse::SE_RESIZE );
									break;

								case TOP:
									m_pDialog->GetMouse ()->SetCursorType ( CMouse::S_RESIZE );
									break;

								case LEFT:
								case RIGHT:
									m_pDialog->GetMouse ()->SetCursorType ( CMouse::E_RESIZE );
									break;

								case BOTTOM:
									m_pDialog->GetMouse ()->SetCursorType ( CMouse::N_RESIZE );
									break;
							}
							break;
						}
					}
				}

				if ( m_eWindowArea == TOP ||
					 m_eWindowArea == TOP_LEFT ||
					 m_eWindowArea == TOP_RIGHT )
				{
					m_rBoundingBox.size.cy = m_rBoundingBox.size.cy + ( m_pos.GetY () - pos.GetY () ) - m_nDragY;
					m_pos.SetY ( pos.GetY () + m_nDragY );

					if ( m_eWindowArea == TOP_LEFT )
					{
						m_rBoundingBox.size.cx = m_rBoundingBox.size.cx + ( m_pos.GetX () - pos.GetX () ) - m_nDragX;
						m_pos.SetX ( pos.GetX () + m_nDragX );
					}
					else if ( m_eWindowArea == TOP_RIGHT )
						m_rBoundingBox.size.cx = pos.GetX () - m_pos.GetX () + m_nDragX;
				}
				else if ( m_eWindowArea == LEFT ||
						  m_eWindowArea == LEFT_BOTTOM )
				{
					m_rBoundingBox.size.cx = m_rBoundingBox.size.cx + ( m_pos.GetX () - pos.GetX () ) - m_nDragX;
					m_pos.SetX ( pos.GetX () + m_nDragX );

					if ( m_eWindowArea == LEFT_BOTTOM )
						m_rBoundingBox.size.cy = pos.GetY () - m_pos.GetY () + m_nDragY;
				}
				else if ( m_eWindowArea == RIGHT ||
						  m_eWindowArea == RIGHT_BOTTOM )
				{
					m_rBoundingBox.size.cx = pos.GetX () - m_pos.GetX () + m_nDragX;

					if ( m_eWindowArea == RIGHT_BOTTOM )
						m_rBoundingBox.size.cy = pos.GetY () - m_pos.GetY () + m_nDragY;
				}
				else if ( m_eWindowArea == BOTTOM )
					m_rBoundingBox.size.cy = pos.GetY () - m_pos.GetY () + m_nDragY;
			}

			if ( m_bDragging &&
				 m_bMovable )
			{
				m_pos = pos + m_posDif;
			}

			// Adjust position
			m_rBoundingBox.pos = m_pos;
			return false;
		}

		case WM_MOUSEWHEEL:
		{
			int zDelta = int ( ( short ) HIWORD ( wParam ) ) / WHEEL_DELTA;
			ScrollPage ( -zDelta );
			return true;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
bool CWindow::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !CControl::CanHaveFocus () )
		return false;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_TAB:
					RequestControlFocus ( m_vControls [ 0 ] );
					return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------
void CWindow::UpdateScrollbars ( void )
{
	if ( !m_pScrollbar )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	int nValueX = m_rBoundingBox.size.cx + ( m_maxControlSize.cx - ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx ) );
	int nValueY = ( ( m_rBoundingBox.size.cy - m_iTitleBarSize ) +
					( m_maxControlSize.cy - ( ( m_rBoundingBox.pos.GetY () + m_iTitleBarSize ) + ( m_rBoundingBox.size.cy - m_iTitleBarSize ) ) ) );

	pScrollbarHor->SetStepSize ( m_rBoundingBox.size.cx / 10 );
	pScrollbarVer->SetStepSize ( m_rBoundingBox.size.cy / 10 );

	// Set scrollbars max range
	m_pScrollbar->SetTrackRange ( nValueX, nValueY );
}

#define WINDOW_SIZE 5

//--------------------------------------------------------------------------------------
void CWindow::UpdateRects ( void )
{
	if ( !m_pScrollbar )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	//CControl::UpdateRects ();
	
	SControlRect rRect = m_rBoundingBox;
	rRect.size.cy -= m_iTitleBarSize;
	rRect.pos.SetY ( rRect.pos.GetY () + m_iTitleBarSize );

	m_pScrollbar->UpdateScrollbars ( rRect );

	m_rTitle = m_rBoundingBox;
	m_rTitle.size.cy = m_iTitleBarSize;

	m_rWindowTop = m_rBoundingBox;
	m_rWindowTop.pos.SetY ( m_rBoundingBox.pos.GetY () );
	m_rWindowTop.size.cx = m_rWindowTop.size.cx - 2; m_rWindowTop.size.cy = WINDOW_SIZE;

	m_rWindowLeft = m_rBoundingBox;
	m_rWindowLeft.pos.SetX ( m_rBoundingBox.pos.GetX () - WINDOW_SIZE );
	m_rWindowLeft.size.cx = WINDOW_SIZE;

	m_rWindowRight = m_rBoundingBox;
	m_rWindowRight.pos.SetX ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx );
	m_rWindowRight.size.cx = WINDOW_SIZE;

	m_rWindowBottom = m_rBoundingBox;
	m_rWindowBottom.pos.SetY ( m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy );
	m_rWindowBottom.size.cx = m_rWindowBottom.size.cx - 2; m_rWindowBottom.size.cy = WINDOW_SIZE;

	m_rWindowTopLeft = m_rBoundingBox;
	m_rWindowTopLeft.pos.SetX ( m_rBoundingBox.pos.GetX () - WINDOW_SIZE );
	m_rWindowTopLeft.pos.SetY ( m_rBoundingBox.pos.GetY () - WINDOW_SIZE );
	m_rWindowTopLeft.size.cx = WINDOW_SIZE; m_rWindowTopLeft.size.cy = WINDOW_SIZE;

	m_rWindowTopRight = m_rBoundingBox;
	m_rWindowTopRight.pos.SetX ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx );
	m_rWindowTopRight.pos.SetY ( m_rWindowTopRight.pos.GetY () - WINDOW_SIZE );
	m_rWindowTopRight.size.cx = WINDOW_SIZE; m_rWindowTopRight.size.cy = WINDOW_SIZE;

	m_rWindowLeftBottom = m_rBoundingBox;
	m_rWindowLeftBottom.pos.SetX ( m_rBoundingBox.pos.GetX () - WINDOW_SIZE );
	m_rWindowLeftBottom.pos.SetY ( m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy );
	m_rWindowLeftBottom.size.cx = WINDOW_SIZE; m_rWindowLeftBottom.size.cy = WINDOW_SIZE;

	m_rWindowRightBottom = m_rBoundingBox;
	m_rWindowRightBottom.pos.SetX ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx - WINDOW_SIZE );
	m_rWindowRightBottom.pos.SetY ( m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy );
	m_rWindowRightBottom.size.cx = WINDOW_SIZE;	m_rWindowRightBottom.size.cy = WINDOW_SIZE;

	m_rButton = m_rBoundingBox;
	m_rButton.pos.SetX ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx - 30 );
	m_rButton.size.cx = 27.f;
	m_rButton.size.cy = 16.f;
}

//--------------------------------------------------------------------------------------
bool CWindow::ContainsRect ( CPos pos )
{
	if ( !CControl::CanHaveFocus () ||
		 !m_pScrollbar )
		return false;

	for ( size_t i = 1; i < 9; i++ )
	{
		SControlRect *rRect = GetWindowRect ( ( E_WINDOW_AREA ) i );
		if ( rRect && rRect->InControlArea ( pos ) )
			return true;
	}

	return ( ( m_pScrollbar->ContainsRect ( pos ) && m_eWindowArea == CLEAR ) ||
			 ( m_rBoundingBox.InControlArea ( pos ) && !m_bMaximized ) ||
			 m_rTitle.InControlArea ( pos ) ||
			 m_rButton.InControlArea ( pos ) );
}

//--------------------------------------------------------------------------------------
SControlRect *CWindow::GetWindowRect ( E_WINDOW_AREA eArea )
{
	if ( m_pFocussedControl &&
		 m_pFocussedControl->OnClickEvent () ||
		 ( m_bDragging ||
		 m_bPressed ||
		 m_bMaximized ) ||
		 m_pScrollbar->OnClickEvent () )
		return NULL;

	switch ( eArea )
	{
		case TOP_LEFT:
			return &m_rWindowTopLeft;
		case LEFT_BOTTOM:
			return &m_rWindowLeftBottom;
		case TOP_RIGHT:
			return &m_rWindowTopRight;
		case RIGHT_BOTTOM:
			return &m_rWindowRightBottom;
		case TOP:
			return &m_rWindowTop;
		case LEFT:
			return &m_rWindowLeft;
		case RIGHT:
			return &m_rWindowRight;
		case BOTTOM:
			return &m_rWindowBottom;
		default:
			return NULL;
	}
}