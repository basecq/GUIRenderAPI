#pragma once


#include "Texture.h"

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002
#define D3DFONT_ZENABLE     0x0004

// Font rendering flags
#define D3DFONT_CENTERED_X  0x0001
#define D3DFONT_CENTERED_Y  0x0002
#define D3DFONT_BORDER		0x0010
#define D3DFONT_COLORTABLE	0x0020


// Helpers functions
void RotateVerts ( VECTOR2 *pVector, size_t size, float fX, float fY, float fAngle );
void SetScissor ( IDirect3DDevice9 *pD3dd, int iX, int iY, int iWidth, int iHeight );
void SetScissor ( IDirect3DDevice9 *pD3dd, RECT rect );

void ClipRect ( LPDIRECT3DDEVICE9 device, LPDIRECT3DSURFACE9 surface, LPDIRECT3DSURFACE9 backbuffer, RECT rScr, RECT rDst );
void ClipRect ( LPDIRECT3DDEVICE9 device, LPDIRECT3DSURFACE9 surface, LPDIRECT3DSURFACE9 backbuffer, int x, int y, int width, int height, int destX, int destY, int destWidth, int destHeight );

// Custom vertex
struct FONT2DVERTEX { D3DXVECTOR4 p; DWORD color; FLOAT tu, tv; };
struct LVERTEX { float	x, y, z, rhw; DWORD	color; float tu, tv; };

class CD3DFont;
class CD3DRender;
class CD3DTexture;

class CGraphics
{
public:
	CGraphics ( IDirect3DDevice9 *pDevice );
	~CGraphics ( void );

	void CreateD3DFont ( int ID, const TCHAR *pszFontName, int iFontHeight, DWORD dwCreateFlags = 0 );
	void CreateD3DRender ( int ID, int iNumVertices );
	void CreateD3DTexture ( int ID, const TCHAR *pszPath );

	CD3DFont *GetFontAt ( int ID );
	CD3DRender *GetRendererAt ( int ID );
	CD3DTexture *GetTextureAt ( int ID );

	void RemoveItem ( HANDLE hItem );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

	IDirect3DDevice9 *GetDevice ( void ) { return m_pd3dDevice; }

private:

	struct SItem
	{
		int ID = -1;
		CD3DFont *pFont			= NULL;
		CD3DRender *pRender		= NULL;
		CD3DTexture *pTexture	= NULL;
	};

	std::vector<SItem> m_ItemAddon;
	IDirect3DDevice9 *m_pd3dDevice= NULL;
};

class CD3DStateBlock
{
public:
	CD3DStateBlock ( void );

	void Initialize ( IDirect3DDevice9 *pDevice );
	void Invalidate ( void );

	HRESULT BeginState ( void );
	HRESULT EndState ( void );

	void SetRenderStates ( void );
private:
	IDirect3DStateBlock9* m_pStateBlock;
	IDirect3DDevice9 *m_pd3dDevice;
};

class CD3DRender
{
public:
	CD3DRender ( int numVertices );
	~CD3DRender ( void );

	HRESULT Initialize ( IDirect3DDevice9 *pD3Ddev );
	HRESULT Invalidate ( void );

	HRESULT BeginRender ( D3DPRIMITIVETYPE primType );
	HRESULT EndRender ( void );

	void D3DColor ( DWORD color );

	void D3DTexCoord2f ( float u, float v );
	HRESULT D3DVertex2f ( float x, float y );

	void D3DTriangle ( float fX, float fY, float fSize, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DCircle ( float fX, float fY, float fSize, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DBox ( float fX, float fY, float fWidth, float fHeight, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dOutlineColor = D3DCOLOR_RGBA ( 0, 0, 0, 255 ), bool bAntAlias = false );
	void D3DLine ( float fStartX, float fStartY, float fEndX, float fEndY, D3DCOLOR d3dColor, bool bAntAlias = true );
private:
	D3DPRIMITIVETYPE		m_primType;
	IDirect3DVertexBuffer9	*m_pD3Dbuf;
	LVERTEX					*m_pVertex;

	DWORD					m_color;
	float					m_tu, m_tv;
	int						m_maxVertex;
	int						m_curVertex;

	bool					m_canRender;

	CD3DStateBlock *m_pState;
	IDirect3DDevice9 *m_pd3dDevice;
};


#ifdef _UNICODE
#define D3DSTRING std::wstring
#else
#define D3DSTRING std::string
#endif

//-----------------------------------------------------------------------------
// Name: class CD3DFont
// Desc: Texture-based font class for doing text in a 3D scene.
//-----------------------------------------------------------------------------
class CD3DFont
{
	TCHAR   m_strFontName [ 80 ];            // Font properties
	DWORD   m_dwFontHeight;
	DWORD   m_dwFontFlags;


	LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text
	DWORD   m_dwTexWidth;                 // Texture dimensions
	DWORD   m_dwTexHeight;
	FLOAT   m_fTextScale;
	FLOAT   m_fTexCoords [ 255  ] [ 4 ];
	DWORD   m_dwSpacing;                  // Character pixel spacing per side
								
	CD3DStateBlock *m_pState;
	IDirect3DDevice9 *m_pd3dDevice;
	float m_fWidth;
public:
	// 2D and 3D text drawing functions
	HRESULT Print ( FLOAT x, FLOAT y, DWORD dwColor, const TCHAR* strText, DWORD dwFlags = 0L );

	void CutString ( int iWidth, D3DSTRING &sString );

	// Function to get extent of text
	HRESULT GetTextExtent ( const TCHAR* strText, SIZE* pSize );

	// Initializing and destroying device-dependent objects
	HRESULT Initialize ( LPDIRECT3DDEVICE9 pd3dDevice );
	
	HRESULT Invalidate ( void );

	void SetWidth ( float fWidth ) { m_fWidth = fWidth; };

	LPDIRECT3DDEVICE9 getDevice () const { return m_pd3dDevice; }

	// Constructor / destructor
	CD3DFont ( const TCHAR* strFontName, DWORD dwHeight, DWORD dwFlags = 0L );
	~CD3DFont ();
};
class CD3DTexture
{
public:
	CD3DTexture ( const TCHAR *szPath );
	CD3DTexture ( LPCVOID pSrc, UINT uSrcSize );

	~CD3DTexture ( void );

	HRESULT Initialize ( LPDIRECT3DDEVICE9 pd3dDevice );
	HRESULT Invalidate ( void );

	void Draw ( float fX, float fY, float fScaleX, float fScaleY, float fRotation = 0.f, D3DCOLOR d3dColor = D3DCOLOR_XRGB ( 255, 255, 255 ) );
	void Draw ( float fX, float fY, D3DCOLOR d3dColor = D3DCOLOR_XRGB ( 255, 255, 255 ) );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

private:
	LPDIRECT3DTEXTURE9      m_pTexture;   // The d3d texture for this font
	LPDIRECT3DVERTEXBUFFER9 m_pVB;        // VertexBuffer for rendering text

	//static ID3DXSprite	*s_pSprite;
	//static size_t		s_tCount;

	IDirect3DDevice9	*m_pDevice;
	TCHAR				*m_szPath;

	LPCVOID				m_pSrc;
	UINT				m_uSrcSize;

	CD3DStateBlock *m_pState;
};