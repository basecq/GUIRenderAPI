#pragma once

#include "CGUI.h"

struct ImageColumn
{
	CTexture * tTrue;
	CTexture * tFalse;
	CTexture * tTitle;

	int Width, Height;
};

class CListView : public CControl
{
public:
	CListView( CDialog *pDialog );
	~CListView ( void );

	void AddColumn ( const SIMPLEGUI_CHAR *szColumnName, int nWidth );
	void RemoveColumn ( UINT nColumnId );
	void RemoveAllColumns ( void );

	void SetColumnName ( UINT nColumnId, const SIMPLEGUI_CHAR *szColumnName );
	const char *GetColumnName ( UINT nColumnId );

	const char *GetColumnItemNameByRow ( UINT nColumnId, UINT nRow );

	void SetColumnWidth ( UINT nColumnId, int nWidth );
	int GetColumnWidth ( UINT nColumnId );

	void AddColumnItem ( UINT nColumnId, const SIMPLEGUI_CHAR *szItem );
	void RemoveColumnItem ( UINT nColumnId, UINT nIndex );

	void RemoveAllItemsFromColumn ( UINT nColumnId );
	void RemoveAllItems ( void );

	size_t GetNumOfColumns ( void );
	size_t GetNumOfItemsFromColumn ( UINT nColumnId );
	size_t GetNumOfItems ( void );

	int GetAllColumnsWidth ( void );

	const char *GetSelectedItem ( UINT nColumnId );

	int GetNextColumn ( UINT nColumnId );
	int GetPrevColumn ( UINT nColumnId );

	int GetGreatColumn ( void );

	int GetColumnIdAtArea ( CPos pos );
	int GetColumnIdAtAreaBorder ( CPos pos );
	int GetColumnOffset ( UINT nColumnId );

	void SetTitleSizable ( bool bSizable );
	void SetTitleMovable ( bool bMovable );

	const char* FindItemInRow ( UINT nRow );

	void Draw ( void );

	void MoveColumn ( UINT nColumnId, UINT nPosition );

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
	CD3DFont *m_pTitleFont;

	struct SListViewColumn
	{
		std::vector<std::string> m_sItem;
		std::string m_sColumnName;
		int m_nWidth;
	};

	std::vector<SListViewColumn> m_vColumnList;
	CScrollablePane *m_pScrollbar;

	int m_nDragX;

	int m_nIndex;
	int m_nSelected;

	bool m_bSizable;
	bool m_bMovable;

	bool m_bSizing;
	bool m_bMoving;

	int m_nOverColumnId;
	int m_nId;

	UINT m_nRowSize;
	SControlRect rTitleRect;
	SControlRect m_rContentArea;
};