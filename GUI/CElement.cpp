#include "CGUI.h"

const SIZE GetControlMinSize ( CControl::EControlType eType )
{
	SIZE size;

	switch ( eType )
	{
		case CControl::TYPE_DROPDOWN:
		case CControl::TYPE_EDITBOX:
		case CControl::TYPE_BUTTON:
			size.cx = 80;
			size.cy = 20;
			break;
		case CControl::TYPE_RADIOBUTTON:
		case CControl::TYPE_CHECKBOX:
			size.cx = 80;
			size.cy = 15;
			break;
		case CControl::TYPE_IMAGE:
		case CControl::TYPE_BOX:
			size.cx = 80;
			size.cy = 80;
			break;
		case CControl::TYPE_TEXTBOX:
		case CControl::TYPE_LISTBOX:
			size.cx = 100;
			size.cy = 100;
			break;
		case CControl::TYPE_SCROLLBARHORIZONTAL:
		case CControl::TYPE_PROGRESSBARHORIZONTAL:
		case CControl::TYPE_TRACKBARHORIZONTAL:
			size.cx = 80;
			size.cy = 18;
			break;
		case CControl::TYPE_SCROLLBARVERTICAL:
		case CControl::TYPE_TRACKBARVERTICAL:
		case CControl::TYPE_PROGRESSBARVERTICAL:
			size.cx = 18;
			size.cy = 80;
			break;
		case CControl::TYPE_LABEL:
			size.cx = 400;
			break;
		case CControl::TYPE_WINDOW:
			size.cx = 100;
			size.cy = 100;
			break;
		default:
			ZeroMemory ( &size, sizeof ( SIZE ) );
			break;
	}

	return size;
}

void CControl::SetControl ( CDialog *pDialog, EControlType eType )
{
	assert ( pDialog && "To fix hook up CDialog to device callbacks" );

	// Default state
	m_eState = SControlColor::STATE_NORMAL;

	// Set default colors
	m_sControlColor.d3dColorSelectedFont = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorFont = D3DCOLOR_RGBA ( 255, 255, 255, 255 );
	m_sControlColor.d3dColorShape = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorOutline = D3DCOLOR_RGBA ( 0, 0, 0, 255 );
	m_sControlColor.d3dColorBoxBack = D3DCOLOR_RGBA ( 40, 40, 40, 255 );
	m_sControlColor.d3dColorBoxSel = D3DCOLOR_RGBA ( 100, 100, 100, 255 );

	m_sControlColor.d3dColorBox [ SControlColor::STATE_NORMAL ] = D3DCOLOR_RGBA ( 80, 80, 80, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_MOUSE_OVER ] = D3DCOLOR_XRGB ( 100, 100, 100, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_PRESSED ] = D3DCOLOR_XRGB ( 60, 60, 60, 255 );
	m_sControlColor.d3dColorBox [ SControlColor::STATE_DISABLED ] = D3DCOLOR_XRGB ( 180, 180, 180, 255 );

	m_bEnabled = true;
	m_bAntAlias = true;
	m_bEnabledStateColor = true;
	m_bVisible = true;

	m_eType = eType;
	m_pParent = NULL;
	m_pFont = NULL;
	m_pTexture = NULL;

	m_minSize = GetControlMinSize ( eType );

	ZeroMemory ( &m_size, sizeof ( SIZE ) );
	ZeroMemory ( &m_pos, sizeof ( CPos ) );
	ZeroMemory ( &m_rContentBox, sizeof ( SControlRect ) );

	m_pDialog = pDialog;
}

void CControl::SetColor ( SControlColor sColor )
{
	m_sControlColor = sColor;
}

SControlColor CControl::GetColor ( SControlColor sColor )
{
	return m_sControlColor;
}

void CControl::SetParent ( CWindow *pParent )
{
	if ( m_pParent == pParent )
		return;

	m_pParent = pParent;
}

CWindow *CControl::GetParent ( void )
{
	return m_pParent;
}

void CControl::SetAction ( tAction pAction )
{
	m_pAction = pAction;
}

tAction CControl::GetAction ( void )
{
	return m_pAction;
}

void CControl::SetPos ( CPos pos )
{
	m_rBoundingBox.pos = m_pos = pos;
}

void CControl::SetPos ( int X, int Y )
{
	SetPos ( CPos ( X, Y ) );
}

CPos *CControl::GetPos ( void )
{
	return &m_pos;
}

CPos *CControl::GetUpdatedPos ( void )
{
	return &m_rBoundingBox.pos;
}

void CControl::SetWidth ( int iWidth )
{
	m_rBoundingBox.size.cx = m_size.cx = max ( m_minSize.cx, iWidth );
}

int CControl::GetWidth ( void )
{
	return m_rBoundingBox.size.cx;
}

void CControl::SetHeight ( int iHeight )
{
	int iY = 0;

	if ( m_pFont )
	{
		SIZE size;
		m_pFont->GetTextExtent ( GetText (), &size );
		iY = size.cy;
	}

	m_rBoundingBox.size.cy = m_size.cy = max ( iHeight, max ( m_minSize.cy, iY ) );
}

int CControl::GetHeight ( void )
{
	return m_rBoundingBox.size.cy;
}

void CControl::SetSize ( SIZE size )
{
	SetSize ( size.cx, size.cy );
}

void CControl::SetSize ( int iWidth, int iHeight )
{
	SetWidth ( iWidth );
	SetHeight ( iHeight );
}

void CControl::SetMinSize ( int nMin, int nMax )
{
	m_minSize.cx = nMin;
	m_minSize.cy = nMax;
}

void CControl::SetMinSize ( SIZE size )
{
	m_minSize = size;
}

SIZE CControl::GetMinSize ( SIZE size )
{
	return m_minSize;
}

bool CControl::IsSizingX ( void )
{
	if ( m_size.cx != m_rContentBox.size.cx )
	{
		m_rContentBox.size.cx = m_size.cx;
		return true;
	}

	return false;
}

bool CControl::IsSizingY ( void )
{
	if ( m_size.cy != m_rContentBox.size.cy )
	{
		m_rContentBox.size.cy = m_size.cy;
		return true;
	}

	return false;
}

bool CControl::IsSizing ( void )
{
	return ( IsSizingX () ||
			 IsSizingY () );
}

bool CControl::IsMoving ( void )
{
	if ( m_pos.GetX () == m_rContentBox.pos.GetX () &&
		 m_pos.GetY () == m_rContentBox.pos.GetY () )
	{
		return false;
	}
	else
	{
		m_rContentBox.pos = m_pos;
		return true;
	}
}

SIZE CControl::GetSize ( void )
{
	return m_rBoundingBox.size;
}

bool CControl::CanHaveFocus ( void )
{
	return ( m_bVisible && m_bEnabled );
}

bool CControl::HasFocus ( void )
{
	return m_bHasFocus;
}

void CControl::SetText ( SIMPLEGUI_STRING sString )
{
	m_sText = sString;
}

const SIMPLEGUI_CHAR *CControl::GetText ( void )
{
	return m_sText.c_str ();
}

void CControl::Draw ( void )
{}

bool CControl::HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return false;
}

bool CControl::HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam )
{
	return false;
}

void CControl::SetTexture ( const TCHAR *szPath )
{
	if ( !m_pDialog )
		return;

	// Check if there is already a texture
	if ( m_pTexture )
		m_pDialog->RemoveTexture ( m_pTexture );

	m_pDialog->LoadTexture ( szPath, &m_pTexture );
}

void CControl::SetTexture ( LPCVOID pSrc, UINT uSrcSize )
{
	if ( !m_pDialog )
		return;

	// Check if there is already a texture
	if ( m_pTexture )
		m_pDialog->RemoveTexture ( m_pTexture );

	m_pDialog->LoadTexture ( pSrc, uSrcSize, &m_pTexture );
}

CD3DTexture *CControl::GetTexture ( void )
{
	return m_pTexture;
}

void CControl::SetFont ( const TCHAR *szFontName, DWORD dwHeight, bool bBold )
{
	if ( !m_pDialog )
		return;

	SAFE_DELETE ( m_pFont );

	m_pDialog->LoadFont ( szFontName, dwHeight, bBold, &m_pFont );
}

void CControl::SetFont ( CD3DFont *pFont )
{
	SAFE_DELETE ( m_pFont );

	m_pFont = pFont;
}

CD3DFont *CControl::GetFont ( void )
{
	return m_pFont;
}

void CControl::SetStateColor ( D3DCOLOR d3dColor, SControlColor::SControlState eState )
{
	m_sControlColor.d3dColorBox [ eState ] = d3dColor;
}

void CControl::SetEnabled ( bool bEnabled )
{
	m_bEnabled = bEnabled;
}

bool CControl::IsEnabled ( void )
{
	return m_bEnabled;
}

void CControl::SetVisible ( bool bVisible )
{
	m_bVisible = bVisible;
}

bool CControl::IsVisible ( void )
{
	return m_bVisible;
}

void CControl::SetEnabledStateColor ( bool bEnable )
{
	m_bEnabledStateColor = bEnable;
}

void CControl::SetAntAlias ( bool bAntAlias )
{
	m_bAntAlias = bAntAlias;
}

CControl::EControlType CControl::GetType ( void )
{
	return m_eType;
}

void CControl::OnFocusIn ( void )
{
	m_bHasFocus = true;
}

void CControl::OnFocusOut ( void )
{
	m_bHasFocus = false;
}

bool CControl::OnMouseOver ( void )
{
	return m_bMouseOver;
}

void CControl::OnMouseEnter ( void )
{
	m_bMouseOver = true;
}

void CControl::OnMouseLeave ( void )
{
	m_bMouseOver = false;
}

void CControl::OnClickEnter ( void )
{
	m_bPressed = true;
}

void CControl::OnClickLeave ( void )
{
	m_bPressed = false;
}

bool CControl::OnClickEvent ( void )
{
	return m_bPressed;
}

bool CControl::SendEvent ( E_EVENT_CONTROL event, int params )
{
	if ( !m_pAction )
		return false;

	m_pAction ( this, event, params );
}

void CControl::UpdateRects ( void )
{
	m_rBoundingBox.size = m_size;

	if ( m_pParent )
	{
		SIZE size = m_pParent->GetSize ();

		if ( m_bRelativeSX )
			m_rBoundingBox.size.cx = max ( m_minSize.cx, m_rBoundingBox.size.cx + ( ( size.cx - m_pParent->GetRealSize ().cx ) ) );

		if ( m_bRelativeSY )
			m_rBoundingBox.size.cy = max ( m_minSize.cy, m_rBoundingBox.size.cy + ( ( size.cy - m_pParent->GetRealSize ().cy ) ) );
	}
}

void CControl::BeginRenderRect ( SControlRect rRect )
{
	m_rScissor = m_rBoundingBox;

	if ( rRect.pos.GetY () + rRect.size.cy < m_rBoundingBox.pos.GetY () + m_rBoundingBox.size.cy - 1 )
	{
		m_rScissor.size.cy = rRect.pos.GetY () + rRect.size.cy - m_rBoundingBox.pos.GetY () - 1;
	}

	if ( m_rBoundingBox.pos.GetY () < rRect.pos.GetY () + m_pParent->GetTitleBarHeight () + 1 )
	{
		int iDragOffSetY = ( rRect.pos.GetY () + m_pParent->GetTitleBarHeight () + 1 ) - m_rBoundingBox.pos.GetY ();
		m_rScissor.pos.SetY ( m_rScissor.pos.GetY () + iDragOffSetY );
		m_rScissor.size.cy = m_rScissor.size.cy - iDragOffSetY;
	}

	if ( rRect.pos.GetX () + rRect.size.cx < m_rBoundingBox.pos.GetX () + m_rBoundingBox.size.cx + 1 )
	{
		m_rScissor.size.cx = rRect.pos.GetX () + rRect.size.cx - m_rBoundingBox.pos.GetX () - 1;
	}

	if ( m_rBoundingBox.pos.GetX () < rRect.pos.GetX () + 1 )
	{
		int iDragOffSetX = rRect.pos.GetX () - m_rBoundingBox.pos.GetX () + 1;
		m_rScissor.pos.SetX ( m_rScissor.pos.GetX () + iDragOffSetX );
		m_rScissor.size.cx = m_rScissor.size.cx - iDragOffSetX;
	}

	SetScissor ( m_pDialog->GetDevice (), m_rScissor.GetRect () );
	m_rScissor = rRect;
}

void CControl::EndRenderRect ( void )
{
	SetScissor ( m_pDialog->GetDevice (), m_rScissor.GetRect () );
}

void CControl::SetRelativePosX ( bool bRelative )
{
	m_bRelativeX = bRelative;
}
void CControl::SetRelativePosY ( bool bRelative )
{
	m_bRelativeY = bRelative;
}

void CControl::SetRelativeSizeX ( bool bRelative )
{
	m_bRelativeSX = bRelative;
}

void CControl::SetRelativeSizeY ( bool bRelative )
{
	m_bRelativeSY = bRelative;
}

void CControl::LinkPos ( CPos pos )
{
	SIZE size [ 2 ];
	ZeroMemory ( size, sizeof ( SIZE ) * 2 );

	if ( m_pParent && m_eType != TYPE_WINDOW )
	{
		if ( m_bRelativeX &&
			 m_size.cx > m_rBoundingBox.size.cx + ( ( m_pParent->GetWidth () - m_pParent->GetRealSize ().cx ) ) )
		{
			size [ 0 ].cx = m_pParent->GetWidth ();
			size [ 1 ].cx = m_pParent->GetRealSize ().cx;
		}

		if ( m_bRelativeY &&
			 m_size.cy > m_rBoundingBox.size.cy + ( ( m_pParent->GetHeight () - m_pParent->GetRealSize ().cy ) ) )
		{
			size [ 0 ].cy = m_pParent->GetHeight ();
			size [ 1 ].cy = m_pParent->GetRealSize ().cy;
		}
	}

	m_rBoundingBox.pos = m_pos + CPos ( size [ 0 ].cx, size [ 0 ].cy ) + pos - CPos ( size [ 1 ].cx, size [ 1 ].cy );

	UpdateRects ();
}

bool CControl::ContainsRect ( CPos pos )
{
	if ( !CanHaveFocus () )
		return false;

	return m_rBoundingBox.InControlArea ( pos );
}