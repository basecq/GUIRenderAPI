#pragma once
#include "CGUI.h"

class CTrackBarHorizontal : public CControl
{
public:
	CTrackBarHorizontal ( CDialog *pDialog );

	void Draw ( void);

	int ValueFromPos ( int iX );

	void SetRange ( int nMin, int nMax );
	void SetValue ( int nValue );

	int GetValue () const
	{
		return m_nValue;
	};

	void GetRange ( int& nMin, int& nMax ) const
	{
		nMin = m_nMin; nMax = m_nMax;
	}

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

private:
	int m_nValue;
	int m_nMin;
	int m_nMax;

	int m_nDragOffset;
	int m_nButtonX;

	SControlRect m_rThumb;
};

