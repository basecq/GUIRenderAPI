#pragma once

#include "CGUI.h"

class CListBox : public CControl
{
public:
	CListBox ( CDialog *pDialog );
	~CListBox ( void );

	void Draw ( void );

	void AddItem ( SEntryItem *pEntry );
	void AddItem ( const SIMPLEGUI_CHAR *szText, const SIMPLEGUI_CHAR *szValue = _UI("") );

	void RemoveItem ( SEntryItem *pEntry );
	void RemoveItem ( UINT uIndex );

	void SetSortedList ( bool bSort );

	SEntryItem *GetSelectedItem ( void );

	int GetSelectedIndex ( void )
	{
		return m_iSelected;
	}

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

private:
	CEntryList *m_pEntryList;

	int m_iIndex;
	int m_iSelected;
};