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
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "checker.bmp", &g_pTexture_0 )))
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

void Renderer::LogImage(int coreIdx, MyImage* img)
{
	LogDebug("--- BEGIN image definition ----\n");
	LogDebug("Image: %d - TriCount: %d - Score: %d\n", coreIdx, img->triCount, img->score);
	for (int i=0;i<img->triCount;i++)
	{
		LogDebug("Tri {[%d,%d], [%d,%d], [%d,%d]} color:{%d,%d,%d,%d} MinMaxY[%d, %d]\n", img->tri[i].v1.x,
														img->tri[i].v1.y,
														img->tri[i].v2.x,
														img->tri[i].v2.y,
														img->tri[i].v3.x,
														img->tri[i].v3.y,
														img->tri[i].Col[3],
														img->tri[i].Col[2],
														img->tri[i].Col[1],
														img->tri[i].Col[0],
														img->tri[i].minY,
														img->tri[i].maxY);
		for (int j=0;j<IMAGE_HEIGHT; j++)
		{
			LogDebug("Span[%d]=[%d, %d]\n",j, img->tri[i].span[j].minX,img->tri[i].span[j].maxX);
		}

	}
	LogDebug("--- END image definition ----\n");
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
	if(g_pTexture_0->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
	Ipp8u* mainTexture = (Ipp8u*)m_lockedRect2.pBits;
	g_pTexture_0->UnlockRect(0);

	if(g_pTexture->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogDebug("Failed 1"); }
	Ipp8u* outTexture = (Ipp8u*)m_lockedRect2.pBits;
	g_pTexture->UnlockRect(0);

	#if	USE_ITT
		__itt_event Render4;
		Render4 = __itt_event_create("Render 4", 8 );
		__itt_event_start( Render4 );
	#endif

	DWORD t1 = timeGetTime();

	DWORD stopTime = t1 + 60*1000;
	bool timeUp = false;

	static IppiSize FullImage = {int(IMAGE_WIDTH)<<2,IMAGE_HEIGHT};
	
	const int CORE_COUNT = 8;
	const DWORD ITERATION_MAX = 100000000;
	
	DWORD bestDiff=0xFFFFFFFF;
	MyImage* coreImage = new MyImage[CORE_COUNT];
	MyImage* bestImg = new MyImage[CORE_COUNT];
	Ipp8u* writeImageBuff[CORE_COUNT];
	Ipp8u* writeDiffImageBuff[CORE_COUNT];
	Ipp8u* writeLineBuff[CORE_COUNT];

	for (int coreIdx=0; coreIdx<CORE_COUNT;coreIdx++)
	{
		ImageUtils::ResetImage(&bestImg[coreIdx]);
		memcpy(&coreImage[coreIdx],&bestImg[coreIdx],sizeof(MyImage));
		writeImageBuff[coreIdx] = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
		writeDiffImageBuff[coreIdx] = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
		writeLineBuff[coreIdx] = (Ipp8u*) ippMalloc(IMAGE_WIDTH*4);
	}

	static IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT}; //512 = width (128 pixel X 4 bytes) - 128 = height
	
	char msg[255];

	LogDebug("Starting Evolution Loop (%d Core * %d Iterations)\n", CORE_COUNT, ITERATION_MAX);
	//Parallel for using all core in a good way
	DWORD curIteration=0;
	#pragma omp parallel for num_threads(CORE_COUNT)
	for (int coreIdx=0;coreIdx<CORE_COUNT;coreIdx++)
	{
		for (DWORD j=0;j<ITERATION_MAX;j++)
		{
			curIteration++;
			//CopyBest
			//LogDebug("Core[%d] - Copy Best Image\n", coreIdx);
			#pragma omp critical
			if (rand()%1500==1)
			{
				memcpy(&coreImage[coreIdx],&bestImg[coreIdx],sizeof(MyImage));
			}

			//Mutate State
			//LogDebug("Core[%d] - Mutate State\n", coreIdx);
			bool dirty = ImageUtils::MutateImage(&coreImage[coreIdx]);

			if (dirty)
			{
				//Render Image
				{
					//Clear background
					//LogDebug("Core[%d] - Clear Background\n", coreIdx);
					//ippiSet_8u_C1R(0,writeImageBuff[coreIdx],512,FullImage);
					memset(writeImageBuff[coreIdx],0,IMAGE_WIDTH*IMAGE_HEIGHT*4);
					
					//Rasterize Image
					//LogDebug("Core[%d] - Rasterize Image\n", coreIdx);
					ImageUtils::RasterizeImage(&coreImage[coreIdx],writeImageBuff[coreIdx], writeLineBuff[coreIdx]);
				}
				
				//Diff Images
				UINT diff = ImageUtils::AbsDiffImage(mainTexture, writeImageBuff[coreIdx],writeDiffImageBuff[coreIdx]);;
				coreImage[coreIdx].score = diff;

				//Evaluate
		
				if (diff < bestImg[coreIdx].score)
				{
					
					//LogDebug("Better solution found: Score=%d\n", diff);
					memcpy(&bestImg[coreIdx],&coreImage[coreIdx], sizeof(MyImage));
					/*
					if (diff < bestDiff*0.9f)
					{
						bestDiff = diff;
						//LogDebug("VERY Better solution: %d\n",bestDiff);
						ImageUtils::RasterizeImage(&bestImg[k],outTexture, writeLineBuff[0]);
						sprintf_s(msg,255,"images/Iteration_%d.jpg",curIteration);
						D3DXSaveTextureToFile(msg,D3DXIFF_BMP,g_pTexture,NULL);
					}
					*/
				}
			}
			if (coreIdx==0)
			{
				if (timeGetTime() > stopTime)
				{
					timeUp = true;
				}
			}
			if (timeUp)
			{
				j = ITERATION_MAX;
			}
		}
	}
	DWORD t2 = timeGetTime();

	#if	USE_ITT
		__itt_event_end( Render4 );
	#endif

	ImageUtils::RasterizeImage(&bestImg[0],outTexture, writeLineBuff[0]);
	
	LogDebug("Core(x%d) -> Iteration(x%d) : TotalIteration(x%d) : time %d\n", CORE_COUNT, ITERATION_MAX, CORE_COUNT*ITERATION_MAX,t2-t1);
	
	D3DXSaveTextureToFile("Z_FINAL_IN.bmp",D3DXIFF_BMP,g_pTexture_0,NULL);
	for (int coreIdx=0; coreIdx<CORE_COUNT;coreIdx++)
	{
			//ippiSet_8u_C1R(0,outTexture,512,FullImage);
			//LogImage(coreIdx,&bestImg[coreIdx]);
			memset(outTexture,0,IMAGE_WIDTH*IMAGE_HEIGHT*4);
			ImageUtils::RasterizeImage(&bestImg[coreIdx],outTexture, writeLineBuff[0]);
			sprintf_s(msg,255,"images/Iteration_%d.jpg",coreIdx);
			D3DXSaveTextureToFile(msg,D3DXIFF_BMP,g_pTexture,NULL);

	}	
	D3DXSaveTextureToFile("Z_FINAL_OUT.bmp",D3DXIFF_BMP,g_pTexture,NULL);

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