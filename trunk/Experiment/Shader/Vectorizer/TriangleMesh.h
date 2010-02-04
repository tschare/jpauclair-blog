#pragma once

#include "Main.h"

#define WIDTH_512 512
class TriangleMesh
{
public:
	TriangleMesh(void);
	~TriangleMesh(void);
	void Set(VertexT v1, VertexT v2, VertexT v3, unsigned int c);
	void efla(int x, int y , int x2 , int y2);
	
	unsigned int color;
	UINT colorA;


	TRs *span;
	int minY ;
	int maxY;
	bool dirty;

	#if	USE_IPP
	void Rasterize(Ipp8u *var);
	Ipp8u *Src ;
	Ipp8u Col[4] ;
	IppiSize size ;
	#endif

	


};
