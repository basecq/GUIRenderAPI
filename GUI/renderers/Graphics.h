/*************************************************************************************************
 * GRAPHICS.H                                                  Copyright (c) 2003 Eamonn Doherty *
 *                                                                                               *
 * Direct3D code                                                                                 *
 *************************************************************************************************/

/*
 *	The code presented here is composed of a series of functions and global variables rather than
 *  as a class. This is to keep the project simple. I highly recommend creating a class to store
 *  the graphics code in for your own project
 */

#pragma once

#include "Define.h"
//Custom vertex format
const DWORD D3DFVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

/* Vertices:

    v0               v1
    |---------------|
    |\              |
    |  \            |
    |    \          |
    |      \        |
    |        \      |
    |          \    |
    |            \  |
    |              \|
    |---------------|
    v3               v2

    z = 0
    rhw = 1
*/

//Custom vertex
struct TLVERTEX
{
    float x;
    float y;
    float z;
    float rhw;
    D3DCOLOR colour;
    float u;
    float v;
};


