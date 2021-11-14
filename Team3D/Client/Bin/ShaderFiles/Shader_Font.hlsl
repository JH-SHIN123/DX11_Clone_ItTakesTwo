
#include "Shader_Defines.hpp"

texture2D		g_DiffuseTexture;
matrix			g_UIWorldMatrix;
matrix			g_UIViewMatrix;
matrix			g_UIProjMatrix;

sampler DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN_FONT
{
	/* 본래 정점버퍼 */
	float3 vPosition : POSITION;
	float3 vVTXNum : NORMAL;	
	float2 vTexUV : TEXCOORD0;

	/* 인스턴스 정점버퍼 */
	float3 vInsPosition : TEXCOORD1;
	float2 vInsTexUV : TEXCOORD2;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};


VS_OUT VS_FONT(VS_IN_FONT In)
{
	VS_OUT				Out = (VS_OUT)0;

	matrix				matWV, matWVP;

	matWV = mul(g_UIWorldMatrix, g_UIViewMatrix);
	matWVP = mul(matWV, g_UIProjMatrix);

	Out.vPosition = mul(vector(In.vInsPosition, 1.f), matWVP);
	Out.vTexUV = In.vInsTexUV;

	//switch (In.vVTXNum.x)
	//{
	//case 0:
	//	Out.vTexUV = vLeftTopUV;
	//	break;
	//case 1:
	//	Out.vTexUV = vRightTopUV;
	//	break;
	//case 2:
	//	Out.vTexUV = vRightBottomUV;
	//	break;
	//case 3:
	//	Out.vTexUV = vLeftBottomUV;
	//	break;
	//}

	return Out;
}


struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET;
};

/* 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (Out.vColor.r == 0.f && Out.vColor.g == 0.f && Out.vColor.b == 0.f)
		discard;

	return Out;
}

technique11		DefaultTechnique
{
	pass Font
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FONT();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};



