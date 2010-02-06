#pragma once
#include "main.h"
class ImageUtils
{
public:
	ImageUtils(void);
	~ImageUtils(void);

	static void ResetImage(MyImage* img);
	static bool MutateImage(MyImage* image);
	static void RasterizeImage(MyImage* img, Ipp8u* dest, Ipp8u* writeLineBuff);
	static UINT AbsDiffImage(Ipp8u* readBuff1, Ipp8u* readBuff2, Ipp8u* writeBuff);
private:
	static void ResetTriangle(MyTriangle* tri);
	static void UpdateImage(MyImage* img);
	static bool MutateVertex(VertexT* v);
	static bool MutateTriangle(MyTriangle* tri);
	static void RasterizeTriangle(MyTriangle* tri,Ipp8u* dest, Ipp8u* writeLineBuff);
	static void eflaE(int x, int y, int x2, int y2, SpanBorder* span);
	static void efla(int x, int y , int x2 , int y2, SpanBorder* span);
};
