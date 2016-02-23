#pragma once

#include "CGUI.h"

class CLabel : public CControl
{
public:
	CLabel ( CDialog *pDialog );
	~CLabel ( void );

	void SetAlign ( DWORD dwAlign );
	void Draw ( void );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );

	void SetHeight ( int nHeight );
	void SetWidth ( int nWidth );

	void SetSize ( SIZE size );
	void SetSize ( int nWidth, int nHeight );

private:
	DWORD m_dwAlign;
	int m_nSizeX;
	int m_nSizeY;
};