////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

texture2D			g_NormalTexture;
texture2D			g_DepthTexture;
texture2D			g_SpecularSrcTexture;
Texture2D<float>	g_SSAOTexture;
Texture2D<float>	g_SSAOTexture_Blur;

cbuffer LightDesc
{
	vector	g_vLightDir;			// only use directional / spot
	vector	g_vLightPos;			// only use point / spot
	float	g_fRange;				// only use point / spot
	float	g_fSpotCosOuterCone;	// only use spot
	float	g_fSpotConeAttRange;	// only use spot
	float	g_fSpotRangeRcp;		// only use spot
}

cbuffer LightColor
{
	vector	g_vLightDiffuse;
	vector	g_vLightAmbient;
	vector	g_vLightSpecular;
}

cbuffer FogDesc
{
	float3	g_vFogColor = {0.5f,0.5f,0.5f};			// 안개의 기본색상 ( 앰비언트 색상과 동일해야함)
	float	g_fFogStartDist = 37.0f;		// 안개 지점에서 카메라까지의 거리
	float3	g_vFogHighlightColor = { 0.8f, 0.7f, 0.4f};	// 카메라와 태양을 잇는 벡터와 평행에 가까운 카메라 벡터의 하이라이팅 픽셀 색상
	float	g_fFogGlobalDensity = 1.5f;	// 안개 밀도 계수(값이 클수록 안개가 짙어진다)
	float	g_fFogHeightFalloff = 0.2f;	// 높이 소멸값
};

cbuffer MtrlDesc
{
	float	g_fPower		= 64.f; // 16.f
	vector	g_vMtrlDiffuse	= (vector)1.f;
	vector	g_vMtrlAmbient	= (vector)1.f;
	vector	g_vMtrlSpecular = (vector)1.f;
}
////////////////////////////////////////////////////////////
/* Fuction */
float3 ApplyFog(float3 originalColor, float eyePosY, float3 eyeToPixel)
{
	float pixelDist = length(eyeToPixel);
	float3 eyeToPixelNorm = eyeToPixel / pixelDist;

	// 픽셀 거리에 대해 안개 시작 지점 계싼
	float fogDist = max(pixelDist - g_fFogStartDist, 0.0);

	// 안개 세기에 대해 거리 계산
	float fogHeightDensityAtViewer = exp(-g_fFogHeightFalloff * eyePosY); // exp : 지수 반환(왼쪽 음에 가까운 지수그래프사용)
	float fogDistInt = fogDist * fogHeightDensityAtViewer;

	// 안개 세기에 대해 높이 계산
	float eyeToPixelY = eyeToPixel.y * (fogDist / pixelDist);
	float t = g_fFogHeightFalloff * eyeToPixelY;
	const float thresholdT = 0.01;
	float fogHeightInt = abs(t) > thresholdT ?
		(1.0 - exp(-t)) / t : 1.0;

	// 위 계산 값을 합해 최종 인수 계산
	float fogFinalFactor = exp(-g_fFogGlobalDensity * fogDistInt * fogHeightInt);

	// 태양 하이라이트 계산 및 안개 색상 혼합
	float sunHighlightFactor = saturate(dot(eyeToPixelNorm, -g_vLightDir));
	sunHighlightFactor = pow(sunHighlightFactor, 8.0);
	float3 fogFinalColor = lerp(g_vFogColor, g_vFogHighlightColor, sunHighlightFactor);

	return lerp(fogFinalColor, originalColor, 0.f);
}

////////////////////////////////////////////////////////////
struct VS_IN
{
	float4 vPosition	: POSITION;	
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition		: SV_POSITION;	
	float2 vTexUV			: TEXCOORD0;
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
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
};

struct PS_OUT
{
	vector	vShade		: SV_TARGET0;
	vector	vSpecular	: SV_TARGET1;
};

PS_OUT PS_DIRECTIONAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(Point_Sampler, In.vTexUV);
	vector	vNormal		= normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	vector	vDepthDesc	= g_DepthTexture.Sample(Point_Sampler, In.vTexUV);
	float	fAODesc		= g_SSAOTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV);
	float	fAODesc_Blur= g_SSAOTexture_Blur.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV);

	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	vector	vLook		= (vector)0.f;
	vector	vLookNorm	= (vector)0.f;
	float	fEyePosY	= 0.f;

	/* ViewportUVInfo : x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
		In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fMainCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_MainProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_MainViewMatrixInverse);
		vLook		= vWorldPos - g_vMainCamPosition;
		vLookNorm	= normalize(vLook);
		fEyePosY	= g_vMainCamPosition.y;
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z &&
		In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fSubCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_SubProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_SubViewMatrixInverse);
		vLook		= vWorldPos - g_vSubCamPosition;
		vLookNorm	= normalize(vLook);
		fEyePosY	= g_vSubCamPosition.y;
	}
	else
		discard;

	Out.vShade		= max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient) * (fAODesc/* + fAODesc_Blur*/);
	Out.vShade.a	= 0.f;

	/* Specular */
	vector	vSpecSrcDesc = g_SpecularSrcTexture.Sample(Point_Sampler, In.vTexUV);
	if (vSpecSrcDesc.w < 0.5f) // w is specular on/off flag
	{
		vector	vSpecSrc = vector(vSpecSrcDesc.xyz * 2.f - 1.f, 0.f);
		vector vReflect = reflect(normalize(g_vLightDir), vSpecSrc);
		Out.vSpecular = pow(max(dot(vLookNorm * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
	}
	Out.vSpecular.a = 0.f;

	/* Fog */
	Out.vShade.xyz = ApplyFog(Out.vShade.xyz, fEyePosY, vLook);
	//Out.vSpecular.xyz = ApplyFog(Out.vSpecular.xyz, fEyePosY, vLook);

	return Out;
}

PS_OUT PS_POINT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(Point_Sampler, In.vTexUV);
	vector	vNormal		= normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	vector	vDepthDesc	= g_DepthTexture.Sample(Point_Sampler, In.vTexUV);
	
	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	vector	vLook		= (vector)0.f;

	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z && In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fMainCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_MainProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_MainViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vMainCamPosition);
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z && In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fSubCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_SubProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_SubViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vSubCamPosition);
	}
	else
		discard;

	vector	vLightDir	= vWorldPos - g_vLightPos;
	float	fDistance	= length(vLightDir) / g_fRange;
	clip(1 - fDistance);
	float	fAtt		= 0.5f * COS_ARR(3.14f * pow(fDistance, 1.5f)) + 0.5f;

	Out.vShade		= (max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse)) * fAtt;
	Out.vShade.a = 0.f;

	vector	vSpecSrcDesc = g_SpecularSrcTexture.Sample(Point_Sampler, In.vTexUV);
	if (vSpecSrcDesc.w < 0.5f) // w is specular on/off flag
	{
		vector	vSpecSrc = vector(vSpecSrcDesc.xyz * 2.f - 1.f, 0.f);
		vector vReflect = reflect(normalize(vLightDir), vSpecSrc);
		Out.vSpecular = pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
	}
	Out.vSpecular.a = 0.f;

	// 조명색 Power
	//Out.vShade *= 5.f;

	return Out;
}
////////////////////////////////////////////////////////////
PS_OUT PS_SPOT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f));
	vector	vDepthDesc = g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vWorldPos = vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ = 0.f;
	vector	vLook = (vector)0.f;

	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z && In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ = vDepthDesc.x * g_fMainCamFar;
		vWorldPos = vWorldPos * fViewZ;
		vWorldPos = mul(vWorldPos, g_MainProjMatrixInverse);
		vWorldPos = mul(vWorldPos, g_MainViewMatrixInverse);
		vLook = normalize(vWorldPos - g_vMainCamPosition);
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z && In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ = vDepthDesc.x * g_fSubCamFar;
		vWorldPos = vWorldPos * fViewZ;
		vWorldPos = mul(vWorldPos, g_SubProjMatrixInverse);
		vWorldPos = mul(vWorldPos, g_SubViewMatrixInverse);
		vLook = normalize(vWorldPos - g_vSubCamPosition);
	}
	else
		discard;

	// Get World Position
	float4 ToLight = vWorldPos - g_vLightPos;
	float DistToLight = length(ToLight);

	// Phong / Specular
	// Diffuse
	Out.vShade = (max(dot(normalize(ToLight) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse));
	Out.vShade.a = 0.f;

	vector	vSpecSrcDesc = g_SpecularSrcTexture.Sample(Point_Sampler, In.vTexUV);
	if (vSpecSrcDesc.w < 0.5f) // w is specular on/off flag
	{
		vector	vSpecSrc = vector(vSpecSrcDesc.xyz * 2.f - 1.f, 0.f);
		vector vReflect = reflect(normalize(ToLight), vSpecSrc);
		Out.vSpecular = pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
	}
	Out.vSpecular.a = 0.f;

	// Cone Att
	float cosAng = dot(normalize(g_vLightDir), normalize(ToLight));
	float conAtt = saturate((cosAng - g_fSpotCosOuterCone) / g_fSpotConeAttRange);
	conAtt *= conAtt;

	// Attenuation
	float DistToLightNorm = 1.0 - saturate(DistToLight * g_fSpotRangeRcp);
	float Attn = DistToLightNorm * DistToLightNorm;

	Out.vShade = Out.vShade;
	Out.vShade *= Attn * conAtt;

	return Out;
}
////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Directional
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_DIRECTIONAL();
	}

	pass Point
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_POINT();
	}

	pass Spot
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(1.f, 1.f, 1.f, 1.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SPOT();
	}
};