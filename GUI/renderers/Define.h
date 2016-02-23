/* 
	Author				: Bill Kim (ibillkim@yahoo.com)	
	Release Date		: 2008. 02. 20.
	File Name			: Define
	Version				: 2.01.03

	Test PC				: CPU - Sonoma 1.86, RAM - 1 GB Graphic - GMA 900
	Test OS				: Windows XP Home Edition + SP2
	Test Application	: Visual Studio 2003, 2005 + DirectX SDK 9.0c(Nov, 2007)
	
	Contents

	Define Header

	2008 �� Copyright MIS Corporation. All Rights Reserved.
*/ 

#pragma once

// Windows Header Files

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
//#include <afx.h>
#include <tchar.h>
#include <assert.h>
// STL Header Files

#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <deque>
#include <algorithm>
#include <Windowsx.h>
#include <limits>
#include <iomanip>
#include <sstream>
using namespace std;

// D3D Header Files
#include <d3d9.h>
#include <d3dx9math.h>
#include <d3d9types.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

#define D3DX_PI						((FLOAT)  3.141592654f)

#define SAFE_DELETE(p)				{ if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p)		{ if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)				{ if(p) { (p)->Release(); (p)=NULL; } }

#define SAFE_LOSTDEVICE(p)			{ if(p) (p)->OnLostDevice(); }
#define SAFE_RESETDEVICE(p)			{ if(p) (p)->OnResetDevice(); }

#define SAFE_INITIALIZE(p, device)	{ if(p) (p)->Initialize(device); }
#define SAFE_INVALIDATE(p)			{ if(p) (p)->Invalidate(); }

typedef D3DXFLOAT16					FLOAT16;
typedef D3DXFLOAT16					*PFLOAT16, *LPFLOAT16;

typedef D3DXVECTOR2					VECTOR2;
typedef D3DXVECTOR2  				*PVECTOR2, *LPVECTOR2;

typedef D3DXVECTOR3					VECTOR, VECTOR3;
typedef D3DXVECTOR3					*PVECTOR, *LPVECTOR, *PVECTOR3, *LPVECTOR3; 

typedef D3DXVECTOR4					VECTOR4;
typedef D3DXVECTOR4					*PVECTOR4, *LPVECTOR4;

typedef D3DXMATRIX					MATRIX;
typedef D3DXMATRIX					*PMATRIX, *LPMATRIX;

typedef D3DXPLANE					PLANE;
typedef D3DXPLANE					*PPLANE, *LPPLANE;

typedef D3DXQUATERNION				QUATERNION;
typedef D3DXQUATERNION				*PQUATERNION, *LPQUATERNION;

typedef D3DXCOLOR					COLOR;
typedef D3DXCOLOR					*PCOLOR, *LPCOLOR;

typedef D3DCOLORVALUE				COLORVALUE;
typedef D3DCOLORVALUE				*PCOLORVALUE, *LPCOLORVALUE;

typedef D3DRECT						DRECT;
typedef D3DRECT						*PDRECT, LPDRECT;

typedef D3DVIEWPORT9				VIEWPORT;
typedef D3DVIEWPORT9				*PVIEWPORT, *LPVIEWPORT;


