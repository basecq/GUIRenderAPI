#include "CGUI.h"

#define TEXTBOX_TEXTSPACE 18

CLogBox::CLogBox ( CDialog *pDialog )
{
	SetControl ( pDialog, TYPE_TEXTBOX );

	/*m_sControlColor.d3dColorFont = D3DCOLOR_RGBA ( , 0, 0, 255 );
	m_sControlColor.d3dColorSelectedFont = D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	*/

	m_bSet = false;

	m_pEntryList = new CEntryList ( pDialog );

	if ( !m_pEntryList )
		MessageBox ( 0, _UI ( "CLogBox::CLogBox: Error for creating CEntryList" ), _UI ( "GUIAPI.asi" ), 0 );

	m_pEntryList->GetScrollbar ()->AddControl ( this );

	m_pLogFile = new CLogFile ( _UI ( "Log.txt" ) );
}

CLogBox::~CLogBox ( void )
{
	SAFE_DELETE ( m_pEntryList );
	SAFE_DELETE ( m_pLogFile );
}

void CLogBox::Draw ( void )
{
	if ( !m_bVisible ||
		!m_pFont )
	{
		return;
	}
	
	m_pDialog->DrawBox ( m_rBoundingBox, m_sControlColor.d3dColorBoxBack, m_sControlColor.d3dColorOutline, m_bAntAlias );
	m_pEntryList->Render ( m_rBoundingBox, 0, 0, 0, m_sControlColor.d3dColorFont, -1 );
}

void CLogBox::SetSortedList ( bool bSort )
{
	m_pEntryList->SetSortedList ( bSort );
}

void CLogBox::AddText ( E_LOGTYPE eType, const SIMPLEGUI_CHAR *szText, ... )
{
	va_list arglist;
	va_start ( arglist, szText );

	time_t now;
	time ( &now );

	SIMPLEGUI_CHAR szStr [ MAX_PATH ], szFormat [ MAX_PATH ];

	SIMPLEGUI_FTIME ( szStr, 128, _UI ( "[%d.%m %H:%M:%S] {FF750000}" ), localtime ( &now ) );
	SIMPLEGUI_STRCAT ( szStr, GetModeType ( eType ) );
	SIMPLEGUI_STRCAT ( szStr, _UI ( "{00000000}: " ) );
	wvsprintf ( szFormat, szText, arglist );
	SIMPLEGUI_STRCAT ( szStr, szFormat );

	m_pEntryList->AddEntry ( new SEntryItem ( szStr ) );

	if ( m_pLogFile && m_bSet )
		m_pLogFile->Log ( GetModeType ( eType ), szText, arglist );

	va_end ( arglist );
}

void CLogBox::RemoveText ( UINT uIndex )
{
	m_pEntryList->RemoveEntry ( m_pEntryList->GetEntryByIndex ( uIndex ) );
}

const SIMPLEGUI_CHAR *CLogBox::GetText ( UINT uIndex )
{
	SEntryItem *pEntry = m_pEntryList->GetEntryByIndex ( uIndex );

	// Check for a valid 'pEntry' pointer
	if ( pEntry )
	{
		return pEntry->m_sText.c_str ();
	}
	else
		return NULL;
}

void CLogBox::ResetList ( void )
{
	m_pEntryList->ResetList ();
}

bool CLogBox::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	CScrollBarVertical *pScrollbarVer = m_pEntryList->GetScrollbar ()->GetVerScrollbar ();
	CScrollBarHorizontal *pScrollbarHor = m_pEntryList->GetScrollbar ()->GetHorScrollbar ();

	if ( !CanHaveFocus () ||
		 !pScrollbarVer ||
		 !pScrollbarHor )
		return false;

	// First acquire focus
	if ( WM_LBUTTONDOWN == uMsg )
			m_pParent->RequestControlFocus ( this );

	if ( m_pEntryList->GetScrollbar ()->HandleMouse ( uMsg, pos, wParam, lParam ) )
		return true;

	switch ( uMsg )
	{
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		{
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
			if ( m_bPressed )
			{
				m_bPressed = false;
				return true;
			}
			break;
		}

		case WM_MOUSEWHEEL:
		{
			int zDelta = ( short ) HIWORD ( wParam ) / WHEEL_DELTA;
			m_pEntryList->GetScrollbar ()->GetVerScrollbar ()->Scroll ( -zDelta * m_pEntryList->GetScrollbar ()->GetVerScrollbar ()->GetStepSize () );

			return true;
		}
	};

	return false;
}

bool CLogBox::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
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
				case VK_LEFT:
				{
					pScrollbarHor->Scroll ( -pScrollbarHor->GetStepSize () );
					return true;
				}
				case VK_UP:
				{
					pScrollbarVer->Scroll ( -m_pEntryList->GetTextSize ().cy );
					return true;
				}
				case VK_RIGHT:
				{
					pScrollbarHor->Scroll ( pScrollbarHor->GetStepSize () );
					return true;
				}
				case VK_DOWN:
				{
					pScrollbarVer->Scroll ( m_pEntryList->GetTextSize ().cy );
					return true;
				}
			}
			break;
		}
	}

	return false;
}

void CLogBox::OnClickLeave ( void )
{
	CControl::OnClickLeave ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnClickLeave ();
}

bool CLogBox::OnClickEvent ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CControl::OnClickEvent () ||
			 pScrollbar->OnClickEvent () );
}

void CLogBox::OnFocusIn ( void )
{
	CControl::OnFocusIn ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusIn ();
}

void CLogBox::OnFocusOut ( void )
{
	CControl::OnFocusOut ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnFocusOut ();
}

void CLogBox::OnMouseEnter ( void )
{
	CControl::OnMouseEnter ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseEnter ();
}

void CLogBox::OnMouseLeave ( void )
{
	CControl::OnMouseLeave ();

	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	if ( pScrollbar )
		pScrollbar->OnMouseLeave ();
}

bool CLogBox::CanHaveFocus ( void )
{
	CScrollablePane *pScrollbar = m_pEntryList->GetScrollbar ();

	return ( CControl::CanHaveFocus () ||
			 pScrollbar->CanHaveFocus () );
}

void CLogBox::UpdateRects ( void )
{
	CControl::UpdateRects ();

	m_pEntryList->UpdateScrollbars ( m_rBoundingBox );
}

bool CLogBox::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return m_pEntryList->ContainsRects ( m_rBoundingBox, pos );
}

void CLogBox::GenerateLogFile ( bool bSet )
{
	m_bSet = bSet;
}

const SIMPLEGUI_CHAR *CLogBox::GetModeType ( E_LOGTYPE eType )
{
	switch ( eType )
	{
		case CLogBox::LOG_NONE:
			break;
		case CLogBox::LOG_DEBUG:
			return _UI ( "DEBUG" );
		case CLogBox::LOG_INFO:
			return _UI ( "INFO" );
		case CLogBox::LOG_ERROR:
			return _UI ( "ERROR" );
		case CLogBox::LOG_WARNING:
			return _UI ( "WARNING" );
		default:
			break;
	}

	return NULL;
}