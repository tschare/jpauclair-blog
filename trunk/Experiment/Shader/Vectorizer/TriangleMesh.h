#pragma once

#include "Main.h"

class TriangleMesh
{
public:
	TriangleMesh(void);
	~TriangleMesh(void);
	void Set(VertexT v1, VertexT v2, VertexT v3, unsigned int c, float alpha);
	void efla(int x, int y , int x2 , int y2);
	void Rasterize(UINT *var);
	unsigned int color;
	float colorA;


	TRs *span;
	int minY ;
	int maxY;
	bool dirty;


};
