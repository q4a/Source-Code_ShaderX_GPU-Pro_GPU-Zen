//-----------------------------------------------------------------------------
// File: SurfaceBasis.cpp
//
// Desc: DirectX window application created by the DirectX AppWizard
//-----------------------------------------------------------------------------
#define STRICT
#define DIRECTINPUT_VERSION 0x0800
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include <dinput.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "DisplacementCompression.h"

#include <stack>


//-----------------------------------------------------------------------------
// Global access to the app (needed for the global WndProc())
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;

const enum
{
	BYTE_BARYCENTRIC,
	WORD_BARYCENTRIC,
	FLOAT_BARYCENTRIC,
} gBarycentricPrecision = FLOAT_BARYCENTRIC;


#define DISPLACEMENT_MAP_SIZE 256

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: Entry point to the program. Initializes everything, and goes into a
//       message-processing loop. Idle time is used to render the scene.
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: Application constructor.   Paired with ~CMyD3DApplication()
//       Member variables should be initialized to a known state here.  
//       The application window has not yet been created and no Direct3D device 
//       has been created, so any initialization that depends on a window or 
//       Direct3D should be deferred to a later stage. 
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "DisplacementCompression" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // Create a D3D font using d3dfont.cpp
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;
	m_pD3DXPatchMesh			= NULL;
	m_pD3DXPatchMeshDest		= NULL;
    m_pDI                       = NULL;
    m_pKeyboard                 = NULL;

	m_pLinearDecl				= NULL;
	m_linearPos					= NULL;
	m_linearNorm				= NULL;
	m_pVB						= NULL; 
	
	m_posVertexData				= NULL;
	m_normVertexData			= NULL;
	m_indexData					= NULL;
	m_DisplacementTexture		= NULL;

	m_numVertices				= 0;
	m_numIndices				= 0;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;

	m_displayMethod				= DM_LINEAR;

	m_lod						= 5;

    // Read settings from registry
    ReadSettings();

	CreateMandelbrot( DISPLACEMENT_MAP_SIZE );
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: Application destructor.  Paired with CMyD3DApplication()
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: Paired with FinalCleanup().
//       The window has been created and the IDirect3D9 interface has been
//       created, but the device has not been created yet.  Here you can
//       perform application-related initialization and cleanup that does
//       not depend on a device.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: perform one time initialization

    // Drawing loading status message until app finishes loading
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    // Initialize DirectInput
    InitInput( m_hWnd );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ReadSettings()
// Desc: Read the app settings from the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::ReadSettings()
{
    HKEY hkey;
    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Read the stored window width/height.  This is just an example,
        // of how to use DXUtil_Read*() functions.
        DXUtil_ReadIntRegKey( hkey, TEXT("Width"), &m_dwCreationWidth, m_dwCreationWidth );
        DXUtil_ReadIntRegKey( hkey, TEXT("Height"), &m_dwCreationHeight, m_dwCreationHeight );

        RegCloseKey( hkey );
    }
}




//-----------------------------------------------------------------------------
// Name: WriteSettings()
// Desc: Write the app settings to the registry
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::WriteSettings()
{
    HKEY hkey;

    if( ERROR_SUCCESS == RegCreateKeyEx( HKEY_CURRENT_USER, DXAPP_KEY, 
        0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hkey, NULL ) )
    {
        // TODO: change as needed

        // Write the window width/height.  This is just an example,
        // of how to use DXUtil_Write*() functions.
        DXUtil_WriteIntRegKey( hkey, TEXT("Width"), m_rcWindowClient.right );
        DXUtil_WriteIntRegKey( hkey, TEXT("Height"), m_rcWindowClient.bottom );

        RegCloseKey( hkey );
    }
}





//-----------------------------------------------------------------------------
// Name: InitInput()
// Desc: Initialize DirectInput objects
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitInput( HWND hWnd )
{
    HRESULT hr;

    // Create a IDirectInput8*
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&m_pDI, NULL ) ) )
        return DXTRACE_ERR( "DirectInput8Create", hr );
    
    // Create a IDirectInputDevice8* for the keyboard
    if( FAILED( hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL ) ) )
        return DXTRACE_ERR( "CreateDevice", hr );
    
    // Set the keyboard data format
    if( FAILED( hr = m_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return DXTRACE_ERR( "SetDataFormat", hr );
    
    // Set the cooperative level on the keyboard
    if( FAILED( hr = m_pKeyboard->SetCooperativeLevel( hWnd, 
                                            DISCL_NONEXCLUSIVE | 
                                            DISCL_FOREGROUND | 
                                            DISCL_NOWINKEY ) ) )
        return DXTRACE_ERR( "SetCooperativeLevel", hr );

    // Acquire the keyboard
    m_pKeyboard->Acquire();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: Called during device initialization, this code checks the display device
//       for some minimum set of capabilities
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: Perform checks to see if these display caps are acceptable.
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: create device objects

    HRESULT hr;

    // Init the font
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    // Create a cube mesh using D3DX
    if( FAILED( hr = D3DXCreateBox( m_pd3dDevice,  1, 1, 1, &m_pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );

	D3DXWELDEPSILONS Epsilons;
	memset( &Epsilons, 0, sizeof(D3DXWELDEPSILONS) );
	Epsilons.Normal = 100000;
	D3DXWeldVertices( m_pD3DXMesh,  0, &Epsilons, 0, 0, 0, 0 );

	// to give us average normals (so the n-patch is more interesting);
	D3DXComputeNormals(m_pD3DXMesh, 0 );

	GetXMeshData( m_pD3DXMesh );

	// this cool idea is from David Martin at MS (D3DX team)
	// to guarentee maximum array size we set a define in code before
	// calling the compiler :-) That means we can max the array to the card
	// capabilites
	D3DXMACRO pMacros[2];
	CHAR szNumConsts[10];



	// the maximum number of constants, reduced to give the compiler some breathing space
	D3DCAPS9 Caps;
	m_pd3dDevice->GetDeviceCaps( &Caps );
	const int NumConsts = Caps.MaxVertexShaderConst - 3; // based on device caps
	sprintf(szNumConsts, "%d", NumConsts);

	// #define NUM_CONSTS <n>
	pMacros[0].Name = "NUM_CONSTS";
	pMacros[0].Definition = szNumConsts;

	// NULL-terminate the array
	pMacros[1].Name = NULL;
	pMacros[1].Name = NULL;

	// linear
	if( FAILED( hr = D3DXCreateEffectFromFile( m_pd3dDevice, "linear.fx", pMacros, 0, 0, 0, &m_linearFX, 0 ) ) )
		return DXTRACE_ERR( "D3DXCreateEffectFromFile", hr );

	// default to linear coordinate method
	if( FAILED( hr = GenerateLinearConstants() ) )
		return DXTRACE_ERR( "GenerateLinearConstants", hr );
	
    return S_OK;
}

HRESULT CMyD3DApplication::GetXMeshData( LPD3DXMESH in )
{
	HRESULT hRes;

	// get input vertex data
	float* inStream = 0;
	hRes = in->LockVertexBuffer(D3DLOCK_READONLY, (void**)&inStream);
	if( FAILED(hRes) ) return hRes;

	m_numVertices = in->GetNumVertices();

	// allocate some memory to store the vertices
	m_posVertexData = new float[ m_numVertices * 3 ];
	m_normVertexData = new float[ m_numVertices * 3 ];
	m_uvVertexData = new float[ m_numVertices * 2 ];

	// copy the vertex data out
	for(unsigned int i=0;i < m_numVertices;i++)
	{
		m_posVertexData[(i*3)+0] = inStream[0];
		m_posVertexData[(i*3)+1] = inStream[1];
		m_posVertexData[(i*3)+2] = inStream[2];
		m_normVertexData[(i*3)+0] = inStream[3];
		m_normVertexData[(i*3)+1] = inStream[4];
		m_normVertexData[(i*3)+2] = inStream[5];

		// generate some spherical uv to apply the displacement map
		m_uvVertexData[(i*2)+0] = 0.5f * (1.f + inStream[3]);
		m_uvVertexData[(i*2)+1] = 0.5f * (1.f - inStream[4]);

		// next vertex
		inStream += 6;
	}

	hRes = in->UnlockVertexBuffer();
	if( FAILED(hRes) ) return hRes;


	// now copy indices
	m_numIndices = in->GetNumFaces() * 3;

	// 16 bit indices only
	const unsigned int ibsize = m_numIndices * sizeof(WORD);

	m_indexData = new WORD[ m_numIndices ];

	WORD* inIBStream = 0;
	hRes = in->LockIndexBuffer( D3DLOCK_READONLY, (void**)&inIBStream);
	if( FAILED(hRes) ) return hRes;

	memcpy(m_indexData, inIBStream, ibsize);

	// now unlock both index buffers
	hRes = in->UnlockIndexBuffer();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}

HRESULT CMyD3DApplication::GenerateLinearConstants()
{
	HRESULT hRes;

	// first un-index the vertex data
	m_linearPos = new float[ m_numVertices * 4];
	m_linearNorm = new float[ m_numVertices * 4];

	for(unsigned int i=0; i < m_numVertices;i++)
	{
		m_linearPos[ (i*4) + 0 ] = m_posVertexData[ (i*3) + 0 ];
		m_linearPos[ (i*4) + 1 ] = m_posVertexData[ (i*3) + 1 ];
		m_linearPos[ (i*4) + 2 ] = m_posVertexData[ (i*3) + 2 ];
		m_linearPos[ (i*4) + 3 ] = 0;

		m_linearNorm[ (i*4) + 0 ] = m_normVertexData[ (i*3) + 0 ];
		m_linearNorm[ (i*4) + 1 ] = m_normVertexData[ (i*3) + 1 ];
		m_linearNorm[ (i*4) + 2 ] = m_normVertexData[ (i*3) + 2 ];
		m_linearNorm[ (i*4) + 3 ] = 0;
	}

	// we use the vertex data directly
	hRes = GenerateLinearBuffers( m_lod );
	if( FAILED(hRes) ) return hRes;


	D3DVERTEXELEMENT9	Decl[MAX_FVF_DECL_SIZE];

	// create a declaration D3D 
	// barycentic coordinates
	Decl[0].Stream		= 0;
	Decl[0].Offset		= 0;
	if( gBarycentricPrecision == BYTE_BARYCENTRIC )
	{
		Decl[0].Type		= D3DDECLTYPE_D3DCOLOR;
	} else if(gBarycentricPrecision == WORD_BARYCENTRIC)
	{
		Decl[0].Type		= D3DDECLTYPE_USHORT4N;
	} else if(gBarycentricPrecision == FLOAT_BARYCENTRIC)
	{
		Decl[0].Type		= D3DDECLTYPE_FLOAT2;
	}

	Decl[0].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[0].Usage		= D3DDECLUSAGE_POSITION;
	Decl[0].UsageIndex	= 0;

	// indices
	Decl[1].Stream		= 0;
	if(gBarycentricPrecision == FLOAT_BARYCENTRIC)
	{
		Decl[1].Offset		= sizeof( float ) * 2;
	} else
	{
		Decl[1].Offset		= sizeof( unsigned int );
	}
	Decl[1].Type		= D3DDECLTYPE_D3DCOLOR;
	Decl[1].Method		= D3DDECLMETHOD_DEFAULT;
	Decl[1].Usage		= D3DDECLUSAGE_POSITION;
	Decl[1].UsageIndex	= 1;

	// D3DDECL_END()
	Decl[2].Stream		= 0xFF;
	Decl[2].Offset		= 0;
	Decl[2].Type		= D3DDECLTYPE_UNUSED;
	Decl[2].Method		= 0;
	Decl[2].Usage		= 0;
	Decl[2].UsageIndex	= 0;

	SAFE_RELEASE( m_pLinearDecl );
	hRes = m_pd3dDevice->CreateVertexDeclaration( Decl, &m_pLinearDecl );
	if( FAILED(hRes) ) return hRes;

	// set the constant vertices
	m_linearFX->SetVectorArray( "VertexPos", (D3DXVECTOR4*)m_linearPos, m_numVertices );
	m_linearFX->SetVectorArray( "VertexNorm", (D3DXVECTOR4*)m_linearNorm, m_numVertices );

	return S_OK;
}

static unsigned int calcPatchVertsPerOrigTri( unsigned int n )
{
	return unsigned int(pow(4, n)) * 3;
}

struct BARYCENTRIC_COORDS
{
	float i, j; // k can be computed easily

	BARYCENTRIC_COORDS() : i(0), j(0) {};
	BARYCENTRIC_COORDS( float x, float y ) : i(x), j(y) {};
	BARYCENTRIC_COORDS( const BARYCENTRIC_COORDS& right) : i(right.i), j(right.j) {};
	float getI() { return i; };
	float getJ() { return j; };
	float getK() { return 1.f - i - j; };

	BYTE getByteQuantisedI()
	{ 
		// this hack helps reduce holes caused by the extreme lack of precision
		unsigned int i = getI() * 256.f;
		if( i >= 256 )
			i = 255;
		return BYTE( i ); 
	};
	BYTE getByteQuantisedJ()
	{ 
		// this hack helps reduce holes caused by the extreme lack of precision
		unsigned int j = getJ() * 256.f;
		if( j >= 256 )
			j = 255;
		return BYTE( j ); 
	};
	BYTE getByteQuantisedK()
	{
		// this hack helps reduce holes caused by the extreme lack of precision
		unsigned int k = getK() * 256.f;
		if( k >= 256 )
			k = 255;
		return BYTE( k ); 
	};

	WORD getWordQuantisedI()
	{ 
		unsigned int i = getI() * 65535.f;
		return WORD( i ); 
	};
	WORD getWordQuantisedJ()
	{ 
		unsigned int j = getJ() * 65535.f;
		return WORD( j ); 
	};
	WORD getWordQuantisedK()
	{ 
		unsigned int k = getK() * 65535.f;
		return WORD( k ); 
	};

	BARYCENTRIC_COORDS& operator+=( const BARYCENTRIC_COORDS& right )
	{
		i = i + right.i;
		j = j + right.j;
		return *this;
	}
	BARYCENTRIC_COORDS operator+( const BARYCENTRIC_COORDS& right ) const
	{
		BARYCENTRIC_COORDS ret(*this);
		ret += right;
		return ret;
	}

	BARYCENTRIC_COORDS& operator/( float right )
	{
		i = i / right;
		j = j / right;
		return *this;
	}

};

struct BARYCENTRIC_TRIANGLE
{
	BARYCENTRIC_COORDS a, b, c;
	unsigned int lod;

	// default standard triangle
	BARYCENTRIC_TRIANGLE() : a(1,0), b(0,1), c(0,0), lod(0) {};

	BARYCENTRIC_TRIANGLE( const BARYCENTRIC_TRIANGLE& right )
	{
		a = right.a;
		b = right.b;
		c = right.c;
		lod = right.lod;
	}
};

static void PatchTesselate( BARYCENTRIC_TRIANGLE tri,
					   // input
						BARYCENTRIC_TRIANGLE& a,
						BARYCENTRIC_TRIANGLE& b,
						BARYCENTRIC_TRIANGLE& c,
						BARYCENTRIC_TRIANGLE& d )
{

	BARYCENTRIC_COORDS ab = (tri.a + tri.b) / 2;
	BARYCENTRIC_COORDS bc = (tri.b + tri.c) / 2;
	BARYCENTRIC_COORDS ac = (tri.a + tri.c) / 2;

	a.a = tri.a;
	a.b = ab;
	a.c = ac;
	a.lod = tri.lod+1;

	b.a = bc;
	b.b = tri.c;
	b.c = ac;
	b.lod = tri.lod+1;

	c.a = bc;
	c.b = ab;
	c.c = tri.b;
	c.lod = tri.lod+1;

	d.a = bc;
	d.b = ac;
	d.c = ab;
	d.lod = tri.lod+1;
}

void CMyD3DApplication::CreateMandelbrot( unsigned int dwSize)
{
	// I generate a mandlebrot to have something interesting to look at
	float val;
	float MinRe = -1.59f;
	float MaxRe = -1.33f;
	float MinIm = -0.1f;
	float MaxIm = MinIm + (MaxRe-MinRe);
	float ReFactor = (MaxRe-MinRe) / (dwSize-1);
	float ImFactor = (MaxIm-MinIm) / (dwSize-1);

	m_DisplacementTexture = new float[dwSize * dwSize];

	
	for( DWORD z=0; z<dwSize; z++ )
	{
		float cim = MaxIm - z*ImFactor;

		for( DWORD x=0; x<dwSize; x++ )
		{
			float cre = MinRe + x*ReFactor;
			float zre = cre, zim = cim;
			for(unsigned loop=0;loop < 80;loop++)
			{
				float zim2 = zim * zim;
				zim = 2 * zre * zim + cim;
				zre = zre*zre - zim2 + cre;

				// generate a pattern
				val = zre*zre+ zim*zim;
				if( val > 4)
					break;
			}

			val = loop / 80.f;

			m_DisplacementTexture[ (z*dwSize) + x ] = val;
		}
	}
}
// looks up the displacement value at the specified coodinate
float CMyD3DApplication::LookupDisplacementValue( WORD i0, WORD i1, WORD i2, float i, float j)
{
	const float k = 1.f - i - j;
	float u,v;
	u =	  (m_uvVertexData[ (i0*2)+ 0] * i)
		+ (m_uvVertexData[ (i1*2)+ 0] * j)
		+ (m_uvVertexData[ (i2*2)+ 0] * k);
	v =	  (m_uvVertexData[ (i0*2)+ 1] * i)
		+ (m_uvVertexData[ (i1*2)+ 1] * j)
		+ (m_uvVertexData[ (i2*2)+ 1] * k);

	int address = (int)(u * DISPLACEMENT_MAP_SIZE * DISPLACEMENT_MAP_SIZE) + (v* DISPLACEMENT_MAP_SIZE);
	return m_DisplacementTexture[ address ];
}

HRESULT CMyD3DApplication::GenerateLinearBuffers( unsigned int LOD )
{
	HRESULT hRes;

	m_lod = LOD;

	SAFE_RELEASE( m_pVB );

	const unsigned int numTri = m_numIndices / 3;
	const unsigned int numVerts = calcPatchVertsPerOrigTri( LOD ) * numTri;
	unsigned int vbSize;
	
	// the barycentric are reusable between any surface
	// of similar subdivision amount, so splitting into 2 streams can save alot
	// ram
	if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
	{
		vbSize = (sizeof(float)*2 + sizeof(unsigned int)) * numVerts;
	} else
	{
		vbSize = (sizeof(unsigned int) * 2) * numVerts;
	}
	hRes = m_pd3dDevice->CreateVertexBuffer( vbSize, 0, 0,D3DPOOL_MANAGED, &m_pVB, NULL );
	if( FAILED(hRes) ) return hRes;

	unsigned int *outStream;

	hRes = m_pVB->Lock(0, vbSize, (void**)&outStream, 0 );
	if( FAILED(hRes) ) return hRes;

	for(unsigned int index=0;index < m_numIndices/3;index++)
	{
		std::stack<BARYCENTRIC_TRIANGLE> lodStack;
		lodStack.push( BARYCENTRIC_TRIANGLE() );

		while( !lodStack.empty() )
		{
			BARYCENTRIC_TRIANGLE tri = lodStack.top();
			lodStack.pop();

			if( tri.lod < LOD)
			{
				BARYCENTRIC_TRIANGLE tri_a, tri_b, tri_c, tri_d;

				PatchTesselate( tri, tri_a, tri_b, tri_c, tri_d );
				lodStack.push( tri_a );
				lodStack.push( tri_b );
				lodStack.push( tri_c );
				lodStack.push( tri_d );
			} else
			{
				if( tri.lod == LOD )
				{
					unsigned int uii, uij;
					// remember you have to split large meshes so a byte index isn't an issue (at the moment)
					BYTE i0 = (BYTE) m_indexData[ (index*3) + 0 ];
					BYTE i1 = (BYTE) m_indexData[ (index*3) + 1 ];
					BYTE i2 = (BYTE) m_indexData[ (index*3) + 2 ];

					float d0 = LookupDisplacementValue( i0, i1, i2, tri.a.getI(), tri.a.getJ() ) * 255;
					float d1 = LookupDisplacementValue( i0, i1, i2, tri.b.getI(), tri.b.getJ() ) * 255;
					float d2 = LookupDisplacementValue( i0, i1, i2, tri.c.getI(), tri.c.getJ() ) * 255;


					if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
					{
						*((float*)outStream+0) = tri.a.getI();
						*((float*)outStream+1) = tri.a.getJ();
						*(outStream+2) = ((unsigned int)(d0)<<24) | (i0<<16) | (i1 << 8) | (i2 << 0);
						*((float*)outStream+3) = tri.b.getI();
						*((float*)outStream+4) = tri.b.getJ();
						*(outStream+5) = ((unsigned int)(d1)<<24) | (i0<<16) | (i1 << 8) | (i2 << 0);
						*((float*)outStream+6) = tri.c.getI();
						*((float*)outStream+7) = tri.c.getJ();
						*(outStream+8) = ((unsigned int)(d2)<<24) | (i0<<16) | (i1 << 8) | (i2 << 0) ;

						outStream += 9;
					} else
					{
						// output coords
						if( gBarycentricPrecision == BYTE_BARYCENTRIC )
						{
							uii = tri.a.getByteQuantisedI();
							uij = tri.a.getByteQuantisedJ();
							*(outStream+0) = (uii<<16) | (uij<<8); // correct for D3DCOLOR unpacking
							uii = tri.b.getByteQuantisedI();
							uij = tri.b.getByteQuantisedJ();
							*(outStream+2) = (uii<<16) | (uij<<8); // correct for D3DCOLOR unpacking
							uii = tri.c.getByteQuantisedI();
							uij = tri.c.getByteQuantisedJ();
							*(outStream+4) = (uii<<16) | (uij<<8); // correct for D3DCOLOR unpacking
						} else if( gBarycentricPrecision == WORD_BARYCENTRIC )
						{
							uii = tri.a.getWordQuantisedI();
							uij = tri.a.getWordQuantisedJ();
							*(outStream+0) = (uii<<0) | (uij<<16); 
							uii = tri.b.getWordQuantisedI();
							uij = tri.b.getWordQuantisedJ();
							*(outStream+2) = (uii<<0) | (uij<<16); 
							uii = tri.c.getWordQuantisedI();
							uij = tri.c.getWordQuantisedJ();
							*(outStream+4) = (uii<<0) | (uij<<16); 
						}

						*(outStream+1) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*(outStream+3) = (i0<<16) | (i1 << 8) | (i2 << 0);
						*(outStream+5) = (i0<<16) | (i1 << 8) | (i2 << 0);
						outStream += 6;
					}
				}
			}
		}
	}

	// now unlock vertex buffer
	hRes = m_pVB->Unlock();
	if( FAILED(hRes) ) return hRes;

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: setup render states

    // Setup a material
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // Set up the textures
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // Set miscellaneous render states
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // Set the world matrix
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );
	m_matWorld = matIdentity;

    // Set up our view matrix. A view matrix can be defined given an eye point,
    // a point to lookat, and a direction for which way is up. Here, we set the
    // eye five units back along the z-axis and up three units, look at the
    // origin, and define "up" to be in the y-direction.
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -3.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );
	m_matView = matView;

    // Set the projection matrix
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
	m_matProj = matProj;

    // Set up lighting states
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // Restore the font
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: Called once per frame, the call is the entry point for animating
//       the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // Update user input state
    UpdateInput( &m_UserInput );

    // Update the world state according to user input
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
	m_matWorld = matWorld;

	// select LOD (if avaible)
	if( m_UserInput.b1 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 0 );
		}
	} else if( m_UserInput.b2 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 1 );
		}
	} else if( m_UserInput.b3 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 2 );
		}
	} else if( m_UserInput.b4 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 3 );
		}
	} else if( m_UserInput.b5 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 4 );
		}
	} else if( m_UserInput.b6 )
	{
		if( m_displayMethod == DM_LINEAR )
		{
			GenerateLinearBuffers( 5 );
		}
	}
	
	// select method
	if( m_UserInput.bf5 )
	{
	} else if( m_UserInput.bf6 )
	{
	} else if( m_UserInput.bf7 )
	{
	} else if( m_UserInput.bf8 )
	{
	} else if( m_UserInput.bf9 && m_displayMethod != DM_LINEAR)
	{
		m_displayMethod = DM_LINEAR;
		GenerateLinearBuffers( m_lod );
	}

	return SetupDisplayMethod();
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: Update the user input.  Called once per frame 
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    HRESULT hr;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &pUserInput->diks, sizeof(pUserInput->diks) );
    hr = m_pKeyboard->GetDeviceState( sizeof(pUserInput->diks), &pUserInput->diks );
    if( FAILED(hr) ) 
    {
        m_pKeyboard->Acquire();
        return; 
    }

    pUserInput->bRotateLeft  = ( (pUserInput->diks[DIK_LEFT] & 0x80)  == 0x80 );
    pUserInput->bRotateRight = ( (pUserInput->diks[DIK_RIGHT] & 0x80) == 0x80 );
    pUserInput->bRotateUp    = ( (pUserInput->diks[DIK_UP] & 0x80)    == 0x80 );
    pUserInput->bRotateDown  = ( (pUserInput->diks[DIK_DOWN] & 0x80)  == 0x80 );

	pUserInput->b1 = ( (pUserInput->diks[DIK_1] & 0x80)  == 0x80 );
	pUserInput->b2 = ( (pUserInput->diks[DIK_2] & 0x80)  == 0x80 );
	pUserInput->b3 = ( (pUserInput->diks[DIK_3] & 0x80)  == 0x80 );
	pUserInput->b4 = ( (pUserInput->diks[DIK_4] & 0x80)  == 0x80 );
	pUserInput->b5 = ( (pUserInput->diks[DIK_5] & 0x80)  == 0x80 );
	pUserInput->b6 = ( (pUserInput->diks[DIK_6] & 0x80)  == 0x80 );
	pUserInput->b7 = ( (pUserInput->diks[DIK_7] & 0x80)  == 0x80 );
	pUserInput->b8 = ( (pUserInput->diks[DIK_8] & 0x80)  == 0x80 );
	pUserInput->b9 = ( (pUserInput->diks[DIK_9] & 0x80)  == 0x80 );

	pUserInput->bf5 = ( (pUserInput->diks[DIK_F5] & 0x80)  == 0x80 );
	pUserInput->bf6 = ( (pUserInput->diks[DIK_F6] & 0x80)  == 0x80 );
	pUserInput->bf7 = ( (pUserInput->diks[DIK_F7] & 0x80)  == 0x80 );

	pUserInput->bf8 = ( (pUserInput->diks[DIK_F8] & 0x80)  == 0x80 );
	pUserInput->bf9 = ( (pUserInput->diks[DIK_F9] & 0x80)  == 0x80 );

}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Called once per frame, the call is the entry point for 3d
//       rendering. This function sets up render states, clears the
//       viewport, and renders the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	// Clear the viewport
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x50505050, 1.0f, 0L );

	m_pd3dDevice->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
    // Begin the scene
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// calculate MVP 
		D3DXMATRIX matMVP;
        D3DXMatrixMultiply( &matMVP, &m_matView, &m_matProj );
        D3DXMatrixMultiply( &matMVP, &m_matWorld, &matMVP );

		// should use a shared effect pool but its just a demo...
		m_linearFX->SetMatrix( "MVP", &matMVP );

		unsigned int numPasses;

		switch( m_displayMethod )
		{
		case DM_LINEAR:
			m_linearFX->Begin( &numPasses, 0 );
			for(unsigned int i=0; i < numPasses;i++)
			{
				m_pd3dDevice->SetVertexDeclaration( m_pLinearDecl );
				m_linearFX->Pass( i );
				const unsigned int numTri = m_numIndices / 3;
				unsigned int numVerts = calcPatchVertsPerOrigTri( m_lod ) * numTri;

				unsigned int vSize;
				if( gBarycentricPrecision == FLOAT_BARYCENTRIC )
				{
					vSize = sizeof(float)*2 + sizeof(unsigned int);
				} else
				{
					vSize = sizeof(unsigned int) * 2;
				}

				// some cards don't handles lots of vertices in one go
				if( numVerts > 65536 )
				{
					numVerts = 65536;
				}

				m_pd3dDevice->SetStreamSource(	0, m_pVB, 0, vSize );
				m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, numVerts /3 );
			}
			m_linearFX->End();
			break;
		}

        // Render stats and help text  
        RenderText();

        // End the scene.
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: Renders stats and help text to the scene.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // Output display stats
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Output statistics & help
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: Overrrides the main WndProc, so the sample can do custom message
//       handling (e.g. processing mouse, keyboard, or menu commands).
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // Draw on the window tell the user that the app is loading
                // TODO: change as needed
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Invalidates device objects.  Paired with RestoreDeviceObjects()
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: Cleanup any objects created in RestoreDeviceObjects()
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	CleanDisplayMethod();

	delete m_posVertexData; m_posVertexData = 0;
	delete m_normVertexData; m_normVertexData = 0;
	delete m_uvVertexData; m_uvVertexData = 0;
	delete m_indexData; m_indexData = 0;
	m_numVertices = 0;
	m_numIndices = 0;

    m_pFont->DeleteDeviceObjects();

	SAFE_RELEASE( m_linearFX ) ;

	SAFE_RELEASE( m_pD3DXPatchMeshDest ); 
	SAFE_RELEASE( m_pD3DXPatchMesh );
	SAFE_RELEASE( m_pD3DXMesh );
    return S_OK;
}


void CMyD3DApplication::CleanDisplayMethod()
{
	SAFE_RELEASE( m_pVB );

	SAFE_RELEASE( m_pLinearDecl );

	delete m_linearPos; m_linearPos = 0;
	delete m_linearNorm; m_linearNorm = 0;

}

HRESULT CMyD3DApplication::SetupDisplayMethod()
{
    HRESULT hr;

	if( m_displayMethod == DM_LINEAR )
	{
		if( m_pLinearDecl == 0 )
		{
			CleanDisplayMethod();
			if( FAILED( hr = GenerateLinearConstants() ) )
				return DXTRACE_ERR( "GenerateLinearConstants", hr );
		}
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Paired with OneTimeSceneInit()
//       Called before the app exits, this function gives the app the chance
//       to cleanup after itself.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{

	delete[] m_DisplacementTexture;

    // TODO: Perform any final cleanup needed
    // Cleanup D3D font
    SAFE_DELETE( m_pFont );

    // Cleanup DirectInput
    CleanupDirectInput();

    // Write the settings to the registry
    WriteSettings();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: CleanupDirectInput()
// Desc: Cleanup DirectInput 
//-----------------------------------------------------------------------------
VOID CMyD3DApplication::CleanupDirectInput()
{
    // Cleanup DirectX input objects
    SAFE_RELEASE( m_pKeyboard );
    SAFE_RELEASE( m_pDI );

}




