#pragma once

#include "CGUI.h"

class CEditBox : public CControl
{
	int m_iStart, m_iIndex;
	int m_iCursorX;
	CTimer m_tCursorTimer;
	bool m_bCursorState;

	CColor * pString, *pCursor;
	CTexture *pEdit [ 3 ];

	int SizeEdge, iPadding;
	bool m_bCenterAlign;
	bool m_bHideContent;
public:
	CEditBox ( CDialog *pDialog );
	~CEditBox ();

	void Draw ();

	void PreDraw ();
	void OnMouseEvent ( CMouse *pMouse );
	bool OnMouseMove ( CMouse *pMouse, bool );
	void MsgProc ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	int GetIndexByEntry ();
	void SetIndex ( int iIndex );

	int GetStart ();
	void SetStart ( int iStart );

	bool GetTextAlign ();
	void SetTextAlign ( bool Center );
	void HideContent ( bool hide );

	void UpdateTheme ( int iIndex );
};