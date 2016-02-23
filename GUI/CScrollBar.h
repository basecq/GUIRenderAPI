#pragma once

#include "CGUI.h"

class CScrollBarVertical : public CControl
{
public:
	CScrollBarVertical ( CDialog *pDialog );

	void Draw ( void );

	void UpdateRects ( void );
	bool ContainsRect ( CPos pos );

	bool HandleMouse ( UINT uMsg, CPos pos, WPARAM wParam, LPARAM lParam );

	void OnClickLeave ( void )
	{
		m_bPressed = m_bDrag = false;
		m_Arrow = CLEAR;
	}

	bool OnClickEvent ( void )
	{
		return  m_bPressed || m_bDrag || m_Arrow != CLEAR;
	}

	void SetTrackRange ( int nStart, int nEnd );

	int GetMinSize ( void ) { return m_nStart; }
	int GetEndSize ( void ) { return m_nEnd; }

	int GetTrackPos ( void )
	{
		return m_nPosition;
	}

	void SetTrackPos ( int nPosition )
	{
		m_nPosition = nPosition; 
		Cap (); 
		UpdateThumbRect ();
	}

	int GetPageSize ( void )
	{
		return m_nPageSize;
	}

	void SetPageSize ( int nPageSize )
	{
		m_nPageSize = nPageSize; 
		Cap (); 
		UpdateThumbRect ();
	}

	void SetStepSize ( int nStep )
	{
		m_nStep = nStep;
	}

	int GetStepSize ( void )
	{
		return m_nStep;
	}

	int GetScrollingDirection ( void )
	{
		static int nOldThumbPos = 0;

		if ( nOldThumbPos != m_nPosition )
		{
			if ( nOldThumbPos > m_nPosition )
				return 2;
			else
				return 1;
		}

		return -1;
	}


	bool IsThumbShowing ( void ) { return m_bShowThumb; }

	// Scroll by nDelta items (plus or minus)
	void Scroll ( int nDelta );    

	// Ensure that item nIndex is displayed, scroll if necessary
	void ShowItem ( int nIndex );  

private:

	// ARROWSTATE indicates the state of the arrow buttons.
	// CLEAR            No arrow is down.
	// CLICKED_UP       Up arrow is clicked.
	// CLICKED_DOWN     Down arrow is clicked.
	// HELD_UP          Up arrow is held down for sustained period.
	// HELD_DOWN        Down arrow is held down for sustained period.
	enum ARROWSTATE
	{
		CLEAR,
		CLICKED_UP,
		CLICKED_DOWN,
		CLICKED_THUMB,
		HELD_UP,
		HELD_DOWN,
	};

	void UpdateThumbRect ( void );
	void Cap ( void );  // Clips position at boundaries. Ensures it stays within legal range.

	SControlRect m_rUpButton;
	SControlRect m_rDownButton;
	SControlRect m_rThumb;

	int m_nStep;	  // Step size used for increase / decrease button clicks.
	int m_nPosition;  // Position of the first displayed item
	int m_nPageSize;  // How many items are displayable in one page
	int m_nStart;     // First item
	int m_nEnd;       // The index after the last item

	bool m_bShowThumb;
	bool m_bDrag;

	CTimer m_timer;
	CPos m_LastMouse;
	ARROWSTATE m_Arrow; // State of the arrows
};