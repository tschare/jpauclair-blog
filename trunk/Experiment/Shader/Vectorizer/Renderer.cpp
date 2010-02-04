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
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "Mona_Lisa.bmp", &g_pTexture_0 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile noir.bmp");
	}
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "blanc.bmp", &g_pTexture_1 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile blanc.bmp");
	}
	for (int i=0;i<8;i++)
	{
		if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "blanc.bmp", &g_pTexture_X[i] )))
		{
			LogError("Failed to D3DXCreateTextureFromFile blanc.bmp");
		}
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




UINT Renderer::RenderCode3(Ipp8u* readBuff, Ipp8u* writeBuff)
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
	Ipp8u *Tmp;//Var1,*Var2;
	
	
	//ippSetNumThreads(2);
	//Regions of interest
	IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT}; //512 = width (128 pixel X 4 bytes) - 128 = height

	//Ipp64f sum64;
	
	const int CORE_COUNT=1;

	Ipp8u* Var1[CORE_COUNT];
	Ipp8u* Var2[CORE_COUNT];
	Ipp8u *Var3[CORE_COUNT]; 
	Ipp64f sum64[CORE_COUNT];

	UINT pos;

	if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); } //D3DLOCK_DONOTWAIT
	
	Tmp = (Ipp8u *)m_lockedRect.pBits;
	pos = (UINT)&*Tmp;
	for(int i=0;i<CORE_COUNT;i++)
	{
		Var1[i] = (Ipp8u*)pos;
		Var3[i] = writeBuff; //(Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
	}

	for(int x=0;x<1;x++)
	{
		//MultiThreading loop
		//#pragma omp parallel for num_threads(CORE_COUNT)
		for (int j=0;j<CORE_COUNT;j++)
		{
			//D3DXSaveTextureToFile("Triangle1.bmp",D3DXIFF_BMP,g_pTexture_1,NULL);
			//if(g_pTexture_X[j]->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
			Var2[j] = readBuff;//(Ipp8u*)m_lockedRect2.pBits;
			
			ippiAbsDiff_8u_C1R(Var1[j],FullImageROI.width,Var2[j],FullImageROI.width,Var3[j],FullImageROI.width,FullImageROI);
			ippiSum_8u_C1R(Var3[j],FullImageROI.width,FullImageROI,&sum64[j]);

			
			g_pTexture_X[j]->UnlockRect(0);
		}
	}
	g_pTexture_0->UnlockRect(0);

	Ipp64f sum64Final=0.0f;
	for(int i=0;i<CORE_COUNT;i++)
	{
		sum64Final += sum64[i];
	}
	t2 = timeGetTime();
	//LogWarning("\nMultiThreaded ippiAbsDiff_8u_C1R Version\n");

	//Log("time : %d \n", t2-t1);
	//Log("diff : %f\n", sum64Final/CORE_COUNT);
#else
	LogWarning("Version Code 3 non disponible");
#endif
	#if	USE_ITT
	__itt_event_end( Render3 );
	#endif

	return UINT((double)sum64Final /(double)CORE_COUNT);
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
	MyTriangle tri[TRIANGLE_COUNT];
	

	//tri[0].v1
	VertexT * v1[TRIANGLE_COUNT];
	VertexT * v2[TRIANGLE_COUNT];
	VertexT * v3[TRIANGLE_COUNT];
	
	MyImage img;
	MyImage bestImg;

	img.score = 0xFFFFFFFA;
	TriangleMesh *tm[TRIANGLE_COUNT];

	for(int i=0;i<TRIANGLE_COUNT;i++)
	{

		img.tri[i].v1.x = rand()%126+1;	img.tri[i].v2.x = rand()%126+1;	img.tri[i].v3.x = rand()%126+1;
		img.tri[i].v1.y = rand()%126+1;	img.tri[i].v2.y = rand()%126+1;	img.tri[i].v3.y = rand()%126+1;
		img.tri[i].color = ((rand()%256)) + ((rand()%256)<<8) + ((rand()%256)<<16) +((rand()%256)<<24);

		v1[i] = new VertexT(5,5,0);
		v2[i] = new VertexT(5,50,0);
		v3[i] = new VertexT(50,5,0);
		tm[i] = new TriangleMesh();

		Var3[i] = (Ipp8u*)pos;
	}

	memcpy(&bestImg,&img,sizeof(bestImg));

	srand(timeGetTime());
	bestImg.score = 0xFFFFFFF;
	Ipp8u *writeBuff = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
	for(UINT x=0;x<0xFFFFFFFF;x++)
	{		
		//Clear background
		ippiSet_8u_C1R(0,Var2,512,FullImage);
		memcpy(&img,&bestImg,sizeof(bestImg));
 		//#pragma omp parallel for// num_threads(8)
		for(int y=0;y<TRIANGLE_COUNT;y++)
		{
			if (rand()%5==0)
			{
				img.tri[y].v1.x = rand()%126+1;	img.tri[y].v2.x = rand()%126+1;	img.tri[y].v3.x = rand()%126+1;
				img.tri[y].v1.y = rand()%126+1;	img.tri[y].v2.y = rand()%126+1;	img.tri[y].v3.y = rand()%126+1;
				img.tri[y].color = ((rand()%256)) + ((rand()%256)<<8) + ((rand()%256)<<16) +((rand()%256)<<24);

				tm[y]->Set(img.tri[y].v1, img.tri[y].v2, img.tri[y].v3, img.tri[y].color);
			}
			tm[y]->Rasterize(Var3[y]);			
			Var3[y]= (Ipp8u*)pos;
			
			
		}
		UINT result = RenderCode3((Ipp8u*)pos, writeBuff);
		img.score = result;
		if (result < bestImg.score)
		{
			memcpy(&bestImg,&img,sizeof(img));
			bestImg.score = img.score;
			LogDebug("SPECIAL  %d - Diff: %d - Now:%d sizeof()%d\n", x, bestImg.score,result,sizeof(bestImg));
			g_pTexture->UnlockRect(0);
			char fileName[100];
			sprintf(fileName, "result\\Triangle_%d.bmp", x);
			D3DXSaveTextureToFile(fileName,D3DXIFF_BMP,g_pTexture,NULL);
		}
		if (x%10000 == 0)
		{
			LogDebug("Iteration %d - Diff: %d - Now:%d sizeof()%d\n", x, bestImg.score,result,sizeof(bestImg));

			
			
		}
		//CheckHeap();
	}
	
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
	for (int i=0;i<8;i++)
	{
		SAFERELEASE(g_pTexture_X[i]);
	}
	SAFERELEASE(g_pEffect);
}