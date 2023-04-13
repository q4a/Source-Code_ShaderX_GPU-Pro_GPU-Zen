

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>


#include "handy.h"


/*static*/ void Tweak::SortTweaksIntoAPrettyList ( void )
{
	// Takes all the tweaks in the pUnsortedTweakRoot list
	// and adds them to the pTweakRoot list.
	if ( pUnsortedTweakRoot != NULL )
	{
		while ( pUnsortedTweakRoot != NULL )
		{
			Tweak *ptweakCur = pUnsortedTweakRoot;
			pUnsortedTweakRoot = ptweakCur->ptweakNext;
			ptweakCur->ptweakNext = NULL;

			ASSERT ( ptweakCur->pchName != NULL );


			if ( ( pTweakRoot == NULL ) || ( stricmp ( pTweakRoot->pchName, ptweakCur->pchName ) > 0 ) )
			{
				// Goes at the head of the list.
				ptweakCur->ptweakNext = pTweakRoot;
				pTweakRoot = ptweakCur;
			}
			else
			{
				Tweak *ptweakInsertAfter = pTweakRoot;
				Tweak *ptweakInsertBefore = ptweakInsertAfter->ptweakNext;
				while ( ( ptweakInsertBefore != NULL ) && ( stricmp ( ptweakInsertBefore->pchName, ptweakCur->pchName ) <= 0 ) )
				{
					ptweakInsertAfter = ptweakInsertBefore;
					ptweakInsertBefore = ptweakInsertBefore->ptweakNext;
				}
				ptweakCur->ptweakNext = ptweakInsertBefore;
				ptweakInsertAfter->ptweakNext = ptweakCur;
			}
		}
		ptweakCurrent = pTweakRoot;
	}
}


/*static*/ void Tweak::TweakNext ( void )
{
	Tweak *ptweakNext = ptweakCurrent->Next();
	if ( ptweakNext != NULL )
	{
		ptweakCurrent = ptweakNext;
	}
}

/*static*/ void Tweak::TweakPrev ( void )
{
	Tweak *ptweakPrev = ptweakCurrent->Prev();
	if ( ptweakPrev != NULL )
	{
		ptweakCurrent = ptweakPrev;
	}
}

/*static*/ void Tweak::TweakInc ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->Inc();
	}
}

/*static*/ void Tweak::TweakDec ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->Dec();
	}
}

/*static*/ void Tweak::TweakIncSlow ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->FineInc();
	}
}

/*static*/ void Tweak::TweakDecSlow ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->FineDec();
	}
}

/*static*/ void Tweak::TweakDefault ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->Default();
	}
}

/*static*/ void Tweak::TweakNeg ( void )
{
	if ( ptweakCurrent != NULL )
	{
		ptweakCurrent->Neg();
	}
}

/*static*/ Tweak *Tweak::Current ( void )
{
	return ptweakCurrent;
}


Tweak::Tweak ( void )
{
}

/*virtual*/ Tweak::~Tweak()
{
}




// Internal current state stuff.
/*static*/ Tweak *Tweak::ptweakCurrent = NULL;

// Where tweaks go when they are created.
/*static*/ Tweak *Tweak::pUnsortedTweakRoot = NULL;
// Where tweaks are put one they've been sorted.
/*static*/ Tweak *Tweak::pTweakRoot = NULL;







