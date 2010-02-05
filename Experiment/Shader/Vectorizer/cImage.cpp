#include "cImage.h"

cImage::cImage(COMMON *pC)
{
	C = pC;
	mRender= __itt_event_create("Render", 6 );
	mGenerate= __itt_event_create("Generate", 8 );
	mCompare= __itt_event_create("Compare", 7 );
	mInit= __itt_event_create("Init Image", 10 );
	
}

cImage::~cImage(void)
{
}

void cImage::Init(UINT aTriangleCount, int aWidth,int aHeigth)
{
	__itt_event_start( mInit );
	mWidth = aWidth;
	mHeight = aHeigth;
	FullImageROI.width = mWidth<<2;
	FullImageROI.height = mHeight;
	mTriangleCount = aTriangleCount;
	mTriangles = new TriangleMesh[mTriangleCount];

	//allocation de la mémoire de l'image
	mImageData = (Ipp8u*) ippMalloc(IMAGE_WIDTH*IMAGE_HEIGHT*4);
	ippiSet_8u_C1R(0,mImageData,FullImageROI.width,FullImageROI);

	mPixelCount = mWidth*mHeight*4;
	mDestinations = new Ipp8u*[mTriangleCount];
	for (int x=0;x<mTriangleCount;x++)
	{
		mDestinations[x] = (Ipp8u*)mImageData;
	}

	mCompareBuffer =  (Ipp8u*) ippMalloc(mWidth*mHeight*4);
	__itt_event_end( mInit );
	
}

void cImage::GenerateRandom()
{
	__itt_event_start( mGenerate );
	VertexT v1,v2,v3;
	UINT color;
	for(int i=0;i<mTriangleCount;i++)
	{
		v1.x = rand()%126+1;	v2.x = rand()%126+1;	v3.x = rand()%126+1;
		v1.y = rand()%126+1;	v2.y = rand()%126+1;	v3.y = rand()%126+1;
		color = ((rand()%256)) + ((rand()%256)<<8) + ((rand()%256)<<16) +((rand()%256)<<24);
		mTriangles[i].Set(v1,v2,v3,color);
	}
	__itt_event_end( mGenerate );
}

void cImage::RenderImage()
{
	__itt_event_start( mRender );

	//clear background
	ippiSet_8u_C1R(0,mImageData,FullImageROI.width,FullImageROI);

	UINT pos = (UINT)&*mImageData;

	//#pragma omp parallel for// num_threads(8)
	for(int y=0;y<mTriangleCount;y++)
	{
		mTriangles[y].Rasterize(mDestinations[y]);
		mDestinations[y]= (Ipp8u*)pos;
	}

	__itt_event_end( mRender );

}
void cImage::SaveImage(char *aImageName)
{
	Log("SaveImage %s", aImageName);
	D3DLOCKED_RECT m_lockedRect;
	Ipp8u *Var2;//, *Var3;
	LPDIRECT3DTEXTURE9 g_pTexture ;
	if(FAILED(C->m_pD3DDevice->CreateTexture(mWidth, mHeight, 1, D3DUSAGE_DYNAMIC,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&g_pTexture,NULL)))
	{
		LogError("Failed CreateTexture");
		return;
	}

	//ippSetNumThreads(1);
	if(g_pTexture->LockRect(0,&m_lockedRect, NULL, D3DLOCK_DISCARD) != D3D_OK)  { LogError("LockRect"); }
	Var2 = (Ipp8u*)m_lockedRect.pBits;

	ippsCopy_8u(mImageData,Var2,mPixelCount);
	g_pTexture->UnlockRect(0);

	if(FAILED(D3DXSaveTextureToFile(aImageName,D3DXIFF_JPG,g_pTexture,NULL))) { LogError("failed D3DXSaveTextureToFile"); }
	
}

void cImage::CompareTo( Ipp8u* aModel8u)
{
	
	__itt_event_start( mCompare );
	ippiAbsDiff_8u_C1R(aModel8u,FullImageROI.width,mImageData,FullImageROI.width,mCompareBuffer,FullImageROI.width,FullImageROI);
		
	ippiSum_8u_C1R(mCompareBuffer,FullImageROI.width,FullImageROI,&mModelDiff);

	__itt_event_end( mCompare );
	
}

//UINT result = RenderCode3((Ipp8u*)pos, writeBuff);
//UINT Renderer::RenderCode3(Ipp8u* readBuff, Ipp8u* writeBuff)
//{
	/*
	#if	USE_ITT
	__itt_event Render3;
	Render3 = __itt_event_create("Render 3", 8 );
	__itt_event_start( Render3 );
	#endif

#if USE_IPP
	
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

	*/
	//return 0;
//}
