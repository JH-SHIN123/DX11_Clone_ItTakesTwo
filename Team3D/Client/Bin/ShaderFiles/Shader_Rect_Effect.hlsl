#include "Shader_Defines.hpp"
#include "Shader_Defines_Effect.hpp"

texture2D		g_DiffuseTexture;
texture2D		g_MaskTexture;
texture2D		g_ColorTexture;
texture2D		g_DistortionTexture;

sampler DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = wrap;
	AddressV = wrap;
};

sampler ColorSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = mirror/*_once*/;
	AddressV = mirror/*_once*/;
};
////////////////////////////////////////////////////////////

struct VS_IN
{
	float3 vPosition			: POSITION;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_Weight		: TEXCOORD1;
	float2 vTexUV_ColorRamp		: TEXCOORD2;
};

struct VS_OUT
{
	float4 vPosition			: SV_POSITION;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_Weight		: TEXCOORD1;
	float2 vTexUV_ColorRamp		: TEXCOORD2;
};

VS_OUT	VS_MAIN_DIST(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition.xyz = In.vPosition;
	Out.vPosition.w = 1.f;

	Out.vTexUV = In.vTexUV;
	Out.vTexUV_Weight = In.vTexUV_Weight;
	Out.vTexUV_ColorRamp = In.vTexUV_ColorRamp;

	return Out;
}

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vTexUV_Weight = In.vTexUV_Weight;
	Out.vTexUV_ColorRamp = In.vTexUV_ColorRamp;

	return Out;
}

VS_OUT	VS_ANGLE_UV(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;
	Out.vTexUV_ColorRamp = In.vTexUV_ColorRamp;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In.vTexUV - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out.vTexUV_Weight = RotateUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct  GS_OUT_DIST
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	float2 vWightUV			: TEXCOORD1;
	float2 vColorRamp_UV	: TEXCOORD2;
	float4 vProjPosition	: TEXCOORD3;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void  GS_MAIN_DIST(/*입력*/ triangle  VS_OUT In[3], /*출력*/ inout TriangleStream<GS_OUT_DIST> TriStream)
{
	GS_OUT_DIST Out = (GS_OUT_DIST)0;

	for (uint i = 0; i < 3; i++)
	{
		Out.vTexUV = In[i].vTexUV;
		Out.vWightUV = In[i].vTexUV_Weight;
		Out.vColorRamp_UV = In[i].vTexUV_ColorRamp;
		Out.vProjPosition = Out.vPosition;
		Out.iViewportIndex = 1;

		float3 vScale = { length(g_WorldMatrix._11_12_13),  length(g_WorldMatrix._21_22_23) ,  length(g_WorldMatrix._31_32_33) };
		float3 vLook = normalize(g_WorldMatrix._31_32_33).xyz;
		float3 vDir = normalize(g_vMainCamPosition - g_WorldMatrix._31_32_33_34).xyz;
		float3 vRight = cross(vLook, vDir).xyz;
		float3 vUp = cross(vRight, vLook).xyz;
		vRight *= vScale.x;
		vUp *= vScale.y;
		vLook *= vScale.z;

		matrix matWorld;
		matWorld._11_12_13_14 = vector(vRight, 0.f);
		matWorld._21_22_23_24 = vector(vUp, 0.f);
		matWorld._31_32_33_34 = vector(vLook, 0.f);
		matWorld._41_42_43_44 = g_WorldMatrix._41_42_43_44;

		vector vPos = mul(In[i].vPosition, matWorld);
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(vPos, matVP);

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		float3 vScale = { length(g_WorldMatrix._11_12_13_14),  length(g_WorldMatrix._21_22_23_24) ,  length(g_WorldMatrix._31_32_33_34) };
		float3 vLook = normalize(g_WorldMatrix._31_32_33_34).xyz;
		float3 vDir = normalize(g_vSubCamPosition - g_WorldMatrix._31_32_33_34).xyz;
		float3 vRight = cross(vLook, vDir).xyz;
		float3 vUp = cross(vRight, vLook).xyz;
		vRight *= vScale.x;
		vUp *= vScale.y;
		vLook *= vScale.z;

		matrix matWorld;
		matWorld._11_12_13_14 = vector(vRight, 0.f);
		matWorld._21_22_23_24 = vector(vUp, 0.f);
		matWorld._31_32_33_34 = vector(vLook, 0.f);
		matWorld._41_42_43_44 = g_WorldMatrix._41_42_43_44;

		vector vPos = mul(In[j].vPosition, matWorld);
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);


		Out.vPosition = mul(vPos, matVP);
		Out.vProjPosition = Out.vPosition;
		Out.vTexUV = In[j].vTexUV;
		Out.vWightUV = In[j].vTexUV_Weight;
		Out.vColorRamp_UV = In[j].vTexUV_ColorRamp;
		Out.iViewportIndex = 2;
		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

[maxvertexcount(6)]
void  GS_MAIN(/*입력*/ triangle  VS_OUT In[3], /*출력*/ inout TriangleStream<GS_OUT_DIST> TriStream)
{
	GS_OUT_DIST Out = (GS_OUT_DIST)0;

	for (uint i = 0; i < 3; i++)
	{
		Out.vTexUV = In[i].vTexUV;
		Out.vWightUV = In[i].vTexUV_Weight;
		Out.vColorRamp_UV = In[i].vTexUV_ColorRamp;
		Out.vProjPosition = Out.vPosition;
		Out.iViewportIndex = 1;

		//float3 vScale = { length(g_WorldMatrix._11_12_13),  length(g_WorldMatrix._21_22_23) ,  length(g_WorldMatrix._31_32_33) };
		//float3 vLook = normalize(g_WorldMatrix._31_32_33).xyz;
		//float3 vDir = normalize(g_vMainCamPosition - g_WorldMatrix._31_32_33_34).xyz;
		//float3 vRight = cross(vLook, vDir).xyz;
		//float3 vUp = cross(vRight, vLook).xyz;
		//vRight *= vScale.x;
		//vUp *= vScale.y;
		//vLook *= vScale.z;

		//matrix matWorld;
		//matWorld._11_12_13_14 = vector(vRight, 0.f);
		//matWorld._21_22_23_24 = vector(vUp, 0.f);
		//matWorld._31_32_33_34 = vector(vLook, 0.f);
		//matWorld._41_42_43_44 = g_WorldMatrix._41_42_43_44;

		//vector vPos = mul(In[i].vPosition, matWorld);
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		//float3 vScale = { length(g_WorldMatrix._11_12_13_14),  length(g_WorldMatrix._21_22_23_24) ,  length(g_WorldMatrix._31_32_33_34) };
		//float3 vLook = normalize(g_WorldMatrix._31_32_33_34).xyz;
		//float3 vDir = normalize(g_vSubCamPosition - g_WorldMatrix._31_32_33_34).xyz;
		//float3 vRight = cross(vLook, vDir).xyz;
		//float3 vUp = cross(vRight, vLook).xyz;
		//vRight *= vScale.x;
		//vUp *= vScale.y;
		//vLook *= vScale.z;

		//matrix matWorld;
		//matWorld._11_12_13_14 = vector(vRight, 0.f);
		//matWorld._21_22_23_24 = vector(vUp, 0.f);
		//matWorld._31_32_33_34 = vector(vLook, 0.f);
		//matWorld._41_42_43_44 = g_WorldMatrix._41_42_43_44;

		//vector vPos = mul(In[j].vPosition, matWorld);
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);


		Out.vPosition = mul(In[j].vPosition, matVP);
		Out.vProjPosition = Out.vPosition;
		Out.vTexUV = In[j].vTexUV;
		Out.vWightUV = In[j].vTexUV_Weight;
		Out.vColorRamp_UV = In[j].vTexUV_ColorRamp;
		Out.iViewportIndex = 2;
		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

////////////////////////////////////////////////////////////

struct  PS_IN_DIST
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	float2 vWeightUV		: TEXCOORD1;
	float2 vColorRamp_UV	: TEXCOORD2;

};

struct  PS_OUT
{
	vector	vColor	: SV_TARGET;
	vector	vEffect	: SV_TARGET1;
};

PS_OUT  PS_DISTORTION_COLOR(PS_IN_DIST In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vFX_tex = g_MaskTexture.Sample(DiffuseSampler, In.vWeightUV);
	float4 vColor = (float4)0.f;
	float fWeight = vFX_tex.r * 0.5f;

	float4 vColorRamp = g_ColorTexture.Sample(ColorSampler, In.vColorRamp_UV);
	vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV + fWeight);
	vColor *= g_vColor;

	if (0.1f >= vColor.r)
		discard;
	//vColor.a = abs(vColor.a - 1.f);
	vColor.a = g_fTime;
	Out.vColor = vColor;
	Out.vEffect = Out.vColor;

	return Out;
}

PS_OUT  PS_MASKING_DISTORTION(PS_IN_DIST In)
{
	PS_OUT		Out = (PS_OUT)0;



	float4 vDistortion = g_DistortionTexture.Sample(DiffuseSampler, In.vWeightUV);
	float fWeight = vDistortion.r * 2.f;
	float2 vDiffUV = In.vTexUV - 0.5f;
	vDiffUV.x += sign(vDiffUV.x) * fWeight;
	vDiffUV.y += sign(vDiffUV.y) * fWeight;
	vDiffUV += 0.5f;
	float4 vFX_tex = g_MaskTexture.Sample(DiffuseSampler, In.vTexUV);

	float4 vColor = g_DiffuseTexture.Sample(DiffuseSampler, vDiffUV);
	vColor.a = vFX_tex.r;
	Out.vColor = vColor;
	Out.vEffect = Out.vColor;

	if (0.f >= vFX_tex.r)
		discard;

	return Out;
}

////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass Distortion
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN_DIST();
		GeometryShader = compile gs_5_0 GS_MAIN_DIST();
		PixelShader = compile ps_5_0 PS_DISTORTION_COLOR();
	}

	pass MaskDistortion
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_ANGLE_UV();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MASKING_DISTORTION();
	}
};