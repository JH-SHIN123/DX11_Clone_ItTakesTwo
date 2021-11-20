////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

////////////////////////////////////////////////////////////
Texture2D<float4>		g_HDRTex;
StructuredBuffer<float> g_AverageLum;

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

cbuffer FinalPassDesc
{
	float g_MiddleGrey = 0.f;
	float g_LumWhiteSqr = 0.f;
};

////////////////////////////////////////////////////////////
/* Function */
float3 ToneMapping(float3 HDRColor)
{
	// 현재 픽셀에 대한 휘도 스케일 계산
	float LScale = dot(HDRColor, LUM_FACTOR);
	LScale *= g_MiddleGrey / g_AverageLum[0];
	LScale = (LScale + LScale * LScale / g_LumWhiteSqr) / (1.0 + LScale);

	// 휘도 스케일을 픽셀 색상에 적용
	return HDRColor * LScale;
}

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

	float3 vColor = g_HDRTex.Sample(Point_Sampler, In.vTexUV).xyz;

	vColor = ToneMapping(vColor);
	Out.vColor = vector(vColor, 1.f);

	return Out;
}

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass FinalPassPS
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}	
};



