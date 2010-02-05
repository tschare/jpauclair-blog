#include "ImageUtils.h"

ImageUtils::ImageUtils(void)
{
}

ImageUtils::~ImageUtils(void)
{
}

void ImageUtils::ResetImage(MyImage* img)
{
	img->triCount = 50;
	img->score = 0xFFFFFFFF;
	for(int i=0;i<150;i++)
	{
		img->tri[i].v1.x=0; img->tri[i].v1.y=0;
		img->tri[i].v2.x=0;	img->tri[i].v2.y=0;
		img->tri[i].v3.x=0;	img->tri[i].v3.y=0;
		for(int j=0;j<256;j++)
		{
			img->tri[i].span[j].minX = 0xFFFFFFFF;
			img->tri[i].span[j].maxX = 0;
		}
		img->tri[i].size.height = 0;
		img->tri[i].size.width = 0;
		img->tri[i].minY = 0;
		img->tri[i].maxY = 256;
		img->tri[i].dirty = true;
		img->tri[i].color = 0xFFFFFFFF;
		img->tri[i].Col[0] = 0xFF;
		img->tri[i].Col[1] = 0xFF;
		img->tri[i].Col[2] = 0xFF;
		img->tri[i].Col[3] = 0xFF;
	}
}



void ImageUtils::MutateImage(MyImage* img)
{
	for(int i=0;i<img->triCount;i++)
	{
		//Set 3 random triangle points
		img->tri[i].v1.x = rand()%126+1;	img->tri[i].v1.y = rand()%126+1;
		img->tri[i].v2.x = rand()%126+1;	img->tri[i].v2.y = rand()%126+1;	
		img->tri[i].v3.x = rand()%126+1;	img->tri[i].v3.y = rand()%126+1;
		
		UINT color = (rand()%256);
		color += (rand()%256) << 8;
		color += (rand()%256) << 16;
		color += (rand()%256) << 24;
		//Set random Color
		img->tri[i].color =  color;

		
		img->tri[i].Col[3] = (color >> 24) & 0xFF;
		img->tri[i].Col[2] = (color >> 16) & 0xFF;
		img->tri[i].Col[1] = (color >>8) & 0xFF;
		img->tri[i].Col[0] =  color & 0xFF;
		
	}
	
	//Update Mesh Definition
	ImageUtils::UpdateImage(img);
}

void ImageUtils::UpdateImage(MyImage* img)
{
	
	for(int i=0;i<img->triCount;i++)
	{

		if (img->tri[i].dirty)
		{
			
			img->tri[i].minY = xmin(img->tri[i].v1.y, xmin(img->tri[i].v2.y, img->tri[i].v3.y));
			img->tri[i].maxY = xmax(img->tri[i].v1.y, xmax(img->tri[i].v2.y, img->tri[i].v3.y));
			img->tri[i].dirty = false;
			
			for (int j = 0; j < IMAGE_HEIGHT ; j++)
			{
				img->tri[i].span[j].minX = IMAGE_WIDTH;
				img->tri[i].span[j].maxX = 0;
			}
			

			eflaE(	img->tri[i].v1.x,
					img->tri[i].v1.y,
					img->tri[i].v2.x,
					img->tri[i].v2.y,
					(SpanBorder*)img->tri[i].span);

			eflaE(	img->tri[i].v2.x,
					img->tri[i].v2.y,
					img->tri[i].v3.x,
					img->tri[i].v3.y,
					(SpanBorder*)img->tri[i].span);

			eflaE(	img->tri[i].v1.x,
					img->tri[i].v1.y,
					img->tri[i].v3.x,
					img->tri[i].v3.y,
					(SpanBorder*)img->tri[i].span);
		}
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
	for (int i = tri->minY; i < tri->maxY ; i++)
	{
		tri->size.width = IMAGE_WIDTH;
		tri->size.height = 1;

		const int WIDTH_512 = 512;
/**/
		for (int j=tri->span[i].minX; j< tri->span[i].maxX; j++)
		{
			
			UINT color = *(((UINT*)dest)+((i * IMAGE_WIDTH) + (j)));
			UINT newColor=0;

			double Alpha = (double)tri->Col[3]/256.0f;
			double AlphaI = 1.0f - Alpha;

			newColor = 0xFF << 24;
			newColor += UINT(AlphaI*((color >> 16) & 0xFF) + Alpha*tri->Col[2]) << 16;
			newColor += UINT(AlphaI*((color >>  8) & 0xFF) + Alpha*tri->Col[1]) <<  8;
			newColor += UINT(AlphaI*((color      ) & 0xFF) + Alpha*tri->Col[0]);
			
			*(((UINT*)dest)+((i * IMAGE_WIDTH) + (j))) = newColor;
		}
		
		/**/


		/**

		ippiSet_8u_C4R(	tri->Col,
						writeLineBuff,
						128,
						tri->size);

		//Combines two images using constant alpha values.
		ippiAlphaCompC_8u_C1R(	writeLineBuff,
								WIDTH_512,
								128,
								dest+((i * WIDTH_512) + (tri->span[i].minX<<2)),
								WIDTH_512,
								0xFF-128,
								dest+((i * WIDTH_512) + (tri->span[i].minX<<2)),
								WIDTH_512,
								tri->size,
								ippAlphaPlus);
		
		/**/
		

	}				
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
				else if (xFinal > span[y].maxX)
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
			else if (xFinal > span[y].maxX)
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
			else if (xFinal > span[j>>16].maxX)
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
			else if (xFinal > span[j>>16].maxX)
			{
				span[j>>16].maxX = xFinal;
			}
		j-=decInc;
	}

}

