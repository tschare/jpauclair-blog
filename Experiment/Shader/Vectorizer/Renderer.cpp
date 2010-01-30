#include "Renderer.h"

Renderer::Renderer( COMMON* pc)
{
	C = pc;
}

Renderer::~Renderer(void)
{
}

void Renderer::SetVertexBuffer(LPDIRECT3DVERTEXBUFFER9 pVertexBuffer)
{
	mVertexBuffer = pVertexBuffer;
}

void Renderer::init()
{
	if(FAILED(C->m_pD3DDevice->CreateTexture(128, 128, 1, D3DUSAGE_RENDERTARGET,D3DFMT_A8R8G8B8,D3DPOOL_DEFAULT,&m_pTexRender,NULL)))
	{
		LogError("Failed CreateTexture");
		PostQuitMessage(0);
	}

	if(FAILED(m_pTexRender->GetSurfaceLevel(0, &m_pTexSurface)))
	{
		LogError("Failed GetSurfaceLevel");
		PostQuitMessage(0);
	}

	if(FAILED(C->m_pD3DDevice->CreateOffscreenPlainSurface(	128,128,D3DFMT_A8R8G8B8,D3DPOOL_SYSTEMMEM,&m_pTexSurface2,NULL)))
	{
		LogError("Failed CreateOffscreenPlainSurface");
		PostQuitMessage(0);
	}

	D3DXMatrixPerspectiveFovLH( &g_matProj, D3DXToRadian( 45.0f ), 128.0f / 128.0f, 0.1f, 100.0f );
	C->m_pD3DDevice->SetTransform( D3DTS_PROJECTION, &g_matProj );

	D3DXMatrixIdentity( &g_matView ); // This sample is not really making use of a view matrix


}
//-----------------------------------------------------------------------------
// Name: initEffect()
// Desc: Initializie an Fx effect.
//-----------------------------------------------------------------------------
void Renderer::initEffect( void )
{
	//
	// Create two test textures for our effect to use...
	//

	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "noir.bmp", &g_pTexture_0 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile noir.bmp");
	}
	if( FAILED(D3DXCreateTextureFromFile( C->m_pD3DDevice, "blanc.bmp", &g_pTexture_1 )))
	{
		LogError("Failed to D3DXCreateTextureFromFile blanc.bmp");
	}
	LPD3DXBUFFER pBufferErrors = NULL;

	if( FAILED(D3DXCreateEffectFromFile( C->m_pD3DDevice, "effect.fx",NULL,NULL, D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, NULL, &g_pEffect, &pBufferErrors )) )
	{
		LPVOID pCompilErrors = pBufferErrors->GetBufferPointer();
		LogError("Fx Compile Error: %s ", (const char*)pCompilErrors);
	}
}

void Renderer::Render()
{

	//snag the old color buffer

	IDirect3DSurface9 *oldColorBuffer = 0;

	D3DXMATRIX matTrans;
	D3DXMatrixTranslation( &matTrans, 0.0f, 0.0f, 2.8f );

	g_matWorld = matTrans;
	C->m_pD3DDevice->SetTransform( D3DTS_WORLD, &g_matWorld );

	LogDebug("\nSalut, Le log est dans LogResult.html");
	LogDebug("\nc'est normal que sa lag");
	LogMemoryUsage();
	RenderShader();
	LogDebug("\nSa process encore");
	LogMemoryUsage();
	RenderCode1();
	LogDebug("\nNe pert pas patience, enleve les LogMemoryUsage, pour moins de lag");
	LogMemoryUsage();
	RenderCode2();
	LogDebug("\nAlmost there\n\n");
	LogMemoryUsage();

	C->m_pD3DDevice->SetRenderTarget(0, oldColorBuffer);
}


void Renderer::RenderShader()
{
	t1 = timeGetTime();

	/*
	 Sum : 12760064
	 time : 547 
	 diff : 778.812500
	*/
	g_pEffect->SetTechnique( "Render" );
	g_pEffect->SetTexture( "g_BaseTexture", g_pTexture_0 );
	g_pEffect->SetTexture( "g_BaseTexture2", g_pTexture_1 );

	////D3DXMATRIX m1 = g_matWorld * g_matView *g_matProj;
	D3DXMATRIX m1 = g_matWorld*g_matView *g_matProj;
	g_pEffect->SetMatrix("g_WorldViewProj", &m1);
	g_pEffect->SetTechnique( "Render" );

	if(FAILED(C->m_pD3DDevice->SetRenderTarget(0, m_pTexSurface)))
	{
		LogDebug("Failed SetRenderTarget");
	}

	for (int x=0;x<1000;x++)
	{
		
		C->m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,APP_BGCOLOR, 1.0f, 0 );
		C->m_pD3DDevice->BeginScene();

		UINT uPasses;
		g_pEffect->Begin( &uPasses, 0 );
	    
		for( UINT uPass = 0; uPass < uPasses; ++uPass )
		{
			g_pEffect->BeginPass( uPass );
			C->m_pD3DDevice->SetStreamSource( 0, mVertexBuffer, 0, sizeof(Vertex) );
			C->m_pD3DDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			C->m_pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
			g_pEffect->EndPass();
		}

		g_pEffect->End();
		
		C->m_pD3DDevice->EndScene();

		C->m_pD3DDevice->GetRenderTargetData(m_pTexSurface,m_pTexSurface2);
		C->m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,APP_BGCOLOR, 1.0f, 0 );

		if(m_pTexSurface2->LockRect(&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK) 
		{
			MessageBox(NULL, "LockRect failed.", "", MB_OK);
		}
		
		BYTE* pBits = (BYTE*)m_lockedRect.pBits;

		int diff=0;
		sum=0;

		//LogDebug("pixel[1]: %d, %d, %d, %d\n", *(pBits),*(pBits+1),*(pBits+2),*(pBits+3));
		for (int i=0;i<128*128;i++)
		{
			diff = *(pBits++) + *(pBits++) + *(pBits++) + *(pBits++);
			sum += diff;

		}

		m_pTexSurface2->UnlockRect();

	}		

	t2 = timeGetTime();
	LogWarning("\nVersion Shader\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(128.0f*128.0f));
}

void Renderer::RenderCode1()
{
	t1 = timeGetTime();

	/*
	Sum : 7340160
	time : 64 
	diff : 448.007813*/

	//Version 2
	int s1=0;

	for(int x=0;x<1000;x++)
	{
	
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits0 = (BYTE*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		BYTE* pBits1 = (BYTE*)m_lockedRect2.pBits;

		sum = 0;
		
		for (int i=0;i<128*128*4;i++)
		{
			//LogDebug("%d %d %d %d\n", *(pBits0), *(pBits0+1), *(pBits0+2), *(pBits0+3));
			//LogDebug("%d %d %d %d\n", *(pBits1), *(pBits1+1), *(pBits1+2), *(pBits1+3));
			s1 = (*(pBits0++)) - *(pBits1++);
			sum += abs(s1);
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}

	t2 = timeGetTime();
	LogWarning("\nVersion Code 1\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(128.0f*128.0f));
}

void Renderer::RenderCode2()
{
	t1 = timeGetTime();

/*
Sum : 7340160
time : 20 
diff : 448.007813
*/
	__m128i* Var1;
	__m128i* Var2;
	__m128i  Var3;

	for(int x=0;x<1000;x++)
	{
		if(g_pTexture_0->LockRect(0,&m_lockedRect, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var1 = (__m128i*)m_lockedRect.pBits;

		if(g_pTexture_1->LockRect(0,&m_lockedRect2, NULL, D3DLOCK_READONLY) != D3D_OK)  { LogDebug("Failed 1"); }
		Var2 = (__m128i*)m_lockedRect2.pBits;

		sum =0;
		int scnt = (128*128*4)/16;
		for (int i=0;i<scnt;i++)
		{

			Var3 = _mm_sad_epu8(*(Var1++),*(Var2++));
			//Var3 = _mm_sub_ps(*(Var1),*(Var2));
			//LogDebug("%d %d %d %d\n", (Var3).m128i_u8[0], (Var3).m128i_u8[1], (Var3).m128i_u8[2], (Var3).m128i_u8[3]);
			sum += (Var3).m128i_u16[0] + (Var3).m128i_u16[4];
		}

		g_pTexture_0->UnlockRect(0);
		g_pTexture_1->UnlockRect(0);
	}
	
	t2 = timeGetTime();
	LogWarning("\nVersion Code 2\n");
	Log("Sum : %d\n",sum );
	Log("time : %d \n", t2-t1);
	Log("diff : %f\n", sum /(128.0f*128.0f));

}

void Renderer::Dispose()
{
	SAFERELEASE(g_pTexture_0);
	SAFERELEASE(g_pTexture_1);
	SAFERELEASE(g_pEffect);
}