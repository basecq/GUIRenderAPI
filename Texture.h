/*************************************************************************************************
 * TEXTURE.H                                                   Copyright (c) 2003 Eamonn Doherty *
 *                                                                                               *
 * CTexture class definition                                                                     *
 *************************************************************************************************/

#pragma once

#include <windows.h>
#include <assert.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <string>
#include <sstream>
#include <list>
using namespace std;

#include "Graphics.h"

//********************************
//* Class Prototype              *
//********************************

class CCTexture
{
public:

    //Set default member values
    CCTexture()
    {
        bLoaded = FALSE;
        texture = NULL;
    }

    //Load texture from file
    int CCTexture::Init ( IDirect3DDevice9* dDevice,string sFilename);

    //Unload a texture
    int CCTexture::Close();

    //Draw texture with limited colour modulation
    void CCTexture::Blit (int X, int Y, D3DCOLOR vertexColour = D3DCOLOR_RGBA(255,255,255,255), float rotate = 0);

    //Draw texture with full colour modulation
    void CCTexture::BlitEx (int X, int Y, D3DCOLOR* vertexColours, float rotate);

    //Release all unreferenced textures
    static int GarbageCollect();

    //Release all unreferenced textures
    static int CleanupTextures();

private:

    //Loaded texture struct
    struct LOADEDTEXTURE
    {
        int referenceCount;             //Amount of CTexture instances containing this texture
        IDirect3DTexture9* texture;     //The texture
        string sFilename;               //The filename of the texture
        int width;                      //Width of the texture
        int height;                     //Height of the texture
    };

    //Linked list of all loaded textures
    static list <LOADEDTEXTURE*> loadedTextures;

    BOOL bLoaded;           //Texture loaded flag
    LOADEDTEXTURE* texture; //The texture



};