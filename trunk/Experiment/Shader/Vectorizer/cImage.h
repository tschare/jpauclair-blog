#pragma once

#include "Main.h"
#include "TriangleMesh.h"

class cImage
{
public:
	cImage(COMMON *pC);
	~cImage(void);
	void RenderImage();
	void GenerateRandom();
	void Init(UINT aTriangleCount, int aWidth,int aHeigth);
	void SaveImage(char *aImageName);
	void CompareTo( Ipp8u* aModel8u );
	
	UINT			mScore;	
	Ipp64f mModelDiff;

private:

	TriangleMesh	*mTriangles;
	Ipp8u		*mCompareBuffer ;
	
	UINT		mPixelCount;
	__itt_event mRender;
	__itt_event mGenerate;
	__itt_event mCompare;
	__itt_event mInit;
	COMMON		*C;
	UINT		mTriangleCount;
	IppiSize	FullImageROI;
	int			mWidth;
	int			mHeight;
	Ipp8u		*mImageData;
	Ipp8u		**mDestinations;
};
