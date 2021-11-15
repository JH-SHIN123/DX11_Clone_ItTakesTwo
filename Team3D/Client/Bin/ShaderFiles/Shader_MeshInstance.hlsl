#include "Shader_Defines.hpp"
#include "Shader_Macro.hpp"

////////////////////////////////////////////////////////////
#define MAX_VERTICES NUM_VERTICES * MAX_CASCADES * NUM_VIEWPORTS
#define NUM_VERTICES 3
#define NUM_VIEWPORTS 2

texture2D	g_DiffuseTexture;
//texture2D	g_NormalTexture;
//texture2D	g_EmmesiveTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;

cbuffer Effect
{
	float	g_fAlpha;
};

texture2D	g_CascadedShadowDepthTexture;

cbuffer ShadowDesc
{
	float	g_CascadeEnds[MAX_CASCADES + 1];
	matrix	g_ShadowTransforms[MAX_CASCADES];
};
////////////////////////////////////////////////////////////

struct VS_IN
{
	float3				vPosition		: POSITION;
	float3				vNormal			: NORMAL;
	float3				vTangent		: TANGENT;
	float2				vTexUV			: TEXCOORD0;
	uint4				vBlendIndex		: BLENDINDEX;
	float4				vBlendWeight	: BLENDWEIGHT;
	row_major matrix	WorldMatrix		: WORLD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition	= mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), In.WorldMatrix));
	Out.vTexUV		= In.vTexUV;

	return Out;
}

VS_OUT_CSM_DEPTH VS_MAIN_CSM_DEPTH(VS_IN In)
{
	VS_OUT_CSM_DEPTH Out = (VS_OUT_CSM_DEPTH)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);

	return Out;
}

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	float4 vWorldPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct GS_IN_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

struct GS_OUT_CSM_DEPTH
{
	float4 vPosition		: SV_POSITION;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition		= mul(In[i].vPosition, matVP);
		Out.vNormal			= In[i].vNormal;
		Out.vTexUV			= In[i].vTexUV;
		Out.vProjPosition	= Out.vPosition;
		Out.vWorldPosition	= In[i].vPosition;
		Out.iViewportIndex	= 1;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition		= mul(In[j].vPosition, matVP);
		Out.vNormal			= In[j].vNormal;
		Out.vTexUV			= In[j].vTexUV;
		Out.vProjPosition	= Out.vPosition;
		Out.iViewportIndex	= 2;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

[maxvertexcount(MAX_VERTICES)]
void GS_MAIN_CSM_DEPTH(triangle GS_IN_CSM_DEPTH In[3], inout TriangleStream<GS_OUT_CSM_DEPTH> TriStream)
{
	GS_OUT_CSM_DEPTH Out = (GS_OUT_CSM_DEPTH)0;

	[unroll]
	for (uint j = 0; j < MAX_CASCADES; ++j)
	{
		[unroll]
		for (uint i = 0; i < 3; i++)
		{
			Out.vPosition = mul(In[i].vPosition, g_ShadowTransforms[j]);
			Out.iViewportIndex = j;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
}
////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	float4 vWorldPosition	: TEXCOORD2;
};

struct PS_OUT
{
	vector	vDiffuse	: SV_TARGET0;
	vector	vNormal		: SV_TARGET1;
	vector	vDepth		: SV_TARGET2;
	vector	vShadow		: SV_TARGET3;
};

struct PS_IN_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

struct PS_OUT_CSM_DEPTH
{
	vector	vShadowDepth : SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse	= vMtrlDiffuse;
	Out.vNormal		= vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	Out.vDepth		= vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	// TEST : Shadow
	vector vWorldPos = In.vWorldPosition;

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
	
	if (shadowPosH.z < shadowFactor + 0.01f)
	{
		shadowFactor = 0.f;
	}
	
	Out.vShadow = shadowFactor;

	return Out;
}

PS_OUT_CSM_DEPTH PS_MAIN_CSM_DEPTH(PS_IN_CSM_DEPTH In)
{
	PS_OUT_CSM_DEPTH		Out = (PS_OUT_CSM_DEPTH)0;

	Out.vShadowDepth = vector(In.vPosition.z, In.vPosition.z, In.vPosition.z, 1.f); /* NDC X 투영공간의 z*/

	return Out;
}
////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	// 0
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
	// 1
	pass Write_CascadedShadowDepth
	{
		SetRasterizerState(Rasterizer_Shadow);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile		vs_5_0 VS_MAIN_CSM_DEPTH();
		GeometryShader = compile	gs_5_0 GS_MAIN_CSM_DEPTH();
		//PixelShader = NULL;
		PixelShader = compile		ps_5_0 PS_MAIN_CSM_DEPTH();
	}
};