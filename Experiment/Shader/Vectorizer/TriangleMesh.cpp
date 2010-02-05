#include "TriangleMesh.h"

TriangleMesh::TriangleMesh(void)
{
	span = new TRs[(int)IMAGE_HEIGHT];
	minY = 0;
	maxY= (int)IMAGE_HEIGHT;
	#if	USE_IPP
	Src = (Ipp8u*) ippMalloc(IMAGE_WIDTH*4);
	#endif
}

TriangleMesh::~TriangleMesh(void)
{

}

void TriangleMesh::Set(VertexT av1, VertexT av2, VertexT av3, UINT c)
{
	mV1 = av1;
	mV2 = av2;
	mV3 = av3;

	dirty = false;
	color = c;
	minY = xmin(mV1.y, xmin(mV2.y, mV3.y));
	maxY = xmax(mV1.y, xmax(mV2.y, mV3.y));

	for (int i = 0; i < IMAGE_HEIGHT ; i++)
	{
		span[i].minx = IMAGE_WIDTH;
		span[i].maxx = 0;
	}

	#if	USE_IPP
		size.width = IMAGE_WIDTH;
		size.height = 1;

		//Col[0] = c & 0xFF000000; Col[1] = c & 0x00FF0000; Col[2] = c & 0x0000FF00; Col[3] = c & 0x000000FF;
		
		Col[3] = (c >> 24) & 0xFF;
		Col[2] = (c >> 16) & 0xFF;
		Col[1] = (c >>8) & 0xFF;
		Col[0] = c & 0xFF;

		//LogDebug("%d %d %d %d\n",Col[0],Col[1],Col[2],Col[3]);
		colorA = Col[3];
		
		ippiSet_8u_C4R(Col,Src,128,size);
	#endif

	//http://www.edepot.com/algorithm.html
	//Algorithm E is the fastest, and JPD is the best
	eflaE(mV1.x, mV1.y, mV2.x, mV2.y);
	eflaE(mV2.x, mV2.y, mV3.x, mV3.y);
	eflaE(mV1.x, mV1.y, mV3.x, mV3.y);
}

#if	USE_IPP
void TriangleMesh::Rasterize(Ipp8u *var)
{
	size.height = 1;
	for (int i = minY; i < maxY ; i++)
	{

		size.width = (span[i].maxx - span[i].minx)<<2;
		
		//Combines two images using constant alpha values.
		ippiAlphaCompC_8u_C1R(	Src,
								WIDTH_512,
								colorA,
								var+((i * WIDTH_512) + (span[i].minx<<2)),
								WIDTH_512,
								0xFF-colorA,
								var+((i * WIDTH_512) + (span[i].minx<<2)),
								WIDTH_512,
								size,
								ippAlphaPlus);
		

	}				
}
#endif

/*
public function Rasterize(dest:flash.display.BitmapData) : void
{
	var i:int = 0;
	var p:Point = null;
	for (i = minY; i < maxY ; i++)
	{
		p = span[i];
		var xCount:int = p.y;
		for (var j:int = p.x; j <= xCount; j++)
		{
			dest.setPixel(j, minY + i,color);
		}
	}				
}
public function RasterizeVector(dest:Vector.<uint>) : void
{
	var i:int = 0;
	var p:Point = null;
	for (i = minY; i < maxY ; i++)
	{
		p = span[i];
		var xCount:int = p.y;
		for (var j:int = p.x; j <= xCount; j++)
		{
			dest[i * 128 + j] = color;
		}
	}				
}	
*/
// THE EXTREMELY FAST LINE ALGORITHM Variation B (Multiplication)
void TriangleMesh::eflaB(int x, int y, int x2, int y2) {
	bool yLonger=false;
	int incrementVal;
	int shortLen=y2-y;
	int longLen=x2-x;

	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}

	if (longLen<0) incrementVal=-1;
	else incrementVal=1;

	double multDiff;
	if (longLen==0.0) multDiff=(double)shortLen;
	else multDiff=(double)shortLen/(double)longLen;
	if (yLonger) {
		for (int i=0;i!=longLen;i+=incrementVal) {
			int xFinal = x+(int)((double)i*multDiff);
			
			if (xFinal < span[y+i].minx)
			{
				span[y+i].minx = xFinal;
			}
			else if (xFinal > span[y+i].maxx)
			{
				span[y+i].maxx = xFinal;
			}
		}
	} else {
		for (int i=0;i!=longLen;i+=incrementVal) {
			int  xFinal = x + i;
			
			if (xFinal < span[(int)(y+i * multDiff)].minx)
			{
				span[(int)(y+i * multDiff)].minx = xFinal;
			}
			else if (xFinal > span[(int)(y+i * multDiff)].maxx)
			{
				span[(int)(y+i * multDiff)].maxx = xFinal;
			}
			//myPixel(surface,x+i,y+(int)((double)i*multDiff));
		}
	}
}

void TriangleMesh::eflaE(int x, int y, int x2, int y2) {
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
				
				if (xFinal < span[y].minx)
				{
					span[y].minx = xFinal;
				}
				else if (xFinal > span[y].maxx)
				{
					span[y].maxx = xFinal;
				}
				//myPixel(surface,j >> 16,y);	
				j+=decInc;
			}
			return;
		}
		longLen+=y;
		for (int j=0x8000+(x<<16);y>=longLen;--y) {
			int xFinal = j>>16;
			
			if (xFinal < span[y].minx)
			{
				span[y].minx = xFinal;
			}
			else if (xFinal > span[y].maxx)
			{
				span[y].maxx = xFinal;
			}
			//myPixel(surface,j >> 16,y);	
			j-=decInc;
		}
		return;	
	}

	if (longLen>0) {
		longLen+=x;
		for (int j=0x8000+(y<<16);x<=longLen;++x) {
			int xFinal = x;
			
			if (xFinal < span[j>>16].minx)
			{
				span[j>>16].minx = xFinal;
			}
			else if (xFinal > span[j>>16].maxx)
			{
				span[j>>16].maxx = xFinal;
			}
			//myPixel(surface,x,j >> 16);
			j+=decInc;
		}
		return;
	}
	longLen+=x;
	for (int j=0x8000+(y<<16);x>=longLen;--x) {
			int xFinal = x;
			
			if (xFinal < span[j>>16].minx)
			{
				span[j>>16].minx = xFinal;
			}
			else if (xFinal > span[j>>16].maxx)
			{
				span[j>>16].maxx = xFinal;
			}
		//myPixel(surface,x,j >> 16);
		j-=decInc;
	}

}



void TriangleMesh::efla(int x, int y , int x2 , int y2)
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
			
			if (xFinal < span[y+i].minx)
			{
				span[y+i].minx = xFinal;
			}
			else if (xFinal > span[y+i].maxx)
			{
				span[y+i].maxx = xFinal;
			}
		}
	}
	else
	{
		for (int i = 0; i != longLen; i += inc)
		{
			xFinal = x + i;
			
			if (xFinal < span[(int)(y+i * multDiff)].minx)
			{
				span[(int)(y+i * multDiff)].minx = xFinal;
			}
			else if (xFinal > span[(int)(y+i * multDiff)].maxx)
			{
				span[(int)(y+i * multDiff)].maxx = xFinal;
			}					
		}
	}
}