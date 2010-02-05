#include "cLog.h"

char LogPath[255];

void LogDebug(char *lpszText, ...)
{
	va_list argList;
	FILE *pFile = NULL;

	va_start(argList, lpszText);
	char msg[255]; 
	vsprintf(msg,lpszText, argList);
	OutputDebugString(msg);

	va_end(argList);
}
void Log(char *lpszText, ...)
{
	SYSTEMTIME now;
	GetLocalTime (&now);

	va_list argList;
	FILE *pFile = NULL;

	va_start(argList, lpszText);
	pFile = fopen(LogPath, "a+");
	
	if(pFile != NULL)
	{
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#000000\">");
		fprintf(pFile, "%04d-%02d-%02d %02d:%02d:%02d.%03d - ", now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds);
		vfprintf(pFile, lpszText, argList);
		fprintf(pFile, "</font><br>\n");		
		fclose(pFile);
	}
	va_end(argList);
}

void LogWarning(char *lpszText, ...)
{
	va_list argList;
	FILE *pFile = NULL;

	va_start(argList, lpszText);
	pFile = fopen(LogPath, "a+");

	if(pFile != NULL)
	{
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#FF8000\"><b>");
		vfprintf(pFile, lpszText, argList);
		fprintf(pFile, "</b></font><br>\n");		
		fclose(pFile);
	}
	va_end(argList);
}

void LogError(char *lpszText, ...)
{
	va_list argList;
	FILE *pFile = NULL;

	va_start(argList, lpszText);
	pFile = fopen(LogPath, "a+");
	
	if(pFile != NULL)
	{
		fprintf(pFile, "<font face=\"Arial\" size=\"2\" color=\"#FF0000\"><b>");
		vfprintf(pFile, lpszText, argList);
		fprintf(pFile, "</b></font><br>\n");		
		fclose(pFile);
	}
	va_end(argList);
}


void LogStart(char *path)
{
	FILE *pFile = NULL;
	if (path)
		sprintf (LogPath,"%s/LogResult.htm", path);
	else
		sprintf (LogPath,"LogResult.htm");
	pFile = fopen(LogPath, "wb");
	
	if(pFile != NULL)
	{
		fprintf(pFile, "<html><head><title>Log File</title></head><body>\n");
		fprintf(pFile, "<font face=\"Arial\" size=\"4\" color=\"#000000\"><b><u>Log File</u></b></font><br>\n");
		fclose(pFile);
	}
}

void LogStop()
{
	FILE *pFile = NULL;
	pFile = fopen(LogPath, "a+");		
	if(pFile != NULL)
	{
		fprintf(pFile, "</body></html>");
		fclose(pFile);
	}	
}

void LogMemoryUsage()
{
	DWORD dwMemoryUsage = GetMemoryUsage();
	float rMemoryUsage = 0.0f;
	
	if(dwMemoryUsage < 1024)
	{
		Log("Memory Usage: %d Bytes", dwMemoryUsage);
	}
	else if(dwMemoryUsage < 1048576)
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1024.0f);
		Log("Memory Usage: %f KB", rMemoryUsage);
	}
	else if(dwMemoryUsage < 1073741824)
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1048576.0f);
		Log("Memory Usage: %f MB", rMemoryUsage);
	}
	else
	{
		rMemoryUsage = ((float)dwMemoryUsage / 1073741824.0f);
		Log("Memory Usage: %f GB", rMemoryUsage);
	}
}

DWORD GetMemoryUsage()
{
	
	DWORD dwProcessID	= GetCurrentProcessId();
	HANDLE hSnapshot	= CreateToolhelp32Snapshot(TH32CS_SNAPHEAPLIST | TH32CS_SNAPMODULE, dwProcessID);
	DWORD dwSize		= 0;		// Will contain the total memory usage when we're done!
	
	MODULEENTRY32 module;
	module.dwSize = sizeof(module);
	
	if (Module32First(hSnapshot, &module))
	{
		do
			dwSize += module.modBaseSize;
		while (Module32Next(hSnapshot, &module));
	}
	
	
	HEAPLIST32 heap;
	heap.dwSize = sizeof(heap); 
	
	if (Heap32ListFirst(hSnapshot, &heap))
	{
		do
		{
			HEAPENTRY32 heapentry;
			heapentry.dwSize = sizeof(heapentry);
			
			if (Heap32First(&heapentry,heap.th32ProcessID,heap.th32HeapID))
			{
				do
				{
					if (heapentry.dwFlags != LF32_FREE)		// If the block is currently used
					{
						dwSize += (SIZE_T)heapentry.dwBlockSize;
					}
				}
				while (Heap32Next(&heapentry));
			}
		}
		while (Heap32ListNext(hSnapshot,&heap));
	}
	
	CloseHandle(hSnapshot);
	
	return dwSize;
}

float getRandomMinMax( float fMin, float fMax )
{
    float fRandNum = (float)rand () / RAND_MAX;
    return fMin + (fMax - fMin) * fRandNum;
}

void PrintMatrix(D3DXMATRIX RockMan)
{
	LogDebug("%f\t" , RockMan.m[0][0]);
	LogDebug("%f\t" , RockMan.m[0][1]);
	LogDebug("%f\t" , RockMan.m[0][2]);
	LogDebug("%f\n" , RockMan.m[0][3]);


	LogDebug("%f\t" , RockMan.m[1][0]);
	LogDebug("%f\t" , RockMan.m[1][1]);
	LogDebug("%f\t" , RockMan.m[1][2]);
	LogDebug("%f\n" , RockMan.m[1][3]);


	LogDebug("%f\t" , RockMan.m[2][0]);
	LogDebug("%f\t" , RockMan.m[2][1]);
	LogDebug("%f\t" , RockMan.m[2][2]);
	LogDebug("%f\n" , RockMan.m[2][3]);

	LogDebug("%f\t" , RockMan.m[3][0]);
	LogDebug("%f\t" , RockMan.m[3][1]);
	LogDebug("%f\t" , RockMan.m[3][2]);
	LogDebug("%f\n" , RockMan.m[3][3]);
}

void PrintVector(D3DXVECTOR3 RockMan)
{
	LogDebug("x:%f y:%f z:%f\n" , RockMan.x,RockMan.y,RockMan.z );

}


void CheckHeap()
{
	int heapstatus = _heapchk();
	switch( heapstatus )
	{
	case _HEAPOK:
		//LogDebug(" OK - heap is fine\n" );
		break;
	case _HEAPEMPTY:
		LogDebug(" OK - heap is empty\n" );
		break;
	case _HEAPBADBEGIN:
		LogDebug( "ERROR - bad start of heap\n" );
		break;
	case _HEAPBADNODE:
		LogDebug( "ERROR - bad node in heap\n" );
		break;
	}

}
