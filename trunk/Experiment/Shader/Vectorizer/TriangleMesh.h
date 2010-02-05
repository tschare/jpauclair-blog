#pragma once

#include "Main.h"

#define WIDTH_512 512
class TriangleMesh
{
public:
	TriangleMesh(void);
	~TriangleMesh(void);
	void Set(VertexT av1, VertexT av2, VertexT av3, unsigned int c);
	void Rasterize(Ipp8u *var);

private:
	void eflaE(int x, int y, int x2, int y2);

	Ipp8u *Src ;
	Ipp8u Col[4] ;
	IppiSize size;
	TRs *span;
	int minY ;
	int maxY;
	bool dirty;
	UINT color;
	UINT colorA;
	VertexT mV1,mV2,mV3;

};
