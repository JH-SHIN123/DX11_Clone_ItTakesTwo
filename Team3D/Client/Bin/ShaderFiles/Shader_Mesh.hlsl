#include "Shader_Include.hpp"
////////////////////////////////////////////////////////////
#define MAX_VERTICES NUM_VERTICES * MAX_CASCADES * NUM_VIEWPORTS
#define NUM_VERTICES 3
#define NUM_VIEWPORTS 2

texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;
texture2D	g_EmissiveTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;

cbuffer BoneMatrixDesc
{
	BONEMATRICES	g_BoneMatrices;
};
////////////////////////////////////////////////////////////

struct VS_IN
{
	float3 vPosition	: POSITION;
	float3 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float2 vTexUV		: TEXCOORD0;
	uint4  vBlendIndex	: BLENDINDEX;
	float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT_CSM_DEPTH
{
	float4 vPosition : SV_POSITION;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix	= (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW		= mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition	= mul(vector(In.vPosition, 1.f), matBW);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTangent	= normalize(mul(vector(In.vTangent, 0.f), matBW));
	Out.vBiNormal	= normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV		= In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_NO_BONE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition	= mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent	= normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix));
	Out.vBiNormal	= normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV		= In.vTexUV;

	return Out;
}

VS_OUT_CSM_DEPTH VS_MAIN_CSM_DEPTH(VS_IN In, uniform bool isSkinned)
{
	VS_OUT_CSM_DEPTH Out = (VS_OUT_CSM_DEPTH)0;

	matrix World = g_WorldMatrix;
	if (true == isSkinned)
	{
		matrix	BoneMatrix = (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
		World = mul(BoneMatrix, World);
	}

	Out.vPosition = mul(vector(In.vPosition, 1.f), World);

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
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float3 vTangent			: TANGENT;
	float3 vBiNormal		: BINORMAL;
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

	/* Main Viewport */
	if (g_iViewportDrawInfo & 1)
	{
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
	}
	
	if (g_iViewportDrawInfo & 2)
	{
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
}

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
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float3 vTangent			: TANGENT;
	float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	float4 vWorldPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct PS_OUT
{
	vector	vDiffuse	: SV_TARGET0;
	vector	vNormal		: SV_TARGET1;
	vector	vDepth		: SV_TARGET2;
	vector	vShadow		: SV_TARGET3;
	vector	vSpecular	: SV_TARGET4;
	vector	vEmissive	: SV_TARGET5;
};

struct PS_OUT_ALPHA
{
	vector	vDiffuse			: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse	= vMtrlDiffuse;
	Out.vDepth		= vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	if (g_IsMaterials.Is_Emissive & 1) Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	// Calculate Shadow
	int iIndex = -1;
	iIndex = Get_CascadedShadowSliceIndex(In.iViewportIndex, In.vWorldPosition);

	// Get_ShadowFactor
	float fShadowFactor = 0.f;
	fShadowFactor = Get_ShadowFactor(In.iViewportIndex, iIndex, In.vWorldPosition);

	Out.vShadow = 1.f - fShadowFactor;
	Out.vShadow.a = 1.f;

	return Out;
}
////////////////////////////////////////////////////////////
PS_OUT_ALPHA PS_MAIN_ALPHA(PS_IN In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse * g_Material.vDiffuse;

	//Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
	//Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	return Out;
}

PS_OUT_ALPHA PS_MAIN_ALPHA_MOONBABOON_GLASSWALL(PS_IN In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse * g_Material.vDiffuse;
	Out.vDiffuse.w += 0.1;

	return Out;
}

PS_OUT_ALPHA PS_MAIN_ALPHA_MOONBABOON_CORE_GLASS(PS_IN In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;
	vector vMtrlDiffuse = g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse * g_Material.vDiffuse;
	Out.vDiffuse.w = 0.6;

	return Out;
}

PS_OUT_ALPHA PS_MAIN_ALPHA_MOONBABOON_CORE_PILLAR_GLASS(PS_IN In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse * g_Material.vDiffuse;
	Out.vDiffuse.w = 0.6;

	return Out;
}

PS_OUT	PS_LOW_EMISSIVE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	if (g_IsMaterials.Is_Emissive & 1) Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vEmissive = 0.f;

	// Calculate Shadow
	int iIndex = -1;
	iIndex = Get_CascadedShadowSliceIndex(In.iViewportIndex, In.vWorldPosition);

	// Get_ShadowFactor
	float fShadowFactor = 0.f;
	fShadowFactor = Get_ShadowFactor(In.iViewportIndex, iIndex, In.vWorldPosition);

	Out.vShadow = 1.f - fShadowFactor;
	Out.vShadow.a = 1.f;

	return Out;
}

PS_OUT	PS_ALIENSCREEN(PS_IN In, uniform bool isGreen)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	
	if(isGreen)
		Out.vDiffuse = vMtrlDiffuse * vector(0.f, 1.f, 0.f, 1.f);

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, 0.f, 0.f);

	// Calculate Normal
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	// Calculate Shadow
	int iIndex = -1;
	iIndex = Get_CascadedShadowSliceIndex(In.iViewportIndex, In.vWorldPosition);

	// Get_ShadowFactor
	float fShadowFactor = 0.f;
	fShadowFactor = Get_ShadowFactor(In.iViewportIndex, iIndex, In.vWorldPosition);

	Out.vShadow = 1.f - fShadowFactor;
	Out.vShadow.a = 1.f;

	return Out;
}

technique11 DefaultTechnique
{
	// 0
	pass Default_Skinned
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
	// 1
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
	// 2
	pass Write_CascadedShadowDepth_Skinned
	{
		SetRasterizerState(Rasterizer_Shadow);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile		vs_5_0 VS_MAIN_CSM_DEPTH(true);
		GeometryShader = compile	gs_5_0 GS_MAIN_CSM_DEPTH();
		PixelShader = NULL;
	}
	// 3
	pass Write_CascadedShadowDepth
	{
		SetRasterizerState(Rasterizer_Shadow);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile		vs_5_0 VS_MAIN_CSM_DEPTH(false);
		GeometryShader = compile	gs_5_0 GS_MAIN_CSM_DEPTH();
		PixelShader = NULL;
	}
	// 4
	pass Default_Alpha
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}
	// 5
	pass Default_Alpha_MoonBaboon_GlassWall
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MOONBABOON_GLASSWALL();
	}
	// 6
	pass Default_Alpha_MoonBaboon_Core_Glass
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MOONBABOON_CORE_GLASS();
	}
	// 7
	pass Default_Alpha_MoonBaboon_Core_Pillar_Glass
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MOONBABOON_CORE_PILLAR_GLASS();
	}
	// 8
	pass Default_Low_Emissive
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LOW_EMISSIVE();
	}
	// 9 Blue Alien
	pass Default_Alien_Blue
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_ALIENSCREEN(false);
	}
	// 10
	pass Default_Alien_Green
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_ALIENSCREEN(true);
	}
};