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
	//float	g_fMainCamFar;
	//float	g_fSubCamFar;
	matrix	g_MainViewMatrixInverse;
	matrix	g_SubViewMatrixInverse;
	matrix	g_FullScreenProjInverse;
}

cbuffer Viewport
{
	float4	g_vMainViewportUVInfo;
	float4	g_vSubViewportUVInfo;
};

////////////////////////////////////////////////////////////
/* Function */
int Get_CascadedShadowSliceIndex(vector vWorldPos)
{
	vector shadowPosNDC;
	int iIndex = -1;
	for (uint i = 0; i < MAX_CASCADES; ++i)
	{
		// TEST Main만 띄워보자.
		//if (0 == iViewportIndex) shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Main[i]);
		//else shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Sub[i]);
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

	return iIndex;
}

float Get_ShadowFactor(vector vWorldPos, int iSliceIndex)
{
	vector shadowPosH = mul(vWorldPos, g_ShadowTransforms[iSliceIndex]);
	shadowPosH.xyz /= shadowPosH.w;

	// Match up Deferred With Cascaded Shadow Depth
	float2 vShadowUV = shadowPosH.xy;
	// viewport index가 필요하겠다.
	vShadowUV.x = vShadowUV.x;
	if (vShadowUV.x < 0.f) return 0.f;
	vShadowUV.y = (vShadowUV.y + float(iSliceIndex)) / float(MAX_CASCADES);

	float shadowFactor = 0.f;
	float percentLit = 0.0f;
	float depth = shadowPosH.z; // 그릴 객체들의 깊이값. (그림자 ndc로 이동한)

	percentLit = g_CascadedShadowDepthTexture.Sample(Wrap_Sampler, vShadowUV).r;
	if (percentLit < depth)
		shadowFactor = percentLit;

	if (shadowPosH.z < shadowFactor + 0.01f)
	{
		shadowFactor = 0.f;
	}

	return shadowFactor;
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
	vector	vShadow		: SV_TARGET;
};

PS_OUT PS_SHADOW(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDepthDesc	= g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	float fShadowfactor = 0.f;

	/* ViewportUVInfo : x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	//if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
	//	In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	//{
		fViewZ = vDepthDesc.x * g_fMainCamFar;
		vWorldPos = vWorldPos * fViewZ;
		vWorldPos = mul(vWorldPos, g_FullScreenProjInverse);
		vWorldPos = mul(vWorldPos, g_MainViewMatrixInverse);
	//}

	/* Carculate Shadow */
	int iIndex = Get_CascadedShadowSliceIndex(vWorldPos);
	if (iIndex < 0) return Out;
	fShadowfactor = Get_ShadowFactor(vWorldPos, iIndex);

	//if (0 == iIndex)
	//	Out.vShadow.x = 1.f;
	//else if(1 == iIndex)
	//	Out.vShadow.y = 1.f;
	//else if (2 == iIndex)
	//	Out.vShadow.z = 1.f;

	Out.vShadow = 1.f - fShadowfactor;
	Out.vShadow.a = 1.f;

	return Out;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass CascadedShadow
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_SHADOW();
	}

	//pass CascadedShadow_Merge
	//{

	//}
};