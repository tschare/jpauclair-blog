#pragma once
#ifndef _CLOG_H_
#define _CLOG_H_


#include "main.h"

void Log(char *lpszText, ...);
void LogError(char *lpszText, ...);
void LogWarning(char *lpszText, ...);

void LogStart(char *path);
void LogStop();
void LogMemoryUsage();

DWORD GetMemoryUsage();
void LogDebug(char *lpszText, ...);
float getRandomMinMax( float fMin, float fMax );
void PrintVector(D3DXVECTOR3 RockMan);
void PrintMatrix(D3DXMATRIX RockMan);
void CheckHeap();


struct tVertex
{
	D3DXVECTOR3 posit;
	D3DCOLOR    color;
	
	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ | D3DFVF_DIFFUSE 
	};
};

#endif
