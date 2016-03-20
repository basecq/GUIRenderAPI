#pragma once

#include <stdio.h>
#include <string>
#include "D3D9.h"

class CDialog;

class CFont
{
	CDialog *m_pDialog;

	ID3DXFont * m_pFont;
	CRITICAL_SECTION cs;
public:
	CFont( CDialog *Gui, IDirect3DDevice9 * pDevice, int iHeight, SIMPLEGUI_CHAR * pszFaceName, bool bold = 0, bool italic = 0 );
	~CFont();

	void OnLostDevice();
	void OnResetDevice();

	void DrawString( int iX, int iY, DWORD dwFlags, CColor * pColor, SIMPLEGUI_STRING sString, int iWidth = 0 );

	int GetStringWidth( const SIMPLEGUI_CHAR * pszString );
	int GetStringHeight();

	void SetColor( int iIndex, CColor cColor );
	CColor & GetColor( int iIndex );

	void CutString( int iWidth, SIMPLEGUI_STRING & rString );
};