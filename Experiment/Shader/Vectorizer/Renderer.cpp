#include "Renderer.h"

Renderer::Renderer( COMMON* pc)
{
	C = pc;
}

Renderer::~Renderer(void)
{
}

void Renderer::SetVertexBuffer(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer)
{
	mVertexBuffer = pVertexBuffer;
}

void Renderer::init()
{

	
	if(FAILED(C->m_pD3DDevice->CreateTexture(IMAGE_WIDTH, IMAGE_HEIGHT, 1, D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTexRender,NULL)))
	{
		LogError("Failed CreateTexture");
		PostQuitMessage(0);
	}

	if(FAILED(m_pTexRender->GetSurfaceLevel(0, &m_pTexSurface)))
	{
		LogError("Failed GetSurfaceLevel");
		PostQuitMessage(0);
	}

	if(FAILED(C->m_pD3DDevice->CreateOffscreenPlainSurface(	IMAGE_WIDTH,IMAGE_HEIGHT,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_pTexSurface2,NULL)))
	{
		LogError("Failed CreateOffscreenPlainSurface");
		PostQuitMessage(0);
	}

	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DXToRadian( 45.0f ), APP_WIDTH / APP_HEIGHT, 0.1f, 100.0f );
	C->m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	D3DXMatrixIdentity( &g_matView ); // This sample is not really making use of a view matrix


}
//-----------------------------------------------------------------------------
// Name: initEffect()
// Desc: Initializie an Fx effect.
//-----------------------------------------------------------------------------
void Renderer::initEffect( void )
{
	//
	// Create two test textures for our effect to use...
	//
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "noir.bmp", &g_pTexture_0 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile noir.bmp");
	}
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "blanc.bmp", &g_pTexture_1 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile blanc.bmp");
	}
	LPD3DXBUFFER pBufferErrors = NULL;

	if( FAILED(D3DXCreateEffectFromFile( C->m_pD3DDevice, "effect.fx",NULL,NULL, D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, NULL, &g_pEffect, &pBufferErrors )) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LogError("Fx Compile Error: %s ", (const char*)pCompilErrors);
	}
}

void Renderer::Render()
{

	//snag the old color buffer

	IDirect3DSurface9 *oldColorBuffer = 0;

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 2.8f );

	g_matWorld = matTrans;
	C->m_pD3DDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

	
	//RenderShader();
	//RenderCode1();
	//RenderCode2();
	RenderCode3();
	//RenderTriangle();
	RenderTriangleFast();
	//LogMemoryUsage();
}


void Renderer::RenderShader()
{

	#if	USE_ITT
	__itt_event Render0;
	Render0 = __itt_event_create("Render 0", 8 );
	__itt_event_start( Render0 );
	#endif

	t1 = timeGetTime();

	/*
	 Sum : 12760064
	 time : 547 
	 diff : 778.812500
	*/
	g_pEffect->SetTechnique( "Render" );
	g_pEffect->SetTexture( "g_BaseTexture", g_pTexture_0 );
	g_pEffect->SetTexture( "g_BaseTexture2", g_pTexture_1 );

	////D3DXMATRIX m1 = g_matWorld * g_matView *g_matProj;
	D3DXMATRIX m1 = g_matWorld*g_matView *g_matProj;
	g_pEffect->SetMatrix("g_WorldViewProj", &m1);
	g_pEffect->SetTechnique( "Render" );

	if(FAILED(C->m_pD3DDevice->SetRenderTarget(0, m_pTexSurface)))
	{
		LogDebug("Failed SetRenderTarget");
	}
	C->m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,APP_BGCOLOR, 1.0f, 0 );
	C->m_pD3DDevice->BeginScene();
	for (int x=0;x<1000;x++)
	{
		
		UINT uPasses;
		g_pEffect->Begin( &uPasses, 0 );
	    
		for( UINT uPass = 0; uPass < uPasses; ++uPass )
		{
			g_pEffect->BeginPass( uPass );
			C->m_pD3DDevice->SetStreamSource( 0, mVertexBuffer, 0, sizeof(Vertex) );
			C->m_pD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			C->m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			g_pEffect->EndPass();
		}

		C->m_pD3DDevice->GetRenderTargetData(m_pTexSurface,m_pTexSurface2);
		C->m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,APP_BGCOLOR, 1.0f, 0 );

		if(m_pTexSurface2->LockRect(&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK) 
		{
			MessageBox(NULL, "LockRect failed.", "", MB_OK);
		}
		
		BYTE* pBits = (BYTE*)m_lockedRect.pBits;

		int diff=0;
		sum=0;

		//LogDebug("pixel[1]: %d, %d, %d, %d\n", *(pBits),*(pBits+1),*(pBits+2),*(pBits+3));
		for (int i=0;i<IMAGE_WIDTH*IMAGE_HEIGHT;i++)
		{
			diff = *(pBits++) + *(pBits++) + *(pBits++) + *(pBits++);
			sum += diff;

		}

		m_pTexSurface2->UnlockRect();

	}		
	g_pEffect->End();

	C->m_pD3DDevice->EndScene();
	t2 = timeGetTime();
	LogWarning("\nVersion Shader\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(IMAGE_WIDTH*IMAGE_HEIGHT));
	
	#if	USE_ITT
	__itt_event_end( Render0 );
	#endif

}

void Renderer::RenderCode1()
{
	#if	USE_ITT
	__itt_event Render1;
	Render1 = __itt_event_create("Render 1", 8 );
	__itt_event_start( Render1 );
	#endif
	t1 = timeGetTime();

	/*
	Sum : 7340160
	time : 64 
	diff : 448.007813*/

	//Version 2
	int s1=0;

	for(int x=0;x<1000;x++)
	{
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits0 = (BYTE*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits1 = (BYTE*)m_lockedRect2.pBits;

		sum = 0;
		
		for (int i=0;i<IMAGE_WIDTH*IMAGE_WIDTH*4;i++)
		{
			s1 = (*(pBits0++)) - *(pBits1++);
			sum += abs(s1);
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}

	t2 = timeGetTime();
	LogWarning("\nVersion Code 1\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(IMAGE_WIDTH*IMAGE_HEIGHT));
	
	#if	USE_ITT
	__itt_event_end( Render1 );
	#endif
}

void Renderer::RenderCode2()
{
	#if	USE_ITT
	__itt_event Render2;
	Render2 = __itt_event_create("Render 2", 8 );
	__itt_event_start( Render2 );
	#endif
	t1 = timeGetTime();

/*
Sum : 7340160
time : 20 
diff : 448.007813
*/
	__m128i* Var1, *Var2;
	__m128i  Var3;

	for(int x=0;x<1000;x++)
	{
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var1 = (__m128i*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var2 = (__m128i*)m_lockedRect2.pBits;

		sum =0;
		int scnt = (IMAGE_WIDTH*IMAGE_HEIGHT*4)/16;
		for (int i=0;i<scnt;i++)
		{

			Var3 = _mm_sad_epu8(*(Var1++),*(Var2++));
			//Var3 = _mm_sub_ps(*(Var1),*(Var2));
			//LogDebug("%d %d %d %d\n", (Var3).m128i_u8[0], (Var3).m128i_u8[1], (Var3).m128i_u8[2], (Var3).m128i_u8[3]);
			//sum += Var3.m128i_u16[0] + Var3.m128i_u16[4];
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}
	
	t2 = timeGetTime();
	LogWarning("\nVersion Code 2\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(IMAGE_WIDTH*IMAGE_HEIGHT));
	#if	USE_ITT
	__itt_event_end( Render2 );
	#endif
}




void Renderer::RenderCode3()
{
	#if	USE_ITT
	__itt_event Render3;
	Render3 = __itt_event_create("Render 3", 8 );
	__itt_event_start( Render3 );
	#endif

#if USE_IPP
	/*
	Sum : 12533760 
	time : 17 
	diff : 765.000000 
	*/		
	t1 = timeGetTime();
	Ipp8u *Var1,*Var2;
	Ipp8u *Var3 = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
	
	//ippSetNumThreads(2);
	//Regions of interest
	IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT}; //512 = width (128 pixel X 4 bytes) - 128 = height

	Ipp64f sum64;
	/*
	const int TRIANGLE_COUNT=50;

	Ipp8u* Var1[TRIANGLE_COUNT];
	Ipp8u* Var2[TRIANGLE_COUNT];
	Ipp64f sum64[TRIANGLE_COUNT];
*/
	for(int x=0;x<100000;x++)
	{
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY | D3DLOCK_DONOTWAIT) != D3D_OK)  { LogDebug("Failed 1"); }
		Var1 = (Ipp8u *)m_lockedRect.pBits;

		//D3DXSaveTextureToFile("Triangle1.bmp",D3DXIFF_BMP,g_pTexture_1,NULL);
		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
		Var2 = (Ipp8u*)m_lockedRect2.pBits;
		
		ippiAbsDiff_8u_C1R(Var1,FullImageROI.width,Var2,FullImageROI.width,Var3,FullImageROI.width,FullImageROI);
		ippiSum_8u_C1R(Var3,FullImageROI.width,FullImageROI,&sum64);

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}
	t2 = timeGetTime();
	LogWarning("\nVersion Code 3\n");

	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum64 /(IMAGE_WIDTH * IMAGE_HEIGHT));
#else
	LogWarning("Version Code 3 non disponible");
#endif
	#if	USE_ITT
	__itt_event_end( Render3 );
	#endif
}


void Renderer::RenderTriangle()
{
	UINT *Var2;//, *Var3;
	LPDIRECT3DTEXTURE9 g_pTexture ;
	if(FAILED(C->m_pD3DDevice->CreateTexture(IMAGE_WIDTH, IMAGE_HEIGHT, 1, D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&g_pTexture,NULL)))
	{
		LogError("Failed CreateTexture");
		PostQuitMessage(0);
	}

	t1 = timeGetTime();

	UINT pos;
	for(int x=0;x<1000;x++)
	{
		VertexT * v1 = new VertexT(5,5,0);
		VertexT * v2 = new VertexT(5,50,0);
		VertexT * v3 = new VertexT(50,5,0);

		if(g_pTexture->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
		Var2 = (UINT*)m_lockedRect2.pBits;
		pos = (UINT)&*Var2;

		for(int y=0;y<50;y++)
		{
			v1->x +=1;
			v2->x +=1;
			v3->x +=1;
			triangle(*v1,*v2,*v3,Var2 );
			(Var2) = (UINT*)pos;
		}
		
		g_pTexture->UnlockRect(0);
	}
	t2 = timeGetTime();
	LogWarning("RenderTriangle\n");
	Log("time : %d \n", t2-t1);
	D3DXSaveTextureToFile("Triangle.bmp",D3DXIFF_BMP,g_pTexture,NULL);
}

void Renderer::RenderTriangleFast()
{

#if	USE_IPP
	Ipp8u *Var2;//, *Var3;
	LPDIRECT3DTEXTURE9 g_pTexture ;
	if(FAILED(C->m_pD3DDevice->CreateTexture(IMAGE_WIDTH, IMAGE_HEIGHT, 1, D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&g_pTexture,NULL)))
	{
		LogError("Failed CreateTexture");
		PostQuitMessage(0);
	}

	t1 = timeGetTime();

	IppiSize FullImage = {int(IMAGE_WIDTH)<<2,IMAGE_HEIGHT};
	UINT pos;

#if	USE_ITT
	__itt_event Render4;
	Render4 = __itt_event_create("Render 4", 8 );
	__itt_event_start( Render4 );
#endif

	//ippSetNumThreads(1);
	if(g_pTexture->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
	Var2 = (Ipp8u*)m_lockedRect2.pBits;

	//pointer pour ce repositionner
	pos = (UINT)&*Var2;

	//reste a tester
	//#pragma omp parallel for default(none) private(i,j,sum) shared(m,n,a,b,c)

	const int TRIANGLE_COUNT=50;

	Ipp8u* Var3[TRIANGLE_COUNT];
	VertexT * v1[TRIANGLE_COUNT];
	VertexT * v2[TRIANGLE_COUNT];
	VertexT * v3[TRIANGLE_COUNT];

	TriangleMesh *tm[TRIANGLE_COUNT];
//	TriangleMesh *tm2[TRIANGLE_COUNT];
//	TriangleMesh *tm3[16];
	for(int i=0;i<TRIANGLE_COUNT;i++)
	{
		v1[i] = new VertexT(5,5,0);
		v2[i] = new VertexT(5,50,0);
		v3[i] = new VertexT(50,5,0);
		tm[i] = new TriangleMesh();
		//tm2[y] = new TriangleMesh();
		//tm3[y] = new TriangleMesh();
		Var3[i] = (Ipp8u*)pos;
	}

	for(int x=0;x<10000;x++)
	{		
		//Clear background
		ippiSet_8u_C1R(0,Var2,512,FullImage);

 		#pragma omp parallel for num_threads(8)
		for(int y=0;y<TRIANGLE_COUNT;y++)
		{
			v1[y]->x = 5;	v2[y]->x = 5;	v3[y]->x = 50;
			tm[y]->Set(*v1[y],*v2[y],*v3[y],0x33FF0000);
			tm[y]->Rasterize(Var3[y]);			
/*
			v1[y]->x = 25;	v2[y]->x = 25;	v3[y]->x = 70;
			tm2[y]->Set(*v1[y],*v2[y],*v3[y],0x3300FF00);
			tm2[y]->Rasterize(Var3[y]);

			v1[y]->x =45;	v2[y]->x =45;	v3[y]->x =90;
			tm3[y]->Set(*v1[y],*v2[y],*v3[y],0x330000FF);
			tm3[y]->Rasterize(Var3[y]);
*/
			Var3[y]= (Ipp8u*)pos;
		}

		//CheckHeap();
	}
	g_pTexture->UnlockRect(0);
	t2 = timeGetTime();
	

#if	USE_ITT
	__itt_event_end( Render4 );
#endif
	LogWarning("RenderTriangleFast\n");
	LogDebug("time : %d \n", t2-t1);
	D3DXSaveTextureToFile("Triangle.bmp",D3DXIFF_BMP,g_pTexture,NULL);
#endif
}

void Renderer::triangle(VertexT &v1, VertexT &v2, VertexT &v3 , UINT *var)
{
	float y1 = v1.y;
	float y2 = v2.y;
	float y3 = v3.y;

	float x1 = v1.x;
	float x2 = v2.x;
	float x3 = v3.x;

	// Bounding rectangle	
	int minx = (int)min(x1, min(x2, x3));
	int maxx = (int)max(x1, max(x2, x3));
	int miny = (int)min(y1, min (y2, y3));
	int maxy = (int)max(y1, max(y2, y3));

	//LogDebug("%d %d %d %d\n", minx,maxx, miny, maxy);
	//char * colorBuffer = new char[128*128*4];
	int stride = 128 ;
	//(char*&)colorBuffer += miny * stride;
	var += (miny * stride) + minx;
	// Scan through bounding rectangle
	for(int y = miny; y < maxy; y++)
	{
		for(int x = minx; x < maxx; x++)
		{
			if( ((x1 - x2) * (y - y1) - (y1 - y2) * (x - x1) > 0 && 
				 (x2 - x3) * (y - y2) - (y2 - y3) * (x - x2) > 0 && 
				 (x3 - x1) * (y - y3) - (y3 - y1) * (x - x3) > 0))
			{
				(*var) = 0x00555555; // White	
			}
			var++;
		}

		var += stride - maxx + minx;
		//var +=4;
	}
}



void Renderer::Dispose()
{
	SAFERELEASE(g_pTexture_0);
	SAFERELEASE(g_pTexture_1);
	SAFERELEASE(g_pEffect);
}