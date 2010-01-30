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

#pragma warning(disable:4996) 

#define APP_BGCOLOR 0x00000000
#define APP_TITLE   "Vectorizer"
#define APP_WIDTH   512
#define APP_HEIGHT  512

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
	DWORD dif;
	float tu, tv;
};


#ifndef xmax
#define xmax(a,b)	(((a) > (b)) ? (a) : (b))
#endif

#ifndef xmin
#define xmin(a,b)	(((a) < (b)) ? (a) : (b))
#endif

#endif