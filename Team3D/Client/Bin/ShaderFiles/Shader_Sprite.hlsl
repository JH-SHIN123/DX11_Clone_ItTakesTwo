#include "Shader_Defines.hpp"

////////////////////////////////////////////////////////////

texture2D	g_DiffuseTexture;

matrix		g_UIWorldMatrix;
matrix		g_UIViewMatrix;
matrix		g_UIProjMatrix;

float2		vLeftTopUV;
float2		vRightTopUV;
float2		vRightBottomUV;
float2		vLeftBottomUV;

int			g_iGSOption;


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
		float3	vVTXNum		: NORMAL;
		float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{           
		float4  vPosition : SV_POSITION;
		float2  vTexUV : TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);

	switch (In.vVTXNum.x)
	{
	case 0:
		Out.vTexUV = vLeftTopUV;
		break;
	case 1:
		Out.vTexUV = vRightTopUV;
		break;
	case 2:
		Out.vTexUV = vRightBottomUV;
		break;
	case 3:
		Out.vTexUV = vLeftBottomUV;
		break;
	}

	return Out;
}

VS_OUT VS_LOADINGBOOK(VS_IN In)
{
	VS_OUT				Out = (VS_OUT)0;

	matrix				matWV, matWVP;

	matWV = mul(g_UIWorldMatrix, g_UIViewMatrix);
	matWVP = mul(matWV, g_UIProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);

	switch (In.vVTXNum.x)
	{
	case 0:
		Out.vTexUV = vLeftTopUV;
		break;
	case 1:
		Out.vTexUV = vRightTopUV;
		break;
	case 2:
		Out.vTexUV = vRightBottomUV;
		break;
	case 3:
		Out.vTexUV = vLeftBottomUV;
		break;
	}

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

	if (0 == g_iGSOption)
	{
		for (uint i = 0; i < 3; i++)
		{
			matrix matVP = g_MainProjMatrix;

			Out.vPosition = mul(In[i].vPosition, matVP);
			Out.vTexUV = In[i].vTexUV;
			Out.iViewportIndex = 1;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
	else
	{
		for (uint j = 0; j < 3; j++)
		{
			matrix matVP = g_SubProjMatrix;

			Out.vPosition = mul(In[j].vPosition, matVP);
			Out.vTexUV = In[j].vTexUV;
			Out.iViewportIndex = 2;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
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

PS_OUT	PS_STICK(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;


	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
	Out.vColor *= vector(0.0f, 1.9f, 200.f, 1.f);

	//Out.vColor.b = 0.f;

	return Out;
}

////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass StickIcon
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_STICK();
	}

	pass LoadingBook
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOADINGBOOK();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};