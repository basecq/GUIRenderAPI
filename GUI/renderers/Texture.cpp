/*************************************************************************************************
 * TEXTURE.CPP                                                 Copyright (c) 2003 Eamonn Doherty *
 *                                                                                               *
 * CTexture class implementation                                                                 *
 *************************************************************************************************/

#include "Texture.h"

//********************************
//* Static Member Initialization *
//********************************

list <CCTexture::LOADEDTEXTURE*> CCTexture::loadedTextures;

//********************************
//* Class Member Functions       *
//********************************

//********************************
//* Global Variables             *
//********************************

//Direct3D 9 interface
IDirect3D9* d3d;

//Graphics adapter interface
IDirect3DDevice9* d3dDevice;

//Capabilities of graphics adapter
D3DCAPS9 d3dCaps;

//Direct3D present parameters
D3DPRESENT_PARAMETERS d3dPresent;

//Vertex buffer for drawing quads
IDirect3DVertexBuffer9* vertexBuffer;


//********************************
//* Functions                    *
//********************************

//Initialize Direct3D
int InitD3DA ( IDirect3DDevice9* dDevice )
{
	d3dDevice = dDevice;




	//Create vertex buffer and set as stream source
	d3dDevice->CreateVertexBuffer ( sizeof ( TLVERTEX ) * 4, NULL, D3DFVF_TLVERTEX, D3DPOOL_MANAGED,
									&vertexBuffer, NULL );


	//Successfully initalized Direct3D
	return TRUE;
}


//Load a texture
//Supported formats: BMP, PPM, DDS, JPG, PNG, TGA, DIB
IDirect3DTexture9 *LoadTexture ( char *fileName )
{
	IDirect3DTexture9 *d3dTexture;
	D3DXIMAGE_INFO SrcInfo;			//Optional

									//Use a magenta colourkey
	D3DCOLOR colorkey = 0xFFFF00FF;

	// Load image from file
	if ( FAILED ( D3DXCreateTextureFromFileEx ( d3dDevice, fileName, 0, 0, 1, 0,
		 D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_FILTER_NONE, D3DX_DEFAULT,
		 colorkey, &SrcInfo, NULL, &d3dTexture ) ) )
	{
		return NULL;
	}

	//Return the newly made texture
	return d3dTexture;
}


//Draw a textured quad on the back-buffer
void BlitD3D ( IDirect3DTexture9 *texture, RECT *rDest, D3DCOLOR vertexColour, float rotate )
{
	TLVERTEX* vertices;

	//Lock the vertex buffer
	vertexBuffer->Lock ( 0, 0, ( void ** ) &vertices, NULL );

	//Setup vertices
	//A -0.5f modifier is applied to vertex coordinates to match texture and screen coords
	//Some drivers may compensate for this automatically, but on others texture alignment errors are introduced
	//More information on this can be found in the Direct3D 9 documentation
	vertices [ 0 ].colour = vertexColour;
	vertices [ 0 ].x = ( float ) rDest->left - 0.5f;
	vertices [ 0 ].y = ( float ) rDest->top - 0.5f;
	vertices [ 0 ].z = 0.0f;
	vertices [ 0 ].rhw = 1.0f;
	vertices [ 0 ].u = 0.0f;
	vertices [ 0 ].v = 0.0f;

	vertices [ 1 ].colour = vertexColour;
	vertices [ 1 ].x = ( float ) rDest->right - 0.5f;
	vertices [ 1 ].y = ( float ) rDest->top - 0.5f;
	vertices [ 1 ].z = 0.0f;
	vertices [ 1 ].rhw = 1.0f;
	vertices [ 1 ].u = 1.0f;
	vertices [ 1 ].v = 0.0f;

	vertices [ 2 ].colour = vertexColour;
	vertices [ 2 ].x = ( float ) rDest->right - 0.5f;
	vertices [ 2 ].y = ( float ) rDest->bottom - 0.5f;
	vertices [ 2 ].z = 0.0f;
	vertices [ 2 ].rhw = 1.0f;
	vertices [ 2 ].u = 1.0f;
	vertices [ 2 ].v = 1.0f;

	vertices [ 3 ].colour = vertexColour;
	vertices [ 3 ].x = ( float ) rDest->left - 0.5f;
	vertices [ 3 ].y = ( float ) rDest->bottom - 0.5f;
	vertices [ 3 ].z = 0.0f;
	vertices [ 3 ].rhw = 1.0f;
	vertices [ 3 ].u = 0.0f;
	vertices [ 3 ].v = 1.0f;

	

	//Unlock the vertex buffer
	vertexBuffer->Unlock ();
	d3dDevice->SetStreamSource ( 0, vertexBuffer, 0, sizeof ( TLVERTEX ) );

	//Set vertex shader
	d3dDevice->SetVertexShader ( NULL );

	//Setup rendering states
	

	d3dDevice->SetTextureStageState ( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	d3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	d3dDevice->SetTextureStageState ( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );

	d3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAOP, D3DTOP_MODULATE );
	d3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	d3dDevice->SetTextureStageState ( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );

	d3dDevice->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	//Set texture
	d3dDevice->SetFVF ( D3DFVF_TLVERTEX );
	d3dDevice->SetTexture ( 0, texture );

	//Draw image
	d3dDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, 2 );
}


//Draw a textured quad on the back-buffer
void BlitExD3D ( IDirect3DTexture9 *texture, RECT *rDest, D3DCOLOR *vertexColours /* -> D3DCOLOR[4] */,
				 float rotate )
{
	TLVERTEX* vertices;

	//Lock the vertex buffer
	vertexBuffer->Lock ( 0, 0, ( void ** ) &vertices, NULL );

	//Setup vertices
	//A -0.5f modifier is applied to vertex coordinates to match texture and screen coords
	//Some drivers may compensate for this automatically, but on others texture alignment errors are introduced
	//More information on this can be found in the Direct3D 9 documentation
	vertices [ 0 ].colour = vertexColours [ 0 ];
	vertices [ 0 ].x = ( float ) rDest->left - 0.5f;
	vertices [ 0 ].y = ( float ) rDest->top - 0.5f;
	vertices [ 0 ].z = 0.0f;
	vertices [ 0 ].rhw = 1.0f;
	vertices [ 0 ].u = 0.0f;
	vertices [ 0 ].v = 0.0f;

	vertices [ 1 ].colour = vertexColours [ 1 ];
	vertices [ 1 ].x = ( float ) rDest->right - 0.5f;
	vertices [ 1 ].y = ( float ) rDest->top - 0.5f;
	vertices [ 1 ].z = 0.0f;
	vertices [ 1 ].rhw = 1.0f;
	vertices [ 1 ].u = 1.0f;
	vertices [ 1 ].v = 0.0f;

	vertices [ 2 ].colour = vertexColours [ 2 ];
	vertices [ 2 ].x = ( float ) rDest->right - 0.5f;
	vertices [ 2 ].y = ( float ) rDest->bottom - 0.5f;
	vertices [ 2 ].z = 0.0f;
	vertices [ 2 ].rhw = 1.0f;
	vertices [ 2 ].u = 1.0f;
	vertices [ 2 ].v = 1.0f;

	vertices [ 3 ].colour = vertexColours [ 3 ];
	vertices [ 3 ].x = ( float ) rDest->left - 0.5f;
	vertices [ 3 ].y = ( float ) rDest->bottom - 0.5f;
	vertices [ 3 ].z = 0.0f;
	vertices [ 3 ].rhw = 1.0f;
	vertices [ 3 ].u = 0.0f;
	vertices [ 3 ].v = 1.0f;

	
	//Unlock the vertex buffer
	vertexBuffer->Unlock ();

	//Set texture
	d3dDevice->SetTexture ( 0, texture );

	//Draw image
	d3dDevice->DrawPrimitive ( D3DPT_TRIANGLEFAN, 0, 2 );
}
//Load texture from file
int CCTexture::Init ( IDirect3DDevice9* dDevice,string sFilename)
{
    D3DSURFACE_DESC surfaceDesc;
    LOADEDTEXTURE* newTexture;
    list<LOADEDTEXTURE*>::iterator itTextures;

    //Make sure texture is not already loaded
    if (bLoaded)
		return FALSE;
	InitD3DA ( dDevice );
    //Convert filename to lowercase letters
    sFilename = _strlwr((char *)sFilename.c_str ());

    //Check if texture is in the loaded list
    for (itTextures = loadedTextures.begin (); itTextures != loadedTextures.end (); itTextures++)
        if ((*itTextures)->sFilename == sFilename)
        {   
            //Get LOADEDTEXTURE object
            texture = *itTextures;  

            //Increment reference counter
            (*itTextures)->referenceCount++;

            //Set loaded flag
            bLoaded = TRUE;

            //Successfully found texture
            return TRUE;
        }

        //Texture was not in the list, make a new texture
        newTexture = new LOADEDTEXTURE;

        //Load texture from file
        newTexture->texture = LoadTexture ((char*)sFilename.c_str());

        //Make sure texture was loaded
        if (!newTexture->texture)
            return FALSE;

        //Get texture dimensions
        newTexture->texture->GetLevelDesc(0, &surfaceDesc);

        //Set new texture parameters
        newTexture->referenceCount = 1;
        newTexture->sFilename = sFilename;
        newTexture->width = surfaceDesc.Width;
        newTexture->height = surfaceDesc.Height;

        //Push new texture onto list
        loadedTextures.push_back (newTexture);

        //Setup current texture instance
        texture = loadedTextures.back();
        bLoaded = TRUE;

        //Successfully loaded texture
        return TRUE;
}


//Unload a texture
int CCTexture::Close()
{
    //Make sure texture is loaded
    if (!bLoaded)
        return FALSE;
    
    //Decrement reference counter and nullify pointer
    texture->referenceCount--;
    texture = NULL;

    //Clear loaded flag
    bLoaded = FALSE;

    //Successfully unloaded texture
    return TRUE;
}


//Release all unreferenced textures
//Release all unreferenced textures
int CCTexture::GarbageCollect()
{
    list<LOADEDTEXTURE*>::iterator it;
    list<LOADEDTEXTURE*>::iterator itNext;

    //Go through loaded texture list
    for (it = loadedTextures.begin(); it != loadedTextures.end ();)   
        if ((*it)->referenceCount <= 0)
        {
            //Get next iterator
            itNext = it;
            itNext++;

            //Release texture
            if ((*it)->texture)
                (*it)->texture->Release();
            (*it)->texture = NULL;

            //Delete LOADEDTEXTURE object
            delete (*it);
            loadedTextures.erase (it);

            //Move to next element
            it = itNext;
        } else it++; //Increment iterator

    //Successfully released unreferenced textures
    return TRUE;
}


//Release all textures
int CCTexture::CleanupTextures()
{
    list<LOADEDTEXTURE*>::iterator it;
 
     //Go through loaded texture list
    for (it = loadedTextures.begin(); it != loadedTextures.end (); it++)
    {
        //Release texture
        if ((*it)->texture)
            (*it)->texture->Release();
        (*it)->texture = NULL;
        
         //Delete LOADEDTEXTURE object
        delete (*it);
    }

    //Clear list
    loadedTextures.clear ();

    //Successfully released all textures
    return TRUE;
}


//Draw texture with limited colour modulation
void CCTexture::Blit (int X, int Y, D3DCOLOR vertexColour, float rotate)
{
    RECT rDest;

    //Setup destination rectangle
    rDest.left = X;
    rDest.right = X + texture->width;
    rDest.top = Y;
    rDest.bottom = Y + texture->height;

    //Draw texture
    BlitD3D (texture->texture, &rDest, vertexColour, rotate);
}


//Draw texture with full colour modulation
void CCTexture::BlitEx (int X, int Y, D3DCOLOR* vertexColours, float rotate)
{
    RECT rDest;

    //Setup destination rectangle
    rDest.left = X;
    rDest.right = X + texture->width;
    rDest.top = Y;
    rDest.bottom = Y + texture->height;

    //Draw texture
    BlitExD3D (texture->texture, &rDest, vertexColours, rotate);
}
