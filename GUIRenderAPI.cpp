// GUIAPI.cpp : Defines the exported functions for the DLL application.
//
#include "GUIRenderAPI.h"
#include "GUI\CGUI.h"

// Windows
CWindow * fServBrowser;
CWindow * fChat;
CWindow * fOption;
CWindow * fUserRegister;
CWindow * fUserLogin;
CWindow * fInfo;
CWindow * fEnterNick;
// Chat elements
CLogBox * cc_tChat;
CEditBox * cc_tEnter;
CButton * cc_bEnter;
// Server Browser elemenets
CListView *sbServList, *sbPlayerList;
CButton *sbTab [ 4 ];
CEditBox *sbEnterIP, *sbEnterPort;
CButton *sbConnect, *sbRefresh, *sbAddToFav;
CDropDown *sbFltPing;
CCheckBox *sbFltNotFull, *sbFltNoPassword, *sbFltNotEmpty;
CEditBox *sbFltLocation, *sbFltMode;
// Register elements
CEditBox *urLogin, *urPass, *urConfirm, *urEmail, *urNick;
CLabel *urLoginText, *urPassText, *urConfirmText, *urEmailText, *urNickText, *urInfoText;
CButton *urSendReg, *urCancelReg;
// Login elements
CLabel *upLoginInfo, *upStrLogin, *upStrPass;
CEditBox *upLogin, *upPassword;
CButton *upSendLogin, *upShowRegister;
CCheckBox *upRemeberMe;
// Enter nick
CEditBox * enNick;
CButton * enOK;
CDialog *pGui = NULL;

#include "injector\utility.hpp"
#include "internal\CallbackResetDevice.hpp"
#include "injector\calling.hpp"

static CallbackResetDevice ResetDevice;
static CallbackManager1C<0x53E4FF> DrawHUDFunc;
static CallbackManager1C<0x46937B> InitialiseTheScriptsFunc;
static CallbackManager1C<0x53BC21> TerminateGame;
static CallbackManager1C<0x748CFB> InitGameFunc;
static CallbackManager2C<0x748E09, 0x748E48> ReInitGameFunc;

CRITICAL_SECTION cs_gui;

VOID RegisterFuncs ()
{
	InitGameFunc.RegisterFuncAfter ( Init );
	DrawHUDFunc.RegisterFuncAfter ( Draw );
	ResetDevice.RegisterFuncBefore ( OnLostDevice );
	ResetDevice.RegisterFuncAfter ( OnResetDevice );

	//InitializeCriticalSection ( &cs_gui );
}

WNDPROC gameProc;
POINT mousePoint;
#define MainWndProc( hWnd, message, wParam, lParam ) ( ( int ( __stdcall* )( HWND, UINT, WPARAM, LPARAM ) ) 0x00747EB0 )( hWnd, message, wParam, lParam )

LRESULT DefWndProc ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( pGui )
	{
		//EnterCriticalSection ( &cs_gui );

		pGui->MsgProc ( uMsg, wParam, lParam );

		/*CMouse *pMouse = pGui->GetMouse ();
		if ( pMouse )
		pMouse->HandleMessage ( uMsg, wParam, lParam );

		CKeyboard *pKey = pGui->GetKeyboard ();
		if ( pKey )
		pKey->HandleMessage ( uMsg, wParam, lParam );*/

		//LeaveCriticalSection ( &cs_gui );
	}

	return CallWindowProc ( gameProc, hWnd, uMsg, wParam, lParam );
}


int RandU ( int nMin, int nMax )
{
	return nMin + ( int ) ( ( double ) rand () / ( RAND_MAX + 1 ) * ( nMax - nMin + 1 ) );
}

VOID Init ()
{
	IDirect3DDevice9* pDevice = *( IDirect3DDevice9** ) 0x00C97C28;
	//EnterCriticalSection ( &cs_gui );

	pGui = new CDialog ( pDevice );

	D3DVIEWPORT9 vp;
	pDevice->GetViewport ( &vp );
	DWORD s_iWidth = vp.Width;
	DWORD s_iHeight = vp.Height;

	pGui->LoadFont ( _UI ( "Tahoma" ), 10, false );

	// Create Servers Brouser
	fServBrowser = pGui->AddWindow ( s_iWidth / 2, s_iHeight / 2, 750, 500, _UI ( "SERVER BROWSER" ) );

	/*int ServerWidth [ 6 ] = { 16, 210, 60, 60, 100, 100 };
	sbServList = new CListView ( pGui, 0, 29, ServerWidth, 350, 6, NULL, "SERVER_LIST"));
	sbServList->SetColumnImage ( 0, "FMPGUI/Pass.png", "FMPGUI/noPass.png", "FMPGUI/Pass.png", 16, 16 );

	sbServList->SetTitle ( "P"));
	sbServList->SetTitle ( "Server Name"));
	sbServList->SetTitle ( "Slots"));
	sbServList->SetTitle ( "Ping"));
	sbServList->SetTitle ( "Mode"));
	sbServList->SetTitle ( "Location"));

	int PlayerWidth [ 2 ] = { 128, 76 };
	sbPlayerList = new CListView ( pGui, 546, 49, PlayerWidth, 200, 2, NULL, "PLAYER_LIST"));

	sbPlayerList->SetTitle ( "Player"));
	sbPlayerList->SetTitle ( "Score"));*/

	sbTab [ 0 ] = pGui->AddButton ( fServBrowser, 20, 0, 200, 20, _UI ( "Internet" ) );
	sbTab [ 1 ] = pGui->AddButton ( fServBrowser, 169, 0, 200, 20, _UI ( "LAN" ) );
	sbTab [ 2 ] = pGui->AddButton ( fServBrowser, 318, 0, 200, 20, _UI ( "VIP" ) );
	sbTab [ 3 ] = pGui->AddButton ( fServBrowser, 467, 0, 200, 20, _UI ( "Favourite" ) );

	sbConnect = pGui->AddButton ( fServBrowser, 560, 380, 80, 20, _UI ( "Connect" ) );
	sbRefresh = pGui->AddButton ( fServBrowser, 660, 380, 80, 20, _UI ( "Refresh" ) );
	sbAddToFav = pGui->AddButton ( fServBrowser, 600, 405, 120, 20, _UI ( "Add to favourites" ) );

	sbFltNotFull = pGui->AddCheckBox ( fServBrowser, 150, 410, 100, true, _UI ( "Add to favourites" ) );

	sbFltNotEmpty = pGui->AddCheckBox ( fServBrowser, 480, 385, 100, true, _UI ( "Add to favourites" ) );
	sbFltNoPassword = pGui->AddCheckBox ( fServBrowser, 480, 410, 100, true, _UI ( "Add to favourites" ) );


	auto pScroll1 = pGui->AddScrollBar ( fServBrowser, 300, 200, 18, 200, 0, 100, 10, 1 );
	auto  pRadio2 = pGui->AddRadioButton ( fServBrowser, 0, 160, 220, 120, _UI ( "Refresh" ) );
	auto  pRadios2 = pGui->AddRadioButton ( fServBrowser, 0, 160, 280, 120, _UI ( "Refresh" ) );

	auto  pRadio12 = pGui->AddRadioButton ( fServBrowser, 1, 220, 220, 120, _UI ( "Refresh" ) );
	auto  pRadios12 = pGui->AddRadioButton ( fServBrowser, 1, 220, 280, 1120, _UI ( "Refresh" ) );

	auto pDropa = pGui->AddListBox ( fServBrowser, 200, 300, 400, 200 );

	for ( size_t i = 0; i < 20; i++ )
	{

		TCHAR szText [ MAX_PATH ];
		sprintf ( szText, "text ASD  asd sasd sa sda sd asd sad sa sds asds asd%i", i );
		pDropa->AddItem ( szText );
	}

	auto fServBrowser1 = pGui->AddWindow ( s_iWidth / 2, s_iHeight / 2 + 500, 750, 500, _UI ( "SERVER BROWSER" ) );


	CButton *sbTab2 [ 4 ];
	sbTab2 [ 0 ] = pGui->AddButton ( fServBrowser1, 20, 0, 200, 20, _UI ( "Internet" ) );
	sbTab2 [ 1 ] = pGui->AddButton ( fServBrowser1, 169, 0, 200, 20, _UI ( "LAN" ) );
	sbTab2 [ 2 ] = pGui->AddButton ( fServBrowser1, 318, 0, 200, 20, _UI ( "VIP" ) );
	sbTab2 [ 3 ] = pGui->AddButton ( fServBrowser1, 467, 0, 200, 20, _UI ( "Favourite" ) );

	auto sbConnect2 = pGui->AddButton ( fServBrowser1, 560, 380, 80, 20, _UI ( "Connect" ) );
	auto sbRefresh2 = pGui->AddButton ( fServBrowser1, 660, 380, 80, 20, _UI ( "Refresh" ) );
	auto sbAddToFav2 = pGui->AddButton ( fServBrowser1, 600, 405, 120, 20, _UI ( "Add to favourites" ) );

	sbAddToFav2->SetRelativePosX ( true );
	sbAddToFav2->SetRelativePosY ( true );
	sbAddToFav2->SetRelativeSizeX ( true );

	auto sbFltNotFull2 = pGui->AddCheckBox ( fServBrowser1, 150, 410, 150, true, _UI ( "Add to favourites" ) );

	auto sbFltNotEmpty2 = pGui->AddCheckBox ( fServBrowser1, 480, 385, 40, 0, _UI ( "Add to favourites" ) );
	auto sbFltNoPassword2 = pGui->AddCheckBox ( fServBrowser1, 480, 420, 150, true, _UI ( "Add to favourites" ) );


	auto  pRadio22 = pGui->AddRadioButton ( fServBrowser1, 0, 160, 220, 120, _UI ( "Refresh" ) );
	auto  pRadios22 = pGui->AddRadioButton ( fServBrowser1, 0, 160, 280, 120, _UI ( "Refresh" ) );

	auto  pRadio122 = pGui->AddRadioButton ( fServBrowser1, 1, 220, 220, 115, _UI ( "Refresh" ) );
	auto  pRadios122 = pGui->AddRadioButton ( fServBrowser1, 1, 220, 280, 115, _UI ( "Refresh" ) );

	//	auto ptrack = pGui->AddTrackBarVertical ( fServBrowser1, 300, 200, 20, 400, -10, 1000, 990 );
	auto pScroll = pGui->AddScrollBar ( fServBrowser1, 300, 200, 18, 200, 0, 100, 10, 1 );

	auto pDrop = pGui->AddDropDown ( fServBrowser1, 500, 200, 200, 20, "WWWWWWWWWWWWW" );



	pDrop->SetSelectedItemByIndex ( 229, true );
	auto pTextBox = pGui->AddTextBox ( fServBrowser1, 20, 350, 350, 108 );
	pTextBox->SetRelativeSizeX ( true );
	pTextBox->SetRelativeSizeY ( true );

	struct sds
	{
		TCHAR szText [ MAX_PATH ];
	};	pDrop->SetSortedList ( true );
	sds sd [] = { { "ANDRE" }, { "BBTHIN" }, { "BB" },
	{ "CAT" }, { "CESAR", }, { "COPGRL1" },
	{ "COPGRL2" }, { "CLAUDE" }, { "CROGRL1" },
	{ "CROGRL2" }, { "DWAYNE" },
	{ "EMMET" }, { "FORELLI" }, { "JANITOR" },
	{ "JETHRO" }, { "JIZZY" }, { "HERN" },
	{ "GANGRL1" }, { "GANGRL2" },
	{ "GUNGRL1" }, { "GUNGRL2" },
	{ "KENDL" }, { "MACCER" }, { "MADDOGG" },
	{ "MECGRL1" }, { "MECGRL2" },
	{ "NURGRL1" }, { "NURGRL2" },
	{ "OGLOC" }, { "PAUL" }, { "PULASKI" },
	{ "ROSE" }, { "RYDER" }, { "RYDER2" },
	{ "RYDER3" }, { "SINDACO" }, { "SMOKE" },
	{ "SMOKEV", }, { "SUZIE" }, { "SWEET" },
	{ "TBONE", }, { "TENPEN" }, { "TORINO" },
	{ "TRUTH", }, { "WUZIMU" }, { "ZERO" }, { "SINDACO" }, { "SMOKE" },
	{ "SMOKEV", }, { "SUZIE" }, { "SWEET" },
	{ "TBONE", }, { "TENPEN" }, { "TORINO" },
	{ "TRUTH", }, { "WUZIMU" }, { "ZERO" }, { "CROGRL2" }, { "DWAYNE" },
	{ "EMMET" }, { "FORELLI" }, { "JANITOR" },
	{ "JETHRO" }, { "JIZZY" }, { "HERN" },
	{ "GANGRL1" }, { "GANGRL2" },
	{ "GUNGRL1" }, { "GUNGRL2" },
	{ "KENDL" }, { "MACCER" }, { "MADDOGG" },
	{ "MECGRL1" }, { "MECGRL2" },
	{ "NURGRL1" }, { "NURGRL2" },
	{ "OGLOC" }, { "PAUL" }, { "PULASKI" },
	{ "ROSE" }, { "RYDER" }, { "RYDER2" },
	{ "RYDER3" }, { "SINDACO" }, { "SMOKE" },
	{ "SMOKEV", }, { "SUZIE" }, { "SWEET" },
	{ "TBONE", }, { "TENPEN" }, { "TORINO" },
	{ "TRUTH", }, { "WUZIMU" }, { "RYDER3" }, { "SINDACO" }, { "SMOKE" },
	{ "SMOKEV", }, { "SUZIE" }, { "SWEET" },
	{ "TBONE", } };

	for ( size_t i = 0; i < 100; i++ )
	{
		pDrop->AddItem ( sd [ i ].szText );
	}

	for ( size_t i = 0; i < 111; i++ )
	{
		pTextBox->AddText ( CLogBox::LOG_DEBUG, "%i", i );
	}

	auto pProgressBar = pGui->AddProgressBarHorizontal ( fServBrowser1, 100, 100, 200, 20, 200, 180 );

	auto pScrolls = pGui->AddScrollBarHorizontal ( fServBrowser1, 300, 200, 200, 18, 0, 20, 15, 199 );

	fServBrowser1->SetAlwaysOnTop ( false );

	auto pLabel = pGui->AddLabel ( fServBrowser1, 400, 400, 300, 200, "Great Text asd sChanched" );

	auto list = pGui->AddListView ( fServBrowser1, 100, 100, 500, 200, "as" );

	list->AddColumn ( "chassd ASDSAD ASD ASDSASD SADS", 100 );
	list->AddColumn ( "chassd 1", 100 );
	list->AddColumn ( "chassd 2", 100 );
	for ( size_t i = 0; i < 112; i++ )
	{
		TCHAR szText [ MAX_PATH ];
		sprintf ( szText, "textasd%i", i );
		list->AddColumnItem ( 0, szText );

		sprintf ( szText, "text asd%i 1", i );
		list->AddColumnItem ( 1, szText );

		sprintf ( szText, "text asd%i 2", i );
		list->AddColumnItem ( 2, szText );
	}
	list->MoveColumn ( 2, 0 );
	pGui->SetVisible ( true );
	//LeaveCriticalSection ( &cs_gui );

	gameProc = ( WNDPROC ) SetWindowLong ( *( HWND* ) 0x00C8CF88, GWL_WNDPROC, ( LONG ) DefWndProc );

	*( DWORD* ) 0x6194A0 = 0xC3;
}

VOID Draw ()
{
	//EnterCriticalSection ( &cs_gui );
	if ( pGui )
		pGui->Draw ();
	//LeaveCriticalSection ( &cs_gui );
}

VOID OnLostDevice ()
{
	//EnterCriticalSection ( &cs_gui );
	if ( pGui )
		pGui->OnLostDevice ();
	//LeaveCriticalSection ( &cs_gui );
}

VOID OnResetDevice ()
{
	//EnterCriticalSection ( &cs_gui );
	if ( pGui )
		pGui->OnResetDevice ();
	//LeaveCriticalSection ( &cs_gui );
}