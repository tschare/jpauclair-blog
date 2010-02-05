#include "Renderer.h"

Renderer::Renderer( COMMON* pc)
{
	C = pc;
}

Renderer::~Renderer(void)
{
}
#define NB_IMAGES 50
void Renderer::init()
{

	//Create number of image

	//Init and generate first set of data
	//#pragma omp parallel for //num_threads(CORE_COUNT)
	for(int x=0;x<NB_IMAGES;x++)
	{
		cImage *Images = new cImage(C);
		Images->Init(50,128,128);
		Images->GenerateRandom();
		mImages.push_back(Images);
	}

	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "Mona_Lisa.bmp", &ModelTexture )))
	{
		LogError("Failed to D3DXCreateTextureFromFile noir.bmp");
	}

	D3DLOCKED_RECT m_lockedRect;
	if(ModelTexture->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); } //D3DLOCK_DONOTWAIT

	mModelData = (Ipp8u *)m_lockedRect.pBits;
	ModelTexture->UnlockRect(0);

}


void Renderer::Render()
{

	char msg[255];

	t1 = timeGetTime();
	Ipp64f TotalFitness=0;
	//#pragma omp parallel for //num_threads(CORE_COUNT)
	for(int x=0;x<NB_IMAGES;x++)
	{
		mImages[x]->RenderImage();
		//sprintf_s(msg,255,"images/%d.jpg",x);
		//mImages[x]->SaveImage(msg);
		mImages[x]->CompareTo(mModelData);
		mImages[x]->mFitnessMin = TotalFitness;
		TotalFitness += mImages[x]->mModelDiff;
		mImages[x]->mFitnessMax = TotalFitness;
	}

	//http://www.ai-junkie.com/ga/intro/gat2.html

	std::vector<cImage*> m_vec;
	while (m_vec.size() < 50)
	{

		float WheelSel = getRandomMinMax(0,TotalFitness);
		int sel1=0;

		for(int x=0;x<NB_IMAGES;x++)
		{
			if((mImages[x]->mFitnessMin < WheelSel) && (mImages[x]->mFitnessMax >= WheelSel))
			{
				sel1=x;
				break;
			}
		}

		WheelSel = getRandomMinMax(0,TotalFitness);
		int sel2=0;

		for(int x=0;x<NB_IMAGES;x++)
		{
			if((mImages[x]->mFitnessMin < WheelSel) && (mImages[x]->mFitnessMax >= WheelSel))
			{
				sel2=x;
				break;
			}
		}

		mImages[sel1]->CrossOver(mImages[sel2],&m_vec);
		//PostQuitMessage(0);
	}

	mImages.clear();

	//nouvelles images
	LogError("%d\n", m_vec.size());

	/*
	Here, parse the NB_IMAGES ( mImages[x].mModelDiff ) and find any suitable, 
	
	1- Keep [x] best image, without modification
	2- crossover maybe [X] pair of image to create [X] child,  (switch triangles between images)
		P1 = a b c d
		p2 = x y z w

		c1 = x b c w
		c2 = a y z d

	3- mutation rate [x] on [x] parent.
	4- create [x] new image?

	Then remove PostQuitMessage, and render will automatically be called, forever, and ever until the judgment day. or memory corruption.
	
	*/

	t2 = timeGetTime();
	Log("Process time %d", t2-t1);
	PostQuitMessage(0);
}



void Renderer::Dispose()
{
	
}