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

/* _____________________________________Effect_____________________________________*/
texture2D	g_MaskingTexture;
texture2D	g_DissolveTexture;
texture2D	g_FlowTexture;

cbuffer Mesh_EffectDesc
{
	float			g_fTime;
	float			g_fAlpha;
	float			g_fRadius; // 시작 사이즈
	float			g_fDissolveTime; // 0~1
	float			g_fFlowPower;
	float2			g_vParticleSize;
	float3			g_vDir;
	float4			g_vPos;
	float4			g_vColor;
	float4			g_vTextureUV_LTRB;
	int2			g_vTextureSize;
	int2			g_vTextureSize_2;
	//float3			g_vDir_Array[256];
	//float4			g_vPos_Array[256];
};
/* ________________________________________________________________________________*/

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

struct VS_OUT_VOLUME
{
	float4 vPosition	: SV_POSITION;
	float3 vVolumeColor : TEXCOORD0;
	float2 vTexUV		: TEXCOORD1;
};

struct VS_OUT_FRESNEL
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vMainCamRefl	: TEXCOORD1;
	float4 vSubCamRefl  : TEXCOORD2;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix = (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW = mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matBW);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT VS_MAIN_NO_BONE(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

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

VS_OUT_FRESNEL VS_FRESNEL(VS_IN In)
{
	VS_OUT_FRESNEL Out = (VS_OUT_FRESNEL)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;

	// MainCam
	float3 vPosW = Out.vPosition.xyz;
	float3 vNormalW = Out.vNormal.xyz;
	float3 l = normalize(vPosW - g_vMainCamPosition);
	float Scale = 2.5f;
	float Power = 2.f; // 점점 진해지는 강도세기

	Out.vMainCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

	// SubCam
	l = normalize(vPosW - g_vSubCamPosition);
	Out.vSubCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

	return Out;
}

VS_OUT_FRESNEL VS_FRESNEL_STAR(VS_IN In)
{
	VS_OUT_FRESNEL Out = (VS_OUT_FRESNEL)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;

	// MainCam
	float3 vPosW = Out.vPosition.xyz;
	float3 vNormalW = Out.vNormal.xyz;
	float3 l = normalize(vPosW - g_vMainCamPosition);
	float Scale = 1.f;
	float Power = 1.f; // 점점 진해지는 강도세기

	Out.vMainCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

	// SubCam
	l = normalize(vPosW - g_vSubCamPosition);
	Out.vSubCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

	return Out;
}

/* _____________________________________Effect_____________________________________*/
VS_OUT	VS_MAIN_EFFECT(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix = (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW = mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matBW);

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT	VS_MAIN_EFFECT_POSDIR(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	matrix	BoneMatrix = (g_BoneMatrices.Matrices[In.vBlendIndex.x] * In.vBlendWeight.x) + (g_BoneMatrices.Matrices[In.vBlendIndex.y] * In.vBlendWeight.y) + (g_BoneMatrices.Matrices[In.vBlendIndex.z] * In.vBlendWeight.z) + (g_BoneMatrices.Matrices[In.vBlendIndex.w] * In.vBlendWeight.w);
	matrix	matBW = mul(BoneMatrix, g_WorldMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matBW);

	float3	vDir = (Out.vPosition - g_vPos).xyz;
	float	fLength = length(vDir);
	Out.vPosition.xyz += (vDir)* g_fTime;

	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), matBW));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), matBW)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	return Out;
}
/* ________________________________________________________________________________*/

/* _____________________________________Volume_____________________________________*/
VS_OUT_VOLUME VS_MAIN_VOLUME(VS_IN In)
{
	VS_OUT_VOLUME Out = (VS_OUT_VOLUME)0;

	matrix WorldMatrix = g_WorldMatrix;
	float3 vVolumeColor = WorldMatrix._14_24_34;
	WorldMatrix._14_24_34 = 0.f;

	Out.vPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);
	Out.vVolumeColor = vVolumeColor;
	Out.vTexUV = In.vTexUV;

	return Out;
}
/* ________________________________________________________________________________*/

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct GS_IN_FRESNEL
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vMainCamRefl	: TEXCOORD1;
	float4 vSubCamRefl  : TEXCOORD2;
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

struct GS_OUT_FRESNEL
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vRefl			: TEXCOORD1;
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

struct GS_IN_VOLUME
{
	float4 vPosition	: SV_POSITION;
	float3 vVolumeColor : TEXCOORD0;
	float2 vTexUV		: TEXCOORD1;
};

struct GS_OUT_VOLUME
{
	float4 vPosition		: SV_POSITION;
	float3 vVolumeColor		: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	float2 vTexUV			: TEXCOORD2;
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

[maxvertexcount(6)]
void GS_FRESNEL(triangle GS_IN_FRESNEL In[3], inout TriangleStream<GS_OUT_FRESNEL> TriStream)
{
	GS_OUT_FRESNEL Out = (GS_OUT_FRESNEL)0;

	/* Main Viewport */
	if (g_iViewportDrawInfo & 1)
	{
		for (uint i = 0; i < 3; i++)
		{
			matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

			Out.vPosition = mul(In[i].vPosition, matVP);
			Out.vNormal = In[i].vNormal;
			Out.vTexUV = In[i].vTexUV;
			Out.vRefl = In[i].vMainCamRefl;
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
			Out.vTexUV = In[j].vTexUV;
			Out.vRefl = In[j].vSubCamRefl;
			Out.iViewportIndex = 2;

			TriStream.Append(Out);
		}
		TriStream.RestartStrip();
	}
}


/* _____________________________________Effect_____________________________________*/
struct GS_OUT_DOUBLE_UV
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float3 vTangent			: TANGENT;
	float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float2 vTexUV_2			: TEXCOORD1;
	float4 vProjPosition	: TEXCOORD2;
	float4 vWorldPosition	: TEXCOORD3;
	uint	iNotParticle	: TEXCOORD4;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;

	/*
	GS_IN
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
	*/
};
[maxvertexcount(12)]//// triangle
void GS_MAIN_ASH_DISSOLVE(triangle GS_IN In[3], inout TriangleStream<GS_OUT_DOUBLE_UV> TriStream)
{
	GS_OUT_DOUBLE_UV		Out[8];

	[unroll]
	for (int k = 0; k < 8; ++k)
		Out[k] = (GS_OUT_DOUBLE_UV)0;

	// 평균
	float2 fAvg_UV		 = (In[0].vTexUV	+ In[1].vTexUV		+ In[2].vTexUV)		/ 3;
	float4 fAvg_Normal	 = (In[0].vNormal	+ In[1].vNormal		+ In[2].vNormal)	/ 3;
	float3 fAvg_BiNormal = (In[0].vBiNormal + In[1].vBiNormal	+ In[2].vBiNormal)	/ 3;
	float3 fAvg_Tangent  = (In[0].vTangent	+ In[1].vTangent	+ In[2].vTangent)	/ 3;
	float4 fAvg_Pos		 = (In[0].vPosition + In[1].vPosition	+ In[2].vPosition)	/ 3;
	fAvg_Pos.w = 1.f;

	// 가장 00에 가까운놈을 기준으로 UV다시 세팅
	float2 vTexUV_Less = In[0].vTexUV;
	float fTexUV_Dist = length(In[0].vTexUV);
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		if (fTexUV_Dist > length(In[i].vTexUV))
		{
			fTexUV_Dist = length(In[i].vTexUV);
			vTexUV_Less = In[i].vTexUV;
		}
	}
	vTexUV_Less = fAvg_UV - vTexUV_Less;
	float4 vTextureUV_LTRB = (float4)0; // 적당한 UV
	vTextureUV_LTRB.x = fAvg_UV.x - vTexUV_Less.x;
	vTextureUV_LTRB.y = fAvg_UV.y - vTexUV_Less.y;
	vTextureUV_LTRB.z = fAvg_UV.x + vTexUV_Less.x;
	vTextureUV_LTRB.w = fAvg_UV.y + vTexUV_Less.y;

	// 시간값에 따른 가중치 
	int3 vTexPos = (int3)0;
	modf(fAvg_UV.x * g_vTextureSize.x, vTexPos.x);
	modf(fAvg_UV.y * g_vTextureSize.y, vTexPos.y);
	float fDissolve_Value = g_DissolveTexture.Load(vTexPos).r;
	float fDissolve_Weight = clamp(g_fDissolveTime * 2.f - fDissolve_Value, 0, 1);

	float3 vWorldPos = g_WorldMatrix._41_42_43 + g_WorldMatrix._21_22_23;
	float3 vDir_Right = normalize(fAvg_Pos - vWorldPos);

	modf(fAvg_UV.x * g_vTextureSize_2.x, vTexPos.x);
	modf(fAvg_UV.y * g_vTextureSize_2.y, vTexPos.y);
	float4 vTextureDir = g_FlowTexture.Load(vTexPos) * g_fFlowPower;
	float3 vDir = (normalize(g_WorldMatrix._31_32_33) * vTextureDir.r * -1.f)
		+ (normalize(g_WorldMatrix._21_22_23) * vTextureDir.g * 2.f)
		+ (vDir_Right * ((vTextureDir.g + vTextureDir.r) * 0.75f));

	float4 vPseudoRandomPos = (fAvg_Pos) /*+ g_vDirection*/;
	vPseudoRandomPos.xyz += (vDir.xyz * g_fTime); // 확장 가중치

	float4 vPos = lerp(fAvg_Pos, vPseudoRandomPos, fDissolve_Weight); // 보간
	float fSize = lerp(g_fRadius, 0, fDissolve_Weight); // 각자의 사이즈를 보간

	if (g_iViewportDrawInfo & 1)
	{
		if (fDissolve_Weight > 0)
		{
			float3		vLook = normalize(g_vMainCamPosition - vPos).xyz;
			float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
			float3		vRight = normalize(cross(vAxisY, vLook));
			float3		vUp	= normalize(cross(vLook, vRight));
			matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

			float		fHalfSize = fSize * 0.5f;

			float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	fHalfSize;
			float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	fHalfSize;

			Out[0].vPosition = vPos + vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[1].vPosition = vPos - vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[2].vPosition = vPos - vWolrdPointPos_X - vWolrdPointPos_Y;
			Out[3].vPosition = vPos + vWolrdPointPos_X - vWolrdPointPos_Y;

			Out[0].vTexUV	= float2(vTextureUV_LTRB.x, vTextureUV_LTRB.y);
			Out[1].vTexUV	= float2(vTextureUV_LTRB.z, vTextureUV_LTRB.y);
			Out[2].vTexUV	= float2(vTextureUV_LTRB.z, vTextureUV_LTRB.w);
			Out[3].vTexUV	= float2(vTextureUV_LTRB.x, vTextureUV_LTRB.w);

			Out[0].vTexUV_2 = float2(0.f, 0.f);
			Out[1].vTexUV_2 = float2(1.f, 0.f);
			Out[2].vTexUV_2 = float2(1.f, 1.f);
			Out[3].vTexUV_2 = float2(0.f, 1.f);

			[unroll]
			for (uint l = 0; l < 4; ++l)
			{
				Out[l].vWorldPosition	= Out[l].vPosition;
				Out[l].vPosition		= mul(Out[l].vPosition, matVP);
				Out[l].vProjPosition	= Out[l].vPosition;
					
				Out[l].vNormal = fAvg_Normal;
				Out[l].vTangent = fAvg_Tangent;
				Out[l].vBiNormal = fAvg_BiNormal;
				Out[l].iViewportIndex = 1;
				Out[l].iNotParticle	= 1;
			}

			TriStream.Append(Out[0]);
			TriStream.Append(Out[1]);
			TriStream.Append(Out[2]);
			TriStream.RestartStrip();

			TriStream.Append(Out[0]);
			TriStream.Append(Out[2]);
			TriStream.Append(Out[3]);
			TriStream.RestartStrip();
		}
		else
		{
			[unroll]
			for (uint n = 0; n < 3; n++)
			{
				matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

				Out[n].vPosition = mul(In[n].vPosition, matVP);
				Out[n].vNormal = In[n].vNormal;
				Out[n].vTangent = In[n].vTangent;
				Out[n].vBiNormal = In[n].vBiNormal;
				Out[n].vTexUV = In[n].vTexUV;
				Out[n].vProjPosition = Out[n].vPosition;
				Out[n].vWorldPosition = In[n].vPosition;
				Out[n].iViewportIndex = 1;
				Out[n].iNotParticle = 0;

				TriStream.Append(Out[n]);
			}
			TriStream.RestartStrip();
		}
	}

	if (g_iViewportDrawInfo & 2)
	{
		if (fDissolve_Weight > 0)
		{
			float3		vLook = normalize(g_vSubCamPosition - vPos).xyz;
			float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
			float3		vRight = normalize(cross(vAxisY, vLook));
			float3		vUp = normalize(cross(vLook, vRight));
			matrix		matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

			float		fHalfSize = fSize * 0.5f;

			float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	fHalfSize;
			float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	fHalfSize;

			Out[4].vPosition = vPos + vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[5].vPosition = vPos - vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[6].vPosition = vPos - vWolrdPointPos_X - vWolrdPointPos_Y;
			Out[7].vPosition = vPos + vWolrdPointPos_X - vWolrdPointPos_Y;

			Out[4].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.y);
			Out[5].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.y);
			Out[6].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.w);
			Out[7].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.w);

			Out[4].vTexUV_2 = float2(0.f, 0.f);
			Out[5].vTexUV_2 = float2(1.f, 0.f);
			Out[6].vTexUV_2 = float2(1.f, 1.f);
			Out[7].vTexUV_2 = float2(0.f, 1.f);

			[unroll]
			for (uint m = 4; m < 8; ++m)
			{
				Out[m].vWorldPosition = Out[m].vPosition;
				Out[m].vPosition = mul(Out[m].vPosition, matVP);
				Out[m].vProjPosition = Out[m].vPosition;
					
				Out[m].vNormal = fAvg_Normal;
				Out[m].vTangent = fAvg_Tangent;
				Out[m].vBiNormal = fAvg_BiNormal;
				Out[m].iViewportIndex = 2;
				Out[m].iNotParticle = 1;
			}

			TriStream.Append(Out[4]);
			TriStream.Append(Out[5]);
			TriStream.Append(Out[6]);
			TriStream.RestartStrip();

			TriStream.Append(Out[4]);
			TriStream.Append(Out[6]);
			TriStream.Append(Out[7]);
		}
		else
		{
			[unroll]
			for (uint o = 0; o < 3; o++)
			{
				matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

				Out[o].vPosition = mul(In[o].vPosition, matVP);
				Out[o].vNormal = In[o].vNormal;
				Out[o].vTangent = In[o].vTangent;
				Out[o].vBiNormal = In[o].vBiNormal;
				Out[o].vTexUV = In[o].vTexUV;
				Out[o].vProjPosition = Out[o].vPosition;
				Out[o].vWorldPosition = In[o].vPosition;
				Out[o].iViewportIndex = 2;
				Out[o].iNotParticle = 0;

				TriStream.Append(Out[o]);
			}
			TriStream.RestartStrip();
		}
	}
}

[maxvertexcount(12)]//// triangle
void GS_MAIN_ASH_DISSOLVE_NORMAL(triangle GS_IN In[3], inout TriangleStream<GS_OUT_DOUBLE_UV> TriStream)
{
	GS_OUT_DOUBLE_UV		Out[8];

	[unroll]
	for (int k = 0; k < 8; ++k)
		Out[k] = (GS_OUT_DOUBLE_UV)0;

	// 평균
	float2 fAvg_UV = (In[0].vTexUV + In[1].vTexUV + In[2].vTexUV) / 3;
	float4 fAvg_Normal = (In[0].vNormal + In[1].vNormal + In[2].vNormal) / 3;
	float3 fAvg_BiNormal = (In[0].vBiNormal + In[1].vBiNormal + In[2].vBiNormal) / 3;
	float3 fAvg_Tangent = (In[0].vTangent + In[1].vTangent + In[2].vTangent) / 3;
	float4 fAvg_Pos = (In[0].vPosition + In[1].vPosition + In[2].vPosition) / 3;
	fAvg_Pos.w = 1.f;

	// 가장 00에 가까운놈을 기준으로 UV다시 세팅
	float2 vTexUV_Less = In[0].vTexUV;
	float fTexUV_Dist = length(In[0].vTexUV);
	[unroll]
	for (int i = 0; i < 3; ++i)
	{
		if (fTexUV_Dist > length(In[i].vTexUV))
		{
			fTexUV_Dist = length(In[i].vTexUV);
			vTexUV_Less = In[i].vTexUV;
		}
	}
	vTexUV_Less = fAvg_UV - vTexUV_Less;
	float4 vTextureUV_LTRB = (float4)0; // 적당한 UV
	vTextureUV_LTRB.x = fAvg_UV.x - vTexUV_Less.x;
	vTextureUV_LTRB.y = fAvg_UV.y - vTexUV_Less.y;
	vTextureUV_LTRB.z = fAvg_UV.x + vTexUV_Less.x;
	vTextureUV_LTRB.w = fAvg_UV.y + vTexUV_Less.y;

	// 시간값에 따른 가중치 
	int3 vTexPos = (int3)0;
	modf(fAvg_UV.x * g_vTextureSize.x, vTexPos.x);
	modf(fAvg_UV.y * g_vTextureSize.y, vTexPos.y);
	float fDissolve_Value = g_DissolveTexture.Load(vTexPos).r;
	float fDissolve_Weight = clamp(g_fDissolveTime * 2.f - fDissolve_Value, 0, 1);

	float3 vWorldPos = g_WorldMatrix._41_42_43 + g_WorldMatrix._21_22_23;
	float3 vDir_Right = normalize(fAvg_Pos - vWorldPos);

	modf(fAvg_UV.x * g_vTextureSize_2.x, vTexPos.x);
	modf(fAvg_UV.y * g_vTextureSize_2.y, vTexPos.y);
	float4 vTextureDir = g_FlowTexture.Load(vTexPos) * g_fFlowPower;
	float3 vDir = (normalize(g_WorldMatrix._31_32_33) * vTextureDir.r * -1.f)
		+ (normalize(g_WorldMatrix._21_22_23) * vTextureDir.g * 2.f)
		+ (vDir_Right * ((vTextureDir.g + vTextureDir.r) * 0.75f));

	float3 vDir_Tan = normalize(fAvg_Tangent * 0.5f + vDir);
	vDir = lerp(vDir_Tan, vDir, fDissolve_Weight); // 보간v
	vDir = normalize(vDir) * g_fFlowPower;

	float4 vPseudoRandomPos = (fAvg_Pos) /*+ g_vDirection*/;
	vPseudoRandomPos.xyz += (vDir.xyz * g_fTime * 1.5f); // 확장 가중치

	float4 vPos = lerp(fAvg_Pos, vPseudoRandomPos, fDissolve_Weight); // 보간
	float fSize = lerp(g_fRadius, 0, fDissolve_Weight); // 각자의 사이즈를 보간

	if (g_iViewportDrawInfo & 1)
	{
		if (fDissolve_Weight > fDissolve_Value)
		{
			float3		vLook = normalize(g_vMainCamPosition - vPos).xyz;
			float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
			float3		vRight = normalize(cross(vAxisY, vLook));
			float3		vUp = normalize(cross(vLook, vRight));
			matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

			float		fHalfSize = fSize * 0.5f;

			float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	fHalfSize;
			float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	fHalfSize;

			Out[0].vPosition = vPos + vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[1].vPosition = vPos - vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[2].vPosition = vPos - vWolrdPointPos_X - vWolrdPointPos_Y;
			Out[3].vPosition = vPos + vWolrdPointPos_X - vWolrdPointPos_Y;

			Out[0].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.y);
			Out[1].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.y);
			Out[2].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.w);
			Out[3].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.w);

			Out[0].vTexUV_2 = float2(0.f, 0.f);
			Out[1].vTexUV_2 = float2(1.f, 0.f);
			Out[2].vTexUV_2 = float2(1.f, 1.f);
			Out[3].vTexUV_2 = float2(0.f, 1.f);

			[unroll]
			for (uint l = 0; l < 4; ++l)
			{
				Out[l].vWorldPosition = Out[l].vPosition;
				Out[l].vPosition = mul(Out[l].vPosition, matVP);
				Out[l].vProjPosition = Out[l].vPosition;

				Out[l].vNormal = fAvg_Normal;
				Out[l].vTangent = fAvg_Tangent;
				Out[l].vBiNormal = fAvg_BiNormal;
				Out[l].iViewportIndex = 1;
				Out[l].iNotParticle = 1;
			}

			TriStream.Append(Out[0]);
			TriStream.Append(Out[1]);
			TriStream.Append(Out[2]);
			TriStream.RestartStrip();

			TriStream.Append(Out[0]);
			TriStream.Append(Out[2]);
			TriStream.Append(Out[3]);
			TriStream.RestartStrip();
		}
		else
		{
			[unroll]
			for (uint n = 0; n < 3; n++)
			{
				matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

				Out[n].vPosition = mul(In[n].vPosition, matVP);
				Out[n].vNormal = In[n].vNormal;
				Out[n].vTangent = In[n].vTangent;
				Out[n].vBiNormal = In[n].vBiNormal;
				Out[n].vTexUV = In[n].vTexUV;
				Out[n].vProjPosition = Out[n].vPosition;
				Out[n].vWorldPosition = In[n].vPosition;
				Out[n].iViewportIndex = 1;
				Out[n].iNotParticle = 0;

				TriStream.Append(Out[n]);
			}
			TriStream.RestartStrip();
		}
	}

	if (g_iViewportDrawInfo & 2)
	{
		if (fDissolve_Weight > fDissolve_Value)
		{
			float3		vLook = normalize(g_vSubCamPosition - vPos).xyz;
			float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
			float3		vRight = normalize(cross(vAxisY, vLook));
			float3		vUp = normalize(cross(vLook, vRight));
			matrix		matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

			float		fHalfSize = fSize * 0.5f;

			float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	fHalfSize;
			float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	fHalfSize;

			Out[4].vPosition = vPos + vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[5].vPosition = vPos - vWolrdPointPos_X + vWolrdPointPos_Y;
			Out[6].vPosition = vPos - vWolrdPointPos_X - vWolrdPointPos_Y;
			Out[7].vPosition = vPos + vWolrdPointPos_X - vWolrdPointPos_Y;

			Out[4].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.y);
			Out[5].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.y);
			Out[6].vTexUV = float2(vTextureUV_LTRB.z, vTextureUV_LTRB.w);
			Out[7].vTexUV = float2(vTextureUV_LTRB.x, vTextureUV_LTRB.w);

			Out[4].vTexUV_2 = float2(0.f, 0.f);
			Out[5].vTexUV_2 = float2(1.f, 0.f);
			Out[6].vTexUV_2 = float2(1.f, 1.f);
			Out[7].vTexUV_2 = float2(0.f, 1.f);

			[unroll]
			for (uint m = 4; m < 8; ++m)
			{
				Out[m].vWorldPosition = Out[m].vPosition;
				Out[m].vPosition = mul(Out[m].vPosition, matVP);
				Out[m].vProjPosition = Out[m].vPosition;

				Out[m].vNormal = fAvg_Normal;
				Out[m].vTangent = fAvg_Tangent;
				Out[m].vBiNormal = fAvg_BiNormal;
				Out[m].iViewportIndex = 2;
				Out[m].iNotParticle = 1;
			}

			TriStream.Append(Out[4]);
			TriStream.Append(Out[5]);
			TriStream.Append(Out[6]);
			TriStream.RestartStrip();

			TriStream.Append(Out[4]);
			TriStream.Append(Out[6]);
			TriStream.Append(Out[7]);
		}
		else
		{
			[unroll]
			for (uint o = 0; o < 3; o++)
			{
				matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);
	
				Out[o].vPosition = mul(In[o].vPosition, matVP);
				Out[o].vNormal = In[o].vNormal;
				Out[o].vTangent = In[o].vTangent;
				Out[o].vBiNormal = In[o].vBiNormal;
				Out[o].vTexUV = In[o].vTexUV;
				Out[o].vProjPosition = Out[o].vPosition;
				Out[o].vWorldPosition = In[o].vPosition;
				Out[o].iViewportIndex = 2;
				Out[o].iNotParticle = 0;

				TriStream.Append(Out[o]);
			}
			TriStream.RestartStrip();
		}
	}
}

/* ________________________________________________________________________________*/
/* _____________________________________Volume_____________________________________*/
[maxvertexcount(6)]
void GS_MAIN_VOLUME(triangle GS_IN_VOLUME In[3], inout TriangleStream<GS_OUT_VOLUME> TriStream)
{
	GS_OUT_VOLUME Out = (GS_OUT_VOLUME)0;

	/* Main Viewport */
	if (g_iViewportDrawInfo & 1)
	{
		for (uint i = 0; i < 3; i++)
		{
			matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

			Out.vPosition = mul(In[i].vPosition, matVP);
			Out.vVolumeColor = In[i].vVolumeColor;
			Out.vProjPosition = Out.vPosition;
			Out.vTexUV = In[i].vTexUV;
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
			Out.vVolumeColor = In[j].vVolumeColor;
			Out.vProjPosition = Out.vPosition;
			Out.vTexUV = In[j].vTexUV;
			Out.iViewportIndex = 2;

			TriStream.Append(Out);
		}
		TriStream.RestartStrip();
	}
}
/* ________________________________________________________________________________*/
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

struct PS_IN_FRESNEL
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vRefl		: TEXCOORD1;
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

struct PS_OUT_CUSTOMBLUR
{
	vector	vDiffuse			: SV_TARGET0;
	vector	vBlurValue			: SV_TARGET1;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
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
	Out.vDiffuse.w = 1.f;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	if (g_IsMaterials.Is_Emissive & 1) Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vEmissive *= 0.2f;

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

	if (isGreen)
		Out.vDiffuse = vMtrlDiffuse * vector(0.f, 1.f, 0.f, 1.f);
	else
		Out.vDiffuse = vMtrlDiffuse * vector(0.f, 0.f, 1.f, 1.f);

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vEmissive *= 0.5f;

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

PS_OUT	PS_LASERBUTTON(PS_IN In, uniform bool isGreen)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (isGreen)
		Out.vDiffuse = vMtrlDiffuse * vector(0.f, 1.f, 0.3f, 1.f);
	else
		Out.vDiffuse = vMtrlDiffuse * vector(0.f, 0.3f, 1.f, 1.f);

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = 0.2f;

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

PS_OUT	PS_LASER(PS_IN In, uniform bool isGreen)
{
	PS_OUT Out = (PS_OUT)0;

	if (isGreen)
		Out.vDiffuse = vector(0.f, 1.f, 0.3f, 1.f);
	else
		Out.vDiffuse = vector(0.f, 0.3f, 1.f, 1.f);

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = 0.5f;

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

PS_OUT_ALPHA PS_FRESNEL(PS_IN_FRESNEL In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;

	vector vOutColor = vector(0.7f, 0.7f, 0.95f, 0.9f);
	vector vInColor = vector(0.3f, 0.3f, 0.7f, 0.4f);

	Out.vDiffuse = lerp(vInColor, vOutColor, In.vRefl);
	return Out;
}

PS_OUT_ALPHA PS_FRESNEL_STAR(PS_IN_FRESNEL In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;

	vector vOutColor = vector(0.4f, 0.4f, 0.4f, 0.4f);
	vector vInColor = vector(0.3f, 0.3f, 0.3f, 0.3f);

	Out.vDiffuse = lerp(vInColor, vOutColor, In.vRefl);
	return Out;
}

PS_OUT PS_RADARSCREEN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse.rgb = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).rgb;
	Out.vDiffuse.w = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	if (g_IsMaterials.Is_Emissive & 1) Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vEmissive *= Out.vDiffuse * 1.5f;
	//Out.vEmissive = pow(Out.vEmissive, 4.5f) * 6.f;
	//Out.vEmissive.r = 0.f;

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

PS_OUT	PS_LASERBUTTONFRAME(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vMtrlSpecular = g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	float	fButtonMask = vMtrlSpecular.r;

	Out.vDiffuse = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
	Out.vDiffuse.w = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(vMtrlSpecular.xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = fButtonMask * vector(1.000000000f, 0.270588249f, 0.000000000f, 1.000000000f);
	Out.vEmissive *= 0.5f;

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

PS_OUT	PS_LASERSTATION(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vMtrlSpecular = g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
	Out.vDiffuse.w = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(vMtrlSpecular.xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
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

PS_OUT	PS_LASERBUTTONLARGE(PS_IN In, uniform bool isGreen)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector	vMtrlSpecular = g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = (vMtrlDiffuse.x + vMtrlDiffuse.y + vMtrlDiffuse.z) / 3.f;
	Out.vDiffuse += 0.3f;
	if (isGreen)
		Out.vDiffuse *= vector(0.f, 0.8f, 0.3f, 1.f);
	else
		Out.vDiffuse *= vector(0.f, 0.3f, 0.8f, 1.f);
	Out.vDiffuse.w = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(vMtrlSpecular.xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
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

PS_OUT_CUSTOMBLUR	PS_3DTEXT(PS_IN In)
{
	PS_OUT_CUSTOMBLUR Out = (PS_OUT_CUSTOMBLUR)0;

	vector	vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if(1 == vMtrlDiffuse.r)
		Out.vDiffuse = vector(1.f, 0.f, 0.f, 1.f);
	else if(1 == vMtrlDiffuse.g)
		Out.vDiffuse = vector(0.f, 1.f, 0.2f, 1.f);
	else if(1 == vMtrlDiffuse.b)
		Out.vDiffuse = vector(1.f, 1.f, 0.f, 1.f);
	Out.vDiffuse.w = 1.f;

	Out.vBlurValue = 0.5f;

	return Out;
}

PS_OUT_CUSTOMBLUR	PS_MESHPARTICLE(PS_IN In)
{
	PS_OUT_CUSTOMBLUR Out = (PS_OUT_CUSTOMBLUR)0;

	Out.vDiffuse = g_vColor;

	Out.vBlurValue = 0.5f;

	return Out;
}

PS_OUT	PS_MONITOR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = 0.15f;

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

PS_OUT	PS_CONTROLROOM_MONITOR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDiffuse.a = 1.f;

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	Out.vEmissive = 0.15f;

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

PS_OUT	PS_PLAYERHIT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	Out.vDiffuse *= vector(1.f, 0.f, 0.f, 1.f);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
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
/* _____________________________________Effect_____________________________________*/
struct PS_IN_DOUBLE_UV
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float3 vTangent			: TANGENT;
	float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float2 vTexUV_2			: TEXCOORD1;
	float4 vProjPosition	: TEXCOORD2;
	float4 vWorldPosition	: TEXCOORD3;
	uint   iNotParticle		: TEXCOORD4;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
PS_OUT	PS_EFFECT_MASKING(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse.rgb = vMtrlDiffuse.rgb;
	Out.vDiffuse.a = 1.f;

	if (0 < In.iNotParticle)
	{
		vector vMaskingTexture = g_MaskingTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV_2);
		
		if (0.01f > vMaskingTexture.r)
			discard;
		Out.vDiffuse.a = vMaskingTexture.r;
	}

	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

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
PS_OUT_ALPHA	PS_EFFECT_MASKING_ALPHAGROUP(PS_IN_DOUBLE_UV In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector vMaskingTexture = g_MaskingTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV_2);

	if (0.01f > vMaskingTexture.r)
		discard;

	Out.vDiffuse.rgb = g_vColor.rgb;
	Out.vDiffuse.a = vMaskingTexture.r;

	return Out;
}

PS_OUT_ALPHA PS_MAIN_WARPGATE_STAR(PS_IN In)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = g_vColor;

	return Out;
}

/* ________________________________________________________________________________*/
/* Volume */
struct PS_IN_VOLUME
{
	float4 vPosition		: SV_POSITION;
	float3 vVolumeColor		: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	float2 vTexUV			: TEXCOORD2;
};
struct PS_OUT_VOLUME
{
	vector vVolume	: SV_TARGET0;
};

PS_OUT_VOLUME PS_MAIN_VOLUME(PS_IN_VOLUME In)
{
	PS_OUT_VOLUME Out = (PS_OUT_VOLUME)0;

	float2 vTexUV = In.vTexUV;
	vTexUV.y -= g_fTime;
	float3 vPatternDesc = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, vTexUV);
	In.vVolumeColor = vPatternDesc * In.vVolumeColor;

	Out.vVolume = vector(In.vVolumeColor, In.vProjPosition.z / In.vProjPosition.w);

	return Out;
}
/* ________________________________________________________________________________*/
PS_OUT_ALPHA PS_PHANTOM(PS_IN In, uniform bool bBlue)
{
	PS_OUT_ALPHA Out = (PS_OUT_ALPHA)0;

	if(bBlue)
		Out.vDiffuse = vector(0.f,0.5f,1.f,0.7f);
	else
		Out.vDiffuse = vector(0.f, 1.f, 0.5f, 0.7f);

	return Out;
}

PS_OUT	PS_MOONBABOONCORE_BUTTON(PS_IN In, uniform bool bActive)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	Out.vDiffuse = vMtrlDiffuse;
	Out.vDepth = vector(In.vProjPosition.w / g_fMainCamFar, In.vProjPosition.z / In.vProjPosition.w, In.vProjPosition.x / In.vProjPosition.w, In.vProjPosition.y / In.vProjPosition.w);

	// Calculate Normal
	if (g_IsMaterials.Is_Normals & 1) Out.vNormal = TextureSampleToWorldSpace(g_NormalTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
	else Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

	// Calculate Specular
	if (g_IsMaterials.Is_Specular & 1) {
		Out.vSpecular = TextureSampleToWorldSpace(g_SpecularTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV).xyz, In.vTangent.xyz, In.vBiNormal.xyz, In.vNormal.xyz);
		Out.vSpecular.w = 0.f;
	}
	else Out.vSpecular = vector(0.f, 0.f, 0.f, 1.f);

	// Calculate Emissive
	if (g_IsMaterials.Is_Emissive & 1)
	{
		if(false == bActive)
			Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV) * vector(1.f, 0.f, 0.f,1.f);
		else
			Out.vEmissive = g_EmissiveTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	}

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
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
	// 1
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
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
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA_MOONBABOON_CORE_GLASS();
	}
	// 7
	pass Default_Alpha_MoonBaboon_Core_Pillar_Glass
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
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
	// 9
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
	// 11
	pass Skinned_PointDraw // Effect
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = compile gs_5_0 GS_MAIN_ASH_DISSOLVE();
		PixelShader = compile ps_5_0 PS_EFFECT_MASKING();
	}
	// 12
	pass Skinned_PointDraw_Move // Effect
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = compile gs_5_0 GS_MAIN_ASH_DISSOLVE_NORMAL();
		PixelShader = compile ps_5_0 PS_EFFECT_MASKING();
	}
	// 13
	pass Skinned_Triangle_Ash_Dissolve // Effect
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
		GeometryShader = compile gs_5_0 GS_MAIN_ASH_DISSOLVE_NORMAL();
		PixelShader = compile ps_5_0 PS_EFFECT_MASKING();
	}
	// 14
	pass Default_Laserbutton_Blue
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERBUTTON(false);
	}
	// 15
	pass Default_Laserbutton_Green
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERBUTTON(true);
	}
	// 16
	pass Default_Laser_Blue
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASER(false);
	}
	// 17
	pass Default_Laser_Green
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASER(true);
	}

	// 18
	pass Default_Fresnel
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FRESNEL();
		GeometryShader = compile gs_5_0 GS_FRESNEL();
		PixelShader = compile ps_5_0 PS_FRESNEL();
	}
	// 19
	pass Default_Radar_Screen
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_RADARSCREEN();
	}
	// 20
	pass Default_BoneAlpha
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN_ALPHA();
	}

	// 21
	pass Volume_Front /* Volume의 앞면 깊이값 */
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_VOLUME();
		GeometryShader = compile gs_5_0 GS_MAIN_VOLUME();
		PixelShader = compile ps_5_0 PS_MAIN_VOLUME();
	}
	// 22 
	pass Volume_Back /* Volume의 뒷면 깊이값 */
	{
		SetRasterizerState(Rasterizer_CW);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_VOLUME();
		GeometryShader = compile gs_5_0 GS_MAIN_VOLUME();
		PixelShader = compile ps_5_0 PS_MAIN_VOLUME();
	}
	// 23
	pass Default_LaserButtonFrame
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERBUTTONFRAME();
	}
	// 24 
	pass Default_LaserStation
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERSTATION();
	}
	// 25 
	pass Default_LaserStation_Green
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERBUTTONLARGE(true);
	}
	// 26 
	pass Default_LaserStation_Blue
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_LASERBUTTONLARGE(false);
	}
	// 27
	pass Default_WarpGate_Star_Activate
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0	VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0	PS_MAIN_WARPGATE_STAR();
	}
	// 28 
	pass Default_3DText
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_3DTEXT();
	}
	// 29
	pass Default_MeshParticle
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MESHPARTICLE();
	}
	// 30
	pass Character_PhantomFirst
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_PhantomFirst, 0xff);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = NULL;
	}
	// 31
	pass Character_PhantomSecond_Blue
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_PhantomSecond, 0xff);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PHANTOM(true);
	}
	// 32
	pass Character_PhantomSecond_Green
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_PhantomSecond, 0xff);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PHANTOM(false);
	}
	// 33
	pass Default_Monitor
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MONITOR();
	}
	// 34
	pass Default_ControlRoom_Monitor
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_CONTROLROOM_MONITOR();
	}
	// 35
	pass Default_PlayerHit
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PLAYERHIT();
	}
	// 36
	pass Default_MoonBaboonCore_Button_Active
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MOONBABOONCORE_BUTTON(true);
	}
	// 37
	pass Default_MoonBaboonCore_Button_NonActive
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_NO_BONE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MOONBABOONCORE_BUTTON(false);
	}
};