#include "Shader_Defines.hpp"

////////////////////////////////////////////////////////////

texture2D	g_DiffuseTexture;
texture2D	g_SubTexture;
matrix		g_UIWorldMatrix;
matrix		g_UIViewMatrix;
matrix		g_UIProjMatrix;

sampler	DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

////////////////////////////////////////////////////////////

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_UIWorldMatrix, g_UIViewMatrix);
	matWVP = mul(matWV, g_UIProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);
		Out.vTexUV = In[i].vTexUV;
		Out.iViewportIndex = 1;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition = mul(In[j].vPosition, matVP);
		Out.vTexUV = In[j].vTexUV;
		Out.iViewportIndex = 2;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4	vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	return Out;
}

////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
};