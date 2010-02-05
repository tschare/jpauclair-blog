#pragma once
#ifndef _CCOMMON_H_
#define _CCOMMON_H_

#ifdef _DEBUG
#   pragma comment(lib, "d3dx9d.lib")
#else
#   pragma comment(lib, "d3dx9.lib")
#endif
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "winmm.lib") 



#define USE_ITT 1

#if USE_ITT
	#pragma comment(lib, "libittnotify.lib")
	#include "libittnotify.h"
#endif

#define USE_IPP 1

#if USE_IPP
	//ipp core
	#include "ipp.h"
	#include "ippcore.h"
	//image processing
	#include "ippi.h"
	#include "ippcv.h"
	#pragma comment(lib, "ippcvemerged.lib")
	#pragma comment(lib, "ippcvmerged.lib")
	#pragma comment(lib, "ippcvmerged_t.lib")
	#pragma comment(lib, "ippcorel.lib")
	#pragma comment(lib, "ippcore_t.lib")
	#pragma comment(lib, "ippiemerged.lib")
	#pragma comment(lib, "ippimerged.lib")
	#pragma comment(lib, "ippimerged_t.lib")
	#pragma comment(lib, "ippscemerged.lib")
	#pragma comment(lib, "ippscmerged.lib")
	#pragma comment(lib, "ippscmerged_t.lib")
	#pragma comment(lib, "ippsemerged.lib")
	#pragma comment(lib, "ippsmerged.lib")
	#pragma comment(lib, "ippsmerged_t.lib")

#endif

#pragma warning(disable:4996) 
#pragma warning(disable:4244) 
#pragma warning(disable:4786)

#define APP_BGCOLOR 0x00000000
#define APP_TITLE   "Vectorizer"
#define APP_WIDTH   512
#define APP_HEIGHT  512

#define IMAGE_WIDTH 128.0f
#define IMAGE_HEIGHT 128.0f

#define SAFERELEASE(p)      if(p != NULL){ p->Release(); p = NULL; }
#define SAFEDELETE(p)       if(p != NULL){ delete p; p = NULL; }
#define SAFEDELETEARRAY(p)  if(p != NULL){ delete[] p; p = NULL; }

class COMMON
{
public:
	LPDIRECT3DDEVICE9        m_pD3DDevice;
};

#define D3DFVF_CUSTOMVERTEX ( D3DFVF_XYZW | D3DFVF_DIFFUSE | D3DFVF_TEX1 )

struct Vertex
{
	float x, y, z, w;
	int dif;
	float tu, tv;
};


struct VertexT
{
	float x, y, z;
	VertexT() {}
	VertexT(float ax, float ay,float az)
	{
		x = ax; y =ay; z= az;
	}
};

struct MyTriangle
{
	VertexT v1, v2, v3;
	UINT color;
	MyTriangle() 
	{
	}
	
};


struct MyImage
{
	MyTriangle tri[50];
	UINT score;
	MyImage() {}
};




struct TRs
{
	int minx;
	int maxx;
};


#ifndef xmax
#define xmax(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef xmin
#define xmin(a,b)	(((a) < (b)) ? (a) : (b))
#endif


//#ifndef xmin
#define min3(a,b,c)	(((a) < (b)) ? (((a) < (c)) ? (a) : (b)) : ((b) < (c)) ? (b) : (c))
#define max3(a,b,c)	(((a) > (b)) ? (((a) > (c)) ? (a) : (b)) : ((b) > (c)) ? (b) : (c))
//#endif

#endif