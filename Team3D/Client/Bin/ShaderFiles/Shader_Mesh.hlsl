#include "Shader_Defines.hpp"

////////////////////////////////////////////////////////////

struct BONEMATRICES
{
	matrix	Matrices[256];
};

////////////////////////////////////////////////////////////

texture2D	g_DiffuseTexture;
//texture2D	g_NormalTexture;
//texture2D	g_EmmesiveTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;

sampler	DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer BoneMatrixDesc
{
	BONEMATRICES	g_BoneMatrices;
};

cbuffer Effect
{
	float	g_fAlpha;
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
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix	= (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW		= mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition	= mul(vector(In.vPosition, 1.f), matBW);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTexUV		= In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_NO_BONE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition	= mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV		= In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition		= mul(In[i].vPosition, matVP);
		Out.vNormal			= In[i].vNormal;
		Out.vTexUV			= In[i].vTexUV;
		Out.vProjPosition	= Out.vPosition;
		Out.iViewportIndex	= 1;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition		= mul(In[j].vPosition, matVP);
		Out.vNormal			= In[j].vNormal;
		Out.vTexUV			= In[j].vTexUV;
		Out.vProjPosition	= Out.vPosition;
		Out.iViewportIndex	= 2;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
};

struct PS_OUT
{
	vector	vDiffuse	: SV_TARGET0;
	vector	vNormal		: SV_TARGET1;
	vector	vDepth		: SV_TARGET2;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vDiffuse	= vMtrlDiffuse;
	Out.vNormal		= vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth		= vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	return Out;
}

////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	// 0
	pass Light_Directional
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
	// 1
	pass Light_Directional_NoBone
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
};