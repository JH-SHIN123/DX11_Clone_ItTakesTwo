////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_SpecularTexture;
texture2D	g_EmissiveTexture;
texture2D	g_EmissiveBlurTexture;
texture2D	g_ShadowTexture;

////////////////////////////////////////////////////////////

struct VS_IN
{
	float4 vPosition	: POSITION;	
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;	
	float2 vTexUV		: TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	
	Out.vPosition	= In.vPosition;
	Out.vTexUV		= In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct PS_OUT
{
	vector vColor	: SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDiffuseDesc		= g_DiffuseTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vShadeDesc			= g_ShadeTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vSpecularDesc		= g_SpecularTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vShadowDesc			= g_ShadowTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vEmissiveDesc		= g_EmissiveTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vEmissiveBlurDesc	= g_EmissiveBlurTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vColor = (vDiffuseDesc * vShadeDesc + vSpecularDesc) * vShadowDesc;
	Out.vColor.xyz += vEmissiveDesc.xyz * 5.f/* Emissive Scale */ + vEmissiveBlurDesc.xyz * 2.f /* Blur - Emissive Scale */;

	if (Out.vColor.w == 0) discard;

	return Out;
}

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Default
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}	
};



