#pragma once
#include "main.h"
class ImageUtils
{
public:
	ImageUtils(void);
	~ImageUtils(void);

	static void ResetImage(MyImage* img);
	static void MutateImage(MyImage* image);
	static void UpdateImage(MyImage* img);
	static void RasterizeImage(MyImage* img, Ipp8u* dest, Ipp8u* writeLineBuff);
	
private:
	static void RasterizeTriangle(MyTriangle* tri,Ipp8u* dest, Ipp8u* writeLineBuff);
	static void eflaE(int x, int y, int x2, int y2, SpanBorder* span);
	static void efla(int x, int y , int x2 , int y2, SpanBorder* span);
};
