#include "CGUI.h"
/*
CEditBox::CEditBox ( CDialog *pDialog )
{
	/*SetControl ( pDialog );
	SetHeight ( 20 );

	SizeEdge = 0;

	m_iStart = 0;
	m_bCenterAlign = 0;
	m_bHideContent = 0;
	SetIndex ( 0 );
	m_bCursorState = false;
	SetAction ( 0 );

	SetThemeElement ( m_pDialog->GetThemeElement ( _UI ( "EditBox" ) ) );
	if ( !GetThemeElement () )
		MessageBox ( 0, _UI ( "Theme element invalid." ), _UI ( "EditBox" ), 0 );
	else
		SetElementState ( _UI ( "Norm" ) );
}

CEditBox::~CEditBox ()
{
	SAFE_DELETE ( pString );
	SAFE_DELETE ( pCursor );
	SAFE_DELETE ( pEdit [ 0 ] );
	SAFE_DELETE ( pEdit [ 1 ] );
	SAFE_DELETE ( pEdit [ 2 ] );
}
int iCount = 0;
void CEditBox::Draw ()
{
	/*CD3DFont *pFont = m_pFont;
	if ( !pFont )
		return;

	CPos Pos = *GetPos ();
	if ( GetParent () )
		Pos = *GetParent ()->GetPos () + Pos;

	SElementState *pState = GetElementState ();
	uistring DataString;

	if ( !m_bHideContent )
		DataString = GetString ();
	else
	{
		for ( size_t i = 0; i < GetString ().size (); i++ )
			DataString.push_back ( _UI ( '*' ) );
	}

	if ( pState )
	{
		pEdit [ 0 ]->Draw ( Pos, SizeEdge, GetHeight () );
		pEdit [ 2 ]->Draw ( Pos.GetX () + GetWidth () - SizeEdge, Pos.GetY (), SizeEdge, GetHeight () );
		pEdit [ 1 ]->Draw ( Pos.GetX () + SizeEdge - 1, Pos.GetY (), GetWidth () - 2 * SizeEdge + 2, GetHeight () );

		SIZE size;
		uistring str;
		if ( DataString.size () < 9 )
			str = DataString;
		else
		{
			int i = 0;
			pFont->GetTextExtent ( DataString.c_str (), &size );
			
			for ( int iWidth = 0; i < DataString.size () && iWidth + 10 < size.cx; )
			{
				if ( i < DataString.size () )
					str.push_back ( DataString [ i ] );
				iWidth += size.cx;
				i++;
			}
		}

		pFont->Print ( Pos.GetX () + iPadding, Pos.GetY () + GetHeight () / 2, pString->GetD3DCOLOR (), str.c_str (), D3DFONT_CENTERED_Y );
		pFont->Print ( Pos.GetX () + iPadding, Pos.GetY () + GetHeight () / 2+20, pString->GetD3DCOLOR (), DataString.c_str (), D3DFONT_CENTERED_Y );

		pFont->GetTextExtent ( _UI ( "Y" ), &size );
		int iHeight = ( GetHeight () - size.cy ) / 2;

		if ( m_bCursorState && HasFocus () && IsEnabled () )
		{
			pFont->GetTextExtent ( DataString.c_str (), &size );

			DataString [ m_iIndex ] = 0;


			pFont->GetTextExtent ( DataString.c_str (), &size );
			m_iCursorX = size.cx;

			if ( m_iCursorX > GetWidth () - 2 * iPadding )
			{
				m_iCursorX = GetWidth () - 2 * iPadding; m_iIndex = str.size ();
			}
			m_pDialog->FillArea ( Pos.GetX () + m_iCursorX + iPadding, Pos.GetY () + iHeight, 1, GetHeight () - 2 * iHeight, pCursor->GetD3DCOLOR () );
		}
	}
}

bool CEditBox::GetTextAlign ( void )
{
	return m_bCenterAlign;
}

void CEditBox::SetTextAlign ( bool bCenter )
{
	m_bCenterAlign = bCenter;
}

void CEditBox::HideContent ( bool bHide )
{
	m_bHideContent = bHide;
}

void CEditBox::PreDraw ()
{
	SetElementState ( HasFocus () ? _UI ( "Active" ) : _UI ( "Norm" ) );

	if ( !m_tCursorTimer.Running () )
	{
		m_bCursorState = !m_bCursorState;
		m_tCursorTimer.Start ( 0.6f );
	}
}

bool CEditBox::OnMouseMove ( CMouse *pMouse, bool bOver )
{
	if ( !m_bEnabled )
		return false;

	CPos Pos = *GetPos ();
	if ( m_pParent )
		Pos = *m_pParent->GetPos () + Pos;

	bool bInArea = pMouse->InControlArea ( Pos.GetX (), Pos.GetY (), GetWidth (), GetHeight () ) && bOver;
	SetMouseOver ( bInArea );

	return bInArea;
}

void CEditBox::OnMouseEvent ( CMouse *pMouse )
{
	if ( !m_bEnabled )
		return;

	CD3DFont *pFont =m_pFont;
	if ( !pFont )
		return;

	SIZE size;

	if ( !pMouse )
		return;

	if ( pMouse->GetLeftButton () )
	{
		if ( GetMouseOver () )
		{
			SendMsg ( EVENT_CONTROL_CLICKED, 0 );

			int iX = pMouse->GetPos ().GetX ();

			int iAbsX = GetPos ()->GetX ();
			if ( m_pParent )
				iAbsX = m_pParent->GetPos ()->GetX () + iAbsX;

			SIMPLEGUI_STRING sString ( &GetText () [ GetStart () ] );
			pFont->GetTextExtent ( sString.c_str (), &size );

			if ( iX >= iAbsX + size.cx )
				SetIndex ( sString.length () );
			else
			{
				for ( size_t i = 0; i <= sString.length (); i++ )
				{
					pFont->GetTextExtent ( sString.c_str (), &size );
					if ( iX < iAbsX + size.cx )
					{
						sString [ i ] = 0;
						pFont->GetTextExtent ( sString.c_str (), &size );

						if ( iX > iAbsX + size.cx + 6 )
							SetIndex ( i );
					}
					sString = &GetText () [ GetStart () ];
				}
			}

			if ( m_pParent )
				m_pParent->SetFocussedControl ( this );
		}
	}
}

void CEditBox::MsgProc ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( !m_bEnabled )
		return ;

	if ( !HasFocus () )
		return ;

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_END:
				{
					SIMPLEGUI_STRING sString = GetText ();

					SetIndex ( SIMPLEGUI_STRLEN ( &sString [ GetStart () ] ) );
					/*pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );

					while ( size.cx > GetWidth () - 5 || m_iCursorX > GetWidth () - 5 )
					{
						SetStart ( GetStart () + 1 );
						SetIndex ( GetIndex () - 1 );

						pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
					}
					break;
				}
				case VK_HOME:
				{
					SetStart ( 0 );
					SetIndex ( 0 );

					break;
				}
				case VK_DELETE:
				{
					SIMPLEGUI_STRING sString = GetText ();

					if ( GetIndex () <= static_cast< int >( sString.length () ) )
						sString.erase ( /*GetStart () + m_iIndex, 1 );

					SetText ( const_cast< SIMPLEGUI_CHAR* >( sString.c_str () ) );

					break;
				}

				case VK_LEFT:
				{
					//if ( !GetIndex () && GetStart () )
					//	SetStart ( GetStart () - 1 );
					if ( GetIndex () )
						SetIndex ( GetIndex () - 1 );

					break;
				}
				case VK_RIGHT:
				{
					SetIndex ( GetIndex () + 1 );

					//uistring sString = GetString ();
					//sString [ GetIndex () ] = 0;

					/*pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
					while ( size.cx > GetWidth () - 5 || m_iCursorX > GetWidth () - 5 )
					{
						SetStart ( GetStart () + 1 );
						SetIndex ( GetIndex () - 1 );

						pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
					}

					break;'
				}
			}
			break;
		}
		
		case  WM_CHAR:
		{
			switch ( ( TCHAR ) wParam )
			{
				case VK_BACK:
				{
					if ( GetIndex () )
					{
						SIMPLEGUI_STRING sString = GetText ();

						sString.erase ( /*GetStart () + GetIndex () - 1, 1 );

						SetText ( sString );
						SetIndex ( GetIndex () - 1 );
					}
					//else if ( GetStart () )
					//{
						//SetStart ( GetStart () - 1 );
						//SetIndex ( 1 );
					//}
					iCount--;
					if ( iCount < 0 )
						iCount = 0;
					break;
				}
				case VK_RETURN:
				{
					if ( m_pParent )
						m_pParent->SetFocussedControl ( 0 );

					SendMsg ( EVENT_CONTROL_END, 0 );

					break;
				}

				// Junk characters we don't want in the string
				case 26:  // Ctrl Z
				case 2:   // Ctrl B
				case 14:  // Ctrl N
				case 19:  // Ctrl S
				case 4:   // Ctrl D
				case 6:   // Ctrl F
				case 7:   // Ctrl G
				case 10:  // Ctrl J
				case 11:  // Ctrl K
				case 12:  // Ctrl L
				case 17:  // Ctrl Q
				case 23:  // Ctrl W
				case 5:   // Ctrl E
				case 18:  // Ctrl R
				case 20:  // Ctrl T
				case 25:  // Ctrl Y
				case 21:  // Ctrl U
				case 9:   // Ctrl I
				case 15:  // Ctrl O
				case 16:  // Ctrl P
				case 27:  // Ctrl [
				case 29:  // Ctrl ]
				case 28:  // Ctrl \ 
					break;

				default:
				{
					SIMPLEGUI_STRING sString ( GetText () );

					int iPrevLen = sString.length ();

					WCHAR c = ( WCHAR ) wParam;

					SendMsg ( EVENT_CONTROL_CHANGE, c );

					if ( c < 32 ) break;

					SIMPLEGUI_CHAR szKey [ 2 ] = { static_cast< SIMPLEGUI_CHAR >( c ) , 0 };
					if ( GetStart () + m_iIndex >= 0  )
					{
						if ( c != 22 )
							sString.insert ( GetStart () + m_iIndex, szKey );
						else
						{
							if ( !OpenClipboard ( 0 ) )
								break;

							HANDLE hData = GetClipboardData ( CF_TEXT );
							SIMPLEGUI_CHAR *pszBuffer = static_cast< SIMPLEGUI_CHAR* >( GlobalLock ( hData ) );

							if ( pszBuffer )
								sString.insert ( GetStart () + m_iIndex, pszBuffer );
							

							GlobalUnlock ( hData );
							CloseClipboard ();
						}
					}
					if ( sString.size ()>10 )
						iCount++;
					SetText ( const_cast< SIMPLEGUI_CHAR* >( sString.c_str () ) );

					if ( c == _UI ( ' ' ) )
						SetIndex ( GetIndex () + 1 );
					else
						SetIndex ( GetIndex () + sString.length () - iPrevLen );

					/*pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
					while ( size.cx > GetWidth () - 5 )
					{
						SetStart ( GetStart () + 1 );
						SetIndex ( GetIndex () - 1 );

						pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
					}

					break;
				}
			}
		}

		return ;
	}
}


int CEditBox::GetIndex ( void )
{
	return m_iIndex;
}

void CEditBox::SetIndex ( int iIndex )
{
	/*uistring sString ( &GetString () [ GetStart () ] );

	if ( iIndex > static_cast< int >( sString.length () ) || iIndex < 0 )
		return;

	CD3DFont *pFont = GetFont ();
	if ( !pFont )
		return;

	SIZE size;

	sString [ iIndex ] = 0;
	pFont->GetTextExtent ( &sString.c_str () [ GetStart () ], &size );
	m_iCursorX = size.cx;

	m_iIndex = iIndex;
}

int CEditBox::GetStart ()
{
	return m_iStart;
}

void CEditBox::SetStart ( int iStart )
{
	m_iStart = iStart;
}

void CEditBox::UpdateTheme ( int iIndex )
{
	SElementState * pState = GetElementState ( iIndex );

	pString = pState->GetColor ( _UI ( "String" ) );
	pCursor = pState->GetColor ( _UI ( "Cursor" ) );

	SizeEdge = pState->GetInt ( _UI ( "SizeEdge" ) );
	SetHeight ( pState->GetInt ( _UI ( "Height" ) ) );

	pEdit [ 0 ] = pState->GetTexture ( _UI ( "Left" ) );
	pEdit [ 1 ] = pState->GetTexture ( _UI ( "Middle" ) );
	pEdit [ 2 ] = pState->GetTexture ( _UI ( "Right" ) );

	iPadding = pState->GetInt ( _UI ( "Padding" ) );
}*/