////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_SpecularTexture;
texture2D	g_SpecularBlurTexture;
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

	vector	vDiffuseDesc		= g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vShadeDesc			= g_ShadeTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vShadowDesc			= g_ShadowTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vSpecularDesc		= g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vSpecularBlurDesc	= g_SpecularBlurTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vEmissiveDesc		= g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vEmissiveBlurDesc	= g_EmissiveBlurTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	vSpecularDesc.w = 0.f;
	vSpecularBlurDesc.w = 0.f;

	float fSpecBlendFactor = 0.3f;
	Out.vColor = (vDiffuseDesc * vShadeDesc + (vSpecularDesc * fSpecBlendFactor + pow(vSpecularBlurDesc, 2.f) * (1.f - fSpecBlendFactor))) * vShadowDesc;
	Out.vColor.xyz += vEmissiveDesc.xyz/* Emissive Scale */ + vEmissiveBlurDesc.xyz /* Blur - Emissive Scale */;

	if (Out.vColor.w == 0) discard; // 이거 없으면 스카이박스가 사라짐.

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



