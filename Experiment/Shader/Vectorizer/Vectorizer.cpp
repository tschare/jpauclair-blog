#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <stdio.h>
#include <sys\stat.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <mmsystem.h>
#include "resource.h"
#include <dos.h>
#include <conio.h>

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
HWND                    g_hWnd          = NULL;
LPDIRECT3D9             g_pD3D          = NULL;
LPDIRECT3DDEVICE9       g_pd3dDevice    = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVertexBuffer = NULL;
LPDIRECT3DTEXTURE9      g_pTexture_0    = NULL;
LPDIRECT3DTEXTURE9      g_pTexture_1    = NULL;
LPD3DXEFFECT            g_pEffect       = NULL;

LPDIRECT3DTEXTURE9 m_pTexRender = NULL;
LPDIRECT3DTEXTURE9 m_pTexRender2 = NULL;
LPDIRECT3DSURFACE9 m_pTexSurface = NULL;
LPDIRECT3DSURFACE9 m_pTexSurface2 = NULL;

D3DPRESENT_PARAMETERS d3dpp;

D3DXMATRIX g_matWorld;
D3DXMATRIX g_matView;
D3DXMATRIX g_matProj;
float      g_fSpinX = 0.0f;
float      g_fSpinY = 0.0f;

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )
#include <emmintrin.h>



struct Vertex
{
    float x, y, z, w;
	DWORD dif;
    float tu, tv;
};

Vertex g_quadVertices[] =
{
	{-1.0f, 1.0f, 0.0f, 1.0f,  0 , 0.0f,0.0f },
	{ 1.0f, 1.0f, 0.0f, 1.0f,  0 , 1.0f,0.0f },
	{-1.0f,-1.0f, 0.0f, 1.0f,  0 ,0.0f,1.0f },
	{ 1.0f,-1.0f, 0.0f, 1.0f,  0 ,1.0f,1.0f }
};

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init(void);
void render(void);
void shutDown(void);
void initEffect(void);
void setTechniqueVariables(void);
bool kill=false;

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPSTR     lpCmdLine,
					int       nCmdShow )
{
	WNDCLASSEX winClass; 
	MSG        uMsg;

	
    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
                             "Vectorizer",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 512, 512, NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	init();
	initEffect();

	while( uMsg.message != WM_QUIT && kill==false)
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
		else
		    render();
	}

	shutDown();

    UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );

	return uMsg.wParam;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static bool bMousing;
	
    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}
		}
        break;

		case WM_LBUTTONDOWN:
		{
			ptLastMousePosit.x = ptCurrentMousePosit.x = LOWORD (lParam);
            ptLastMousePosit.y = ptCurrentMousePosit.y = HIWORD (lParam);
			bMousing = true;
		}
		break;

		case WM_LBUTTONUP:
		{
			bMousing = false;
		}
		break;

		case WM_MOUSEMOVE:
		{
			ptCurrentMousePosit.x = LOWORD (lParam);
			ptCurrentMousePosit.y = HIWORD (lParam);

			if( bMousing )
			{
				g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
				g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
			}
			
			ptLastMousePosit.x = ptCurrentMousePosit.x;
            ptLastMousePosit.y = ptCurrentMousePosit.y;
		}
		break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);	
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

    D3DDISPLAYMODE d3ddm;

    g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

    
    ZeroMemory( &d3dpp, sizeof(d3dpp) );

    d3dpp.Windowed               = TRUE;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat       = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                          D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                          &d3dpp, &g_pd3dDevice );

	g_pd3dDevice->CreateVertexBuffer( 4*sizeof(Vertex), D3DUSAGE_WRITEONLY, 
                                      D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, 
                                      &g_pVertexBuffer, NULL );
    void *pVertices = NULL;

    g_pVertexBuffer->Lock( 0, sizeof(g_quadVertices), (void**)&pVertices, 0 );
    memcpy( pVertices, g_quadVertices, sizeof(g_quadVertices) );
    g_pVertexBuffer->Unlock();
	
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DXToRadian( 45.0f ), 
                                512.0f / 512.0f, 0.1f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	D3DXMatrixIdentity( &g_matView ); // This sample is not really making use of a view matrix

	g_pd3dDevice->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pTexRender,
		NULL);

	m_pTexRender->GetSurfaceLevel(0, &m_pTexSurface);

	//g_pd3dDevice->CreateRenderTarget(256,256,d3dpp.BackBufferFormat,D3DMULTISAMPLE_NONE,0,true,&m_pTexSurface,NULL);
/**/
	g_pd3dDevice->CreateOffscreenPlainSurface(	128,
												128,
												D3DFMT_A8R8G8B8,
												D3DPOOL_SYSTEMMEM,
												&m_pTexSurface2,
												NULL);
	/**
	g_pd3dDevice->CreateTexture(128, 128, 1, D3DUSAGE_DYNAMIC,
		D3DFMT_A8R8G8B8,
		D3DPOOL_SYSTEMMEM,
		&m_pTexRender2,
		NULL);
		

	m_pTexRender2->GetSurfaceLevel(0, &m_pTexSurface2);
	/**/

}


//-----------------------------------------------------------------------------
// Name: initEffect()
// Desc: Initializie an Fx effect.
//-----------------------------------------------------------------------------
void initEffect( void )
{
	//
	// Create two test textures for our effect to use...
	//

	D3DXCreateTextureFromFile( g_pd3dDevice, "noir.bmp", &g_pTexture_0 );
	D3DXCreateTextureFromFile( g_pd3dDevice, "blanc.bmp", &g_pTexture_1 );

	HRESULT hr;
	LPD3DXBUFFER pBufferErrors = NULL;

	hr = D3DXCreateEffectFromFile( g_pd3dDevice, 
		                           "effect.fx",
		                           NULL,
		                           NULL, 
		                           D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, 
		                           NULL, 
		                           &g_pEffect, 
		                           &pBufferErrors );

	if( FAILED(hr) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		MessageBox(NULL, (const char*)pCompilErrors, "Fx Compile Error",
			MB_OK|MB_ICONEXCLAMATION);
	}
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
void shutDown( void )
{
	if( g_pTexture_0 != NULL )
		g_pTexture_0->Release();

	if( g_pTexture_1 != NULL )
		g_pTexture_1->Release();

	if( g_pEffect != NULL )
		g_pEffect->Release();

    if( g_pVertexBuffer != NULL )
        g_pVertexBuffer->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

//-----------------------------------------------------------------------------
// Name: setTechniqueVariables()
// Desc: 
//-----------------------------------------------------------------------------
void setTechniqueVariables( void )
{
	//
	// The only variables this effect has are two textures...
	//

    g_pEffect->SetTexture( "g_BaseTexture", g_pTexture_0 );
	g_pEffect->SetTexture( "g_BaseTexture2", g_pTexture_1 );
	
	////D3DXMATRIX m1 = g_matWorld * g_matView *g_matProj;
	D3DXMATRIX m1 = g_matWorld*g_matView *g_matProj;
	g_pEffect->SetMatrix("g_WorldViewProj", &m1);

	//g_pEffect->SetTexture( "texture1", g_pTexture_1 );
}



void LogDebug(char *lpszText, ...)
{
	va_list argList;
	FILE *pFile = NULL;

	va_start(argList, lpszText);
	char msg[255]; 
	vsprintf(msg,lpszText, argList);
	OutputDebugString(msg);

	va_end(argList);
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
void render( void )
{

	//snag the old color buffer

	IDirect3DSurface9 *oldColorBuffer = 0;

	D3DXMATRIX matTrans,matProjection,matRot,matOldProjection;

	D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 2.0f );
		/*
	D3DXMatrixRotationYawPitchRoll( &matRot, 
		D3DXToRadian(g_fSpinX), 
		D3DXToRadian(g_fSpinY), 
		0.0f );
		*/
	g_matWorld = matTrans;
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

	g_pEffect->SetTechnique( "Render" );
	setTechniqueVariables();

	//g_pd3dDevice->GetRenderTarget(0, &oldColorBuffer);
//	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matOldProjection);


	__int64 t1 = timeGetTime();

	long sum=0;

#pragma region Version Shader

/*
 Sum : 12760064
 time : 547 
 diff : 778.812500
*/
	/*
	if(FAILED(g_pd3dDevice->SetRenderTarget(0, m_pTexSurface)))
	{
		LogDebug("Failed SetRenderTarget");
	}
	//g_pd3dDevice->GetRenderTargetData(m_pTexSurface,m_pTexSurface2);
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );
	g_pd3dDevice->BeginScene();
	for (int x=0;x<1000;x++)
	{
		
		UINT uPasses;
		g_pEffect->Begin( &uPasses, 0 );
	    
		for( UINT uPass = 0; uPass < uPasses; ++uPass )
		{
			g_pEffect->BeginPass( uPass );
			g_pd3dDevice->SetStreamSource( 0, g_pVertexBuffer, 0, sizeof(Vertex) );
			g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			g_pEffect->EndPass();
		}

		g_pEffect->End();
		
		g_pd3dDevice->GetRenderTargetData(m_pTexSurface,m_pTexSurface2);
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,D3DCOLOR_COLORVALUE(0.0f,0.0f,0.0f,1.0f), 1.0f, 0 );
		D3DSURFACE_DESC m_desc; 
		D3DLOCKED_RECT m_lockedRect;

		if(m_pTexSurface2->LockRect(&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK) 
		{
			MessageBox(NULL, "LockRect failed.", "", MB_OK);
		}
		
		BYTE* pBits = (BYTE*)m_lockedRect.pBits;

		int diff=0;
		sum=0;

		//LogDebug("pixel[1]: %d, %d, %d, %d\n", *(pBits),*(pBits+1),*(pBits+2),*(pBits+3));
		for (int i=0;i<128*128;i++)
		{
			diff = *(pBits++) + *(pBits++) + *(pBits++) + *(pBits++);
			sum += diff;

		}

		m_pTexSurface2->UnlockRect();
		
	}		
	g_pd3dDevice->EndScene();
		

*/

#pragma endregion Version Shader

#pragma region Version 2

	/*
	Sum : 7340160
	time : 64 
	diff : 448.007813*/

	//Version 2
	int s1=0;
	D3DLOCKED_RECT m_lockedRect;
	D3DLOCKED_RECT m_lockedRect2;
	for(int x=0;x<1000;x++)
	{
	
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits0 = (BYTE*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits1 = (BYTE*)m_lockedRect2.pBits;

		sum = 0;
		
		for (int i=0;i<128*128*4;i++)
		{
			//LogDebug("%d %d %d %d\n", *(pBits0), *(pBits0+1), *(pBits0+2), *(pBits0+3));
			//LogDebug("%d %d %d %d\n", *(pBits1), *(pBits1+1), *(pBits1+2), *(pBits1+3));
			s1 = (*(pBits0++)) - *(pBits1++);
			sum += abs(s1);
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}

#pragma endregion Version 2

#pragma region Version 3
	
/*
Sum : 7340160
time : 20 
diff : 448.007813

__m128i* Var1;
__m128i* Var2;
__m128i Var3;

	D3DLOCKED_RECT m_lockedRect;
	D3DLOCKED_RECT m_lockedRect2;
	for(int x=0;x<1000;x++)
	{
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var1 = (__m128i*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var2 = (__m128i*)m_lockedRect2.pBits;

		sum =0;
		int scnt = (128*128*4)/16;
		for (int i=0;i<scnt;i++)
		{

			Var3 = _mm_sad_epu8(*(Var1++),*(Var2++));

			//Var3 = _mm_sub_ps(*(Var1),*(Var2));
			//LogDebug("%d %d %d %d\n", (Var3).m128i_u8[0], (Var3).m128i_u8[1], (Var3).m128i_u8[2], (Var3).m128i_u8[3]);
			sum += (Var3).m128i_u16[0] + (Var3).m128i_u16[4];
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}
	*/

#pragma endregion Version 3

	__int64 t2 = timeGetTime();
	LogDebug("Sum : %d\n",sum );
	LogDebug("time : %d \n", t2-t1);
	LogDebug("diff : %f\n", sum /(128.0f*128.0f));
	//D3DXSaveSurfaceToFile("3.bmp",D3DXIFF_BMP,m_pTexSurface2,NULL,NULL);
	//D3DXSaveSurfaceToFile("4.bmp",D3DXIFF_BMP,m_pTexSurface,NULL,NULL);

	//Sleep(2000);
	PostQuitMessage(0);
	//kill=true;
}

