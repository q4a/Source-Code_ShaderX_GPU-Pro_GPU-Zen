
#pragma once


#include <stdarg.h>
#include <stdio.h>


// Some stuff I've found handy down the years.


// Fixes the scope of i in:
// for ( int i = ... )
// But annoyingly throws up a "conditional expression is constant" warning.
#pragma warning ( disable : 4127 )
#define for if(0){} else for



// ASSERT and TRACE

#ifdef _DEBUG

//#define ASSERT(exp) { assert(exp); __assume(exp); }
#define ASSERT(exp) { if (!(exp)){_asm { int 3 };} }

inline void TRACE ( char *fmt, ... )
{
	va_list vlist;

	char pchTemp[1000];

	va_start ( vlist, fmt );
	vsprintf ( pchTemp, fmt, vlist );
	OutputDebugString ( pchTemp );
	va_end ( vlist );
}

#else //#ifdef DEBUG

#define ASSERT sizeof
#define TRACE sizeof

#endif //#else //#ifdef DEBUG







// Some very simple "tweak" things that come in handy.

struct Tweak
{
	const char *pchName;
	const char *pchDescription;
	Tweak *ptweakNext;		// Next sibling.

	enum Type
	{
		TweakType_Float,	// A float
		TweakType_Int		// An int
	} type;


	virtual Type GetType ( void ) = 0;

	virtual void Inc ( void ) = 0;
	virtual void Dec ( void ) = 0;
	virtual void FineInc ( void ) = 0;
	virtual void FineDec ( void ) = 0;
	virtual void Neg ( void ) = 0;
	virtual void Default ( void ) = 0;

	virtual float GetFloat ( void ) = 0;
	virtual int GetInt ( void ) = 0;

	Tweak *Next ( void )
	{
		return ptweakNext;
	}
	Tweak *Prev ( void )
	{
		Tweak *ptweakPrev = NULL;
		Tweak *ptweakCur = pTweakRoot;
		while ( ptweakCur != this )
		{
			ptweakPrev = ptweakCur;
			ptweakCur = ptweakCur->ptweakNext;
			ASSERT ( ptweakCur != NULL );
		}
		return ptweakPrev;
	}

protected:
	// stuff not allowed.
	Tweak (const Tweak &other) {ASSERT(false);}
	void operator = (Tweak &other) {ASSERT(false);}

	Tweak ( void );
	virtual ~Tweak();




	// Where tweaks go when they are created.
	static Tweak *pUnsortedTweakRoot;
	// Where tweaks are put one they've been sorted.
	static Tweak *pTweakRoot;


	// Internal current state stuff.
	static Tweak *ptweakCurrent;


public:
	// Call this sometime after all tweaks have been added - sorts them nicely,
	// after the random order they get added in by the static initialisation.
	static void SortTweaksIntoAPrettyList ( void );

	// These are called by whatever UI keyboard mapping you have.
	static void TweakNext ( void );
	static void TweakPrev ( void );
	static void TweakInc ( void );
	static void TweakDec ( void );
	static void TweakIncSlow ( void );
	static void TweakDecSlow ( void );
	static void TweakDefault ( void );
	static void TweakNeg ( void );
	static Tweak *Current ( void );

};





struct TweakFloat : Tweak
{
	float val;			// Current value.
	float def;			// Default value.

	// NOTE! pchNewName and pchNewDescription are not owned by this, they're static.
	TweakFloat ( const char *pchNewName, const char *pchNewDescription, float fDefaultValue )
	{
		// This just adds this in any order.
		// They will be sorted later.
		type = TweakType_Float;
		pchName = pchNewName;
		if ( pchNewDescription == NULL )
		{
			// Don't want NULLs all over the place.
			pchDescription = "";
		}
		else
		{
			pchDescription = pchNewDescription;
		}
		ptweakNext = pUnsortedTweakRoot;
		pUnsortedTweakRoot = this;

		def = fDefaultValue;
		val = fDefaultValue;
	}
	
	virtual Type GetType ( void )
	{
		return TweakType_Float;
	}

	virtual float GetFloat ( void )
	{
		return val;
	}
	virtual int GetInt ( void )
	{
		return (int)val;
	}
	virtual void Inc ( void )
	{
		val *= 1.1f;
	}
	virtual void Dec ( void )
	{
		val /= 1.1f;
	}
	virtual void Neg ( void )
	{
		val = -val;
	}
	virtual void FineInc ( void )
	{
		val *= 1.01f;
	}
	virtual void FineDec ( void )
	{
		val /= 1.01f;
	}
	virtual void Default ( void )
	{
		val = def;
	}

	// And now actually getting/setting the info.
	operator float ( void ) const
	{
		return val;
	}
	void operator= ( const float &newval )
	{
		val = newval;
	}

};

struct TweakInt : Tweak
{
	int val;			// Current value.
	int def;			// Default value.

	// NOTE! pchNewName and pchNewDescription are not owned by this, they're static.
	TweakInt ( char *pchNewName, char *pchNewDescription, int iDefaultValue )
	{
		// This just adds this in any order.
		type = TweakType_Int;
		pchName = pchNewName;
		if ( pchNewDescription == NULL )
		{
			// Don't want NULLs all over the place.
			pchDescription = "";
		}
		else
		{
			pchDescription = pchNewDescription;
		}
		ptweakNext = pUnsortedTweakRoot;
		pUnsortedTweakRoot = this;

		def = iDefaultValue;
		val = iDefaultValue;
	}
	
	virtual Type GetType ( void )
	{
		return TweakType_Int;
	}

	virtual float GetFloat ( void )
	{
		return (float)val;
	}
	virtual int GetInt ( void )
	{
		return val;
	}
	virtual void Inc ( void )
	{
		if ( ( val >= 0 ) && ( val < 16 ) )
		{
			val++;
		}
		else
		{
			val += val >> 4;
		}
	}
	virtual void Dec ( void )
	{
		if ( ( val >= 0 ) && ( val < 16 ) )
		{
			val--;
		}
		else
		{
			val -= val >> 4;
		}
	}
	virtual void Neg ( void )
	{
		val = -val;
	}
	virtual void FineInc ( void )
	{
		if ( val >= 0 )
		{
			val++;
		}
		else
		{
			val--;
		}
	}
	virtual void FineDec ( void )
	{
		if ( val >= 0 )
		{
			val--;
		}
		else
		{
			val++;
		}
	}
	virtual void Default ( void )
	{
		val = def;
	}

	// And now actually getting/setting the info.
	operator int ( void ) const
	{
		return val;
	}
	void operator= ( const int &newval )
	{
		val = newval;
	}

};



#define TWEAK_INT(name,descript,value) TweakInt name ( #name, descript, value )
#define TWEAK_FLOAT(name,descript,value) TweakFloat name ( #name, descript, value )
#define EXTERN_TWEAK_INT(name) extern TweakInt name;
#define EXTERN_TWEAK_FLOAT(name) extern TweakFloat name;

#ifdef _DEBUG
#define TWEAK_INT_DEBUG(name,descript,value) TweakInt name ( #name, descript, value )
#define TWEAK_FLOAT_DEBUG(name,descript,value) TweakFloat name ( #name, descript, value )
#else
#define TWEAK_INT_DEBUG(name,descript,value) const int name = value;
#define TWEAK_FLOAT_DEBUG(name,descript,value) const float name = value;
#endif















