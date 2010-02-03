#include "TriangleMesh.h"

TriangleMesh::TriangleMesh(void)
{
	span = new TRs[(int)IMAGE_HEIGHT];
	minY = 0;
	maxY= (int)IMAGE_HEIGHT;
}

TriangleMesh::~TriangleMesh(void)
{

}

void TriangleMesh::Set(VertexT v1, VertexT v2, VertexT v3, unsigned int c, float alpha)
{
	dirty = false;
	color = c;
	colorA = alpha;
	minY = xmin(v1.y, xmin(v2.y, v3.y));
	maxY = xmax(v1.y, xmax(v2.y, v3.y));

	for (int i = 0; i < IMAGE_HEIGHT ; i++)
	{
		span[i].minx = IMAGE_WIDTH;
		span[i].maxx = 0;
	}

	efla(v1.x, v1.y, v2.x, v2.y);
	efla(v2.x, v2.y, v3.x, v3.y);
	efla(v1.x, v1.y, v3.x, v3.y);
}

void TriangleMesh::Rasterize(UINT *var)
{
	
	for (int i = minY; i < maxY ; i++)
	{
		for (int j = span[i].minx; j <= span[i].maxx; j++)
		{
			(*(var + (i * int(IMAGE_HEIGHT)) + j)) = color;
		}
	}				
}


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