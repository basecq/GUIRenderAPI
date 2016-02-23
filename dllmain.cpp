// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "GUIAPI.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	if ( ul_reason_for_call == DLL_PROCESS_ATTACH )
		RegisterFuncs ();

	return TRUE;
}

