#include "CGUI.h"

CEntryList::CEntryList ( CDialog *pDialog ) : 
	m_bSort ( false )
{
	// Assert if 'pDialog' is invalid
	assert ( pDialog );

	m_pDialog = pDialog;

	if ( m_pDialog )
		m_pFont = m_pDialog->GetFont ();
	else
		m_pFont = NULL;

	m_pScrollbar = new CScrollablePane ( pDialog );

	// Assert if 'm_pScrollbarHor' is invalid
	assert ( m_pScrollbar &&
			 _UI ( "Error for creating scrollbar" ) );
}

CEntryList::~CEntryList ( void )
{
	SAFE_DELETE ( m_pFont );
	SAFE_DELETE ( m_pScrollbar );

	ResetList ();
}

void CEntryList::SetFont ( CD3DFont *pFont )
{
	// Assert if 'pFont' is invalid
	assert ( pFont &&
			 _UI ( "Invalid CD3DFont parameter 'pFont' pointer" ) );

	if ( pFont )
		m_pFont = pFont;
}

void CEntryList::AddEntry ( SEntryItem *pEntry )
{
	// Assert if 'pEntry' is invalid
	assert ( pEntry &&
			 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );

	if ( !pEntry )
		return;

	if ( m_bSort )
	{
		m_vEntryList.insert ( std::upper_bound ( m_vEntryList.begin (), m_vEntryList.end (), pEntry,
		[] ( const SEntryItem* a, const SEntryItem* b )
		{
			return ( a->m_sText < b->m_sText );
		} ), pEntry );
	}
	else
		m_vEntryList.push_back ( pEntry );

	if ( m_pScrollbar &&
		 m_pFont )
	{
		SIZE size;
		m_pFont->GetTextExtent ( pEntry->m_sText.c_str (), &size );

		m_TextSize.cy = size.cy;
		m_TextSize.cx = max ( m_TextSize.cx, size.cx, );

		// Set up scroll bar values
		m_pScrollbar->SetTrackRange ( m_TextSize.cx, m_vEntryList.size () );
	}
}

void CEntryList::RemoveEntry ( SEntryItem *pEntry )
{
	// Assert if 'pEntry' is invalid
	assert ( pEntry &&
			 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );

	if ( !IsEntryInList ( pEntry ) )
		return;

	m_vEntryList.erase ( std::find ( m_vEntryList.begin (), m_vEntryList.end (), pEntry ) );
	SAFE_DELETE ( pEntry );

	m_pScrollbar->SetTrackRange ( m_TextSize.cx, m_vEntryList.size () );
}

SEntryItem *CEntryList::GetEntryByIndex ( int nIndex )
{
	// Assert if 'nIndex' is out of range
	//assert ( nIndex < m_vEntryList.size () &&
	//		 _UI ( "'nIdex' is out of range" ) );

	if ( nIndex < m_vEntryList.size () && 
		 nIndex > -1 )
		return m_vEntryList [ nIndex ];
	else
		return NULL;
}

SIZE CEntryList::GetTextSize ( void )
{
	return m_TextSize;
}

void CEntryList::Render ( SControlRect rRect, D3DCOLOR d3dColorSelected,
						  D3DCOLOR d3dColorNormal,
						  D3DCOLOR d3dColorSelectedFont,
						  D3DCOLOR d3dColorFont, UINT uIndex )
{
	if ( !m_pScrollbar )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	rRect.pos.SetX ( rRect.pos.GetX () + 4 - pScrollbarHor->GetTrackPos () );

	// Draw all contexts
	for ( int i = pScrollbarVer->GetTrackPos (); i < pScrollbarVer->GetTrackPos () + pScrollbarVer->GetPageSize (); i++ )
	{
		if ( i < ( int ) GetSize () )
		{
			if ( i != pScrollbarVer->GetTrackPos () )
				rRect.pos.SetY ( rRect.pos.GetY () + GetTextSize ().cy );

			SEntryItem *pEntry = GetEntryByIndex ( i );

			// Check for a valid 'pEntry' pointer and if text is not NULL
			if ( pEntry &&
				 pEntry->m_sText.c_str () != NULL )
			{
				if ( IsEntrySelected ( pEntry ) ||
					 uIndex == i )
				{
					SControlRect rBoxSel = rRect;
					rBoxSel.pos.SetX ( rRect.pos.GetX () - 2 );
					rBoxSel.pos.SetY ( rBoxSel.pos.GetY () + 1 );
					rBoxSel.size.cx = rRect.size.cx + pScrollbarHor->GetTrackPos ();
					rBoxSel.size.cy = GetTextSize ().cy - 1;

					D3DCOLOR d3dColor = d3dColorNormal;
					if ( IsEntrySelected ( pEntry ) )
						d3dColor = d3dColorSelected;

					m_pDialog->DrawBox ( rBoxSel, d3dColor, 0, false );
					m_pDialog->DrawFont ( rRect, d3dColorSelectedFont, pEntry->m_sText.c_str (), 0, m_pFont );
				}
				else
					m_pDialog->DrawFont ( rRect, d3dColorFont, pEntry->m_sText.c_str (), D3DFONT_COLORTABLE, m_pFont );
			}
		}
	}

	m_pScrollbar->OnDraw ();
}

bool CEntryList::IsEntryInList ( SEntryItem *pEntry )
{
	if ( !pEntry )
		return false;

	return std::find ( m_vEntryList.begin (), m_vEntryList.end (), pEntry ) != m_vEntryList.end ();
}

/*************************************************************************
Insert an item into the CEntryList after a specified item already in
the list.
*************************************************************************/
void CEntryList::InsertItem ( SEntryItem *pEntry, SEntryItem *pEntryPos )
{
	// Assert if 'pEntry' is invalid
	assert ( pEntry &&
			 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );

	if ( !IsEntryInList ( pEntry ) )
		return;

	std::vector<SEntryItem*>::iterator iter;

	// If pEntryPos is NULL begin insert at begining, else insert after item 'pEntryPos'
	if ( !pEntryPos )
	{
		iter = m_vEntryList.begin ();
	}
	else
	{
		iter = std::find ( m_vEntryList.begin (), m_vEntryList.end (), pEntryPos );

		// Assert if item 'pEntryPos' is not in the list
		if ( iter == m_vEntryList.end () )
		{
			assert ( iter != m_vEntryList.end () && 
					 _UI ( "The specified SEntryItem for parameter 'pEntryPos' is not attached to this CEntryList." ) );
		}
	}

	m_vEntryList.insert ( iter, pEntry );
}

void CEntryList::SetSortedList ( bool bSort )
{
	m_bSort = bSort;
}

int CEntryList::GetIndexByEntry (  SEntryItem *pEntry )
{
	// Assert if 'pEntry' is invalid
	assert ( pEntry &&
			 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );

	if ( !IsEntryInList ( pEntry ) )
		return 0;

	return std::distance ( m_vEntryList.begin (), std::find ( m_vEntryList.begin (), m_vEntryList.end (), pEntry ) );
}

size_t CEntryList::GetSize ( void )
{
	return m_vEntryList.size ();
}

SEntryItem *CEntryList::GetPrevItem ( SEntryItem *pEntry )
{
	for ( size_t i = 0; i < m_vEntryList.size (); i++ )
	{
		if ( m_vEntryList [ i ] )
		{
			if ( m_vEntryList [ i ] == pEntry && int ( i - 1 ) > 0 )
				return m_vEntryList [ i - 1 ];
		}
	}

	return NULL;
}

SEntryItem *CEntryList::GetNextItem ( SEntryItem *pEntry )
{
	size_t size = m_vEntryList.size ();
	for ( size_t i = 0; i < size; i++ )
	{
		if ( m_vEntryList [ i ] )
		{
			if ( m_vEntryList [ i ] == pEntry && i + 1 < size )
				return m_vEntryList [ i + 1 ];
		}
	}

	return NULL;
}

void CEntryList::ResetList ( void )
{
	for ( size_t i = 0; i < m_vEntryList.size(); i++ )
	{
		SAFE_DELETE ( m_vEntryList [ i ] );
	}

	m_vEntryList.clear ();
}

void CEntryList::SetSelectedEntryByIndex ( int nIndex, bool bSelect )
{
	/// Assert if 'nIndex' is out of range
	//assert ( nIndex < m_vEntryList.size () &&
	//		 _UI ( "'nIdex' is out of range" ) );

	SetSelectedEntry ( GetEntryByIndex ( nIndex ), bSelect );
}

void CEntryList::SetSelectedEntry ( SEntryItem *pEntry, bool bSelect )
{
	// Assert if 'pEntry' is invalid
	//assert ( pEntry &&
	//		 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );
	if ( !pEntry ) 
		return;

	// Enter new selected entry
	m_mSelectedItem [ pEntry ] = bSelect;

	// Leave old selected entry
	if ( m_pSelectedEntry )
		m_mSelectedItem [ m_pSelectedEntry ] = false;

	m_pSelectedEntry = pEntry;

	m_pScrollbar->GetVerScrollbar ()->ShowItem ( GetIndexByEntry ( pEntry ) );
}

bool CEntryList::IsEntrySelectedByIndex ( int nIndex )
{
	// Assert if 'nIndex' is out of range
	assert ( nIndex < m_vEntryList.size () &&
			 _UI ( "'nIdex' is out of range" ) );

	return IsEntrySelected ( GetEntryByIndex ( nIndex ) );
}

bool CEntryList::IsEntrySelected (  SEntryItem *pEntry )
{
	// Assert if 'pEntry' is invalid
	assert ( pEntry &&
			 _UI ( "Insert an SEntryItem parameter 'pEntry' valid" ) );

	return ( pEntry && m_mSelectedItem [ pEntry ] );
}

SEntryItem *CEntryList::GetSelectedEntry ( void )
{
	if ( m_pSelectedEntry && 
		 m_mSelectedItem [ m_pSelectedEntry ] )
		return m_pSelectedEntry;

	return NULL;
}

int CEntryList::GetSelectedEntryIndex ( void )
{
	SEntryItem *pEntry = GetSelectedEntry ();
	if ( pEntry )
		return ( int ) GetIndexByEntry ( pEntry );

	return -1;
}

void CEntryList::UpdateScrollbars ( SControlRect rRect )
{
	if ( !m_pScrollbar )
		return;

	CScrollBarVertical *pScrollbarVer = m_pScrollbar->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pScrollbar->GetHorScrollbar ();

	// Set up scroll bar values

	m_pScrollbar->SetPageSize ( rRect.size.cx - ( pScrollbarHor->GetHeight () + 4 ), rRect.size.cy / m_TextSize.cy );
	m_pScrollbar->UpdateScrollbars ( rRect );
}

bool CEntryList::ContainsRects ( SControlRect rRect, CPos pos )
{
	if ( !m_pScrollbar )
		return false;

	return ( m_pScrollbar->ContainsRect ( pos ) ||
			 rRect.InControlArea ( pos ) );
}

CScrollablePane *CEntryList::GetScrollbar ( void )
{
	return m_pScrollbar;
}