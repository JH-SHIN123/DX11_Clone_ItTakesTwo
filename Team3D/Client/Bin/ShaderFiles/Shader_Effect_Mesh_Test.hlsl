#include "Shader_Include.hpp"
#include "Shader_Defines_Effect.hpp"

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

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix = (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) 
		+ (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) 
		+ (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) 
		+ (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW = mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matBW);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW));
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

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
	//float4 vNormal			: NORMAL;
	//float3 vTangent			: TANGENT;
	//float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	//float4 vWorldPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(12)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out[8];

	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(0.01f, 0.01f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* ÁÂ»ó */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = In[0].vTexUV;
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* ¿ì»ó */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = In[0].vTexUV;
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* ¿ìÇÏ */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = In[0].vTexUV;
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* ÁÂÇÏ */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = In[0].vTexUV;
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = In[0].vTexUV;
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* ¿ì»ó */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = In[0].vTexUV;
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* ¿ìÇÏ */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = In[0].vTexUV;
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* ÁÂÇÏ */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = In[0].vTexUV;
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition		: SV_POSITION;
	//float4 vNormal			: NORMAL;
	//float3 vTangent			: TANGENT;
	//float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	//float4 vWorldPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct PS_OUT
{
	vector	vDiffuse	: SV_TARGET0;
	//vector	vNormal		: SV_TARGET1;
	//vector	vDepth		: SV_TARGET2;
	//vector	vShadow		: SV_TARGET3;
	//vector	vSpecular	: SV_TARGET4;
	//vector	vEmissive	: SV_TARGET5;
};

struct PS_OUT_ALPHA
{
	vector	vDiffuse			: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;

	return Out;
}
////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{	
	pass Default_Skinned // 0
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};