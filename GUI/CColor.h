#pragma once

#include <map>
#include "TinyXML\TinyXML.h"

class CColor
{
	int m_iRed, m_iGreen, m_iBlue, m_iAlpha;
public:
	CColor();
	CColor( int iRed, int iGreen, int iBlue, int iAlpha );
	CColor( D3DCOLOR d3dColor );
	CColor( TiXmlElement * pElement );
	~CColor();

	void SetD3DCOLOR( D3DCOLOR d3dColor );
	void SetRed( int iRed );
	void SetGreen( int iGreen );
	void SetBlue( int iBlue );
	void SetAlpha( int iAlpha );

	D3DCOLOR GetD3DCOLOR();
	D3DCOLOR GetD3DXCOLOR();
	int GetRed();
	int GetGreen();
	int GetBlue();
	int GetAlpha();

	CColor operator / ( int iDivisor );
	CColor operator * ( int iMultiplicator );

	CColor operator - ( CColor cSubColor );
	CColor operator + ( CColor cAddColor );
};

struct SElement;

struct SElementState
{
	SElement * pParent;

	CColor * GetColor( SIMPLEGUI_STRING sString );
	CTexture * GetTexture( SIMPLEGUI_STRING sString );
	int GetInt( SIMPLEGUI_STRING sString );

	std::map<SIMPLEGUI_STRING, CColor*> mColors;
	std::map<SIMPLEGUI_STRING, CTexture*> mTextures;
	std::map<SIMPLEGUI_STRING, int> mInts;
};

struct SElement
{
	SIMPLEGUI_STRING sDefaultState;
	std::map<SIMPLEGUI_STRING, SElementState*> m_mStates;
};