#include "CGraphics.h"


#pragma warning (push)
#pragma warning (disable : 4996)


//=========================================================================

//-------------------------------------------------------------------------
// Custom vertex types for rendering text
//-------------------------------------------------------------------------

#define D3DFVF_BITMAPFONT ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#define D3DFVF_PRIMITIVES	( D3DFVF_XYZRHW | D3DFVF_DIFFUSE )

#define MAX_NUM_VERTICES 50 * 6

inline FONT2DVERTEX InitFont2DVertex ( const D3DXVECTOR4& p, D3DCOLOR color,
									   FLOAT tu, FLOAT tv )
{
	return { p,   color, tu, tv };
}

inline LVERTEX Init2DVertex ( float x, float y,
							  DWORD color, float tu, float tv )
{
	return { x, y, 1.0f, 1.0f, color, tu, tv };
}

// Helper's function

void RotateVerts ( VECTOR2 *pVector, size_t size, float fX, float fY, float fAngle )
{
	fAngle /= ( 180.f / D3DX_PI );

	for ( size_t i = 0; i < size; i++ )
	{
		float fOldX = pVector [ i ].x;
		float fOldY = pVector [ i ].y;

		pVector [ i ].x = fX + ( fOldX - fX ) * cos ( fAngle ) + ( fOldY - fY ) * sin ( fAngle );
		pVector [ i ].y = fY - ( fOldX - fX ) * sin ( fAngle ) + ( fOldY - fY ) * cos ( fAngle );

	}
}

void SetScissor ( IDirect3DDevice9 *pD3dd, RECT rect )
{
	if ( rect.left + rect.right > 0 &&
		 rect.top + rect.bottom > 0 )
	{
		rect.right += 1;
		rect.bottom += 1;

		pD3dd->SetScissorRect ( &rect );
		pD3dd->SetRenderState ( D3DRS_SCISSORTESTENABLE, TRUE );
	}
	else
		pD3dd->SetRenderState ( D3DRS_SCISSORTESTENABLE, FALSE );
}


void SetScissor ( IDirect3DDevice9 *pD3dd, int iX, int iY, int iWidth, int iHeight )
{
	RECT rRect;
	SetRect ( &rRect, iX, iY, iX + iWidth, iY + iHeight );

	SetScissor ( pD3dd, rRect );
}

void ClipRect ( LPDIRECT3DDEVICE9 device, LPDIRECT3DSURFACE9 surface, LPDIRECT3DSURFACE9 backbuffer, int x, int y, int width, int height, int destX, int destY, int destWidth, int destHeight )
{
	RECT source;

	if ( x >= destX && 
		 ( x + width ) <= ( destX + destWidth ) )
	{
		source.left = 0;
		source.right = width;
	}
	else if ( ( x >= destX && 
			  x <= ( destX + destWidth ) ) && 
			  ( ( x + width ) > ( destX + destWidth ) ) )
	{
		source.left = 0;
		source.right = width - ( ( x + width ) - ( destX + destWidth ) );
		source.right = abs ( source.right );
	}
	else if ( x < destX && 
			  ( x + width ) < ( destX + destWidth ) )
	{
		source.left = abs ( x );
		source.right = width;
	}
	else if ( ( x < destX ) && 
			  ( ( x + width ) > ( destX + destWidth ) ) )
	{
		source.left = abs ( x );
		source.right = source.left + ( destWidth );
	}
	else
	{
		return;
	}


	if ( y >= destY && 
		 ( y + height ) <= ( destY + destHeight ) )
	{
		source.top = 0;
		source.bottom = height;
	}
	else if ( ( y >= destY && 
			  y <= ( destY + destHeight ) ) && 
			  ( ( y + height ) > ( destY + destHeight ) ) )
	{
		source.top = 0;
		source.bottom = height - ( ( y + height ) - ( destY + destHeight ) );
		source.bottom = abs ( source.bottom );
	}
	else if ( y < destY && 
			  ( y + height ) > destY 
			  && ( y + height ) <= ( destY + destHeight ) )
	{
		source.top = abs ( y );
		source.bottom = height;
	}
	else if ( ( y < destY ) && 
			  ( ( y + height ) > ( destY + destHeight ) ) )
	{
		source.top = abs ( y );
		source.bottom = source.top + ( destHeight );
	}
	else
	{
		return;
	}

	RECT destination;

	if ( x >= destX && 
		 ( x + width ) <= ( destX + destWidth ) )
	{
		destination.left = x;
		destination.right = x + width;
	}
	else if ( ( x >= destX && 
			  x <= ( destX + destWidth ) ) 
			  && ( ( x + width ) > ( destX + destWidth ) ) )
	{
		destination.left = x;
		destination.right = ( destX + destWidth );
	}
	else if ( ( x < destX ) && 
			  ( ( x + width ) < ( destX + destWidth ) ) && 
			  ( ( x + width ) >= x ) )
	{
		destination.left = destX;
		destination.right = width - abs ( x );
	}
	else if ( ( x < destX ) && 
			  ( ( x + width ) > ( destX + destWidth ) ) )
	{
		destination.left = destX;
		destination.right = ( destX + destWidth );
	}
	else
	{
		return;
	}

	if ( y >= destY && 
		 ( y + height ) <= ( destY + destHeight ) )
	{
		destination.top = y;
		destination.bottom = y + height;
	}
	else if ( ( y >= destY && 
			  y <= ( destY + destHeight ) ) && 
			  ( y + height ) > ( destY + destHeight ) )
	{
		destination.top = y;
		destination.bottom = ( destY + destHeight );
	}
	else if ( y < destY && 
			  ( y + height ) > destY && 
			  ( y + height ) <= ( destY + destHeight ) )
	{
		destination.top = destY;
		destination.bottom = height - abs ( y );
	}
	else if ( ( y < destY ) && 
			  ( ( y + height ) > ( destY + destHeight ) ) )
	{
		destination.top = destY;
		destination.bottom = ( destY + destHeight );
	}
	else
	{
		return;
	}

	device->StretchRect ( surface, &source, backbuffer, &destination, D3DTEXF_NONE );

	DeleteObject ( &source );
	DeleteObject ( &destination );
};

void ClipRect ( LPDIRECT3DDEVICE9 device, LPDIRECT3DSURFACE9 surface, LPDIRECT3DSURFACE9 backbuffer, RECT rScr, RECT rDst )
{
	/*ClipRect ( device, surface, backbuffer,
			   rScr.left, rScr.top, rScr.left + rScr.right, rScr.top + rScr.bottom,
			   rDst.left, rDst.top, rDst.left + rDst.right, rDst.top + rDst.bottom );
	*/
	device->StretchRect ( surface, &rScr, backbuffer, &rDst, D3DTEXF_NONE );
}

CGraphics::CGraphics ( IDirect3DDevice9 *pDevice )
{
	m_pd3dDevice = pDevice;
}

CGraphics::~CGraphics ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		SAFE_DELETE ( m_ItemAddon [ i ].pFont );
		SAFE_DELETE ( m_ItemAddon [ i ].pTexture );
		SAFE_DELETE ( m_ItemAddon [ i ].pRender );
	}
}

void CGraphics::CreateD3DFont ( int ID, const TCHAR *pszFontName, int iFontHeight, DWORD dwCreateFlags )
{
	CD3DFont *pFont = new CD3DFont ( pszFontName, iFontHeight, dwCreateFlags );
	if ( pFont )
	{
		pFont->Initialize ( m_pd3dDevice );
		
	}

	SItem sItem;
	sItem.pFont = pFont;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

void CGraphics::CreateD3DRender ( int ID, int iNumVertices )
{
	CD3DRender *pRender = new CD3DRender ( iNumVertices );
	if ( pRender )
		pRender->Initialize ( m_pd3dDevice );

	SItem sItem;
	sItem.pRender = pRender;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

void CGraphics::CreateD3DTexture ( int ID, const TCHAR *pszPath )
{
	CD3DTexture *pTexture = new CD3DTexture ( m_pd3dDevice );
	
	if ( pTexture )
		pTexture->CreateTextureFromFile ( pszPath );

	SItem sItem;
	sItem.pTexture = pTexture;
	sItem.ID = ID;

	m_ItemAddon.push_back ( sItem );
}

CD3DFont *CGraphics::GetFontAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pFont;
		}
	}
	
	return NULL;
}

CD3DRender *CGraphics::GetRendererAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pRender &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pRender;
		}
	}
	
	return NULL;
}

CD3DTexture *CGraphics::GetTextureAt ( int ID )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pTexture &&
			 m_ItemAddon [ i ].ID == ID )
		{
			return m_ItemAddon [ i ].pTexture;
		}
	}
	
	return NULL;
}

void CGraphics::RemoveItem ( HANDLE hItem )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont == ( CD3DFont* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pFont )
		else if ( m_ItemAddon [ i ].pTexture == ( CD3DTexture* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pTexture )
		else if ( m_ItemAddon [ i ].pRender == ( CD3DRender* ) hItem )
			SAFE_DELETE ( m_ItemAddon [ i ].pRender )
	}
}

void CGraphics::OnLostDevice ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont )
			m_ItemAddon [ i ].pFont->Invalidate ();
		else if ( m_ItemAddon [ i ].pRender )
			m_ItemAddon [ i ].pRender->Invalidate ();
		else if ( m_ItemAddon [ i ].pTexture )
			m_ItemAddon [ i ].pTexture->OnResetDevice ();
	}
}

void CGraphics::OnResetDevice ( void )
{
	for ( size_t i = 0; i < m_ItemAddon.size (); i++ )
	{
		if ( m_ItemAddon [ i ].pFont )	
			m_ItemAddon [ i ].pFont->Initialize ( m_pd3dDevice );				
		else if ( m_ItemAddon [ i ].pRender )
			m_ItemAddon [ i ].pRender->Initialize ( m_pd3dDevice );
		else if ( m_ItemAddon [ i ].pTexture )
			m_ItemAddon [ i ].pTexture->OnLostDevice ();
	}
}

CD3DStateBlock::CD3DStateBlock ( void )
{
	m_pd3dDevice = NULL;
	m_pStateBlock = NULL;
}

void CD3DStateBlock::Initialize ( IDirect3DDevice9 *pDevice )
{
	if ( pDevice && !m_pStateBlock )
		pDevice->CreateStateBlock ( D3DSBT_ALL, &m_pStateBlock );

	m_pd3dDevice = pDevice;
}

void CD3DStateBlock::Invalidate ( void )
{
	SAFE_RELEASE ( m_pStateBlock );
	m_pd3dDevice = NULL;
}

HRESULT CD3DStateBlock::BeginState ( void )
{
	if ( !m_pStateBlock )
		return E_FAIL;

	if ( FAILED ( m_pStateBlock->Capture () ) )
		return E_FAIL;

	return S_OK;
}

HRESULT CD3DStateBlock::EndState ( void )
{
	if ( !m_pStateBlock )
		return E_FAIL;

	if ( FAILED ( m_pStateBlock->Apply () ) )
		return E_FAIL;

	return S_OK;
}

void CD3DStateBlock::SetRenderStates ( void )
{
	if ( !m_pd3dDevice )
		return;

	m_pd3dDevice->SetPixelShader ( NULL );
	m_pd3dDevice->SetVertexShader ( NULL );

	m_pd3dDevice->SetRenderState ( D3DRS_ALPHABLENDENABLE, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	m_pd3dDevice->SetRenderState ( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHATESTENABLE, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHAREF, 0x08 );
	m_pd3dDevice->SetRenderState ( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
	m_pd3dDevice->SetRenderState ( D3DRS_FILLMODE, D3DFILL_SOLID );
	m_pd3dDevice->SetRenderState ( D3DRS_CULLMODE, D3DCULL_CCW );
	m_pd3dDevice->SetRenderState ( D3DRS_STENCILENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_CLIPPING, TRUE );
	m_pd3dDevice->SetRenderState ( D3DRS_CLIPPLANEENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_VERTEXBLEND, D3DVBF_DISABLE );
	m_pd3dDevice->SetRenderState ( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_FOGENABLE, FALSE );
	m_pd3dDevice->SetRenderState ( D3DRS_COLORWRITEENABLE,
								   D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA );

	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXCOORDINDEX, 0 );
	m_pd3dDevice->SetTextureStageState ( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	m_pd3dDevice->SetTextureStageState ( 1, D3DTSS_COLOROP, D3DTOP_DISABLE );
	m_pd3dDevice->SetTextureStageState ( 1, D3DTSS_ALPHAOP, D3DTOP_DISABLE );

	m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	m_pd3dDevice->SetSamplerState ( 0, D3DSAMP_MIPFILTER, D3DTEXF_NONE );
}

//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont ( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags )
{
#ifdef _UNICODE 
	wcsncpy_s ( m_strFontName, strFontName, sizeof ( m_strFontName ) / sizeof ( TCHAR ) );
#else
	strncpy_s ( m_strFontName, strFontName, sizeof ( m_strFontName ) / sizeof ( TCHAR ) );
#endif

	m_strFontName [ sizeof ( m_strFontName ) / sizeof ( TCHAR ) - 1 ] = '\0';
	m_dwFontHeight = dwHeight;
	m_dwFontFlags = dwFlags;
	m_dwSpacing = 0;
	m_fWidth = 20000.f;

	m_pd3dDevice = NULL;
	m_pTexture = NULL;
	m_pVB = NULL;

	// Create a state block
	m_pState = new CD3DStateBlock ();
}

//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont ()
{
	Invalidate ();
}

//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Initialize ( LPDIRECT3DDEVICE9 pd3dDevice )
{
	HRESULT hr;

	if ( m_pState )
		m_pState->Initialize ( pd3dDevice );

	m_pd3dDevice = pd3dDevice;

	// Establish the font and texture size
	m_fTextScale = 1.0f;
	m_dwTexWidth = m_dwTexHeight = 1024;

	// If requested texture is too big, use a smaller texture and smaller font,
	// and scale up when rendering.
	D3DCAPS9 d3dCaps;
	m_pd3dDevice->GetDeviceCaps ( &d3dCaps );

	if ( m_dwTexWidth > d3dCaps.MaxTextureWidth )
	{
		m_fTextScale = ( FLOAT ) d3dCaps.MaxTextureWidth / ( FLOAT ) m_dwTexWidth;
		m_dwTexWidth = m_dwTexHeight = d3dCaps.MaxTextureWidth;
	}

	// Create vertex buffer for the letters
	hr = m_pd3dDevice->CreateVertexBuffer ( MAX_NUM_VERTICES * sizeof ( FONT2DVERTEX ),
											D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0,
											D3DPOOL_DEFAULT, &m_pVB, NULL );
	if ( FAILED ( hr ) )
		return hr;

	// Create a new texture for the font
	hr = m_pd3dDevice->CreateTexture ( m_dwTexWidth, m_dwTexHeight, 1,
									   0, D3DFMT_A4R4G4B4,
									   D3DPOOL_MANAGED, &m_pTexture, NULL );
	if ( FAILED ( hr ) )
		return hr;

	// Prepare to create a bitmap
	DWORD*      pBitmapBits;
	BITMAPINFO bmi;
	ZeroMemory ( &bmi.bmiHeader, sizeof ( BITMAPINFOHEADER ) );
	bmi.bmiHeader.biSize = sizeof ( BITMAPINFOHEADER );
	bmi.bmiHeader.biWidth = ( int ) m_dwTexWidth;
	bmi.bmiHeader.biHeight = -( int ) m_dwTexHeight;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biBitCount = 32;

	// Create a DC and a bitmap for the font
	HDC     hDC = CreateCompatibleDC ( NULL );
	HBITMAP hbmBitmap = CreateDIBSection ( hDC, &bmi, DIB_RGB_COLORS,
										   ( void** ) &pBitmapBits, NULL, 0 );
	SetMapMode ( hDC, MM_TEXT );

	// Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
	// antialiased font, but this is not guaranteed.
	INT nHeight = -MulDiv ( m_dwFontHeight, ( INT ) ( GetDeviceCaps ( hDC, LOGPIXELSY ) * m_fTextScale ), 72 );
	DWORD dwBold = ( m_dwFontFlags & D3DFONT_BOLD ) ? FW_BOLD : FW_NORMAL;
	DWORD dwItalic = ( m_dwFontFlags & D3DFONT_ITALIC ) ? TRUE : FALSE;

	HFONT hFont = CreateFont ( nHeight, 0, 0, 0, dwBold, dwItalic,
								FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
								CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
								VARIABLE_PITCH, m_strFontName );
	if ( NULL == hFont )
		return E_FAIL;

	SelectObject ( hDC, hbmBitmap );
	SelectObject ( hDC, hFont );

	// Set text properties
	SetTextColor ( hDC, RGB ( 255, 255, 255 ) );
	SetBkColor ( hDC, 0x00000000 );
	SetTextAlign ( hDC, TA_TOP );

	// Loop through all printable character and output them to the bitmap..
	// Meanwhile, keep track of the corresponding tex coords for each character.
	DWORD x = 0;
	DWORD y = 0;
	TCHAR str [ 2 ] = { ' ' , 0 };
	SIZE size;

	// Calculate the spacing between characters based on line height
	GetTextExtentPoint32 ( hDC, str, 1, &size );
	x = m_dwSpacing = ( DWORD ) ceil ( size.cy * 0.3f );

	for ( int c = 32; c < 255; c++ )
	{
		str [ 0 ] = btowc ( c );
		if ( !GetTextExtentPoint32 ( hDC, str, 1, &size ) )
			return E_FAIL;

		if ( ( DWORD ) ( x + size.cx + m_dwSpacing ) > m_dwTexWidth )
		{
			x = m_dwSpacing;
			y += size.cy + 1;
		}

		if ( !ExtTextOut ( hDC, x + 0, y + 0, ETO_OPAQUE, NULL, str, 1, NULL ) )
			return E_FAIL;

		m_fTexCoords [ c - 32 ] [ 0 ] = ( ( FLOAT ) ( x + 0 - m_dwSpacing ) ) / m_dwTexWidth;
		m_fTexCoords [ c - 32 ] [ 1 ] = ( ( FLOAT ) ( y + 0 + 0 ) ) / m_dwTexHeight;
		m_fTexCoords [ c - 32 ] [ 2 ] = ( ( FLOAT ) ( x + size.cx + m_dwSpacing ) ) / m_dwTexWidth;
		m_fTexCoords [ c - 32 ] [ 3 ] = ( ( FLOAT ) ( y + size.cy + 0 ) ) / m_dwTexHeight;

		x += size.cx + ( 2 * m_dwSpacing );
	}

	// Lock the surface and write the alpha values for the set pixels
	D3DLOCKED_RECT d3dlr;
	m_pTexture->LockRect ( 0, &d3dlr, 0, 0 );
	BYTE* pDstRow = reinterpret_cast< BYTE* >( d3dlr.pBits );
	WORD* pDst16;
	BYTE bAlpha; // 4-bit measure of pixel intensity

	for ( y = 0; y < m_dwTexHeight; y++ )
	{
		pDst16 = reinterpret_cast< WORD* >( pDstRow );

		for ( x = 0; x < m_dwTexWidth; x++ )
		{
			bAlpha = static_cast< BYTE >( ( pBitmapBits [ m_dwTexWidth * y + x ] >> 8 ) ) & 0xFF;
			*pDst16 = static_cast< WORD >( ( bAlpha << 8 ) ) | 0x0FFF;
			pDst16++;
		}

		pDstRow += d3dlr.Pitch;
	}

	// Done updating texture, so clean up used objects
	m_pTexture->UnlockRect ( 0 );
	DeleteObject ( hbmBitmap );
	DeleteDC ( hDC );
	DeleteObject ( hFont );

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Invalidate ()
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pTexture );

	if ( m_pState )
		m_pState->Invalidate ();

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent ( const TCHAR* str, SIZE* pSize )
{
	if ( !str ||
		 !pSize )
		return E_FAIL;

	FLOAT fRowWidth = 0.0f;
	FLOAT fRowHeight = ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] )*m_dwTexHeight;
	FLOAT fWidth = 0.0f;
	FLOAT fHeight = fRowHeight;

	TCHAR *strText = ( TCHAR* ) str;

	while ( *strText )
	{
		int c = *strText++ & 0xFF;

		if ( c == '{' )
		{
			std::vector<TCHAR> costumColor;
			TCHAR *strData = strText;

			bool bTagFound = true;
			size_t count = 0;

			while ( *strData != '}' )
			{
				if ( count > 8 )
				{
					bTagFound = false;
					break;
				}
				TCHAR ch = toupper ( *strText );
				bool numeric =	( ch >= '0' && ch <= '9' ) ||
								( ch >= 'A' && ch <= 'F' );

				if ( numeric )
				{
					costumColor.push_back ( ch );
				}

				strData++;
				count++;
			}

			if ( bTagFound &&
				 costumColor.size () >= 8 )
			{
				c = *strData++;
				strText = strData;
				continue;
			}
		}

		FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
		FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];

		if ( *strText == '\n' ||
			 fRowWidth >= m_fWidth )
		{
			fRowWidth = 0.0f;
			fHeight += fRowHeight;
		}

		fRowWidth += ( tx2 - tx1 ) * m_dwTexWidth - 2 * m_dwSpacing;

		if ( fRowWidth > fWidth )
			fWidth = fRowWidth;
	}

	pSize->cx = ( int ) fWidth;
	pSize->cy = ( int ) fHeight;

	return S_OK;
}

//-----------------------------------------------------------------------------
// Name: Print()
// Desc: Draws 2D text. Note that sx and sy are in pixels
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Print ( FLOAT sx, FLOAT sy, DWORD dwColor,
							 const TCHAR* szText, DWORD dwFlags )
{
	if ( !m_pState )
		return E_FAIL;

	if ( FAILED ( m_pState->BeginState () ) )
		return E_FAIL;

	m_pState->SetRenderStates ();

	m_pd3dDevice->SetTexture ( 0, m_pTexture );
	m_pd3dDevice->SetFVF ( D3DFVF_BITMAPFONT );
	m_pd3dDevice->SetStreamSource ( 0, m_pVB, 0, sizeof ( FONT2DVERTEX ) );

	TCHAR *szStr = ( TCHAR* ) szText;

	// Center the text block in the viewport
	if ( dwFlags & D3DFONT_CENTERED_X )
	{
		SIZE pSize;
		GetTextExtent ( szStr, &pSize );
		sx -= ( FLOAT ) pSize.cx / 2.0f;
	}

	if ( dwFlags & D3DFONT_CENTERED_Y )
	{
		SIZE sz;
		GetTextExtent ( szStr, &sz );
		sy -= ( FLOAT ) sz.cy / 2.0f;
	}

	// Adjust for character spacing
	sx -= m_dwSpacing;
	FLOAT fStartX = sx;

	// Fill vertex buffer
	FONT2DVERTEX* pVertices = NULL;
	DWORD         dwNumTriangles = 0;
	m_pVB->Lock ( 0, 0, ( void** ) &pVertices, D3DLOCK_DISCARD );

	DWORD dwCustomColor = dwColor;
	LONG addSize = 0;
	while ( *szStr )
	{
		int c = *szStr++ & 0xFF;

		if ( c == '{' )
		{
			size_t tCount = 0;
			std::vector<TCHAR> vColor;
			TCHAR *szStrTag = szStr;
			bool bTagFound = true;

			while ( *szStrTag != '}' )
			{
				if ( tCount > 8 )
				{
					bTagFound = false;
					break;
				}

				TCHAR ch = toupper ( *szStrTag );
				bool numeric =	( ch >= '0' && ch <= '9' ) || 
								( ch >= 'A' && ch <= 'F' );

				if ( numeric )
				{
					vColor.push_back ( ch );
				}

				szStrTag++;
				tCount++;
			}

			if ( bTagFound &&
				 vColor.size () >= 8 )
			{
#ifdef _UNICODE 
				swscanf ( vColor.data (), L"%X", &dwCustomColor );
#else
				sscanf_s ( vColor.data (), "%X", &dwCustomColor );
#endif
				dwCustomColor |= ( dwColor >> 24 ) << 24;
				c = *szStrTag++;
				szStr = szStrTag;
				continue;
			}
		}

		if ( c == '\n' ||
			 addSize >= m_fWidth )
		{
			addSize = 0;
			sx = fStartX;
			sy += ( m_fTexCoords [ 0 ] [ 3 ] - m_fTexCoords [ 0 ] [ 1 ] )*m_dwTexHeight;
		}

		FLOAT tx1 = m_fTexCoords [ c - 32 ] [ 0 ];
		FLOAT ty1 = m_fTexCoords [ c - 32 ] [ 1 ];
		FLOAT tx2 = m_fTexCoords [ c - 32 ] [ 2 ];
		FLOAT ty2 = m_fTexCoords [ c - 32 ] [ 3 ];
		FLOAT w = ( tx2 - tx1 ) *  m_dwTexWidth / m_fTextScale;
		FLOAT h = ( ty2 - ty1 ) * m_dwTexHeight / m_fTextScale;

		if ( c != ' ' )
		{
			if ( dwFlags & D3DFONT_COLORTABLE )
				dwColor = dwCustomColor;
	
			if ( dwFlags & D3DFONT_BORDER )
			{
				float _x = sx, _y = sy;
				float addy = 0.5f;
				float fHeight = h, fWidth = w;

				DWORD dwShadow = 0xFF000000;

				_x = sx - addy, _y = sy - addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.9f, 1.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );

				_x = sx + addy, _y = sy + addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.9f, 1.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );

				_x = sx - addy, _y = sy + addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.9f, 1.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );

				_x = sx + addy, _y = sy - addy;
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx1, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y, 0.9f, 1.0f ), dwShadow, tx2, ty1 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x + fWidth, _y + fHeight, 0.9f, 1.0f ), dwShadow, tx2, ty2 );
				*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( _x, _y, 0.9f, 1.0f ), dwShadow, tx1, ty1 );

				dwNumTriangles += 8;
			}

			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty1 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty1 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + w - 0.5f, sy + h - 0.5f, 0.9f, 1.0f ), dwColor, tx2, ty2 );
			*pVertices++ = InitFont2DVertex ( D3DXVECTOR4 ( sx + 0 - 0.5f, sy + 0 - 0.5f, 0.9f, 1.0f ), dwColor, tx1, ty1 );
			dwNumTriangles += 2;

			if ( dwNumTriangles * 3 > ( MAX_NUM_VERTICES - 6 ) )
			{
				//Unlock, render, and relock the vertex buffer
				m_pVB->Unlock ();
				m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );
				pVertices = NULL;
				m_pVB->Lock ( 0, 0, ( void** ) &pVertices, D3DLOCK_DISCARD );
				dwNumTriangles = 0L;
			}
		}

		addSize += w - ( 2 * m_dwSpacing );
		sx += w - ( 2 * m_dwSpacing );
	}

	// Unlock and render the vertex buffer
	m_pVB->Unlock ();
	if ( dwNumTriangles > 0 )
		m_pd3dDevice->DrawPrimitive ( D3DPT_TRIANGLELIST, 0, dwNumTriangles );

	// Restore the modified renderstates
	if ( FAILED ( m_pState->EndState () ) )
		return E_FAIL;

	return S_OK;
}

void CD3DFont::CutString ( int iMaxWidth, D3DSTRING &sString )
{
	SIZE size;
	GetTextExtent ( sString.c_str (), &size );

	while ( size.cx > iMaxWidth )
	{
		if ( sString.empty () )
			break;

		sString.pop_back ();
		GetTextExtent ( sString.c_str (), &size );
	}
}

CD3DRender::CD3DRender ( int numVertices )
{
	m_canRender = false;

	m_pD3Dbuf = NULL;
	m_pVertex = NULL;

	m_color = 0;
	m_tu = 0.0f;
	m_tv = 0.0f;

	m_maxVertex = numVertices;
	m_curVertex = 0;

	m_pd3dDevice = NULL;
	m_pState = NULL;
}

CD3DRender::~CD3DRender ( void )
{
	Invalidate ();
}

HRESULT CD3DRender::Initialize ( IDirect3DDevice9 *pD3Ddevevice )
{
	if ( !m_canRender )
	{
		m_pd3dDevice = pD3Ddevevice;

		if ( !m_pState )
			m_pState = new CD3DStateBlock ();

		if ( m_pState )
			m_pState->Initialize ( pD3Ddevevice );

		if ( FAILED ( m_pd3dDevice->CreateVertexBuffer ( m_maxVertex * sizeof ( LVERTEX ),
			 D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, 0, D3DPOOL_DEFAULT, &m_pD3Dbuf, NULL ) ) )
			return E_FAIL;

		m_canRender = true;
	}

	return S_OK;
}

HRESULT CD3DRender::Invalidate ( void )
{
	SAFE_RELEASE ( m_pD3Dbuf );

	m_pVertex = NULL;
	m_canRender = false;

	if ( m_pState )
		m_pState->Invalidate ();

	return S_OK;
}

HRESULT CD3DRender::BeginRender ( D3DPRIMITIVETYPE primType )
{
	if ( !m_canRender )
		return E_FAIL;

	if ( m_pVertex )
		return E_FAIL;

	if ( FAILED ( m_pD3Dbuf->Lock ( 0, 0, ( void ** ) &m_pVertex, D3DLOCK_DISCARD ) ) )
		return E_FAIL;

	m_primType = primType;

	if ( !m_pState ) 
		return E_FAIL;

	if ( FAILED ( m_pState->BeginState () ) )
		return E_FAIL;

	m_pState->SetRenderStates ();

	return S_OK;
}

HRESULT CD3DRender::EndRender ( void )
{
	int numPrims;

	m_pVertex = NULL;

	if ( !m_pState )
		return E_FAIL;

	if ( !m_canRender )
	{
		m_curVertex = 0;
		return E_FAIL;
	}

	switch ( m_primType )
	{
		case D3DPT_POINTLIST:
			numPrims = m_curVertex;
			break;

		case D3DPT_LINELIST:
			numPrims = m_curVertex / 2;
			break;

		case D3DPT_LINESTRIP:
			numPrims = m_curVertex - 1;
			break;

		case D3DPT_TRIANGLELIST:
			numPrims = m_curVertex / 3;
			break;

		case D3DPT_TRIANGLESTRIP:
		case D3DPT_TRIANGLEFAN:
			numPrims = m_curVertex - 2;
			break;

		default:
			numPrims = 0;
			break;
	}

	m_curVertex = 0;

	if ( numPrims > 0 )
	{
		m_pD3Dbuf->Unlock ();

		m_pd3dDevice->SetFVF ( D3DFVF_PRIMITIVES );
		m_pd3dDevice->SetTexture ( 0, NULL );

		m_pd3dDevice->SetStreamSource ( 0, m_pD3Dbuf, 0, sizeof ( LVERTEX ) );
		m_pd3dDevice->DrawPrimitive ( m_primType, 0, numPrims );
	}

	if ( FAILED ( m_pState->EndState () ) )
		return E_FAIL;

	return S_OK;
}

void CD3DRender::D3DColor ( DWORD color )
{
	m_color = color;
}

void CD3DRender::D3DTexCoord2f ( float u, float v )
{
	m_tu = u;
	m_tv = v;
}

HRESULT CD3DRender::D3DVertex2f ( float x, float y )
{
	if ( m_canRender &&
		 m_pVertex &&
		 ( ++m_curVertex < m_maxVertex ) )
	{
		*m_pVertex++ = Init2DVertex ( x, y, m_color, m_tu, m_tv );
	}
	else
		return E_FAIL;

	return S_OK;
}

void CD3DRender::D3DTriangle ( float fX, float fY, float fSize, float fAngle,
							   D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 4;
	D3DXVECTOR2 vVector [ iVertexSize ];

	vVector [ 0 ].x = fX;
	vVector [ 0 ].y = fY - fSize;
	vVector [ 1 ].x = fX + fSize;
	vVector [ 1 ].y = fY;
	vVector [ 2 ].x = fX - fSize;
	vVector [ 2 ].y = fY;
	vVector [ 3 ].x = fX;
	vVector [ 3 ].y = fY - fSize;

	if( fAngle )
		RotateVerts ( vVector, iVertexSize, fX, fY, fAngle );

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		D3DColor ( d3dColor );

		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		D3DColor ( d3dOutlineColor );

		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}
}

void CD3DRender::D3DCircle ( float fX, float fY, float fScale,
							 D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 100;
	D3DXVECTOR2 vVector [ iVertexSize ];

	float _cos = cos ( 180.f / D3DX_PI );
	float _sin = sin ( 180.f / D3DX_PI );
	float fSize = fScale / 2.f;

	for ( size_t i = 0; i < iVertexSize; i++ )
	{
		vVector [ i ].x = fX + fSize + _cos * ( ( fX - fSize * cos ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fX ) - _sin * ( ( fY - fSize * sin ( D3DX_PI * ( i / ( iVertexSize / 2.f ) ) ) ) - fY );
		vVector [ i ].y = fY + fSize + _sin * ( ( fX - fSize * cos ( D3DX_PI * ( i / ( iVertexSize / 2.0f ) ) ) ) - fX ) + _cos * ( ( fY - fSize * sin ( D3DX_PI * ( i / ( iVertexSize / 2.f ) ) ) ) - fY );
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}
	
	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dOutlineColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}
}

void CD3DRender::D3DBox ( float fX, float fY,
						  float fWidth, float fHeight, float fAngle,
						  D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor, bool bAntAlias )
{
	const int iVertexSize = 6;
	D3DXVECTOR2 vVector [ iVertexSize ];

	vVector [ 0 ].x = fX;
	vVector [ 0 ].y = fY;
	vVector [ 1 ].x = fX + fWidth;
	vVector [ 1 ].y = fY;
	vVector [ 2 ].x = fX + fWidth;
	vVector [ 2 ].y = fY + fHeight;
	vVector [ 3 ].x = fX;
	vVector [ 3 ].y = fY + fHeight;
	vVector [ 4 ].x = fX;
	vVector [ 4 ].y = fY;
	vVector [ 5 ].x = fX;
	vVector [ 5 ].y = fY + fHeight;

	if ( fAngle )
		RotateVerts ( vVector, iVertexSize, fX, fY, fAngle );

	if ( SUCCEEDED ( BeginRender ( D3DPT_TRIANGLEFAN ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}

	if ( SUCCEEDED ( BeginRender ( D3DPT_LINESTRIP ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dOutlineColor );
		for ( size_t i = 0; i < iVertexSize; i++ )
			D3DVertex2f ( vVector [ i ].x, vVector [ i ].y );

		EndRender ();
	}
}

void CD3DRender::D3DLine ( float fStartX, float fStartY, float fEndX, float fEndY,
						   D3DCOLOR d3dColor, bool bAntAlias )
{
	if ( SUCCEEDED ( BeginRender ( D3DPT_LINELIST ) ) )
	{
		m_pd3dDevice->SetRenderState ( D3DRS_MULTISAMPLEANTIALIAS, bAntAlias );
		m_pd3dDevice->SetRenderState ( D3DRS_ANTIALIASEDLINEENABLE, bAntAlias );

		D3DColor ( d3dColor );
		D3DVertex2f ( fStartX, fStartY );
		D3DVertex2f ( fEndX, fEndY );
		EndRender ();
	}
}

//ID3DXSprite	*CD3DTexture::s_pSprite = NULL;
//size_t		CD3DTexture::s_tCount = 0;

CD3DTexture::CD3DTexture ( const TCHAR *szPath ) :
	m_pTexture ( NULL )
{
#ifdef _UNICODE
	m_szPath = new TCHAR [ wcslen ( szPath ) ];
	wcscpy ( m_szPath, szPath );
#else
	m_szPath = new TCHAR [ strlen ( szPath ) ];
	strcpy ( m_szPath, szPath );
#endif
}

CD3DTexture::CD3DTexture ( LPCVOID pSrc, UINT uSrcSize ) :
	m_pTexture ( NULL )
{
	m_pSrc = pSrc;
	m_uSrcSize = uSrcSize;
}

CD3DTexture::~CD3DTexture ( void )
{
	Invalidate ();
}

HRESULT CD3DTexture::Initialize ( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if ( m_pState )
		m_pState->Initialize ( m_pDevice );

	m_pDevice = pd3dDevice;

	HRESULT hr;

	//Create vertex buffer and set as stream source
	hr = m_pDevice->CreateVertexBuffer ( sizeof ( TLVERTEX ) * 4, NULL, D3DFVF_TLVERTEX, D3DPOOL_MANAGED,
										 &m_pVB, NULL );

	if ( FAILED ( hr ) )
		return hr;

	if ( m_szPath )
	{
		D3DCOLOR colorkey = 0xFFFF00FF;
		hr = D3DXCreateTextureFromFileEx ( m_pDevice, m_szPath, 0, 0, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED,
										   D3DX_FILTER_NONE, D3DX_DEFAULT,
										   colorkey, NULL, NULL, &m_pTexture );
		if ( FAILED ( hr ) )
			return hr;
	}
	else if ( m_pSrc && 
			  m_uSrcSize )
	{
		hr = D3DXCreateTextureFromFileInMemory ( m_pDevice, m_pSrc, m_uSrcSize, &m_pTexture );

		if ( FAILED ( hr ) )
			return hr;
	}

	return S_OK;
}

HRESULT CD3DTexture::Invalidate ( void )
{
	SAFE_RELEASE ( m_pVB );
	SAFE_RELEASE ( m_pTexture );

	SAFE_DELETE_ARRAY ( m_szPath );

	if ( m_pState )
		m_pState->Invalidate ();

	return S_OK;
}

void CD3DTexture::Draw ( float fX, float fY, float fScaleX, float fScaleY,
						 float fRotation, D3DCOLOR d3dColor )
{
	if ( !m_pState )
		return;

	if ( FAILED ( m_pState->BeginState () ) )
		return;

	m_pState->SetRenderStates ();

	TLVERTEX* vertices;

	//Lock the vertex buffer
	m_pVB->Lock ( 0, 0, ( void ** ) &vertices, NULL );

	//Setup vertices
	//A -0.5f modifier is applied to vertex coordinates to match texture and screen coords
	//Some drivers may compensate for this automatically, but on others texture alignment errors are introduced
	//More information on this can be found in the Direct3D 9 documentation
	vertices [ 0 ].colour = d3dColor;
	vertices [ 0 ].x = fX - 0.5f;
	vertices [ 0 ].y = fY - 0.5f;
	vertices [ 0 ].z = 0.0f;
	vertices [ 0 ].rhw = 1.0f;
	vertices [ 0 ].u = 0.0f;
	vertices [ 0 ].v = 0.0f;

	vertices [ 1 ].colour = d3dColor;
	vertices [ 1 ].x = fScaleX - 0.5f;
	vertices [ 1 ].y = fY - 0.5f;
	vertices [ 1 ].z = 0.0f;
	vertices [ 1 ].rhw = 1.0f;
	vertices [ 1 ].u = 1.0f;
	vertices [ 1 ].v = 0.0f;

	vertices [ 2 ].colour = d3dColor;
	vertices [ 2 ].x = fScaleX - 0.5f;
	vertices [ 2 ].y = fScaleY - 0.5f;
	vertices [ 2 ].z = 0.0f;
	vertices [ 2 ].rhw = 1.0f;
	vertices [ 2 ].u = 1.0f;
	vertices [ 2 ].v = 1.0f;

	vertices [ 3 ].colour = d3dColor;
	vertices [ 3 ].x = fX - 0.5f;
	vertices [ 3 ].y = fScaleY - 0.5f;
	vertices [ 3 ].z = 0.0f;
	vertices [ 3 ].rhw = 1.0f;
	vertices [ 3 ].u = 0.0f;
	vertices [ 3 ].v = 1.0f;


	//Unlock the vertex buffer
	m_pVB->Unlock ();
	m_pDevice->SetStreamSource ( 0, m_pVB, 0, sizeof ( TLVERTEX ) );

	//Set texture
	m_pDevice->SetFVF ( D3DFVF_TLVERTEX );
	m_pDevice->SetTexture ( 0, m_pTexture );

	//Draw image
	m_pDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, 2 );

	m_pState->EndState ();
}

void CD3DTexture::Draw ( float fX, float fY, D3DCOLOR d3dColor )
{
	D3DSURFACE_DESC surfaceDesc;

	//Get texture dimensions
	m_pTexture->GetLevelDesc ( 0, &surfaceDesc );
	Draw ( fX, fY, surfaceDesc.Width, surfaceDesc.Height, d3dColor );
}

void CD3DTexture::OnLostDevice ()
{
	/*if ( s_pSprite )
		s_pSprite->OnLostDevice ();

	SAFE_RELEASE ( m_pTexture );*/
}

void CD3DTexture::OnResetDevice ()
{
	/*if ( s_pSprite )
		s_pSprite->OnResetDevice ();

	if ( m_szPath )
		CreateTextureFromFile ( m_szPath );
	else
		CreateTextureFromMemory ( m_pSrc, m_uSrcSize );*/
}