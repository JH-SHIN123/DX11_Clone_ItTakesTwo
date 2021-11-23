////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

////////////////////////////////////////////////////////////
Texture2D<float4>		g_HDRTex;
Texture2D<float4>		g_BloomTexture;
Texture2D<float4>		g_DOFBlurTex; // 다운스케일링 -> 업스케일링(Linear)
Texture2D				g_DepthTex; 
StructuredBuffer<float> g_AverageLum;

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

cbuffer FinalPassDesc
{
	float	g_MiddleGrey = 0.f;
	float	g_LumWhiteSqr = 0.f;
	float	g_BloomScale = 20.f; // 빛을 흘릴 스케일
	float2	g_DOFFarValues = { 10.f, 20.f }; // 초점이 맞지 않기 시작하는 거리와, 완전히 초점이 나가버리는 범위 값
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

float3 DistanceDOF(float3 colorFocus, float3 colorBlurred, float depth)
{
	float blurFactor = saturate((depth - g_DOFFarValues.x) * g_DOFFarValues.y);

	return lerp(colorFocus, colorBlurred, blurFactor);
}
////////////////////////////////////////////////////////////

struct VS_IN
{
	float4 vPosition	: POSITION;	
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition		: SV_POSITION;	
	float2 vTexUV			: TEXCOORD;
	float4 vProjPosition	: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	
	Out.vPosition		= In.vPosition;
	Out.vTexUV			= In.vTexUV;
	Out.vProjPosition = Out.vPosition;

	return Out;
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD;
	float4 vProjPosition	: TEXCOORD1;
};

struct PS_OUT
{
	vector vColor	: SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float3 vColor = g_HDRTex.Sample(Point_Sampler, In.vTexUV).xyz;

	// 먼 평면에 없는 픽셀에 대해서만 거리 DOF 계산
	vector	vDepthDesc = g_DepthTex.Sample(Point_Sampler, In.vTexUV);
	vector	vViewPos = vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	vViewZ = 0.f;

	if (vViewPos.z < 1.0)
	{
		if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
			In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
		{
			// View space Z
			vViewZ = vDepthDesc.x * g_fMainCamFar;
			vViewPos = vViewPos * vViewZ;
			vViewPos = mul(vViewPos, g_MainProjMatrixInverse);
		}
		else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z &&
			In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
		{
			// View space Z
			vViewZ = vDepthDesc.x * g_fSubCamFar;
			vViewPos = vViewPos * vViewZ;
			vViewPos = mul(vViewPos, g_SubProjMatrixInverse);
		}
		else discard;

		float3 colorBlurred = g_DOFBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

		// 거리 DOF 색상 계산
		vColor = DistanceDOF(vColor, colorBlurred, vViewPos.z);
	}

	vColor += g_BloomScale * g_BloomTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV.xy).xyz;

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



