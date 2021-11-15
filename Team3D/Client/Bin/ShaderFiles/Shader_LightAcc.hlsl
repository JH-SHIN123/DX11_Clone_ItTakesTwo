////////////////////////////////////////////////////////////
#include "Shader_Defines.hpp"
#include "Shader_Macro.hpp"

texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
texture2D	g_CascadedShadowDepthTexture;
texture2D	g_CascadedShadowDepthTexture_Sub;

cbuffer CascadedShadowDesc
{
	float	g_CascadeEnds[MAX_CASCADES + 1];

	matrix	g_ShadowTransforms_Main[MAX_CASCADES];
	matrix	g_ShadowTransforms_Sub[MAX_CASCADES];
};

cbuffer Directional
{
	vector	g_vLightDir;
}

cbuffer Point
{
	vector	g_vLightPos;
	float	g_fRadius;
}

cbuffer LightColor
{
	vector	g_vLightDiffuse;
	vector	g_vLightAmbient;
	vector	g_vLightSpecular;
}

cbuffer MtrlDesc
{
	float	g_fPower		= 20.f;
	vector	g_vMtrlDiffuse	= (vector)1.f;
	vector	g_vMtrlAmbient	= (vector)1.f;
	vector	g_vMtrlSpecular = (vector)1.f;
}

cbuffer TransformDesc
{
	//float	g_fMainCamFar;
	//vector	g_vMainCamPosition;
	matrix	g_MainProjMatrixInverse;
	matrix	g_MainViewMatrixInverse;

	//float	g_fSubCamFar;
	//vector	g_vSubCamPosition;
	matrix	g_SubProjMatrixInverse;
	matrix	g_SubViewMatrixInverse;
}

cbuffer Viewport
{
	float4	g_vMainViewportUVInfo;
	float4	g_vSubViewportUVInfo;
};

////////////////////////////////////////////////////////////
/* Function */
int Get_CascadedShadowSliceIndex(vector vWorldPos, int iViewportIndex)
{
	vector shadowPosNDC;
	int iIndex = -1;
	for (uint i = 0; i < MAX_CASCADES; ++i)
	{
		// TEST Main만 띄워보자.
		if (0 == iViewportIndex) shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Main[i]);
		else shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Sub[i]);
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

float Get_ShadowFactor(vector vWorldPos, matrix shadowTransformMatrix, int iSliceIndex, int iViewportIndex)
{
	vector shadowPosH = mul(vWorldPos, shadowTransformMatrix);
	shadowPosH.xyz /= shadowPosH.w;

	// Match up Deferred With Cascaded Shadow Depth
	float2 vShadowUV = shadowPosH.xy;
	vShadowUV.y = (vShadowUV.y + float(iSliceIndex)) / float(MAX_CASCADES);

	float shadowFactor = 0.f;
	float percentLit = 0.0f;
	float depth = shadowPosH.z; // 그릴 객체들의 깊이값. (그림자 ndc로 이동한)

	if(0 == iViewportIndex)
		percentLit = g_CascadedShadowDepthTexture.Sample(Wrap_Sampler, vShadowUV).r;
	else if (1 == iViewportIndex)
		percentLit = g_CascadedShadowDepthTexture_Sub.Sample(Wrap_Sampler, vShadowUV).r;

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
	vector	vShade		: SV_TARGET0;
	vector	vSpecular	: SV_TARGET1;
	vector	vShadow		: SV_TARGET2;
};

PS_OUT PS_DIRECTIONAL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vNormal		= vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector	vDepthDesc	= g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	vector	vLook		= (vector)0.f;
	float fShadowfactor = 0.f;

	/* ViewportUVInfo : x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
		In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fMainCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_MainProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_MainViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vMainCamPosition);

		/* Carculate Shadow */
		int iIndex = Get_CascadedShadowSliceIndex(vWorldPos, 0);
		if (iIndex < 0) return Out;
		fShadowfactor = Get_ShadowFactor(vWorldPos, g_ShadowTransforms_Main[iIndex], iIndex, 0);

		//if (0 == iIndex)
		//	Out.vShadow.x = 1.f;
		//else if(1 == iIndex)
		//	Out.vShadow.y = 1.f;
		//else if (2 == iIndex)
		//	Out.vShadow.z = 1.f;

		//Out.vShadow = 1.f - fShadowfactor;
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z &&
		In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fSubCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_SubProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_SubViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vSubCamPosition);

		/* Carculate Shadow */
		int iIndex = Get_CascadedShadowSliceIndex(vWorldPos, 1);
		if (iIndex < 0) return Out;
		fShadowfactor = Get_ShadowFactor(vWorldPos, g_ShadowTransforms_Sub[iIndex], iIndex, 1);
	}
	else
		discard;

	vector vReflect = reflect(normalize(g_vLightDir), vNormal);

	Out.vShade		= max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 0.f;
	Out.vSpecular	= pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
	Out.vSpecular.a = 0.f;

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

PS_OUT PS_POINT(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vNormalDesc = g_NormalTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vNormal		= vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector	vDepthDesc	= g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
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
	float	fDistance	= length(vLightDir);
	float	fAtt		= saturate((g_fRadius - fDistance) / g_fRadius);
	vector	vReflect	= reflect(normalize(g_vLightDir), vNormal);

	Out.vShade		= (max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 0.f;
	Out.vSpecular	= (pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular)) * fAtt;
	Out.vSpecular.a = 0.f;

	return Out;
}

////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Directional
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_DIRECTIONAL();
	}

	pass Point
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_POINT();
	}
};