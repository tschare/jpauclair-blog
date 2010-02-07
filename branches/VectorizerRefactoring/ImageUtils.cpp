#include "ImageUtils.h"

#define RangeRand(x) (rand() % (x))
#define IntervalRand(a,b) ((rand() % (b-a + 1)) + a)
#define DoMutate(x) ((rand() % Settings::x) == 1)
#define R255 (rand() % 256)

namespace Settings
{
    int ScreenWidth;
    int ScreenHeight;

    int MaxPoints = 10;
    int MinPoints = 3;
    int MinPolygons = 3;
    int MaxPolygons = 150;
    int AlphaMin = 20;
    int AlphaMax = 120;
    int PointMidMovement = 20;
    int PointSmallMovement = 3;

    // mutations
    int ComponentMutation = 1500;
    int AddPolyMutation = 700;
    int MovePolyMutation = 700;
    int DelPolyMutation = 1500;
    int AddPointMutation = 1500;
    int DelPointMutation = 1500;
    int AlphaMutation = 1500;
    int PointMinMutation = 1500;
    int PointMidMutation = 1500;
    int PointMaxMutation = 1500;

    // flags
};


ImageUtils::ImageUtils(void)
{
}

ImageUtils::~ImageUtils(void)
{
}

void ImageUtils::ResetTriangle(MyTriangle* tri)
{
		tri->v1.x = rand()%126+1;	tri->v1.y = rand()%126+1;
		tri->v2.x = rand()%126+1;	tri->v2.y = rand()%126+1;	
		tri->v3.x = rand()%126+1;	tri->v3.y = rand()%126+1;
		tri->Col[0] = R255;
		tri->Col[1] = R255;
		tri->Col[2] = R255;
		tri->Col[3] = R255;

		for(int j=0;j<IMAGE_HEIGHT;j++)
		{
			tri->span[j].minX = 0xFFFFFFFF;
			tri->span[j].maxX = 0;
		}

		tri->size.height = 1;
		tri->size.width = IMAGE_WIDTH;
		tri->minY = 0;
		tri->maxY = IMAGE_HEIGHT;
}

void ImageUtils::ResetImage(MyImage* img)
{
	img->triCount = 10;
	img->score = 0xFFFFFFFF;
	for(int i=0;i<150;i++)
	{
		ResetTriangle(&img->tri[i]);
	}
}

bool ImageUtils::MutateVertex(VertexT* v)
{
	bool dirty = false;
    if (DoMutate(PointMaxMutation)) 
	{
		v->x = (RangeRand(IMAGE_WIDTH));
		v->y = (RangeRand(IMAGE_HEIGHT));
        dirty = true;
    }
    if (DoMutate(PointMidMutation)) 
	{
		v->x += RangeRand(Settings::PointMidMovement) * 2 - Settings::PointMidMovement;
		v->y += RangeRand(Settings::PointMidMovement) * 2 - Settings::PointMidMovement;
        dirty = true;
    }
    if (DoMutate(PointMinMutation)) 
	{
		v->x += RangeRand(Settings::PointSmallMovement) * 2 - Settings::PointSmallMovement;
		v->y += RangeRand(Settings::PointSmallMovement) * 2 - Settings::PointSmallMovement;
        dirty = true;
    }
    if (v->x < 0) v->x = 0;
    if (v->y < 0) v->y = 0;
    if (v->x >= IMAGE_WIDTH) v->x = IMAGE_WIDTH-1;
	if (v->y >= IMAGE_HEIGHT) v->y = IMAGE_HEIGHT-1;

	return dirty;
}

bool ImageUtils::MutateTriangle(MyTriangle* tri)
{
	bool dirty = false;
	if (DoMutate(AlphaMutation))
	{
		tri->Col[3] = IntervalRand(Settings::AlphaMin, Settings::AlphaMax); // A
		dirty |= true; 
	}

	if (DoMutate(ComponentMutation)) { tri->Col[2] = R255; dirty |= true; } // R
	if (DoMutate(ComponentMutation)) { tri->Col[1] = R255; dirty |= true; } // G
	if (DoMutate(ComponentMutation)) { tri->Col[0] = R255; dirty |= true; } // B
	
	dirty |= MutateVertex(&tri->v1);
	dirty |= MutateVertex(&tri->v2);
	dirty |= MutateVertex(&tri->v3);

	return dirty;


}

bool ImageUtils::MutateImage(MyImage* img)
{
	bool dirty = false;

    if (DoMutate(AddPolyMutation))
	{
		if (img->triCount < 150)
		{
			
			ResetTriangle(&img->tri[img->triCount]);
			img->triCount++;
			dirty |= true;
			//LogDebug("AddPoly: %d\n", img->triCount);
		}
	}
    if (DoMutate(DelPolyMutation))
	{
		if (img->triCount > 1)
		{
			int delPos = RangeRand(img->triCount);
			memcpy(	&img->tri[delPos],
					&img->tri[delPos+1],
					sizeof(MyTriangle)*(img->triCount-delPos));

			img->triCount--;
			dirty |= true;
			//LogDebug("DelPoly: %d\n", img->triCount);
		}
	}
    if (DoMutate(MovePolyMutation))
	{
		int origpos = RangeRand(img->triCount);
		int newpos  = RangeRand(img->triCount);
		img->triCount++;
		if (newpos != origpos)
		{
			MyTriangle t = img->tri[origpos];
			img->tri[origpos] = img->tri[newpos];
			img->tri[newpos] = t;
			
			dirty |= true;
			//LogDebug("SwapPoly(%d, %d)\n", origpos, newpos);
		}
	}
	
	for(int i=0;i<img->triCount;i++)
	{

		dirty |= MutateTriangle(&img->tri[i]);
	}
	
	//Update Mesh Definition
	if (dirty)
	{
		ImageUtils::UpdateImage(img);
	}

	return dirty;
}

void ImageUtils::UpdateImage(MyImage* img)
{
	//img->triCount=1;
	for(int i=0;i<img->triCount;i++)
	{
		/**
		img->tri[0].v1.x = 22; img->tri[0].v1.y = 10;
		img->tri[0].v2.x =  42; img->tri[0].v2.y = 1;
		img->tri[0].v3.x = 27; img->tri[0].v3.y = 124;
		/*
		img->tri[1].v1.x = 5; img->tri[1].v1.y = 5;
		img->tri[1].v2.x =  5; img->tri[1].v2.y = 45;
		img->tri[1].v3.x = 35; img->tri[1].v3.y = 75;
		*/

		img->tri[i].minY = xmin(img->tri[i].v1.y, xmin(img->tri[i].v2.y, img->tri[i].v3.y));
		img->tri[i].maxY = xmax(img->tri[i].v1.y, xmax(img->tri[i].v2.y, img->tri[i].v3.y));
		
		for (int j = 0; j < IMAGE_HEIGHT ; j++)
		{
			img->tri[i].span[j].minX = IMAGE_WIDTH+1;
			img->tri[i].span[j].maxX = -1;
		}
		

		

		eflaE(	img->tri[i].v1.x,
				img->tri[i].v1.y,
				img->tri[i].v2.x,
				img->tri[i].v2.y,
				(SpanBorder*)img->tri[i].span);

		eflaE(	img->tri[i].v3.x,
				img->tri[i].v3.y,
				img->tri[i].v2.x,
				img->tri[i].v2.y,
				(SpanBorder*)img->tri[i].span);

		eflaE(	img->tri[i].v1.x,
				img->tri[i].v1.y,
				img->tri[i].v3.x,
				img->tri[i].v3.y,
				(SpanBorder*)img->tri[i].span);


	}
}

void ImageUtils::RasterizeImage(MyImage* img, Ipp8u* dest, Ipp8u* writeLineBuff)
{
	for(int i=0;i<img->triCount;i++)
	{
		RasterizeTriangle(&(img->tri[i]), dest, writeLineBuff);
	}
}


void ImageUtils::RasterizeTriangle(MyTriangle* tri,Ipp8u* dest, Ipp8u* writeLineBuff)
{
	tri->size.height = 1;
	/*
	ippiSet_8u_C4R(	tri->Col,
				writeLineBuff,
				128,
				tri->size);
*/
	for (int i = tri->minY; i < tri->maxY ; i++)
	{
		const int WIDTH_512 = 512;
/**/
		for (int j=tri->span[i].minX; j< tri->span[i].maxX; j++)
		{
			
			UINT color = *(((UINT*)dest)+((i * IMAGE_WIDTH) + (j)));
			UINT newColor=0;

			double Alpha = (double)tri->Col[3]/256.0f;
			double AlphaI = 1.0f - Alpha;

			newColor = 0xFF << 24;
			newColor += UINT(AlphaI*(double)((color >> 16) & 0xFF) + Alpha*(double)(tri->Col[2] << 16));
			newColor += UINT(AlphaI*(double)((color >>  8) & 0xFF) + Alpha*(double)(tri->Col[1] <<  8));
			newColor += UINT(AlphaI*(double)((color      ) & 0xFF) + Alpha*(double)(tri->Col[0]));
			//newColor=0xFFFF0000;
			*(((UINT*)dest)+((i * IMAGE_WIDTH) + (j))) = newColor;
		}
		
		/**


		tri->size.width = (tri->span[i].maxX - tri->span[i].minX)<<2;
		
		//Combines two images using constant alpha values.
		ippiAlphaCompC_8u_C1R(	writeLineBuff,
								WIDTH_512,
								tri->Col[3],
								dest+((i * WIDTH_512) + (tri->span[i].minX<<2)),
								WIDTH_512,
								0xFF-tri->Col[3],
								dest+((i * WIDTH_512) + (tri->span[i].minX<<2)),
								WIDTH_512,
								tri->size,
								ippAlphaPlus);
		/**/
		

	}				
}

UINT ImageUtils::AbsDiffImage(Ipp8u* readBuff1, Ipp8u* readBuff2, Ipp8u* writeBuff)
{
	#if	USE_ITT
		__itt_event Render3;
		Render3 = __itt_event_create("Render 3", 8 );
		__itt_event_start( Render3 );
	#endif

	/**
	static IppiSize FullImageROI = {IMAGE_WIDTH*4,IMAGE_HEIGHT};

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
	**/

	DWORD diff=0;
	Ipp8u* src = readBuff1;
	Ipp8u* imgDest = readBuff2;

	for(UINT pos=0;pos < IMAGE_WIDTH*IMAGE_HEIGHT*4; pos++)
	{
		diff += abs(*src - *imgDest);
		src++;
		imgDest++;
	}
	

	#if	USE_ITT
		__itt_event_end( Render3 );
	#endif

	return UINT(diff);
}


void ImageUtils::efla(int x, int y , int x2 , int y2, SpanBorder* span)
{
	int shortLen = y2-y;
	int longLen = x2-x;

	bool yLonger = false;

	if ((shortLen ^ (shortLen >> 31)) - (shortLen >> 31) > (longLen ^ (longLen >> 31)) - (longLen >> 31))
	{
		shortLen ^= longLen;
		longLen ^= shortLen;
		shortLen ^= longLen;

		yLonger = true;
	}
	else
	{
		yLonger = false;
	}

	int  inc = (longLen < 0) ? -1 : 1;

	float multDiff = (longLen == 0) ? shortLen : (shortLen / longLen);
	int xFinal = 0;

	if (yLonger)
	{
		for (int i = 0; i != longLen; i += inc)
		{
			xFinal = x + i * multDiff;
			
			if (xFinal < span[y+i].minX)
			{
				span[y+i].minX = xFinal;
			}
			else if (xFinal > span[y+i].maxX)
			{
				span[y+i].maxX = xFinal;
			}
		}
	}
	else
	{
		for (int i = 0; i != longLen; i += inc)
		{
			xFinal = x + i;
			
			if (xFinal < span[(int)(y+i * multDiff)].minX)
			{
				span[(int)(y+i * multDiff)].minX = xFinal;
			}
			else if (xFinal > span[(int)(y+i * multDiff)].maxX)
			{
				span[(int)(y+i * multDiff)].maxX = xFinal;
			}					
		}
	}
}

void ImageUtils::eflaE(int x, int y, int x2, int y2, SpanBorder* span) 
{
   	bool yLonger=false;
	int shortLen=y2-y;
	int longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;				
		yLonger=true;
	}
	int decInc;
	if (longLen==0) decInc=0;
	else decInc = (shortLen << 16) / longLen;

	if (yLonger) {
		if (longLen>0) {
			longLen+=y;
			for (int j=0x8000+(x<<16);y<=longLen;++y) {
				int xFinal = j>>16;
				
				if (xFinal < span[y].minX)
				{
					span[y].minX = xFinal;
				}
				if (xFinal > span[y].maxX)
				{
					span[y].maxX = xFinal;
				}
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
			int xFinal = j>>16;
			
			if (xFinal < span[y].minX)
			{
				span[y].minX = xFinal;
			}
			if (xFinal > span[y].maxX)
			{
				span[y].maxX = xFinal;
			}
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
			int xFinal = x;
			
			if (xFinal < span[j>>16].minX)
			{
				span[j>>16].minX = xFinal;
			}
			if (xFinal > span[j>>16].maxX)
			{
				span[j>>16].maxX = xFinal;
			}
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
			int xFinal = x;
			
			if (xFinal < span[j>>16].minX)
			{
				span[j>>16].minX = xFinal;
			}
			if (xFinal > span[j>>16].maxX)
			{
				span[j>>16].maxX = xFinal;
			}
		j-=decInc;
	}

}

