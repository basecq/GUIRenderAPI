#include "CGUI.h"
#include <stdarg.h>
#include <stdio.h>

CDialog::CDialog ( IDirect3DDevice9 *pDevice )
{
	InitializeCriticalSection ( &cs );

	if ( !pDevice )
		MessageBox ( 0, _UI ( "pDevice invalid." ), 0, 0 );

	m_pDevice = pDevice;
	m_pFocussedWindow = m_pMouseOverWindow = NULL;

	m_pRender = new CD3DRender ( 128 );
	if ( m_pRender )
		m_pRender->Initialize ( pDevice );

	m_pMouse = new CMouse ( this );

	m_bVisible = true;
}

CDialog::~CDialog ( void )
{
	EnterCriticalSection ( &cs );

	for ( size_t i = 0; i < m_vFont.size (); i++ )
		SAFE_DELETE ( m_vFont [ i ] );

	for ( size_t i = 0; i < m_vTexture.size (); i++ )
		SAFE_DELETE ( m_vTexture [ i ] );

	SAFE_DELETE ( m_pMouse );
	SAFE_DELETE ( m_pFocussedWindow );
	SAFE_DELETE ( m_pMouseOverWindow );

	RemoveAllWindows ();

	LeaveCriticalSection ( &cs );
	DeleteCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
void CDialog::LoadFont ( const SIMPLEGUI_CHAR *szFontName, DWORD dwHeight, bool bBold, CD3DFont **ppCreated )
{
	CD3DFont *pFont = new CD3DFont ( szFontName, dwHeight, bBold );
	if ( !pFont )
		return;

	if ( FAILED ( pFont->Initialize ( m_pDevice ) ) )
		return;

	if ( ppCreated != NULL )
		*ppCreated = pFont;

	m_vFont.push_back ( pFont );
}

//--------------------------------------------------------------------------------------
void CDialog::LoadTexture ( const SIMPLEGUI_CHAR *szPath, CD3DTexture **ppCreated )
{
	CD3DTexture *pTexture = new CD3DTexture ( m_pDevice );

	if ( !pTexture )
		return;

	if ( FAILED ( pTexture->CreateTextureFromFile ( szPath ) ) )
		return;

	if ( ppCreated != NULL )
		*ppCreated = pTexture;

	m_vTexture.push_back ( pTexture );
}

//--------------------------------------------------------------------------------------
void CDialog::LoadTexture ( LPCVOID pSrc, UINT uSrcSize, CD3DTexture **ppCreated )
{
	CD3DTexture *pTexture = new CD3DTexture ( m_pDevice );
	if ( !pTexture )
		return;

	if ( FAILED ( pTexture->CreateTextureFromMemory ( pSrc, uSrcSize ) ) )
		return;

	if ( ppCreated != NULL )
		*ppCreated = pTexture;

	m_vTexture.push_back ( pTexture );
}

//--------------------------------------------------------------------------------------
void CDialog::DrawFont ( SControlRect &rect, DWORD dwColor, const SIMPLEGUI_CHAR *szText, DWORD dwFlags, CD3DFont *pFont )
{
	if ( !szText )
		return;

	if ( !pFont && !m_vFont.empty () )
		pFont = GetFont ();

	CPos pos = rect.pos;
	pFont->Print ( pos.GetX (), pos.GetY (), dwColor, szText, dwFlags );
}

//--------------------------------------------------------------------------------------
void CDialog::DrawBox ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias )
{
	if ( !m_pRender )
		return;

	CPos pos = rect.pos;
	SIZE size = rect.size;

	m_pRender->D3DBox ( pos.GetX (), pos.GetY (), size.cx, size.cy, 0.f, d3dColor, d3dColorOutline, bAntAlias );
}

//--------------------------------------------------------------------------------------
void CDialog::DrawTriangle ( SControlRect &rect, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias )
{
	if ( !m_pRender )
		return;

	CPos pos = rect.pos;
	SIZE size = rect.size;

	int nMax = max ( size.cx, size.cy );
	m_pRender->D3DTriangle ( pos.GetX (), pos.GetY (), size.cx - size.cy + nMax, fAngle, d3dColor, d3dColorOutline, bAntAlias );
}

//--------------------------------------------------------------------------------------
void CDialog::DrawCircle ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias )
{
	if ( !m_pRender )
		return;

	CPos pos = rect.pos;
	SIZE size = rect.size;

	int nMax = max ( size.cx, size.cy );
	m_pRender->D3DCircle ( pos.GetX (), pos.GetY (), size.cx - size.cy + nMax, d3dColor, d3dColorOutline, bAntAlias );
}

CProgressBarHorizontal *CDialog::AddProgressBarHorizontal ( CWindow *pWindow, int iX, int iY, int iWidth, int iHeight, float fMax, float fValue, tAction Callback )
{
	CProgressBarHorizontal* pProgressBar = new CProgressBarHorizontal ( this );

	if ( pProgressBar )
	{
		pProgressBar->SetPos ( CPos ( iX, iY ) );
		pProgressBar->SetSize ( iWidth, iHeight );
		pProgressBar->SetMaxValue ( fMax );
		pProgressBar->SetValue ( fValue );
		pProgressBar->SetAction ( Callback );

		if ( pWindow )
			pWindow->AddControl ( pProgressBar );
	}

	return pProgressBar;
}

CProgressBarVertical *CDialog::AddProgressBarVertical ( CWindow *pWindow, int iX, int iY, int iWidth, int iHeight, float fMax, float fValue, tAction Callback )
{
	CProgressBarVertical* pProgressBar = new CProgressBarVertical ( this );

	if ( pProgressBar )
	{
		pProgressBar->SetPos ( CPos ( iX, iY ) );
		pProgressBar->SetSize ( iWidth, iHeight );
		pProgressBar->SetMaxValue ( fMax );
		pProgressBar->SetValue ( fValue );
		pProgressBar->SetAction ( Callback );

		if ( pWindow )
			pWindow->AddControl ( pProgressBar );
	}

	return pProgressBar;
}

//--------------------------------------------------------------------------------------
CWindow *CDialog::AddWindow ( int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CWindow* pWindow = new CWindow ( this );

	if ( pWindow )
	{
		pWindow->SetPos ( CPos ( X, Y ) );
		pWindow->SetSize ( Width, Height );
		pWindow->SetText ( szString );
		pWindow->SetAction ( Callback );
		pWindow->SetFont ( this->GetFont () );

		AddWindow ( pWindow );
	}

	return pWindow;
}

//--------------------------------------------------------------------------------------
CButton *CDialog::AddButton ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CButton* pButton = new CButton ( this );

	if ( pButton )
	{
		pButton->SetPos ( CPos ( X, Y ) );
		pButton->SetSize ( Width, Height );
		pButton->SetText ( szString );
		pButton->SetAction ( Callback );
		pButton->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pButton );
	}

	return pButton;
}

//--------------------------------------------------------------------------------------
CCheckBox *CDialog::AddCheckBox ( CWindow *pWindow, int X, int Y, int Width, bool bChecked, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CCheckBox* pCheckBox = new CCheckBox ( this );

	if ( pCheckBox )
	{
		pCheckBox->SetPos ( CPos ( X, Y ) );
		pCheckBox->SetSize ( Width, 20 );
		pCheckBox->SetText ( szString );
		pCheckBox->SetAction ( Callback );
		pCheckBox->SetChecked ( bChecked );
		pCheckBox->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pCheckBox );
	}

	return pCheckBox;
}

//--------------------------------------------------------------------------------------
CListBox *CDialog::AddListBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback )
{
	CListBox* pListBox = new CListBox ( this );

	if ( pListBox )
	{
		pListBox->SetPos ( CPos ( X, Y ) );
		pListBox->SetSize ( Width, Height );
		pListBox->SetAction ( Callback );
		pListBox->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pListBox );
	}

	return pListBox;
}

CListView *CDialog::AddListView ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CListView* pListBiew = new CListView ( this );

	if ( pListBiew )
	{
		pListBiew->SetPos ( CPos ( X, Y ) );
		pListBiew->SetSize ( Width, Height );
		pListBiew->SetAction ( Callback );
		pListBiew->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pListBiew );
	}

	return pListBiew;
}

//--------------------------------------------------------------------------------------
CLogBox *CDialog::AddTextBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback )
{
	CLogBox* pTextBox = new CLogBox ( this );

	if ( pTextBox )
	{
		pTextBox->SetPos ( CPos ( X, Y ) );
		pTextBox->SetSize ( Width, Height );
		pTextBox->SetAction ( Callback );
		pTextBox->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pTextBox );
	}

	return pTextBox;
}

/*//--------------------------------------------------------------------------------------
CListView *CDialog::AddListView ( CWindow *pWindow, int X, int Y, int Width, int Height, int Columns, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CListView* pListView = new CListView ( this );

	if ( pListView )
	{
		pListView->SetPos ( CPos ( X, Y ) );
		//pListView->SetSize ( Width, Height );
		pListView->SetText ( szString );
		pListView->SetAction ( Callback );
		pListView->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pListView );
	}

	return pListView;
}
*/
//--------------------------------------------------------------------------------------
CLabel *CDialog::AddLabel ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CLabel* pLabel = new CLabel ( this );

	if ( pLabel )
	{
		pLabel->SetPos ( CPos ( X, Y ) );
		pLabel->SetText ( szString );
		pLabel->SetFont ( this->GetFont () );
		pLabel->SetSize ( Width, Height );
		pLabel->SetAction ( Callback );

		if ( pWindow )
			pWindow->AddControl ( pLabel );
	}

	return pLabel;
}
/*
//--------------------------------------------------------------------------------------
CEditBox *CDialog::AddEditBox ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CEditBox* pEditBox = new CEditBox ( this );

	if ( pEditBox )
	{
		pEditBox->SetPos ( CPos ( X, Y ) );
		pEditBox->SetSize ( Width, Height );
		pEditBox->SetText ( szString );
		pEditBox->SetAction ( Callback );
		pEditBox->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pEditBox );
	}

	return pEditBox;
}
*/
//--------------------------------------------------------------------------------------
CDropDown *CDialog::AddDropDown ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CDropDown* pDropDown = new CDropDown ( this );

	if ( pDropDown )
	{
		pDropDown->SetPos ( CPos ( X, Y ) );
		pDropDown->SetSize ( Width, Height );
		pDropDown->SetText ( szString );
		pDropDown->SetAction ( Callback );
		pDropDown->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pDropDown );
	}

	return pDropDown;
}

//--------------------------------------------------------------------------------------
CRadioButton *CDialog::AddRadioButton ( CWindow *pWindow, int iGroup, int X, int Y, int Width, const SIMPLEGUI_CHAR *szString, tAction Callback )
{
	CRadioButton *pRadioButton = new CRadioButton ( this );

	if ( pRadioButton )
	{
		pRadioButton->SetGroup ( iGroup );
		pRadioButton->SetPos ( CPos ( X, Y ) );
		pRadioButton->SetSize ( Width, 20 );
		pRadioButton->SetText ( szString );
		pRadioButton->SetAction ( Callback );
		pRadioButton->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pRadioButton );
	}

	return pRadioButton;
}

/*//--------------------------------------------------------------------------------------
CPictureBox *CDialog::AddImage ( CWindow *pWindow, const TCHAR * szPath, int X, int Y, int Width, int Height, tAction Callback )
{
	CPictureBox *pImage = new CPictureBox ( this );

	if ( pImage )
	{
		pImage->SetPos ( CPos ( X, Y ) );
		pImage->SetHeight ( Height );
		pImage->SetTexture ( szPath );
		pImage->SetAction ( Callback );

		if ( pWindow )
			pWindow->AddControl ( pImage );
	}

	return pImage;
}
*/
//--------------------------------------------------------------------------------------
CTrackBarHorizontal *CDialog::AddTrackBar ( CWindow * pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback )
{
	CTrackBarHorizontal *pTrackBar = new CTrackBarHorizontal ( this );

	if ( pTrackBar )
	{
		pTrackBar->SetPos ( CPos ( X, Y ) );
		pTrackBar->SetSize ( Width, Height );
		pTrackBar->SetAction ( Callback );
		pTrackBar->SetRange ( nMin, nMax );
		pTrackBar->SetValue ( nValue );

		if ( pWindow )
			pWindow->AddControl ( pTrackBar );
	}

	return pTrackBar;
}

CTrackBarVertical *CDialog::AddTrackBarVertical ( CWindow * pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback )
{
	CTrackBarVertical *pTrackBar = new CTrackBarVertical ( this );

	if ( pTrackBar )
	{
		pTrackBar->SetPos ( CPos ( X, Y ) );
		pTrackBar->SetSize ( Width, Height );
		pTrackBar->SetAction ( Callback );
		pTrackBar->SetRange ( nMin, nMax );
		pTrackBar->SetValue ( nValue );

		if ( pWindow )
			pWindow->AddControl ( pTrackBar );
	}

	return pTrackBar;
}

//--------------------------------------------------------------------------------------
CScrollBarVertical *CDialog::AddScrollBar ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback )
{
	CScrollBarVertical *pScrollBar = new CScrollBarVertical ( this );

	if ( pScrollBar )
	{
		pScrollBar->SetPos ( CPos ( X, Y ) );
		pScrollBar->SetSize ( Width, Height );
		pScrollBar->SetAction ( Callback );
		pScrollBar->SetTrackRange ( nMin, nMax );
		pScrollBar->SetPageSize ( nPagSize );
		pScrollBar->ShowItem ( nValue );
		pScrollBar->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pScrollBar );
	}

	return pScrollBar;
}

//--------------------------------------------------------------------------------------
CScrollBarHorizontal *CDialog::AddScrollBarHorizontal ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback )
{
	CScrollBarHorizontal *pScrollBar = new CScrollBarHorizontal ( this );

	if ( pScrollBar )
	{
		pScrollBar->SetPos ( CPos ( X, Y ) );
		pScrollBar->SetSize ( Width, Height );
		pScrollBar->SetAction ( Callback );
		pScrollBar->SetTrackRange ( nMin, nMax );
		pScrollBar->SetPageSize ( nPagSize );
		pScrollBar->ShowItem ( nValue );
		pScrollBar->SetFont ( this->GetFont () );

		if ( pWindow )
			pWindow->AddControl ( pScrollBar );
	}

	return pScrollBar;
}
//--------------------------------------------------------------------------------------
void CDialog::Draw ( void )
{
	if ( !m_bVisible )
		return;

	EnterCriticalSection ( &cs );

	for ( size_t i = 0; i < m_vWindows.size (); i++ )
	{
		if ( !m_vWindows [ i ] )
			continue;

		if ( !m_vWindows [ i ]->IsVisible () )
			continue;

		m_vWindows [ i ]->UpdateRects ();
		m_vWindows [ i ]->Draw ();
	}

	m_pMouse->Draw ();

	LeaveCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
void CDialog::MsgProc ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	EnterCriticalSection ( &cs );

	if ( !m_pMouse ) 
		return;

	m_pMouse->HandleMessage ( uMsg, wParam, lParam );

	if ( m_vWindows.empty () )
		return;

	CPos pos = m_pMouse->GetPos ();

	if ( !GetAsyncKeyState(VK_LBUTTON) )
		m_pMouse->SetCursorType ( CMouse::DEFAULT );

	// Check for any window with focus
	if ( m_pFocussedWindow )
	{
		CControl *pControl = m_pFocussedWindow->GetFocussedControl ();

		// If the widget is a dropdown, leave handling message outside the windows
		if ( pControl &&
			 ( pControl->GetType () == CControl::TYPE_DROPDOWN ) )
		{

			m_pFocussedWindow->OnMouseMove ( pControl, uMsg );

			// Let then give it the first chance at handling keyboard.
			if ( pControl->HandleKeyboard ( uMsg, wParam, lParam ) )
				return;

			if ( pControl->HandleMouse ( uMsg, pos, wParam, lParam ) ||
				 pControl->ContainsRect ( pos ) )
				return;
		}
	}

	// First handle messages from the windows widgets

	// See if the mouse is over any windows
	CWindow* pWindow = GetWindowAtPos ( pos );
	if ( pWindow )
	{
		bool bOnClick = false;
		if ( m_pFocussedWindow && m_pFocussedWindow->OnClickEvent () )
			bOnClick = true;

		if ( !bOnClick && pWindow->ControlMessages ( uMsg, pos, wParam, lParam ) )
			return;
	}

	// Handle windows messages
	switch ( uMsg )
	{
		// Keyboard messages
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			// If a window is in focus, and it's enabled, then give
			// it the first chance at handling the message.
			if ( m_pFocussedWindow &&
				 m_pFocussedWindow->IsEnabled () )
			{
				if ( m_pFocussedWindow->HandleKeyboard ( uMsg, wParam, lParam ) )
					return;
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
			if ( m_pFocussedWindow )
			{
				// See if the mouse is over any windows		
				CWindow* pWindow = GetWindowAtPos ( pos );

				// If the control is in focus, and if the mouse is outside the window, then leave 
				// the click event
				if ( uMsg == WM_LBUTTONUP )
				{
					// See if the window has a focused control
					CControl *pControl = m_pFocussedWindow->GetFocussedControl ();
					if ( pControl && pControl->OnClickEvent () )
					{
						pControl->OnClickLeave ();
						m_pFocussedWindow->ClearControlFocus ();
					}
				}

				// If the window is not always on top, and it's enabled, then give
				// it the first chance at handling the message.
				if ( !( pWindow && pWindow->GetAlwaysOnTop () ) &&
					 m_pFocussedWindow->IsEnabled () )
				{
					if ( m_pFocussedWindow->HandleMouse ( uMsg, pos, wParam, lParam ) )
						return;
				}
				else
				{			
					// If the window is in focus, and if the mouse is outside the window, then leave 
					// the click event
					if ( uMsg == WM_LBUTTONUP )
					{
						if ( m_pFocussedWindow->OnClickEvent () )
							m_pFocussedWindow->OnClickLeave ();
					}
				}
			}

			// Not yet handled, see if the mouse is over any windows		
			CWindow* pWindow = GetWindowAtPos ( pos );
			if ( pWindow && pWindow->IsEnabled () )
			{
				if ( m_pFocussedWindow )
				{
					// See if the window has a focused control
					CControl *pControl = m_pFocussedWindow->GetFocussedControl ();

					// If the control is in focus, and if the mouse is outside the window, if there
					// was a window which had focus then leave the click event
					if ( pWindow != m_pFocussedWindow &&
						 uMsg == WM_LBUTTONUP &&
						 pControl &&
						 pControl->OnClickEvent () )
					{
						pControl->OnClickLeave ();
					}
				}

				if ( pWindow->HandleMouse ( uMsg, pos, wParam, lParam ) )
					return;
			}
			else
			{
				if ( m_pFocussedWindow )
				{
					// See if the window has a focused control
					CControl *pControl = m_pFocussedWindow->GetFocussedControl ();

					if ( uMsg == WM_LBUTTONDOWN )
					{
						// Clear the focused control
						m_pFocussedWindow->ClearControlFocus ();

						// Which had focus it just lost it
						m_pFocussedWindow->OnFocusOut ();
						m_pFocussedWindow = NULL;
					}
					else
					{
						// If the control is in focus, and if the mouse is outside the window, then leave 
					    // the click event
						if ( uMsg == WM_LBUTTONUP &&
							 pControl &&
							 pControl->OnClickEvent () )
						{
							pControl->OnClickLeave ();
						}
					}
				}
			}

			if ( !( GetAsyncKeyState ( VK_LBUTTON ) && pWindow ) && 
				 uMsg == WM_MOUSEMOVE )
			{
				// If the mouse is still over the same window, nothing needs to be done
				if ( pWindow == m_pMouseOverWindow )
					return;

				// Handle mouse leaving the old window
				if ( m_pMouseOverWindow )
				{
					m_pMouseOverWindow->OnMouseLeave ();
					//m_pMouseOverWindow->ClearControlFocus ();
				}
				// Handle mouse entering the new window
				m_pMouseOverWindow = pWindow;
				if ( pWindow != NULL )
					m_pMouseOverWindow->OnMouseEnter ();
			}
		}
		break;
	}

	LeaveCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
void CDialog::AddWindow ( CWindow *pWindow )
{
	if ( !pWindow )
		return;

	m_vWindows.push_back ( pWindow );
	m_pFocussedWindow = pWindow;
}

//--------------------------------------------------------------------------------------
void CDialog::RemoveWindow ( CWindow *pWindow )
{
	for ( size_t i = 0; i < m_vWindows.size (); i++ )
	{
		if ( m_vWindows [ i ] == pWindow )
		{
			SAFE_DELETE ( pWindow );
			m_vWindows.erase ( m_vWindows.begin () + i );
			return;
		}
	}
}

//--------------------------------------------------------------------------------------
void CDialog::RemoveAllWindows ( void )
{
	for ( size_t i = 0; i < m_vWindows.size (); i++ )
		SAFE_DELETE ( m_vWindows [ i ] );

	m_vWindows.clear ();
}

//--------------------------------------------------------------------------------------
void CDialog::SetFocussedWindow ( CWindow *pWindow )
{
	if ( m_pFocussedWindow == pWindow )
		return;

	EnterCriticalSection ( &cs );

	if ( m_pFocussedWindow )
		m_pFocussedWindow->OnFocusOut ();

	if ( pWindow )
		pWindow->OnFocusIn ();

	if ( pWindow )
		BringWindowToTop ( pWindow );

	m_pFocussedWindow = pWindow;

	LeaveCriticalSection ( &cs );
}

void CDialog::ClearFocussedWindow ( void )
{
	if ( m_pFocussedWindow )
	{
		m_pFocussedWindow->OnFocusOut ();
		m_pFocussedWindow = NULL;
	}
}

//--------------------------------------------------------------------------------------
CWindow *CDialog::GetFocussedWindow ( void )
{
	return m_pFocussedWindow;
}

//--------------------------------------------------------------------------------------
void CDialog::BringWindowToTop ( CWindow *pWindow )
{
	for ( size_t i = 0; i < m_vWindows.size (); i++ )
	{
		if ( !m_vWindows [ i ] )
			continue;

		if ( !m_vWindows [ i ]->GetAlwaysOnTop () && 
			 m_vWindows [ i ] == pWindow )
		{
			m_vWindows.erase ( m_vWindows.begin () + i );
			m_vWindows.insert ( m_vWindows.end (), pWindow );
		}

		if ( m_vWindows [ i ]->GetAlwaysOnTop () )
		{
			CWindow *pOldWin = m_vWindows [ i ];

			m_vWindows.erase ( m_vWindows.begin () + i );
			m_vWindows.insert ( m_vWindows.end (), pOldWin );
		}
	}
}

//--------------------------------------------------------------------------------------
CWindow *CDialog::GetWindowAtPos ( CPos pos )
{
	for ( int i = static_cast< int >( m_vWindows.size () ) - 1; i >= 0; i-- )
	{
		if ( m_vWindows [ i ]->ContainsRect ( pos ) )
			return m_vWindows [ i ];
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
CWindow* CDialog::GetWindowByText ( const SIMPLEGUI_CHAR *pszText )
{
	for ( size_t i = 0; i < m_vWindows.size (); i++ )
	{
		if ( m_vWindows [ i ] )
		{
			if ( !SIMPLEGUI_STRCMP ( m_vWindows [ i ]->GetText (), pszText ) )
				return m_vWindows [ i ];
		}
	}

	return NULL;
}

//--------------------------------------------------------------------------------------
void CDialog::OnLostDevice ( void )
{
	EnterCriticalSection ( &cs );

	for ( size_t i = 0; i < m_vTexture.size (); i++ )
		SAFE_LOSTDEVICE ( m_vTexture [ i ] );

	for ( size_t i = 0; i < m_vFont.size (); i++ )
		SAFE_INVALIDATE ( m_vFont [ i ] );

	SAFE_INVALIDATE ( m_pRender );

	LeaveCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
void CDialog::OnResetDevice ( void )
{
	EnterCriticalSection ( &cs );

	for ( size_t i = 0; i < m_vTexture.size (); i++ )
		SAFE_RESETDEVICE ( m_vTexture [ i ] );

	for ( size_t i = 0; i < m_vFont.size (); i++ )
		SAFE_INITIALIZE ( m_vFont [ i ], m_pDevice );

	if ( m_pRender )
		m_pRender->Initialize ( m_pDevice );

	LeaveCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
CD3DRender *CDialog::GetRenderer ( void )
{
	return m_pRender;
}

//--------------------------------------------------------------------------------------
CMouse* CDialog::GetMouse ( void )
{
	return m_pMouse;
}

//--------------------------------------------------------------------------------------
IDirect3DDevice9* CDialog::GetDevice ( void )
{
	return m_pDevice;
}

//--------------------------------------------------------------------------------------
CD3DFont* CDialog::GetFont ( int ID )
{
	if ( ID > -1 && ID < m_vFont.size () )
		return m_vFont [ ID ];

	return NULL;
}

//--------------------------------------------------------------------------------------
void CDialog::SetVisible ( bool bVisible )
{
	EnterCriticalSection ( &cs );
	m_bVisible = bVisible;
	LeaveCriticalSection ( &cs );
}

//--------------------------------------------------------------------------------------
bool CDialog::IsVisible ( void )
{
	return m_bVisible;
}
