////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

texture2D	g_NormalTexture;
texture2D	g_DepthTexture;
texture2D	g_SpecularSrcTexture;


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
	float	g_fPower		= 16.f;
	vector	g_vMtrlDiffuse	= (vector)1.f;
	vector	g_vMtrlAmbient	= (vector)1.f;
	vector	g_vMtrlSpecular = (vector)1.f;
}

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

	vector	vNormalDesc = g_NormalTexture.Sample(Wrap_Sampler, In.vTexUV);
	vector	vNormal		= vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
	vector	vDepthDesc	= g_DepthTexture.Sample(Wrap_Sampler, In.vTexUV);
	
	vector	vWorldPos	= vector(In.vProjPosition.x, In.vProjPosition.y, vDepthDesc.y, 1.f);
	float	fViewZ		= 0.f;
	vector	vLook		= (vector)0.f;

	/* ViewportUVInfo : x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	if (In.vTexUV.x >= g_vMainViewportUVInfo.x && In.vTexUV.x <= g_vMainViewportUVInfo.z &&
		In.vTexUV.y >= g_vMainViewportUVInfo.y && In.vTexUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fMainCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_MainProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_MainViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vMainCamPosition);
	}
	else if (In.vTexUV.x >= g_vSubViewportUVInfo.x && In.vTexUV.x <= g_vSubViewportUVInfo.z &&
		In.vTexUV.y >= g_vSubViewportUVInfo.y && In.vTexUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ		= vDepthDesc.x * g_fSubCamFar;
		vWorldPos	= vWorldPos * fViewZ;
		vWorldPos	= mul(vWorldPos, g_SubProjMatrixInverse);
		vWorldPos	= mul(vWorldPos, g_SubViewMatrixInverse);
		vLook		= normalize(vWorldPos - g_vSubCamPosition);
	}
	else
		discard;


	Out.vShade		= max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient);
	Out.vShade.a = 0.f;

	/* Specular */
	vector	vSpecSrcDesc = g_SpecularSrcTexture.Sample(Wrap_Sampler, In.vTexUV);

	if (vSpecSrcDesc.w < 0.5f) // w is specular on/off flag
	{
		vector	vSpecSrc = vector(vSpecSrcDesc.xyz * 2.f - 1.f, 0.f);
		vector vReflect = reflect(normalize(g_vLightDir), vSpecSrc);
		Out.vSpecular = pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular);
		Out.vSpecular.a = 0.f;
	}

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

	Out.vShade		= (max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * (g_vLightDiffuse * g_vMtrlDiffuse) + (g_vLightAmbient * g_vMtrlAmbient)) * fAtt;
	Out.vShade.a = 0.f;

	//vector	vSpecSrcDesc = g_SpecularSrcTexture.Sample(Wrap_Sampler, In.vTexUV);
	//vector	vSpecSrc = vector(vSpecSrcDesc.xyz * 2.f - 1.f, 0.f);
	//vector	vReflect = reflect(normalize(g_vLightDir), vSpecSrcDesc);
	//Out.vSpecular	= (pow(max(dot(vLook * -1.f, vReflect), 0.f), g_fPower) * (g_vLightSpecular * g_vMtrlSpecular)) * fAtt;
	//Out.vSpecular.a = 0.f;

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