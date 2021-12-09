////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

////////////////////////////////////////////////////////////
Texture2D<float4>		g_HDRTex;
Texture2D<float4>		g_BloomTexture;
Texture2D<float4>		g_DOFBlurTex; // 다운스케일링 -> 업스케일링(Linear)
Texture2D				g_DepthTex;
Texture2D				g_EffectTex;
Texture2D				g_EffectBlurTex;

/* Etc Resources */
Texture2D				g_RadiarBlurMaskTex;

StructuredBuffer<float> g_AverageLum;

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

cbuffer FinalPassDesc
{
	float	g_MiddleGrey = 0.f;
	float	g_LumWhiteSqr = 0.f;
	float	g_BloomScale = 0.15f; // 빛을 흘릴 스케일
	float2	g_DOFFarValues = { 100.f, 1.0f / max(250.f, 0.001f) }; // 초점이 맞지 않기 시작하는 거리와, 완전히 초점이 나가버리는 범위 값
};

cbuffer RadialBlurDesc
{
	//float	g_Samples[10] = { 
	//	-0.08, -0.05, -0.03, -0.02, -0.01,
	//	0.01, 0.02, 0.03, 0.05, 0.08, };

	float	g_Samples[20] = {
	-0.2, -0.17, -0.14, -0.12, -0.09, -0.07, -0.05, -0.03, -0.02, -0.01,
	0.01, 0.02, 0.03,0.05, 0.07,0.09,0.12, 0.14, 0.17, 0.2 };

	float	g_SampleDist = 0.15f; // 0.15f
	float	g_SampleStrength = 20.7f;

	bool	g_bRadiarBlur_Main = false;
	bool	g_bRadiarBlur_Sub = false;
	 
	float2	g_RadiarBlur_FocusPos_Main = { 0.25f,0.5f };
	float2	g_RadiarBlur_FocusPos_Sub = { 0.75f,0.5f };
};

////////////////////////////////////////////////////////////
/* Function */
float3 ToneMapping_DXSample(float3 HDRColor)
{
	float LScale = dot(HDRColor, LUM_FACTOR);
	LScale *= g_MiddleGrey / g_AverageLum[0];
	LScale = (LScale + LScale * LScale / g_LumWhiteSqr) / (1.0 + LScale);
	return HDRColor * LScale;
}

float3 ToneMapping_EA(float3 HDRColor)
{
	//float3 Color =HDRColor;
	//float3 x = max(0.f, Color - 0.004);
	//Color = (x * (6.2f * x + 0.5f)) / (x * (6.2f * x + 1.7f) + 0.06f);
	//return float4(Color, 1.f);

	float3 Color = pow(HDRColor, 2.2f);
	float3 x = max(0.f, Color - g_MiddleGrey);
	Color = (x * (g_LumWhiteSqr * x + 0.5f)) / (x * (g_LumWhiteSqr * x + 1.7f) + 0.06f);
	return Color;
}

float3 ToneMapping_Flimic(float3 HDRColor)
{
	// 곡선 형태를 지정하느 파라미터
	const float a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;

	float3	Color = saturate((HDRColor * (a * HDRColor + b)) / (HDRColor * (c * HDRColor + d) + e));
	float	Lum = saturate((g_LumWhiteSqr * (a * g_LumWhiteSqr + b)) / (g_LumWhiteSqr * (c * g_LumWhiteSqr + d) + e));

	return Color / Lum;
}

float3 DistanceDOF(float3 colorFocus, float3 colorBlurred, float depth)
{
	//float blurFactor = saturate((depth - g_DOFFarValues.x) / (g_DOFFarValues.y - g_DOFFarValues.x));
	float blurFactor = saturate((depth - g_DOFFarValues.x) * g_DOFFarValues.y);

	return lerp(colorFocus, colorBlurred, blurFactor);
}

float3 RadiarBlur(float2 vTexUV, float2 vFocusPos)
{

	float2	dir = vFocusPos - vTexUV;
	float	dist = length(dir);
	dir /= dist;
	
	float3 vColor = g_HDRTex.Sample(Wrap_MinMagMipLinear_Sampler, vTexUV).xyz;
	float3 sum = vColor;
	[unroll]
	for (int i = 0; i < 20; ++i)
	{
		sum += g_HDRTex.Sample(Wrap_MinMagMipLinear_Sampler, vTexUV + dir * g_Samples[i] * g_SampleDist).xyz;
	}
	sum /= 21.0; 

	float2 fMaskUV = vTexUV;
	fMaskUV.x *= 2.f;
	float fBlurMask = g_RadiarBlurMaskTex.Sample(Wrap_MinMagMipLinear_Sampler, fMaskUV).x;

	float ratio = saturate(dist * g_SampleStrength * fBlurMask);
	return lerp(vColor, sum, ratio);
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
	Out.vProjPosition	= Out.vPosition;

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

	float3 vColor = g_HDRTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz;

	// 먼 평면에 없는 픽셀에 대해서만 거리 DOF 계산
	vector	vDepthDesc = g_DepthTex.Sample(Point_Sampler, In.vTexUV);
	vector	vViewPos = vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	vViewZ = 0.f;

	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
		In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		// View space Z
		vViewZ = vDepthDesc.x * g_fMainCamFar;
		vViewPos *= vViewZ;
		vViewPos = mul(vViewPos, g_MainProjMatrixInverse);

		// Test - Radiar Blur 
		if(true == g_bRadiarBlur_Main) vColor = RadiarBlur(In.vTexUV, g_RadiarBlur_FocusPos_Main);

	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z &&
		In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		// View space Z
		vViewZ = vDepthDesc.x * g_fSubCamFar;
		vViewPos *= vViewZ;
		vViewPos = mul(vViewPos, g_SubProjMatrixInverse);
	}
	else discard;

	// DOF
	float3 colorBlurred = g_DOFBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vColor = DistanceDOF(vColor, colorBlurred, vViewPos.z); // 거리 DOF 색상 계산

	// Bloom
	vColor += g_BloomScale * g_BloomTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV.xy).xyz;

	// Add Effect
	vColor += g_EffectTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) + g_EffectBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * 2.f;

	// Tone Mapping
	vColor = ToneMapping_EA(vColor);

	// Final
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



