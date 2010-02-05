#include "Renderer.h"
#include "ImageUtils.h"

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

	//Randomize
	srand(timeGetTime());
	
	//Start Evolution loop
	EvolutionLoop();
	
}



UINT Renderer::AbsDiffImage(Ipp8u* readBuff1, Ipp8u* readBuff2, Ipp8u* writeBuff)
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
		DWORD t1 = timeGetTime();

		static IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT}; //512 = width (128 pixel X 4 bytes) - 128 = height

		Ipp64f sum64;

		ippiAbsDiff_8u_C1R(	readBuff1,
							FullImageROI.width,
							readBuff2,
							FullImageROI.width,
							writeBuff,
							FullImageROI.width,
							FullImageROI);
		
		ippiSum_8u_C1R(	writeBuff,
						FullImageROI.width,
						FullImageROI,
						&sum64);
		
		DWORD t2 = timeGetTime();

		//LogWarning("\nMultiThreaded ippiAbsDiff_8u_C1R Version\n");
		//Log("time : %d \n", t2-t1);
		//Log("diff : %f\n", sum64/CORE_COUNT);
	#else
		LogWarning("Version Code 3 non disponible");
	#endif
	#if	USE_ITT
		__itt_event_end( Render3 );
	#endif

	return UINT(sum64);
}


void Renderer::EvolutionLoop()
{
	LPDIRECT3DTEXTURE9 g_pTexture;

	//Create texture
	if(FAILED(C->m_pD3DDevice->CreateTexture(IMAGE_WIDTH, IMAGE_HEIGHT, 1, D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&g_pTexture,NULL)))
	{
		LogError("Failed CreateTexture");
		PostQuitMessage(0);
	}

	//Lock Main texture
	if(g_pTexture->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
	Ipp8u* mainTexture = (Ipp8u*)m_lockedRect2.pBits;

	#if	USE_ITT
		__itt_event Render4;
		Render4 = __itt_event_create("Render 4", 8 );
		__itt_event_start( Render4 );
	#endif

	DWORD t1 = timeGetTime();

	static IppiSize FullImage = {int(IMAGE_WIDTH)<<2,IMAGE_HEIGHT};
	
	const int CORE_COUNT = 8;
	const DWORD ITERATION_MAX = 10000;
	

	MyImage* coreImage = new MyImage[CORE_COUNT];
	MyImage bestImg;// = new MyImage;
	ImageUtils::ResetImage(&bestImg);
	Ipp8u* writeImageBuff[CORE_COUNT];
	Ipp8u* writeLineBuff[CORE_COUNT];

	for (int coreIdx=0; coreIdx<CORE_COUNT;coreIdx++)
	{
		writeImageBuff[coreIdx] = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
		writeLineBuff[coreIdx] = (Ipp8u*) ippMalloc(IMAGE_WIDTH*4);
	}

	static IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT}; //512 = width (128 pixel X 4 bytes) - 128 = height

	LogDebug("Starting Evolution Loop (%d Core * %d Iterations)\n", CORE_COUNT, ITERATION_MAX);
	//Parallel for using all core in a good way
	#pragma omp parallel for num_threads(CORE_COUNT)
	for (int coreIdx=0;coreIdx<CORE_COUNT;coreIdx++)
	{
		for (DWORD j=0;j<ITERATION_MAX;j++)
		{
			//CopyBest
			//LogDebug("Core[%d] - Copy Best Image\n", coreIdx);
			memcpy(&coreImage[coreIdx],&bestImg,sizeof(bestImg));

			//Mutate State
			//LogDebug("Core[%d] - Mutate State\n", coreIdx);
			ImageUtils::MutateImage(&coreImage[coreIdx]);

			//Render Image
			{
				//Clear background
				//LogDebug("Core[%d] - Clear Background\n", coreIdx);
				ippiSet_8u_C1R(0,writeImageBuff[coreIdx],512,FullImage);
				
				//Rasterize Image
				//LogDebug("Core[%d] - Rasterize Image\n", coreIdx);
				ImageUtils::RasterizeImage(&coreImage[coreIdx],mainTexture, writeLineBuff[coreIdx]);//writeImageBuff[coreIdx]);
			}
			
			//Diff Images

			UINT diff = 0xFFFFFFFF;
			
			//Evaluate
			if (diff < coreImage[coreIdx].score)
			{
				LogDebug("Better solution found: Score=%d", diff);
				memcpy(&bestImg,&coreImage[coreIdx], sizeof(bestImg));
			}
		}
	}
	DWORD t2 = timeGetTime();

	#if	USE_ITT
		__itt_event_end( Render4 );
	#endif

	LogDebug("Core(x%d) -> Iteration(x%d) : TotalIteration(x%d) : time %d\n", CORE_COUNT, ITERATION_MAX, CORE_COUNT*ITERATION_MAX,t2-t1);
	g_pTexture->UnlockRect(0);
	D3DXSaveTextureToFile("ZZZZZZ.bmp",D3DXIFF_BMP,g_pTexture,NULL);

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