#pragma once

#include "CGUI.h"

class CWindow : public CControl
{
public:
	CWindow ( CDialog *pDialog );
	~CWindow ( void );

	void AddControl ( CControl *pControl );
	void RemoveControl ( CControl *pControl );
	void RemoveAllControls ( void );

	void Draw ( void );

	bool ControlMessages ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

	void OnClickLeave ( void );
	bool OnClickEvent ( void );

	void OnFocusIn ( void );
	void OnFocusOut ( void );

	void OnMouseEnter ( void );
	void OnMouseLeave ( void );

	bool CanHaveFocus ( void );

	void OnMouseMove ( CControl *pControl, UINT uMsg );
	void ShowScrollbars ( bool bShow );

	void SetCloseButton ( bool bEnabled );

	void RequestControlFocus ( CControl *pControl );
	CControl *GetFocussedControl ( void );

	void ClearControlFocus ( void );

	CControl *GetNextControl ( CControl *pControl );
	CControl *GetPrevControl ( CControl *pControl );

	CControl *GetControlByText ( const SIMPLEGUI_CHAR *pszText );

	void BringControlToTop ( CControl *pControl );
	int GetTitleBarHeight ( void );

	CControl *GetControlAtArea ( CPos pos );
	bool IsSizing ( void );

	void SetAlwaysOnTop ( bool bEnable );
	bool GetAlwaysOnTop ( void );

	void SetMovable ( bool bEnabled );
	bool GetMovable ( void );

	void SetSizable ( bool bEnabled );
	bool GetSizable ( void );

	void SetMaximized ( bool bMinimize );
	bool GetMaximized ( void );

	void SetSize ( SIZE size );
	void SetSize ( int iWidth, int iHeight );

	SIZE GetRealSize ( void );

	void UpdateScrollbars ( void );
	void ScrollPage ( int nDelta );

private:
	bool m_bDragging;
	bool m_bCloseButtonEnabled;
	bool m_bMovable;
	bool m_bSizable;
	bool m_bOnTop;
	bool m_bMaximized;
	bool m_bShowScrollbar;

	SIZE m_realSize;
	SIZE m_maxControlSize;

	CScrollablePane *m_pScrollbar;

	enum E_WINDOW_AREA
	{
		CLEAR,
		TOP_LEFT, LEFT_BOTTOM, TOP_RIGHT, RIGHT_BOTTOM,
		TOP, LEFT, RIGHT, BOTTOM

	};

	SControlRect *GetWindowRect ( E_WINDOW_AREA eArea );

	SControlRect m_rButton;
	SControlRect m_rTitle;

	SControlRect m_rWindowTop;
	SControlRect m_rWindowLeft;
	SControlRect m_rWindowRight;
	SControlRect m_rWindowBottom;

	SControlRect m_rWindowTopLeft;
	SControlRect m_rWindowLeftBottom;
	SControlRect m_rWindowTopRight;
	SControlRect m_rWindowRightBottom;

	CControl *m_pControlMouseOver;
	CControl *m_pFocussedControl;

	int m_iTitleBarSize;
	int m_nDragX;
	int m_nDragY;

	CPos m_posDif;

	std::vector<CControl*> m_vControls;

	E_WINDOW_AREA m_eWindowArea;
};