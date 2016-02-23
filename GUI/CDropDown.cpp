#include "CGUI.h"

#define TEXTBOX_TEXTSPACE 18

//--------------------------------------------------------------------------------------
CDropDown::CDropDown ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_DROPDOWN );

	m_sControlColor.d3dColorFont = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorSelectedFont = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

	m_iSelected = m_iIndex = -1;
	m_uSize = 20;

	m_pEntryList = new CEntryList ( pDialog );

	if ( !m_pEntryList )
		MessageBox ( 0, _UI ( "CDropDown::CDropDown: Error for creating CEntryList" ), _UI ( "GUIAPI.asi" ), 0 );

	m_pEntryList->GetScrollbar ()->AddControl ( this );
}

//--------------------------------------------------------------------------------------
CDropDown::~CDropDown()
{
	SAFE_DELETE ( m_pEntryList );
}

//--------------------------------------------------------------------------------------
void CDropDown::Draw ()
{
	if ( !m_bVisible ||
		 !m_pEntryList )
		return;

	if ( !m_bEnabledStateColor )
		m_eState = SControlColor::STATE_NORMAL;
	else if ( !m_bEnabled )
		m_eState = SControlColor::STATE_DISABLED;
	else if ( m_bPressed )
		m_eState = SControlColor::STATE_PRESSED;
	else if ( m_bMouseOver &&
			  CControl::ContainsRect ( m_pDialog->GetMouse ()->GetPos () ) )
		m_eState = SControlColor::STATE_MOUSE_OVER;
	else
		m_eState = SControlColor::STATE_NORMAL;

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBox [ m_eState ], m_sControlColor.d3dColorOutline, m_bAntAlias );

	SControlRect rText = m_rBoundingBox;
	rText.pos.SetX ( rText.pos.GetX () + 2 );
	rText.pos.SetY ( rText.pos.GetY () + m_rBoundingBox.size.cy / 2 - 8 );

	SControlRect rShape = m_rBoundingBox;
	rShape.pos.SetX ( m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx - 12 );
	rShape.pos.SetY ( m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy / 2 - 1 );
	rShape.size.cx = 7;

	std::string str ( GetSelectedItem () ?
					  GetSelectedItem ()->m_sText.c_str () :
					  GetText () );

	m_pFont->CutString ( rText.size.cx - ( rShape.size.cx + 10 ), str );
	m_pDialog->DrawFont ( rText, m_sControlColor.d3dColorSelectedFont, str.c_str (), 0, m_pFont );

	m_pDialog->DrawTriangle ( rShape, 180.f, m_sControlColor.d3dColorShape, 0, m_bAntAlias );
	SetScissor ( m_pDialog->GetDevice (), m_rBack.GetRect () );

	if ( m_bPressed )
	{
		m_pDialog->DrawBox ( m_rBack, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
		m_pEntryList->Render ( m_rBack,
							   m_sControlColor.d3dColorBoxSel,
							   m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ],
							   m_sControlColor.d3dColorSelectedFont,
							   m_sControlColor.d3dColorFont,
							   m_iIndex );
	}
}

void CDropDown::AddItem ( SEntryItem *pEntry )
{
	m_pEntryList->AddEntry ( pEntry );
}

void CDropDown::AddItem ( const SIMPLEGUI_CHAR *szText, const SIMPLEGUI_CHAR *szValue )
{
	m_pEntryList->AddEntry ( new SEntryItem ( szText ) );
}

void CDropDown::RemoveItem ( SEntryItem *pEntry )
{
	m_pEntryList->RemoveEntry ( pEntry );
}

void CDropDown::RemoveItem ( UINT uIndex )
{
	m_pEntryList->RemoveEntry ( m_pEntryList->GetEntryByIndex ( uIndex ) );
}

SEntryItem *CDropDown::GetSelectedItem ( void )
{
	return m_pEntryList->GetSelectedEntry ();
}

void CDropDown::SetSelectedItem ( SEntryItem *pEntry, bool bSelect )
{
	m_pEntryList->SetSelectedEntry ( pEntry, bSelect );
}

void CDropDown::SetSelectedItemByIndex ( UINT uIndex, bool bSelect )
{
	m_pEntryList->SetSelectedEntryByIndex ( uIndex, bSelect );
}

void CDropDown::OnClickLeave ( void )
{
	CControl::OnClickLeave ();

	m_pEntryList->GetScrollbar ()->OnClickLeave ();
}

bool CDropDown::OnClickEvent ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CControl::OnClickEvent () ||
			 pScrollbar->OnClickEvent () );
}

void CDropDown::OnFocusIn ( void )
{
	CControl::OnFocusIn ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusIn ();
}

void CDropDown::OnFocusOut ( void )
{
	CControl::OnFocusOut ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusOut ();
}

void CDropDown::OnMouseEnter ( void )
{
	CControl::OnMouseEnter ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseEnter ();
}

void CDropDown::OnMouseLeave ( void )
{
	CControl::OnMouseLeave ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseLeave ();
}

bool CDropDown::CanHaveFocus ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CControl::CanHaveFocus () ||
			 pScrollbar->CanHaveFocus () );
}

void CDropDown::SetSortedList ( bool bSort )
{
	m_pEntryList->SetSortedList ( bSort );
}

void CDropDown::OpenBox ( void )
{
	if ( m_pParent && 
		 !m_bHasFocus )
		m_pParent->RequestControlFocus ( this );

	m_bPressed = true;
}

void CDropDown::CloseBox ( void )
{
	if ( m_pParent )
		m_pParent->ClearControlFocus ();

	m_bPressed = false;
	SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
}

//--------------------------------------------------------------------------------------
bool CDropDown::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	CScrollBarVertical *pScrollbarVer = m_pEntryList->GetScrollbar ()->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pEntryList->GetScrollbar ()->GetHorScrollbar ();

	if ( !CanHaveFocus () ||
		 !pScrollbarVer ||
		 !pScrollbarHor )
		return false;

	// Let the scroll bar handle it first.
	if ( m_pEntryList->GetScrollbar ()->HandleMouse ( uMsg, pos, wParam, lParam ) )
		return true;

	switch ( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			m_iIndex = -1;
			if ( m_bPressed )
			{			
				SControlRect rText = m_rBack;
				rText.pos.SetX ( rText.pos.GetX () + 4 );
				rText.size.cx -= ( pScrollbarVer->GetWidth () + 3 );
				rText.size.cy = TEXTBOX_TEXTSPACE - 2;

				for ( int i = pScrollbarVer->GetTrackPos (); i < pScrollbarVer->GetTrackPos () + pScrollbarVer->GetPageSize (); i++ )
				{
					if ( i < ( int ) m_pEntryList->GetSize () )
					{
						if ( i != pScrollbarVer->GetTrackPos () )
							rText.pos.SetY ( rText.pos.GetY () + m_pEntryList->GetTextSize ().cy );

						SEntryItem *pEntry = m_pEntryList->GetEntryByIndex ( i );

						// Check for a valid 'pEntry' pointer and if text is not NULL
						// and determine which item has been selected
						if ( pEntry &&
							 pEntry->m_sText.c_str () != NULL &&
							 rText.InControlArea ( pos ) )
						{
							m_iIndex = i;
							return true;
						}
					}
				}
			}
			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( CControl::ContainsRect ( pos ) )
			{
				// Toggle dropdown
				m_bPressed ? 
					CloseBox () : 
					OpenBox ();

				return true;
			}

			// Mouse click not on main control or in dropdown
			if ( m_bPressed &&
				 !m_rBack.InControlArea ( pos ) )
			{
				CloseBox ();
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			if ( m_bPressed )
			{
				// Perhaps this click is within the dropdown
				if ( m_rBack.InControlArea ( pos ) )
				{
					m_iSelected = m_iIndex;
					m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

					CloseBox ();
				}

				return true;
			}

			break;
		}

		case WM_MOUSEWHEEL:
		{	
			int zDelta = ( short ) HIWORD ( wParam ) / WHEEL_DELTA;
			if ( m_bPressed )
			{
				UINT uLines;
				SystemParametersInfo ( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );

				m_pEntryList->GetScrollbar ()->OnMouseWheel ( -zDelta * uLines );
			}
			else
			{
				if ( zDelta > 0 )
				{
					if ( m_iSelected > 0 )
					{
						m_iSelected--;

						if ( !m_bPressed )
							SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
					}
				}
				else
				{
					if ( m_iSelected + 1 < ( int ) m_pEntryList->GetSize () )
					{
						m_iSelected--;

						if ( !m_bPressed )
							SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
					}
				}
			}
			return true;
		}
	};

	return false;
}

//--------------------------------------------------------------------------------------
bool CDropDown::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	CScrollBarVertical *pScrollbarVer = pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = pScrollbar->GetHorScrollbar ();

	if ( !CanHaveFocus () ||
		 !pScrollbarVer ||
		 !pScrollbarHor )
	{
		return false;
	}

	switch ( uMsg )
	{
		case WM_KEYDOWN:
		{
			switch ( wParam )
			{
				case VK_RETURN:
				{
					if ( m_bPressed )
					{
						CloseBox ();
						return true;
					}
					break;
				}

				case VK_LEFT:
				case VK_UP:
				{
					if ( m_iSelected > 0 )
					{
						m_iSelected--;
						pScrollbarVer->ShowItem ( m_iSelected );
					}
					else
					{
						m_iSelected = m_pEntryList->GetSize () - 1;
						pScrollbarVer->Scroll ( m_iSelected );
					}

					SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
					m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

					return true;
				}

				case VK_RIGHT:
				case VK_DOWN:
				{
					if ( m_iSelected + 1 < ( int ) m_pEntryList->GetSize () )
					{
						m_iSelected++;
						pScrollbarVer->ShowItem ( m_iSelected );
					}
					else
					{
						m_iSelected = 0;
						pScrollbarVer->ShowItem ( m_iSelected );
					}

					SendEvent ( EVENT_CONTROL_SELECT, m_iSelected );
					m_pEntryList->SetSelectedEntryByIndex ( m_iSelected, true );

					return true;
				}
			}
			break;
		}
	}

	return false;
}

//--------------------------------------------------------------------------------------
void CDropDown::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_rBack = m_rBoundingBox;
	m_rBack.size.cy = m_uSize * m_pEntryList->GetTextSize().cy;
	m_rBack.pos.SetY ( m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy );

	m_pEntryList->UpdateScrollbars ( m_rBack );
}

//--------------------------------------------------------------------------------------
bool CDropDown::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( ( m_rBack.InControlArea ( pos ) && m_bPressed ) ||
			 m_pEntryList->ContainsRects ( m_rBoundingBox, pos ) );
}