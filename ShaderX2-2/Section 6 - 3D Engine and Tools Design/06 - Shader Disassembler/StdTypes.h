typedef		unsigned int			Natural	;
typedef		unsigned int			Nat32	;
typedef		int						Integer	;
typedef		int						Bool	;
typedef		float					Real	;

#define		Null					0
#define		False					0
#define		True					1

#define MMask_And(a,b)		((a) & (b))
#define MMask_Test(a,b)		(((a) & (b)) == (b))

#ifdef _DEBUG	// _DEBUG

#define Assert(f)		_asm { int 3 }
#else
#define Assert(f)		((void)0)

#endif