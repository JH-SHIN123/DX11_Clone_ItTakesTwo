#include "Shader_Include.hpp"
////////////////////////////////////////////////////////////
#define MAX_VERTICES NUM_VERTICES * MAX_CASCADES * NUM_VIEWPORTS
#define NUM_VERTICES 3
#define NUM_VIEWPORTS 2

texture2D	g_DiffuseTexture;
texture2D	g_DarkTexture;
float		g_fTimeUV;

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
	float4 vShade		: COLOR0;
	float4 vSpecular	: COLOR1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition	= mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal		= normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent	= normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal	= normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));

	In.vTexUV.x		-= g_fTimeUV;
	Out.vTexUV		= In.vTexUV;



	//vector	vWorldPosition = mul(vector(In.vPosition, 1.f), WorldMatrix);
	//vector	vWorldNormal = normalize(mul(vector(In.vNormal, 0.f), WorldMatrix));
	//vector	vLightDirection = vector(0.f, 1.f, 0.f, 0.f);

	//Out.vShade = max(dot(normalize(vLightDirection * -1.f, vWorldNormal), 0.f);
	//Out.vShade.a = 1.f;

	//vector	vReflect	= reflect(vLightDirection, normalize(vWorldNormal));
	//vector	vLook		= vWorldPosition - vCameraPosition;

	//Out.vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

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
	float4 vShade		: COLOR0;
	float4 vSpecular	: COLOR1;
};

struct GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float3 vTangent			: TANGENT;
	float3 vBiNormal		: BINORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vShade			: COLOR0;
	float4 vSpecular		: COLOR1;
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
			Out.iViewportIndex = 2;

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
	float4 vShade			: COLOR0;
	float4 vSpecular		: COLOR1;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct PS_OUT
{
	vector	vColor	: SV_TARGET;
};

PS_OUT	PS_GROUND(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vDayDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	vector vNightDiffuse = g_DarkTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	float fBoundary = In.vTexUV.x + g_fTimeUV;

	if (fBoundary > 0.5125f)
		Out.vColor = vNightDiffuse;
	else if (fBoundary >= 0.4875f)
	{
		fBoundary = (fBoundary - 0.4875f) * 40.f;
		Out.vColor = lerp(vDayDiffuse, vNightDiffuse, fBoundary);
	}
	else
		Out.vColor = vDayDiffuse;

	//Out.vColor = (vLightDiffuse * vMtrlDiffuse) * (In.vShade + (vLightAmbient * vMtrlAmbient)) + (vLightSpecular * vMtrlSpecular) * In.vSpecular;

	return Out;
}

PS_OUT	PS_ATMOSPHERE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r;

	float fBoundary = In.vTexUV.x + g_fTimeUV;

	if (fBoundary > 0.5125f)
		discard;
	else if (fBoundary >= 0.4875f)
	{
		fBoundary = (fBoundary - 0.4875f) * 40.f;
		Out.vColor.a *= 1.f - fBoundary;
	}

	return Out;
}

/* ________________________________________________________________________________*/

technique11 DefaultTechnique
{
	pass Ground
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_GROUND();
	}

	pass Atmosphere
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_ATMOSPHERE();
	}
};