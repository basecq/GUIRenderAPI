#pragma once

#include "CGUI.h"


class CLogBox : public CControl
{
public:
	enum E_LOGTYPE
	{
		LOG_NONE,
		LOG_DEBUG,
		LOG_INFO,
		LOG_ERROR,
		LOG_WARNING
	};

	CLogBox ( CDialog *pDialog );
	~CLogBox ( void );

	void Draw ( void );

	void AddText ( E_LOGTYPE eType, const SIMPLEGUI_CHAR *szText, ... );
	void RemoveText ( UINT uIndex );

	const SIMPLEGUI_CHAR *GetText ( UINT uIndex );

	void ResetList ( void );
	void SetSortedList ( bool bSort );

	void OnClickLeave ( void );
	bool OnClickEvent ( void );

	void OnFocusIn ( void );
	void OnFocusOut ( void );

	void OnMouseEnter ( void );
	void OnMouseLeave ( void );

	bool CanHaveFocus ( void );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

	void GenerateLogFile ( bool bSet );

private:
	const SIMPLEGUI_CHAR *GetModeType ( E_LOGTYPE eType );

	bool m_bSet;
	CLogFile *m_pLogFile;
	CEntryList *m_pEntryList;
};