#include "CListView.h"

#define TEXTBOX_TEXTSPACE 18
#define LISTVIEW_TITLESIZE 8
#define LISTVIEW_MINCOLUMNSIZE 50

CListView::CListView ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_LISTVIEW );

	m_sControlColor.d3dColorFont = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorSelectedFont = D3DCOLOR_RGBA ( 255, 255, 255, 255 );

	m_nSelected = m_nIndex = m_nOverColumnId = -1;
	m_nRowSize = 0;
	m_bSizable = m_bMovable = true;

	m_pScrollbar = new CScrollablePane ( pDialog );
	if ( !m_pScrollbar )
		MessageBox ( 0, _UI ( "CListView::CListView: Error for creating CScrollBarHorizontal" ), _UI ( "GUIAPI.asi" ), 0 );

	m_pScrollbar->AddControl ( this );

	m_pDialog->LoadFont ( _UI ( "Arial" ), 10, true, &m_pTitleFont );
}

CListView::~CListView ( void )
{
	RemoveAllColumns ();
	SAFE_DELETE ( m_pScrollbar );
}

void CListView::AddColumn ( const SIMPLEGUI_CHAR *szColumnName, int nWidth )
{
	if ( szColumnName == NULL ) 
		return;

	if ( nWidth < LISTVIEW_MINCOLUMNSIZE )
	{
		nWidth = LISTVIEW_MINCOLUMNSIZE;
	}

	SListViewColumn sColumn;
	sColumn.m_nWidth = nWidth;
	sColumn.m_sColumnName = szColumnName;

	m_vColumnList.push_back ( sColumn );
}

void CListView::RemoveColumn ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () )
		return;

	RemoveAllItemsFromColumn ( nColumnId );
	m_vColumnList.erase ( m_vColumnList.begin () + nColumnId );
}

void CListView::RemoveAllColumns ( void )
{
	RemoveAllItems ();
	m_vColumnList.clear ();
}

void CListView::SetColumnName ( UINT nColumnId, const SIMPLEGUI_CHAR *szColumnName )
{
	if ( nColumnId > m_vColumnList.size () || 
		 szColumnName == NULL )
		return;

	m_vColumnList [ nColumnId ].m_sColumnName = szColumnName;
}

const char *CListView::GetColumnName ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () )
		return NULL;

	return m_vColumnList [ nColumnId ].m_sColumnName.c_str ();
}

const char *CListView::GetColumnItemNameByRow ( UINT nColumnId, UINT nRow )
{
	if ( nColumnId > m_vColumnList.size () || 
		 nRow >= m_vColumnList [ nColumnId ].m_sItem.size () )
	{
		return NULL;
	}

	return m_vColumnList [ nColumnId ].m_sItem [ nRow ].c_str ();
}

void CListView::SetColumnWidth ( UINT nColumnId, int nWidth )
{
	if ( nColumnId > m_vColumnList.size () )
	{
		return;
	}

	if ( nWidth < LISTVIEW_MINCOLUMNSIZE )
	{
		nWidth = LISTVIEW_MINCOLUMNSIZE;
	}

	m_vColumnList [ nColumnId ].m_nWidth = nWidth;

	// Set up horizontal scroll bar range
	if ( !m_bSizing )
		m_pScrollbar->SetTrackRange ( GetAllColumnsWidth (), 0 );
}

int CListView::GetColumnWidth ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () )
	{
		return 0;
	}

	return m_vColumnList [ nColumnId ].m_nWidth;
}

void CListView::AddColumnItem ( UINT nColumnId, const SIMPLEGUI_CHAR *szItem )
{
	if ( nColumnId > m_vColumnList.size () ||
		 szItem == NULL )
	{
		return;
	}

	m_vColumnList [ nColumnId ].m_sItem.push_back ( szItem );
	m_nRowSize = max ( m_nRowSize, m_vColumnList [ nColumnId ].m_sItem.size () );

	// Set up scroll bar ranges
	m_pScrollbar->SetTrackRange ( GetAllColumnsWidth (), m_nRowSize );
}

void CListView::RemoveColumnItem ( UINT nColumnId, UINT nIndex )
{
	if ( nColumnId > m_vColumnList.size () ||
		 nIndex > m_vColumnList [ nColumnId ].m_sItem.size () )
	{
		return;
	}

	m_vColumnList [ nColumnId ].m_sItem.erase ( m_vColumnList [ nColumnId ].m_sItem.begin () + nIndex );
}

void CListView::RemoveAllItemsFromColumn ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () )
		return;
	
	m_vColumnList [ nColumnId ].m_sItem.clear ();
}

void CListView::RemoveAllItems ( void )
{
	for ( auto column : m_vColumnList )
	{
		column.m_sItem.clear ();
	}
}

size_t CListView::GetNumOfColumns ( void )
{
	return m_vColumnList.size ();
}

size_t CListView::GetNumOfItemsFromColumn ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () )
		return 0;
	
	return m_vColumnList [ nColumnId ].m_sItem.size ();
}

size_t CListView::GetNumOfItems ( void )
{
	return m_nRowSize;
}

int CListView::GetAllColumnsWidth ( void )
{
	int nWidth = 0;
	for ( auto column : m_vColumnList )
	{
		nWidth += column.m_nWidth;
	}

	return nWidth;
}

const char *CListView::GetSelectedItem ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () ||
		 m_nSelected < 0 ||
		 m_nSelected >= m_vColumnList [ nColumnId ].m_sItem.size () )
	{
		return NULL;
	}

	return m_vColumnList [ nColumnId ].m_sItem [ m_nSelected ].c_str ();
}

int CListView::GetNextColumn ( UINT nColumnId )
{
	size_t size = m_vColumnList.size ();
	for ( size_t i = 0; i < size; i++ )
	{
		if ( nColumnId == i && i + 1 < size )
			return i + 1;
	}

	return -1;
}

int CListView::GetPrevColumn ( UINT nColumnId )
{
	for ( int i = 0; i < m_vColumnList.size (); i++ )
	{
		if ( nColumnId == i && int ( i - 1 ) > 0 )
			return i - 1;
	}

	return -1;
}

int CListView::GetColumnIdAtArea ( CPos pos )
{
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	SControlRect rRect = m_rBoundingBox;
	rRect.pos.SetX ( rRect.pos.GetX () - pScrollbarHor->GetTrackPos () );

	for ( size_t i = 0; i < m_vColumnList.size (); i++ )
	{
		rRect.pos.SetX ( rRect.pos.GetX () + ( i ? GetColumnWidth ( i - 1 ) : 0 ) );
		rRect.size.cx = GetColumnWidth ( i );
		rRect.size.cy = rTitleRect.size.cy;

		if ( rRect.InControlArea ( pos ) )
		{
			return i;
		}
	}

	return -1;
}

int CListView::GetColumnIdAtAreaBorder ( CPos pos )
{
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	SControlRect rRect = m_rBoundingBox;
	rRect.pos.SetX ( rRect.pos.GetX () - pScrollbarHor->GetTrackPos () - 4 );

	for ( size_t i = 0; i < m_vColumnList.size (); i++ )
	{
		rRect.pos.SetX ( rRect.pos.GetX () + GetColumnWidth ( i ) );
		rRect.size.cx = 4;
		rRect.size.cy = rTitleRect.size.cy;

		if ( rRect.InControlArea ( pos ) )
		{
			return i;
		}
	}
	return -1;
}

int CListView::GetColumnOffset ( UINT nColumnId )
{
	if ( nColumnId > m_vColumnList.size () ) 
		return 0;

	int nX = m_rBoundingBox.pos.GetX () + GetColumnWidth ( nColumnId );
	for ( size_t i = 0; i < m_vColumnList.size (); i++ )
	{
		if ( nColumnId == i )
			return nX;

		nX += m_rBoundingBox.pos.GetX () + GetColumnWidth ( nColumnId );
	}

	return 0;
}

const char* CListView::FindItemInRow ( UINT nRow )
{
	UINT nColumnId = 0;
	while ( nColumnId < m_vColumnList.size () )
	{
		const char *szItem = GetColumnItemNameByRow ( nColumnId, nRow );
		if ( szItem )
			return szItem;

		nColumnId++;
	}

	return NULL;
}

void CListView::MoveColumn ( UINT nColumnId, UINT nPosition )
{
	if ( nColumnId > m_vColumnList.size () ) 
		return;

	// if position is too big, insert at end.
	if ( nPosition > m_vColumnList.size () )
	{
		nPosition = m_vColumnList.size ()-1;
	}

	SListViewColumn sColumn = m_vColumnList [ nColumnId ];

	m_vColumnList.erase ( m_vColumnList.begin () + nColumnId );
	m_vColumnList.insert ( m_vColumnList.begin () + nPosition, sColumn );
}

void CListView::SetTitleSizable ( bool bSizable )
{
	m_bSizable = bSizable;
}

void CListView::SetTitleMovable ( bool bMovable )
{
	m_bMovable = bMovable;
}

void CListView::Draw ( void )
{
	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline );

	int nVerScrollTrackPos = pScrollbarVer->GetTrackPos ();
	int nHorScrollTrackPos = pScrollbarHor->GetTrackPos ();

	rTitleRect.pos.SetX ( rTitleRect.pos.GetX () + 4 - nHorScrollTrackPos );

	CD3DRender *pRender = m_pDialog->GetRenderer ();

	pRender->D3DLine ( rTitleRect.pos.GetX (), rTitleRect.pos.GetY () + rTitleRect.size.cy, rTitleRect.pos.GetX () + rTitleRect.size.cx + nHorScrollTrackPos, rTitleRect.pos.GetY () + rTitleRect.size.cy,
					   m_sControlColor.d3dColorOutline );

	SControlRect rTextRect = m_rContentArea;
	rTextRect.pos.SetX ( rTextRect.pos.GetX () + 4 - nHorScrollTrackPos );

	// Draw all contexts
	for ( size_t i = 0; i < m_vColumnList.size (); i++ )
	{
		rTitleRect.pos.SetX ( rTitleRect.pos.GetX () + ( i ? GetColumnWidth ( i - 1 ) : 0 ) );

		std::string szStr = GetColumnName ( i );
		m_pTitleFont->CutString ( GetColumnWidth ( i ) - 4, szStr );

		rTextRect.pos.SetX ( rTextRect.pos.GetX () + ( i ? GetColumnWidth ( i - 1 ) : 0 ) );
		rTextRect.pos.SetY ( m_rContentArea.pos.GetY () );

		D3DCOLOR d3dColor = m_sControlColor.d3dColorBox [ SControlColor::STATE_NORMAL ];
		if ( ( int ) i == m_nOverColumnId )
		{			
			d3dColor = m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ];		
		}

		SControlRect rRect = rTitleRect;
		rRect.pos.SetX ( rTextRect.pos.GetX () - 4 );
		rRect.pos.SetY ( rTextRect.pos.GetY () - rTitleRect.size.cy / 2 );
		rRect.size.cx = GetColumnWidth ( i );
		rRect.size.cy = rTitleRect.size.cy;
		m_pDialog->DrawBox ( rRect, d3dColor, m_sControlColor.d3dColorOutline );

		m_pDialog->DrawFont ( SControlRect ( rTitleRect.pos.GetX () + GetColumnWidth ( i ) / 2,
							  rTitleRect.pos.GetY () + rTitleRect.size.cy / 2 ),
							  m_sControlColor.d3dColorFont,
							  szStr.c_str (),
							  D3DFONT_COLORTABLE | D3DFONT_CENTERED_X | D3DFONT_CENTERED_Y,
							  m_pTitleFont );

		if ( i != 0 )
		{
			pRender->D3DLine ( rTitleRect.pos.GetX () - 4, rTitleRect.pos.GetY (), rTitleRect.pos.GetX () - 4, rTitleRect.pos.GetY () + m_rBoundingBox.size.cy, m_sControlColor.d3dColorOutline );
		}

		for ( UINT j = nVerScrollTrackPos; j < nVerScrollTrackPos + pScrollbarVer->GetPageSize (); j++ )
		{
			std::string szStr;
			const char *szText = GetColumnItemNameByRow ( i, j );
			if ( szText )
			{
				szStr = szText;
				m_pFont->CutString ( GetColumnWidth ( i ) - 4, szStr );
			}

			// Check if the text is not NULL
			if ( szStr.c_str () != NULL &&
				 rTextRect.pos.GetX () < m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx )
			{
				SIZE size;
				m_pFont->GetTextExtent ( szStr.c_str (), &size );
				rTextRect.pos.SetY ( rTextRect.pos.GetY () + size.cy );

				// Check if selected text is not NULL, or if there is a 
				// selected index
				if ( m_nSelected == j ||
					 m_nIndex == j )
				{
					SControlRect rBoxSel = rTextRect;
					rBoxSel.pos.SetX ( rTextRect.pos.GetX () - 2 );
					rBoxSel.pos.SetY ( rBoxSel.pos.GetY ()  );
					rBoxSel.size.cx = rTextRect.size.cx + nHorScrollTrackPos;
					rBoxSel.size.cy = size.cy - 1;

					D3DCOLOR d3dColor = m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ];
					if ( m_nSelected == j )
						d3dColor = m_sControlColor.d3dColorBoxSel;

					m_pDialog->DrawBox ( rBoxSel, d3dColor, m_sControlColor.d3dColorOutline, true );
					m_pDialog->DrawFont ( rTextRect, m_sControlColor.d3dColorSelectedFont, szStr.c_str (), 0, m_pFont );
				}
				else
					m_pDialog->DrawFont ( rTextRect, m_sControlColor.d3dColorFont, szStr.c_str (), D3DFONT_COLORTABLE, m_pFont );
			}
		}
	}

	m_pScrollbar->OnDraw ();
}

void CListView::OnClickLeave ( void )
{
	CControl::OnClickLeave ();

	m_pScrollbar->OnClickLeave ();

	m_bSizing = m_bMoving = false;
}

bool CListView::OnClickEvent ( void )
{
	return ( CControl::OnClickEvent () ||
			 m_pScrollbar->OnClickEvent () ||
			 m_bSizing ||
			 m_bMoving );
}

void CListView::OnFocusIn ( void )
{
	CControl::OnFocusIn ();

	if ( m_pScrollbar )
		m_pScrollbar->OnFocusIn ();
}

void CListView::OnFocusOut ( void )
{
	CControl::OnFocusOut ();

	if ( m_pScrollbar )
		m_pScrollbar->OnFocusOut ();
}

void CListView::OnMouseEnter ( void )
{
	CControl::OnMouseEnter ();

	if ( m_pScrollbar )
		m_pScrollbar->OnMouseEnter ();
}

void CListView::OnMouseLeave ( void )
{
	CControl::OnMouseLeave ();

	if ( !m_bSizing &&
		 !m_bMoving )
	{
		m_nOverColumnId = -1;
	}

	if ( m_pScrollbar )
		m_pScrollbar->OnMouseLeave ();
}

bool CListView::CanHaveFocus ( void )
{
	return ( CControl::CanHaveFocus () ||
			 m_pScrollbar->CanHaveFocus () );
}

bool CListView::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	if ( !CanHaveFocus () ||
		 !pScrollbarVer ||
		 !pScrollbarHor )
		return false;

	if ( ( !m_bSizing && !m_bMoving ) &&
		 ( m_bMovable || m_bSizable ) )
	{
		m_nOverColumnId = GetColumnIdAtArea ( pos );
	}

	// First acquire focus
	if ( WM_LBUTTONDOWN == uMsg )
		m_pParent->RequestControlFocus ( this );

	if ( m_pScrollbar->HandleMouse ( uMsg, pos, wParam, lParam ) )
	{
		return true;
	}

	if ( !GetAsyncKeyState ( VK_LBUTTON ) && 
		 m_bMoving )
	{
		int iColumnId = GetColumnIdAtArea ( pos );
		if ( iColumnId > -1 )
			MoveColumn ( m_nId, iColumnId );
	}

	switch ( uMsg )
	{
		case WM_MOUSEMOVE:
		{
			m_nIndex = -1;

			if ( ( GetAsyncKeyState ( VK_LBUTTON ) &&
				 !m_bHasFocus ) ||
				 m_pScrollbar->ContainsRect ( pos ) )
			{
				break;
			}

			if ( m_bMoving )
			{
				m_pDialog->GetMouse ()->SetCursorType ( CMouse::MOVE );	
				return true;
			}
			else if ( GetColumnIdAtAreaBorder ( pos ) > -1 && 
					  m_bSizable )
			{
				m_pDialog->GetMouse ()->SetCursorType ( CMouse::E_RESIZE );	
				return true;
			}

			if ( m_bSizing )
			{
				int nDelta = pos.GetX () - m_rBoundingBox.pos.GetX ();

				SetColumnWidth ( m_nId, nDelta + m_nDragX );
				return true;
			}

			SControlRect rText = m_rContentArea;
			rText.pos.SetX ( rText.pos.GetX () + 4 );
			rText.size.cx -= ( pScrollbarVer->GetWidth () + 4 );
			rText.size.cy = TEXTBOX_TEXTSPACE - 2;

			for ( int i = pScrollbarVer->GetTrackPos (); i < pScrollbarVer->GetTrackPos () + pScrollbarVer->GetPageSize (); i++ )
			{
				if ( i < m_nRowSize )
				{
					const char *szItem = FindItemInRow ( i );

					SIZE size;
					m_pFont->GetTextExtent ( szItem, &size );

					rText.pos.SetY ( rText.pos.GetY () + size.cy );

					// Check if selected text is not NULL and determine 
					// which item has been selected
					if ( szItem != NULL &&
						 rText.InControlArea ( pos ) )
					{
						m_nIndex = i;
						return true;
					}
				}
			}

			break;
		}

		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
			if ( m_bSizable )
			{
				m_nId = GetColumnIdAtAreaBorder ( pos );
				if ( m_nId > -1 )
				{
					m_bSizing = true;
					m_nDragX = m_rBoundingBox.pos.GetX () + GetColumnWidth ( m_nId ) - pos.GetX ();
					return true;
				}
			}

			if ( m_bMovable )
			{
				m_nId = GetColumnIdAtArea ( pos );
				if ( m_nId > -1 )
				{
					m_bMoving = true;
					m_nDragX = m_rBoundingBox.pos.GetX () + GetColumnWidth ( m_nId ) - pos.GetX ();
					return true;
				}
			}

			if ( m_rBoundingBox.InControlArea ( pos ) )
			{
				// Pressed while inside the control
				m_bPressed = true;
				return true;
			}

			break;
		}

		case WM_LBUTTONUP:
		{
			if ( m_bSizing )
			{
				m_pScrollbar->SetTrackRange ( GetAllColumnsWidth (), 0 );
				m_bSizing = false;
			}

			m_bMoving = false;

			if ( m_bPressed )
			{
				m_bPressed = false;

				if ( m_rContentArea.InControlArea ( pos ) )
				{
					if ( m_nIndex != -1 )
					{
						m_nSelected = m_nIndex;
					}

					SendEvent ( EVENT_CONTROL_SELECT, true );
					return true;
				}
			}

			break;
		}

		case WM_MOUSEWHEEL:
		{
			UINT uLines;
			SystemParametersInfo ( SPI_GETWHEELSCROLLLINES, 0, &uLines, 0 );
			int zDelta = ( short ) HIWORD ( wParam ) / WHEEL_DELTA;
			pScrollbarVer->Scroll ( -zDelta * uLines );

			return true;
		}
	};

	return false;
}

bool CListView::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

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
				case VK_LEFT:
				case VK_UP:
				{
					if ( m_nIndex > 0 )
					{
						m_nIndex--;
						m_nSelected = m_nIndex;
						pScrollbarVer->ShowItem ( m_nSelected );

						SendEvent ( EVENT_CONTROL_SELECT, m_nSelected );
					}
					else
					{
						m_nSelected = m_nIndex = m_nRowSize - 1;
						pScrollbarVer->Scroll ( m_nRowSize - 1 );
					}
					return true;
				}

				case VK_RIGHT:
				case VK_DOWN:
				{
					if ( m_nIndex + 1 < m_nRowSize )
					{
						m_nIndex++;
						m_nSelected = m_nIndex;
						pScrollbarVer->ShowItem ( m_nSelected );
					}
					else
					{
						m_nSelected = m_nIndex = 0;
						pScrollbarVer->ShowItem ( 0 );
					}

					SendEvent ( EVENT_CONTROL_SELECT, m_nSelected );

					return true;
				}
			}
			break;
		}
	}

	return false;
}

void CListView::UpdateRects ( void )
{
	if ( !m_pScrollbar )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	CControl::UpdateRects ();

	SIZE size;
	m_pTitleFont->GetTextExtent ( "Y", &size );

	rTitleRect = m_rBoundingBox;
	rTitleRect.size.cy = size.cy + LISTVIEW_TITLESIZE;

	m_rContentArea = m_rBoundingBox;
	m_rContentArea.pos.SetY ( m_rContentArea.pos.GetY () + rTitleRect.size.cy / 2 );

	m_rContentArea.size.cy = m_rContentArea.size.cy - rTitleRect.size.cy;

	m_pFont->GetTextExtent ( "Y", &size );

	// Set up scroll bar values
	m_pScrollbar->SetPageSize ( m_rContentArea.size.cx - ( pScrollbarHor->GetHeight () + 4 ), m_rContentArea.size.cy / size.cy );
	m_pScrollbar->UpdateScrollbars ( m_rBoundingBox );
}

bool CListView::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return ( m_pScrollbar->ContainsRect ( pos ) || 
			 m_rBoundingBox.InControlArea ( pos ) );
}