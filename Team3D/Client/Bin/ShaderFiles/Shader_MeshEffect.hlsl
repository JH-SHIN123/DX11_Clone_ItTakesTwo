#include "Shader_Include.hpp"

////////////////////////////////////////////////////////////
#define MAX_VERTICES NUM_VERTICES * MAX_CASCADES * NUM_VIEWPORTS
#define NUM_VERTICES 3
#define NUM_VIEWPORTS 2

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
//texture2D	g_EmmesiveTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;
texture2D	g_DistortionTexture;
texture2D	g_ColorRampTexture;

float g_fTime;
float g_fRadianAngle;
float4 g_vColor;
float4 g_vColorRamp_UV;

cbuffer Effect
{
	float	g_fAlpha;
};
BlendState BlendState_Alpha2
{
	BlendEnable[0] = true;
	SrcBlend = SRC_COLOR;
	DestBlend = INV_SRC_COLOR;
	BlendOp = Add;
};
BlendState BlendState_Alpha3
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = INV_SRC_ALPHA;
	DestBlend = SRC_ALPHA;
	BlendOp = Add;

	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = Add;
};
sampler	Mirror_MinMagMipLinear_Sampler = sampler_state
{
	AddressU = mirror;
	AddressV = mirror;
	Filter = MIN_MAG_MIP_LINEAR;
};

RasterizerState Rasterizer_CCW
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = true;
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
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT_TRIPLE_UV
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
	float2 vTexUV_2		: TEXCOORD1;
	float2 vTexUV_3		: TEXCOORD2;

};

struct VS_OUT_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), In.WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), In.WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_TRIPLE_UV VS_TRIPLE_UV(VS_IN In)
{
	VS_OUT_TRIPLE_UV Out = (VS_OUT_TRIPLE_UV)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), In.WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), In.WorldMatrix));
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In.vTexUV - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out.vTexUV_2 = RotateUV;

	fSin = sin(g_fRadianAngle * -0.5f);
	fCos = cos(g_fRadianAngle * -0.5f);
	RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	RotateUV = In.vTexUV - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out.vTexUV_3 = RotateUV;

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
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float4 vProjPosition		: TEXCOORD1;
	float4 vWorldPosition		: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	/* Main Viewport */
	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);
		Out.vNormal = In[i].vNormal;
		Out.vTangent = In[i].vTangent;
		Out.vBiNormal = In[i].vBiNormal;
		Out.vTexUV = In[i].vTexUV;
		Out.vProjPosition = Out.vPosition;
		Out.vWorldPosition = In[i].vPosition;
		Out.iViewportIndex = 1;

		TriStream.Append(Out);
	}
	TriStream.RestartStrip();

	/* Sub Viewport */
	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition = mul(In[j].vPosition, matVP);
		Out.vNormal = In[j].vNormal;
		Out.vTangent = In[j].vTangent;
		Out.vBiNormal = In[j].vBiNormal;
		Out.vTexUV = In[j].vTexUV;
		Out.vProjPosition = Out.vPosition;
		Out.vWorldPosition = In[j].vPosition;
		Out.iViewportIndex = 2;

		TriStream.Append(Out);
	}
	TriStream.RestartStrip();
}

struct GS_OUT_TRIPLE_UV
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_2				: TEXCOORD1;
	float2 vTexUV_3				: TEXCOORD2;
	float4 vProjPosition		: TEXCOORD3;
	float4 vWorldPosition		: TEXCOORD4;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(6)]
void GS_TRIPLE_UV(triangle VS_OUT_TRIPLE_UV In[3], inout TriangleStream<GS_OUT_TRIPLE_UV> TriStream)
{
	GS_OUT_TRIPLE_UV Out = (GS_OUT_TRIPLE_UV)0;

	/* Main Viewport */
	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition	= mul(In[i].vPosition, matVP);
		Out.vNormal		= In[i].vNormal;
		Out.vTangent	= In[i].vTangent;
		Out.vBiNormal	= In[i].vBiNormal;
		Out.vTexUV		= In[i].vTexUV;
		Out.vTexUV_2	= In[i].vTexUV_2;
		Out.vTexUV_3	= In[i].vTexUV_3;

		Out.vProjPosition = Out.vPosition;
		Out.vWorldPosition = In[i].vPosition;
		Out.iViewportIndex = 1;

		TriStream.Append(Out);
	}
	TriStream.RestartStrip();

	/* Sub Viewport */
	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition	= mul(In[j].vPosition, matVP);
		Out.vNormal		= In[j].vNormal;
		Out.vTangent	= In[j].vTangent;
		Out.vBiNormal	= In[j].vBiNormal;
		Out.vTexUV		= In[j].vTexUV;
		Out.vTexUV_2	= In[j].vTexUV_2;
		Out.vTexUV_2	= In[j].vTexUV_2;

		Out.vProjPosition = Out.vPosition;
		Out.vWorldPosition = In[j].vPosition;
		Out.iViewportIndex = 2;

		TriStream.Append(Out);
	}
	TriStream.RestartStrip();
}

struct GS_IN_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

struct GS_OUT_CSM_DEPTH
{
	float4 vPosition		: SV_POSITION;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};


[maxvertexcount(MAX_VERTICES)]
void GS_MAIN_CSM_DEPTH(triangle GS_IN_CSM_DEPTH In[3], inout TriangleStream<GS_OUT_CSM_DEPTH> TriStream)
{
	GS_OUT_CSM_DEPTH Out = (GS_OUT_CSM_DEPTH)0;

	/* Main Viewport - Viewport 0, 1, 2 */
	[unroll]
	for (uint mainViewIndex = 0; mainViewIndex < MAX_CASCADES; ++mainViewIndex)
	{
		[unroll]
		for (uint i = 0; i < 3; i++)
		{
			Out.vPosition = mul(In[i].vPosition, g_ShadowTransforms_Main[mainViewIndex]);
			Out.iViewportIndex = mainViewIndex;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}

	/* Sub Viewport - Viewport 3, 4, 5 */
	[unroll]
	for (uint subViewIndex = 0; subViewIndex < MAX_CASCADES; ++subViewIndex)
	{
		[unroll]
		for (uint j = 0; j < 3; j++)
		{
			Out.vPosition = mul(In[j].vPosition, g_ShadowTransforms_Sub[subViewIndex]);
			Out.iViewportIndex = MAX_CASCADES + subViewIndex;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
}
////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float4 vProjPosition		: TEXCOORD1;
	float4 vWorldPosition		: TEXCOORD2;
	uint   iViewportIndex		: SV_VIEWPORTARRAYINDEX;
};

struct PS_IN_TRIPLE_UV
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_2				: TEXCOORD1;
	float2 vTexUV_3				: TEXCOORD2;
	float4 vProjPosition		: TEXCOORD3;
	float4 vWorldPosition		: TEXCOORD4;
	uint   iViewportIndex		: SV_VIEWPORTARRAYINDEX;
};

struct PS_OUT
{
	vector	vDiffuse			: SV_TARGET0;
	vector	vNormal				: SV_TARGET1;
	vector	vDepth				: SV_TARGET2;
	vector	vShadow				: SV_TARGET3;
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

	Out.vDiffuse.rgb = vMtrlDiffuse.b;
	Out.vDiffuse.a = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	//// Calculate Shadow
	//int iIndex = -1;
	//iIndex = Get_CascadedShadowSliceIndex(In.iViewportIndex, In.vWorldPosition);
	//
	//// Get_ShadowFactor
	//float fShadowFactor = 0.f;
	//fShadowFactor = Get_ShadowFactor(In.iViewportIndex, iIndex, In.vWorldPosition);
	//
	//Out.vShadow = 1.f - fShadowFactor;
	//Out.vShadow.a = 1.f;

	return Out;
}

PS_OUT	PS_MAIN_RESPAWN_PORTAL(PS_IN_TRIPLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vFX_tex = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV_2);
	float fWeight = (vFX_tex.b * 0.8f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV + fWeight);

	Out.vDiffuse.rgb = (vMtrlDiffuse.r - (vMtrlDiffuse.g * 0.5f)) * g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, g_vColorRamp_UV) * 3.f;
	Out.vDiffuse.a = Out.vDiffuse.b * 0.9f;
	//vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	//if (0.f < vFX_tex.a)
	//	Out.vDiffuse.rgb = (vMtrlDiffuse.a) * g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, g_vColorRamp_UV + 0.5f) * 5.f;


	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_GRAVITYPIPE(PS_IN_TRIPLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 vDistortionUV = In.vTexUV;
	vDistortionUV.x += g_fTime * 0.33333333f;
	vDistortionUV.y += g_fTime;
	float4 vFX_tex = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistortionUV);
	float fWeight = (vFX_tex.b * 0.5f);

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV + fWeight);

	float2 vflipUV = { In.vTexUV.y, In.vTexUV.x };
	vflipUV += g_fTime * 0.33333333f;
	vector vColor = g_ColorRampTexture.Sample(Wrap_MinMagMipLinear_Sampler, vflipUV - fWeight);
	Out.vDiffuse.rgb = vMtrlDiffuse.r * vColor.rgb;
	Out.vDiffuse.a = Out.vDiffuse.r * 0.9f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_WORMHOLE(PS_IN_TRIPLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 vFlipUV; 
	vFlipUV.x = In.vTexUV.y;
	vFlipUV.y = In.vTexUV.x;
	In.vTexUV.x += g_fTime * 0.25;
	In.vTexUV.y += g_fTime;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);


	vector vColor = g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, vFlipUV);
	Out.vDiffuse.rgb = (vMtrlDiffuse.r * 2.f ) * vColor.rgb;
	Out.vDiffuse.a = Out.vDiffuse.r;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_RESPAWNTENNEL(PS_IN_TRIPLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	float2 vFlipUV;
	vFlipUV.x= In.vTexUV.y + 1.f;
	vFlipUV.y= In.vTexUV.x;

	vector vColor = g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, vFlipUV);
	Out.vDiffuse.rgb = vColor.rgb;
	Out.vDiffuse.a = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	return Out;
}

PS_OUT	PS_MAIN_ALPHA(PS_IN In, uniform bool isOpaque)
{
	PS_OUT Out = (PS_OUT)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse * g_Material.vDiffuse;
	if (true == isOpaque) Out.vDiffuse.w = 1.f;
	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

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
	pass Default // 0
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Write_CascadedShadowDepth // 1
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile		vs_5_0 VS_MAIN_CSM_DEPTH();
		GeometryShader = compile	gs_5_0 GS_MAIN_CSM_DEPTH();
		PixelShader = compile		ps_5_0 PS_MAIN_CSM_DEPTH();
	}

	pass Default_Alpha // 2
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA(false);
	}

	pass Default_Alpha_Opaque // 3
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA(true);
	}

	pass RespawnPortal // 4
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TRIPLE_UV();
		GeometryShader = compile gs_5_0 GS_TRIPLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_RESPAWN_PORTAL();
	}

	pass GravityPipe // 5
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TRIPLE_UV();
		GeometryShader = compile gs_5_0 GS_TRIPLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_GRAVITYPIPE();
	}

	pass Wormhole // 6
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TRIPLE_UV();
		GeometryShader = compile gs_5_0 GS_TRIPLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_WORMHOLE();
	}

	pass RespawnTennel // 7
	{
		SetRasterizerState(Rasterizer_CCW);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_TRIPLE_UV();
		GeometryShader = compile gs_5_0 GS_TRIPLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_RESPAWNTENNEL();
	}
};