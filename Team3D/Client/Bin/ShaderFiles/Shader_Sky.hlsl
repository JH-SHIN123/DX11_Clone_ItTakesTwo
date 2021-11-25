#include "Shader_Include.hpp"
////////////////////////////////////////////////////////////
texture2D	g_DiffuseTexture;
texture2D	g_SpecularTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;

cbuffer ViewportInfo
{
	int		g_iViewportIndex;
};
////////////////////////////////////////////////////////////

struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float2 vTexUV		: TEXCOORD0;
	uint4  vBlendIndex	: BLENDINDEX;
	float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = (matrix)0;

		if (1 == g_iViewportIndex)
			matVP = mul(g_MainViewMatrix, g_MainProjMatrix);
		else if (2 == g_iViewportIndex)
			matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);
		Out.vTexUV = In[i].vTexUV;
		Out.iViewportIndex = g_iViewportIndex;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}
////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct PS_OUT
{
	vector	vColor	: SV_TARGET;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vOuterColor = vector(0.032425, 0.085004, 0.401042, 1);
	vector vInnerColor = vector(0.130208, 0.029161, 0.10219, 1);

	vector lerpColor;
	float fLerp;
	if (In.vTexUV.x < 0.5)
	{
		fLerp = max(In.vTexUV.x * 2.f, 1.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}
	else
	{
		fLerp = 1.f - min(In.vTexUV.x * 2.f - 1.f, 0.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}

	Out.vColor = lerpColor + g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV * 2.f) * 2.5f;

	return Out;
}

PS_OUT	PS_CLOUD_NEAR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vOuterColor = vector(0.041667, 0.588406, 1, 1);
	vector vInnerColor = vector(0.247906, 0.33004, 1, 1);

	vector lerpColor;
	float fLerp;
	if (In.vTexUV.x < 0.5)
	{
		fLerp = max(In.vTexUV.x * 2.f, 1.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}
	else
	{
		fLerp = 1.f - min(In.vTexUV.x * 2.f - 1.f, 0.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}

	Out.vColor = lerpColor + g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * 5.f;
	Out.vColor.a = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).r;

	return Out;
}

PS_OUT	PS_CLOUD_FAR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vOuterColor = vector(0.365985, 0.697789, 0.864583, 1);
	vector vInnerColor = vector(0.154794, 0.023163, 1, 1);

	vector lerpColor;
	float fLerp;
	if (In.vTexUV.x < 0.5f)
	{
		fLerp = max(In.vTexUV.x * 2.f, 1.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}
	else
	{
		fLerp = 1.f - min(In.vTexUV.x * 2.f - 1.f, 0.f);
		lerpColor = lerp(vOuterColor, vInnerColor, fLerp);
	}

	Out.vColor = lerpColor + g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * 5.f;
	Out.vColor.a = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).g;

	return Out;
}
////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass BackGround
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	pass Cloud_Near // 1
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_CLOUD_NEAR();
	}

	pass Cloud_Far // 2
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_CLOUD_FAR();
	}
};