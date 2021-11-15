////////////////////////////////////////////////////////////
#include "Shader_Defines.hpp"
#include "Shader_Macro.hpp"

texture2D	g_DepthTexture;
texture2D	g_CascadedShadowDepthTexture;

cbuffer CascadedShadowDesc
{
	float	g_CascadeEnds[MAX_CASCADES + 1];

	matrix	g_ShadowTransforms[MAX_CASCADES];
};

cbuffer TransformDesc
{
	float	g_fCamFar;
	matrix	g_ProjMatrixInverse;
	matrix	g_ViewMatrixInverse;
}

cbuffer Viewport
{
	float4	g_vViewportUVInfo;
};

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
	vector	vShadow		: SV_TARGET0;
};

PS_OUT PS_CASCADED_SHADOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDepthDesc	= g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	float	fShadowfactor = 0.f;

	fViewZ		= vDepthDesc.x * g_fCamFar;
	vWorldPos	= vWorldPos * fViewZ;
	vWorldPos	= mul(vWorldPos, g_ProjMatrixInverse);
	vWorldPos	= mul(vWorldPos, g_ViewMatrixInverse);

	// Main에 대해서만 처리해보자.
	vector shadowPosNDC;
	int iIndex = -1;
	for (uint i = 0; i < MAX_CASCADES; ++i)
	{
		// TEST Main만 띄워보자.
		shadowPosNDC = mul(vWorldPos, g_ShadowTransforms[i]);
		shadowPosNDC.z /= shadowPosNDC.w;

		// 2. 절두체에 위치하는지 & 몇번째 슬라이스에 존재하는지 체크(cascadeEndWorld값과 비교해가며 슬라이스 인덱스 구하기)
		if (shadowPosNDC.x >= 0 && shadowPosNDC.x <= 1.0 && shadowPosNDC.y >= 0.0 && shadowPosNDC.y <= 1.0 && shadowPosNDC.z >= 0.0 && shadowPosNDC.z <= 1.0)
		{
			// 여기서 문제 발생 (Aspect 변경시)
			if (-shadowPosNDC.z <= g_CascadeEnds[i + 1])
			{
				iIndex = i;
				break;
			}
		}
	}

	vector shadowPosH = mul(vWorldPos, g_ShadowTransforms[iIndex]);
	shadowPosH.xyz /= shadowPosH.w;

	// Match up Deferred With Cascaded Shadow Depth
	float2 vShadowUV = shadowPosH.xy;
	vShadowUV.y = (vShadowUV.y + float(iIndex)) / float(MAX_CASCADES);

	float shadowFactor = 0.f;
	float percentLit = 0.0f;
	float depth = shadowPosH.z; // 그릴 객체들의 깊이값. (그림자 ndc로 이동한)

	percentLit = g_CascadedShadowDepthTexture.Sample(Wrap_Sampler, vShadowUV).r;

	if (percentLit < depth)
		shadowFactor = percentLit;

	Out.vShadow = shadowFactor;
	Out.vShadow.a = 1.f;

	//if (shadowPosH.z < shadowFactor + 0.01f)
	//{
	//	shadowFactor = 0.f;
	//}

	//Out.vShadow = 0.f;
	//Out.vShadow.a = 1.f;

	return Out;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Cascaded_Shadow
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_CASCADED_SHADOW();
	}
};