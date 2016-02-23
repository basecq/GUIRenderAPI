#pragma once

#include "CGUI.h"

class CMouse
{
public:
	enum E_CURSOR_TYPE
	{
		DEFAULT,
		E_RESIZE,
		S_RESIZE,
		N_RESIZE,
		SE_RESIZE,
		NE_RESIZE,
		MOVE
	};

	CMouse ( CDialog *pDialog );
	~CMouse ( void );

	bool HandleMessage ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void SetPos ( int iX, int iY );
	void SetPos ( CPos cPos );
	CPos GetPos ( void );

	bool InControlArea ( RECT rc );
	bool InControlArea ( CPos pos, int iWidth, int iHeight );
	bool InControlArea ( int iX, int iY, int iWidth, int iHeight );
	bool InControlArea ( CControl *pControl, int iHeight = 0 );

	void Draw ( void );

	int GetLeftButton ( int iState = -1 );
	int GetRightButton ( int iState = -1 );
	int GetMiddleButton ( int iState = -1 );
	int GetWheel ( int iState = -1 );

	void SetLeftButton ( int iState );
	void SetRightButton ( int iState );
	void SetMiddleButton ( int iState );
	void SetWheel ( int iState );

	void SetDragging ( CWindow *pWindow );
	CWindow *GetDragging ( void );

	void SavePos ( void );
	void LoadPos ( void );
	CPos GetSavedPos ( void );

	void SetCursorType ( E_CURSOR_TYPE  eType ) { m_eCursorType = eType; }
	E_CURSOR_TYPE GetCursorType ( void ) { return m_eCursorType; }

	void SetSize ( int );

private:
	CDialog *m_pDialog;
	CD3DTexture *m_pTexture [ 7 ];
	CWindow *m_pWindow;

	E_CURSOR_TYPE m_eCursorType;

	CPos m_Pos, m_SavedPos;
	int m_iSize;

	int m_iLeftButton, m_iRightButton, m_iMiddleButton, m_iWheel;
	CTimer m_tLeftButton, m_tRightButton, m_tMiddleButton;
};