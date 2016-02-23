#pragma once

#include "CGUI.h"

struct SEntryItem
{
	SIMPLEGUI_STRING m_sText;
	SIMPLEGUI_STRING m_sValue;

	SEntryItem ( void ) {};
	SEntryItem ( SIMPLEGUI_STRING sText, SIMPLEGUI_STRING sValue = _UI ( "" ) )
	{
		m_sText = sText;
		m_sValue = sValue;
	}
};

class CEntryList
{
public:
	CEntryList ( CDialog *pDialog );
	~CEntryList ( void );

	void SetFont ( CD3DFont *pFont );

	void AddEntry (  SEntryItem *pEntry );
	void RemoveEntry (  SEntryItem *pEntry );

	int GetIndexByEntry (  SEntryItem *pEntry );
	SEntryItem *GetEntryByIndex ( int nIndex );

	SIZE GetTextSize ( void );

	void Render ( SControlRect rRect, D3DCOLOR d3dColorSelected, D3DCOLOR d3dColorNormal, D3DCOLOR d3dColorSelectedFont, D3DCOLOR d3dColorFont, UINT uIndex );
	
	bool IsEntryInList (  SEntryItem *pEntry );
	void InsertItem (  SEntryItem *pEntry,  SEntryItem *pEntryPos );

	void SetSortedList ( bool bSort );
	void ResetList ( void );

	size_t GetSize ( void );

	SEntryItem *GetPrevItem ( SEntryItem *pEntry );
	SEntryItem *GetNextItem ( SEntryItem *pEntry );

	void SetSelectedEntryByIndex ( int nIndex, bool bSelect );
	void SetSelectedEntry ( SEntryItem *pEntry, bool bSelect );

	bool IsEntrySelectedByIndex ( int nIndex );
	bool IsEntrySelected (  SEntryItem *pEntry );
	SEntryItem *GetSelectedEntry ( void );
	int GetSelectedEntryIndex ( void );

	void UpdateScrollbars ( SControlRect rRect );
	bool ContainsRects ( SControlRect rRect, CPos pos );

	CScrollablePane *GetScrollbar ( void );
private:
	bool m_bSort;
	SEntryItem* m_pSelectedEntry;

	std::vector<SEntryItem*> m_vEntryList;
	std::map<SEntryItem*, bool> m_mSelectedItem;

	SIZE m_TextSize;

	CD3DFont *m_pFont;
	CDialog *m_pDialog;

	CScrollablePane *m_pScrollbar;
};

