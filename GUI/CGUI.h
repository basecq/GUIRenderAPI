#pragma once

class CTexture;
class CTimer;
class CPos;
class CColor;
class CMouse;
class CKeyboard;
class CControl;
class CWindow;
class CButton;
class CCheckBox;
class CProgressBarHorizontal;
class CLogBox;
class CListBox;
class CListView;
class CDropDown;
class CScrollBarVertical;
class CRadioButton;
class CBox;
class CPictureBox;
class CTrackBarHorizontal;
class CScrollBarHorizontal;
class CTrackBarVertical;
class CEntryList;
class CScrollablePane;

#include "renderers\CGraphics.h"

#include "LogFile.h"
#include "CTexture.h"
#include "CFont.h"

#include "CVar.h"
#include "CTimer.h"

#include "CPos.h"

#include "CMouse.h"
#include "CKeyboard.h"
#include "CElement.h"
#include "Entry.h"
#include "CFigure.h"
#include "Entry.h"
#include "ScrollbarHorizontal.h"
#include "CTrackBar.h"
#include "CText.h"
#include "CWindow.h"
#include "CScrollBar.h"
#include "CButton.h"
#include "CRadioButton.h"
#include "CCheckBox.h"
#include "CEditBox.h"
#include "CDropDown.h"
#include "CTextBox.h"
#include "CListBox.h"
#include "CListView.h"
#include "CProgressBar.h"
#include "ProgressBarVertical.h"
#include "ScrollablePane.h"
#include "TrackBar.h"


struct SD3DItem
{
	CD3DFont *pFont = NULL;
	CD3DTexture *pTexture = NULL;
};

class CDialogManager
{
public:

	void InitializeDeviceObjs ( IDirect3DDevice9 *pDevice )
	{
	
	}

	void PreReset ( void );
	void PostReset ( void );
		

private:
	std::vector<SD3DItem> m_vD3DItem;
};

class CDialog
{
public:
	CDialog ( IDirect3DDevice9 *pDevice );
	~CDialog ();

	CProgressBarHorizontal *AddProgressBarHorizontal ( CWindow *pWindow, int X, int Y, int Width, int iHeight, float fMax, float fValue, tAction Callback = NULL );
	CProgressBarVertical *AddProgressBarVertical ( CWindow *pWindow, int X, int Y, int Width, int iHeight, float fMax, float fValue, tAction Callback = NULL );
	
	CWindow* AddWindow ( int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CButton* AddButton ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CCheckBox* AddCheckBox ( CWindow *pWindow, int X, int Y, int Width, bool bChecked, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CLogBox* AddTextBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback = NULL );
	CListBox* AddListBox ( CWindow *pWindow, int X, int Y, int Width, int Height, tAction Callback = NULL );
	CListView* AddListView ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CLabel* AddLabel ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CEditBox* AddEditBox ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CDropDown* AddDropDown ( CWindow *pWindow, int X, int Y, int Width, int Height, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CRadioButton *AddRadioButton ( CWindow *pWindow, int iGroup, int X, int Y, int Width, const SIMPLEGUI_CHAR *szString = NULL, tAction Callback = NULL );
	CPictureBox *AddImage ( CWindow *pWindow, const TCHAR *szPath, int X, int Y, int Width, int Height, tAction Callback = NULL );
	
	CTrackBarHorizontal *AddTrackBar ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback = NULL );
	CTrackBarVertical *AddTrackBarVertical ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nValue, tAction Callback = NULL );

	CScrollBarVertical *AddScrollBar ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback = NULL );
	CScrollBarHorizontal *AddScrollBarHorizontal ( CWindow *pWindow, int X, int Y, int Width, int Height, int nMin, int nMax, int nPagSize, int nValue, tAction Callback = NULL );

	void LoadTexture ( const SIMPLEGUI_CHAR *szPath, CD3DTexture **ppCreated = NULL );
	void LoadTexture ( LPCVOID pSrc, UINT uSrcSize, CD3DTexture **ppCreated = NULL );
	void RemoveTexture ( CD3DTexture *pTexture );

	void LoadFont ( const SIMPLEGUI_CHAR *szFontName, DWORD dwHeight = 15, bool bBold = false, CD3DFont **ppCreated = NULL );

	void DrawFont ( SControlRect &rect, DWORD dwColor, const SIMPLEGUI_CHAR* szText, DWORD dwFlags, CD3DFont *pFont = NULL );
	void DrawBox ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );
	void DrawTriangle ( SControlRect &rect, float fAngle, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );
	void DrawCircle ( SControlRect &rect, D3DCOLOR d3dColor, D3DCOLOR d3dColorOutline, bool bAntAlias = true );

	void SetFocussedWindow ( CWindow *pWindow );
	void ClearFocussedWindow ( void );
	CWindow *GetFocussedWindow ( void );
	void BringWindowToTop ( CWindow *pWindow );

	CWindow *GetWindowByText ( const SIMPLEGUI_CHAR *pszText );
	CWindow *GetWindowAtPos ( CPos pos );

	void AddWindow ( CWindow *pWindow );
	void RemoveWindow ( CWindow *pWindow );
	void RemoveAllWindows ( void );

	void Draw ( void );

	void OnLostDevice ( void );
	void OnResetDevice ( void );

	void MsgProc ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	IDirect3DDevice9 *GetDevice ( void );

	CMouse *GetMouse ( void );
	CD3DRender *GetRenderer ( void );
	CD3DFont *GetFont ( int ID = 0 );

	void SetVisible ( bool bVisible );
	bool IsVisible ( void );

private:
	bool m_bVisible;

	CMouse *m_pMouse;

	CD3DRender *m_pRender;
	IDirect3DDevice9 *m_pDevice;

	std::vector<CD3DFont*>m_vFont;
	std::vector<CD3DTexture*>m_vTexture;
	std::vector<CWindow*> m_vWindows;

	CWindow *m_pFocussedWindow;
	CWindow *m_pMouseOverWindow;

	CRITICAL_SECTION cs;
};