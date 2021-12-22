////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

////////////////////////////////////////////////////////////
Texture2D<float4>		g_HDRTex;
Texture2D<float4>		g_BloomTexture;
Texture2D<float4>		g_DOFBlurTex; // 다운스케일링 -> 업스케일링(Linear)
Texture2D				g_DepthTex;
Texture2D				g_EffectTex;
Texture2D				g_EffectBlurTex;
Texture2D				g_EffectPreCustomBlurTex;
Texture2D				g_EffectPreCustomBlurTex_Value;
Texture2D				g_EffectPreCustomBlurTex_Blur;
Texture2D				g_EffectPostCustomBlurTex;
Texture2D				g_EffectPostCustomBlurTex_Value;
Texture2D				g_EffectPostCustomBlurTex_Blur;

/* Etc Resources */
Texture2D				g_RadiarBlurMaskTex;
Texture2D				g_VolumeTex_Front;
Texture2D				g_VolumeTex_Back;
Texture2D				g_VolumeMaskTex;
Texture2D				g_VignatteTex;

StructuredBuffer<float> g_AverageLum;

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

cbuffer FinalPassDesc
{
	float	g_MiddleGrey = 0.f;
	float	g_LumWhiteSqr = 0.f;
	float	g_BloomScale = 0.15f; // 빛을 흘릴 스케일
	float2	g_DOFFarValues = { 100.f, 1.0f / max(250.f, 0.001f) }; // 초점이 맞지 않기 시작하는 거리와, 완전히 초점이 나가버리는 범위 값
	float	g_fTime = 0.f;
};

cbuffer RadialBlurDesc
{
	//float	g_Samples[10] = { 
	//	-0.08, -0.05, -0.03, -0.02, -0.01,
	//	0.01, 0.02, 0.03, 0.05, 0.08, };

	float	g_Samples[20] = {
	-0.2, -0.17, -0.14, -0.12, -0.09, -0.07, -0.05, -0.03, -0.02, -0.01,
	0.01, 0.02, 0.03,0.05, 0.07,0.09,0.12, 0.14, 0.17, 0.2 };

	float	g_SampleDist = 0.2; // 0.15f
	float	g_SampleStrength = 20.f;

	bool	g_bRadiarBlur_FullScreen = false;
	bool	g_bRadiarBlur_Main = false;
	bool	g_bRadiarBlur_Sub = false;
	 
	float2	g_RadiarBlur_FocusPos_FullScreen = { 0.5f,0.5f };
	float2	g_RadiarBlur_FocusPos_Main = { 0.25f,0.5f };
	float2	g_RadiarBlur_FocusPos_Sub = { 0.75f,0.5f };

	float	g_fRadiarBlurRatio_FullScreen = 1.f;
	float	g_fRadiarBlurRatio_Main = 1.f;
	float	g_fRadiarBlurRatio_Sub = 1.f;
};

cbuffer FogDesc
{
	bool	g_bFog = false;
	float3	g_vFogColor = { 0.f ,0.f, 0.f };			// 안개의 기본색상 ( 앰비언트 색상과 동일해야함)
	float	g_fFogStartDist = 0.01f;							// 안개 지점에서 카메라까지의 거리
	float	g_fFogGlobalDensity = 20.f;						// 안개 밀도 계수(값이 클수록 안개가 짙어진다)
	float	g_fFogHeightFalloff = 0.007f;					// 높이 소멸값
};

cbuffer BlurDesc
{
	bool g_MainBlur = false;
	bool g_SubBlur = false;
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

	float3 Color = pow(abs(HDRColor), 2.2f);
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

float3 RadiarBlur(float2 vTexUV, float2 vFocusPos, float fRatio)
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

	float ratio = saturate(dist * g_SampleStrength * fBlurMask * fRatio);
	return lerp(vColor, sum, ratio);
}

float3 RadiarBlur_EffectPreCustomBlur(float2 vTexUV, float2 vFocusPos, float fRatio)
{
	float2	dir = vFocusPos - vTexUV;
	float	dist = length(dir);
	dir /= dist;

	float3 vColor = g_EffectPreCustomBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, vTexUV).xyz;
	float3 sum = vColor;
	[unroll]
	for (int i = 0; i < 20; ++i)
	{
		sum += g_EffectPreCustomBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, vTexUV + dir * g_Samples[i] * g_SampleDist).xyz;
	}
	sum /= 21.0;

	float2 fMaskUV = vTexUV;
	fMaskUV.x *= 2.f;
	float fBlurMask = g_RadiarBlurMaskTex.Sample(Wrap_MinMagMipLinear_Sampler, fMaskUV).x;

	float ratio = saturate(dist * g_SampleStrength * fBlurMask * fRatio);
	return lerp(vColor, sum, ratio);
}

float3 VolumeBlend(float3 vColor, float2 vTexUV, float fProjDepth, float distToEye)
{
	float fVolumeFront = g_VolumeTex_Front.Sample(Point_Sampler, vTexUV).w;
	float fVolumeBack = g_VolumeTex_Back.Sample(Point_Sampler, vTexUV).w;
	float fVolumeFactor = 0.f;


	float3 vFogColor = 0.f;
	float3 fInnerColor = g_VolumeTex_Front.Sample(Point_Sampler, vTexUV).xyz;
	float3 fOuterColor = g_VolumeTex_Back.Sample(Point_Sampler, vTexUV).xyz;

	if (fVolumeBack - fVolumeFront < 0) /* 카메라가 안에 들어왔을때 */
	{
		fVolumeFactor = saturate(sqrt((distToEye - 1.f/*FogStart*/) / 10.f /*range*/));

		float2 vVolumePatternUV = vTexUV;
		vVolumePatternUV.x += g_fTime;
		fVolumeFactor *= g_VolumeMaskTex.Sample(Wrap_MinMagMipLinear_Sampler, vVolumePatternUV) * 2.5f;
	}
	else
		fVolumeFactor = (fProjDepth - fVolumeFront) / (fVolumeBack - fVolumeFront);
	
	if(fVolumeFactor > 0)
		fVolumeFactor = exp(-fVolumeFactor * 0.66);
	
	float fLerpFactor = saturate(fVolumeFactor);

	// 볼륨컬러
	vFogColor = lerp(fInnerColor, fOuterColor, fLerpFactor);

	if (fLerpFactor > 0.4f)
		fLerpFactor = 0.4f;
	
	return lerp(vColor, vFogColor, fLerpFactor);
}

float3 HeightFog(float3 finalColor, float eyePosY, float3 eyeToPixel)
{
	//////////////////////////////////////////////////////////////////////////////////
	float pixelDist = length(eyeToPixel); // Cam과 픽셀간의 거리
	float3 eyeToPixelNorm = normalize(eyeToPixel);

	// 픽셀 거리에 대해 안개 시작 지점 계산
	float fogDist = max(pixelDist - g_fFogStartDist, 0.0);

	// 안개 세기에 대해 거리 계산
	float fogHeightDensityAtViewer = exp(-g_fFogHeightFalloff * eyePosY); // exp : 지수 반환(왼쪽 음에 가까운 지수그래프사용)
	float fogDistInt = fogDist * fogHeightDensityAtViewer;

	// 안개 세기에 대해 높이 계산
	float eyeToPixelY = eyeToPixel.y * (fogDist * 100 / pixelDist);
	float t = g_fFogHeightFalloff * eyeToPixelY;
	const float thresholdT = 0.0001;
	float fogHeightInt = abs(t) > thresholdT ?
		(1.0 - exp(-t)) / t : 1.0;

	// 위 계산 값을 합해 최종 인수 계산
	float fogFinalFactor = exp(-g_fFogGlobalDensity * fogDistInt * fogHeightInt);

	return lerp(g_vFogColor, finalColor, fogFinalFactor);
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
	
	vector	vWorldPos = 0.f;
	vector	vCamPos = 0.f;
	float3 eyeToPixel = 0.f;
	float distToEye = 0.f;

	bool bFogActive = false;
	bool bBlur = false;

	float fBarOffset = 0.00115f;
	if (In.vTexUV.x >= g_vMainViewportUVInfo.x + fBarOffset && In.vTexUV.x <= g_vMainViewportUVInfo.z - fBarOffset &&
		In.vTexUV.y >= g_vMainViewportUVInfo.y + fBarOffset && In.vTexUV.y <= g_vMainViewportUVInfo.w - fBarOffset)
	{
		// View space Z
		vViewZ = vDepthDesc.x * g_fMainCamFar;
		vViewPos *= vViewZ;
		vViewPos = mul(vViewPos, g_MainProjMatrixInverse);

		vWorldPos = mul(vViewPos, g_MainViewMatrixInverse);
		vCamPos = g_vMainCamPosition;

		// Radiar Blur 
		if(true == g_bRadiarBlur_Main) vColor = RadiarBlur(In.vTexUV, g_RadiarBlur_FocusPos_Main, g_fRadiarBlurRatio_Main);

		// Trigger Active
		bFogActive = g_bFog;
		bBlur = g_MainBlur;
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x + fBarOffset && In.vTexUV.x <= g_vSubViewportUVInfo.z - fBarOffset &&
		In.vTexUV.y >= g_vSubViewportUVInfo.y + fBarOffset && In.vTexUV.y <= g_vSubViewportUVInfo.w - fBarOffset)
	{
		// View space Z
		vViewZ = vDepthDesc.x * g_fSubCamFar;
		vViewPos *= vViewZ;
		vViewPos = mul(vViewPos, g_SubProjMatrixInverse);

		vWorldPos = mul(vViewPos, g_SubViewMatrixInverse);
		vCamPos = g_vSubCamPosition;

		// Radiar Blur 
		if (true == g_bRadiarBlur_Sub) vColor = RadiarBlur(In.vTexUV, g_RadiarBlur_FocusPos_Sub, g_fRadiarBlurRatio_Sub);

		bBlur = g_SubBlur;
	}
	else discard;

	// Vignatte
	vColor *= pow((1.f - g_VignatteTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).a) + 0.14f, 3);

	// DOF
	float3 colorBlurred = g_DOFBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (bBlur) vColor = colorBlurred;
	else vColor = DistanceDOF(vColor, colorBlurred, vViewPos.z); // 거리 DOF 색상 계산

	// 카메라와의 거리구하기
	eyeToPixel = vWorldPos - vCamPos;
	distToEye = length(eyeToPixel);

	// Fog - MainView만 적용
	if (bFogActive) vColor = HeightFog(vColor, vCamPos.y, eyeToPixel);

	// Volume
	vColor = VolumeBlend(vColor, In.vTexUV, vDepthDesc.y, distToEye);

	// Bloom
	vColor += g_BloomScale * g_BloomTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV.xy).xyz;

	// Add Effect
	vColor += g_EffectTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) + g_EffectBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * 1.5f;

	// Add EffectPreCustomBlur
	float3 vEffectPreCustomBlur = 0.f;
	
	if (g_bRadiarBlur_FullScreen)
		vEffectPreCustomBlur = RadiarBlur_EffectPreCustomBlur(In.vTexUV, g_RadiarBlur_FocusPos_FullScreen, g_fRadiarBlurRatio_FullScreen);
	else
		vEffectPreCustomBlur = g_EffectPreCustomBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	float fBlurValue = g_EffectPreCustomBlurTex_Value.Sample(Point_Sampler, In.vTexUV).r;
	vColor += vEffectPreCustomBlur + g_EffectPreCustomBlurTex_Blur.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * (fBlurValue * 10.f);
	
	// Add EffectPostCustomBlur
	fBlurValue = g_EffectPostCustomBlurTex_Value.Sample(Point_Sampler, In.vTexUV).r;
	vColor += g_EffectPostCustomBlurTex.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) + g_EffectPostCustomBlurTex_Blur.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * (fBlurValue * 10.f);

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
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
}

/* Fog */
//cbuffer FogDesc
//{
//	float3	g_vFogColor = { 1.f,0.9f,0.6f };			// 안개의 기본색상 ( 앰비언트 색상과 동일해야함)
//	float	g_fFogStartDist = 20.f;		// 안개 지점에서 카메라까지의 거리
//	float3	g_vFogHighlightColor = { 0.8f, 0.7f, 0.4f };	// 카메라와 태양을 잇는 벡터와 평행에 가까운 카메라 벡터의 하이라이팅 픽셀 색상
//	float	g_fFogGlobalDensity = 1.5f;	// 안개 밀도 계수(값이 클수록 안개가 짙어진다)
//	float	g_fFogHeightFalloff = 0.2f;	// 높이 소멸값
//};
//float3 ApplyFog(float3 finalColor, float eyePosY, float3 eyeToPixel)
//{
//	//////////////////////////////////////////////////////////////////////////////////
//	//float pixelDist = length(eyeToPixel); // Cam과 픽셀간의 거리
//	//float3 eyeToPixelNorm = normalize(eyeToPixel);
//
//	//// 픽셀 거리에 대해 안개 시작 지점 계산
//	//float fogDist = max(pixelDist - g_fFogStartDist, 0.0);
//
//	//// 안개 세기에 대해 거리 계산
//	//float fogHeightDensityAtViewer = exp(-g_fFogHeightFalloff * eyePosY); // exp : 지수 반환(왼쪽 음에 가까운 지수그래프사용)
//	//float fogDistInt = fogDist * fogHeightDensityAtViewer;
//
//	//// 안개 세기에 대해 높이 계산
//	//float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
//	//float t = g_fFogHeightFalloff * eyeToPixelY;
//	//const float thresholdT = 0.01;
//	//float fogHeightInt = abs(t) > thresholdT ?
//	//	(1.0 - exp(-t)) / t : 1.0;
//
//	//// 위 계산 값을 합해 최종 인수 계산
//	//float fogFinalFactor = exp(-g_fFogGlobalDensity * fogDistInt);
//
//	//// 태양 하이라이트 계산 및 안개 색상 혼합
//	////float sunHighlightFactor = saturate(dot(eyeToPixelNorm, -g_vLightDir));
//	////sunHighlightFactor = pow(sunHighlightFactor, 8.0);
//	////float3 fogFinalColor = lerp(g_vFogColor, g_vFogHighlightColor, sunHighlightFactor);
//
//	return lerp(g_vFogColor, finalColor, fogFinalFactor);
//}
// 

