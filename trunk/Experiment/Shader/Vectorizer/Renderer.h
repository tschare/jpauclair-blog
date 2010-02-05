#pragma once

#include "Main.h"
#include "TriangleMesh.h"
#include "cImage.h"
class Renderer
{
public:
	Renderer( COMMON* pc);
	~Renderer(void);
	void Renderer::init();
	void Renderer::Render();

	void Dispose();


	std::vector<cImage*> mImages;
private:

	LPDIRECT3DTEXTURE9 ModelTexture;
	

	Ipp8u *mModelData;
	DWORD t1,t2;
	long sum;
	COMMON *C;




};
