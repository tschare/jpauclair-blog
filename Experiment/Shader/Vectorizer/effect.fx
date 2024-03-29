//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
texture g_BaseTexture;
texture g_BaseTexture2;
float4x4 g_WorldViewProj;    // World * View * Projection matrix

//--------------------------------------------------------------------------------------
// Texture samplers
//--------------------------------------------------------------------------------------
sampler BaseTextureSampler = 
sampler_state
{
    Texture = <g_BaseTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler BaseTextureSampler2 = 
sampler_state
{
    Texture = <g_BaseTexture2>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Position   : POSITION;   // vertex position
    float4 Diffuse    : COLOR0;   // vertex position
    float2 TextureUV  : TEXCOORD0;  // vertex texture coords 
};


//--------------------------------------------------------------------------------------
// This shader computes standard transform
//--------------------------------------------------------------------------------------
VS_OUTPUT RenderVS( float4 vPos : POSITION, 
					float4 Diffuse    : COLOR0,
                    float2 vTexCoord0 : TEXCOORD0 )
{
    VS_OUTPUT Output;
    
    Output.Position = mul(vPos, g_WorldViewProj);
    Output.TextureUV = vTexCoord0;
    Output.Diffuse = Diffuse;
    return Output;    
}


//--------------------------------------------------------------------------------------
// Pixel shader output structure
//--------------------------------------------------------------------------------------
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color    
};


//--------------------------------------------------------------------------------------
// This shader outputs the pixel's color by using the texture's color
//--------------------------------------------------------------------------------------
PS_OUTPUT RenderPS( VS_OUTPUT In ) 
{ 
    PS_OUTPUT Output;
	
    
    float4 Color1 = tex2D(BaseTextureSampler, In.TextureUV);   
    float4 Color2 = tex2D(BaseTextureSampler2, In.TextureUV);
	
    Output.RGBColor = abs(Color1 - Color2);
    return Output;
}


//--------------------------------------------------------------------------------------
// Renders scene to render target
//--------------------------------------------------------------------------------------
technique Render
{
    pass P0
    {          
        VertexShader = compile vs_3_0 RenderVS( );
        PixelShader  = compile ps_3_0 RenderPS( );
    }
}
