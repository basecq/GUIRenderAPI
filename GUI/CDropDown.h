#pragma once

#include "CGUI.h"

class CDropDown : public CControl
{
public:
	CDropDown ( CDialog *pDialog );
	~CDropDown ( void );

	void Draw ( void );

	void AddItem ( SEntryItem *pEntry );
	void AddItem ( const SIMPLEGUI_CHAR *szText, const SIMPLEGUI_CHAR *szValue = _UI ( "" ) );

	void RemoveItem ( SEntryItem *pEntry );
	void RemoveItem ( UINT uIndex );

	SEntryItem *GetSelectedItem ( void );
	int GetSelectedIndex ( void ) { return m_iSelected; }

	void SetSelectedItem ( SEntryItem *pEntry, bool bSelect );
	void SetSelectedItemByIndex ( UINT uIndex, bool bSelect );

	void OnClickLeave ( void );
	bool OnClickEvent ( void );

	void OnFocusIn ( void );
	void OnFocusOut ( void );

	void OnMouseEnter ( void );
	void OnMouseLeave ( void );

	bool CanHaveFocus ( void );

	void SetSortedList ( bool bSort );

	void SetPageSize ( UINT uSize )
	{
		m_uSize = uSize;
	}

	UINT GetPageSize ( void )
	{
		return m_uSize;
	}

	void OpenBox ( void );
	void CloseBox ( void );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );
	bool HandleKeyboard ( UINT uMsg, WPARAM wParam, LPARAM lParam );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

private:
	CEntryList *m_pEntryList;

	bool m_bOpened;
	bool m_bKeyUp;
	bool m_bDropped;
	
	UINT m_uSize;
	int m_iIndex;
	int m_iSelected;

	SControlRect m_rBack;
};