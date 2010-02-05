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

void cImage::CrossOver( cImage *aImages , std::vector<cImage*> *Vec)
{
	float n = getRandomMinMax(0,1);
	
	if(n < 0.7)
	{
		n = getRandomMinMax(0,mTriangleCount);
		//swap triangles
	}else
	{
		Vec->push_back(this);
		Vec->push_back(aImages);
	}
}
