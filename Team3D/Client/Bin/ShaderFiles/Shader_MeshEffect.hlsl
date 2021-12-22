#include "Shader_Include.hpp"
#include "Shader_Defines_Effect.hpp"

////////////////////////////////////////////////////////////
texture2D	g_DiffuseTexture;
texture2D	g_NormalTexture;
texture2D	g_SpecularTexture;
texture2D	g_EmissiveTexture;
//texture2D	g_AmbientTexture;
//texture2D	g_OpacityTexture;
//texture2D	g_LightTexture;
texture2D	g_DiffuseTexture_Second;
texture2D	g_DistortionTexture;
texture2D	g_ColorRampTexture;

////////////////////////////////////////////////////////////

RasterizerState Rasterizer_CWW
{
	FillMode = Solid;
	CullMode = Front;
	FrontCounterClockwise = true; // 둘중에 하나만 켜야함.
};

struct VS_IN
{
	float3				vPosition		: POSITION;
	float3				vNormal			: NORMAL;
	float3				vTangent		: TANGENT;
	float2				vTexUV			: TEXCOORD0;

	uint4				vBlendIndex		: BLENDINDEX;
	float4				vBlendWeight	: BLENDWEIGHT;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
};

struct VS_OUT_DOUBLE_UV
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float3 vTangent		: TANGENT;
	float3 vBiNormal	: BINORMAL;
	float2 vTexUV		: TEXCOORD0;
	float2 vTexUV_2		: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_DOUBLE_UV VS_DOUBLE_UV(VS_IN In)
{
	VS_OUT_DOUBLE_UV Out = (VS_OUT_DOUBLE_UV)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTangent = normalize(mul(vector(In.vTangent.xyz, 0.f), g_WorldMatrix)).xyz;
	Out.vBiNormal = normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz));
	Out.vTexUV = In.vTexUV;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In.vTexUV - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out.vTexUV_2 = RotateUV;

	return Out;
}

struct VS_OUT_FRESNEL
{
	float4 vPosition	: SV_POSITION;
	float4 vNormal		: NORMAL;
	float2 vTexUV		: TEXCOORD0;
	float4 vMainCamRefl	: TEXCOORD1;
	float4 vSubCamRefl  : TEXCOORD2;
};
VS_OUT_FRESNEL VS_FRESNEL(VS_IN In)
{
	VS_OUT_FRESNEL Out = (VS_OUT_FRESNEL)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vNormal = normalize(mul(vector(In.vNormal, 0.f), g_WorldMatrix));
	Out.vTexUV = In.vTexUV;

	// MainCam
	float3 vPosW = Out.vPosition.xyz;
	float3 vNormalW = Out.vNormal.xyz;
	float3 l = normalize(vPosW - g_vMainCamPosition).xyz;
	float Scale = 2.5f;
	float Power = 2.f; // 점점 진해지는 강도세기

	Out.vMainCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

	// SubCam
	l = normalize(vPosW - g_vSubCamPosition).xyz;
	Out.vSubCamRefl = Scale * pow(1.0 + dot(l, vNormalW), Power);

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

struct GS_OUT_DOUBLE_UV
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_2				: TEXCOORD1;
	float4 vProjPosition		: TEXCOORD2;
	float4 vWorldPosition		: TEXCOORD3;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(6)]
void GS_DOUBLE_UV(triangle VS_OUT_DOUBLE_UV In[3], inout TriangleStream<GS_OUT_DOUBLE_UV> TriStream)
{
	GS_OUT_DOUBLE_UV Out = (GS_OUT_DOUBLE_UV)0;

	/* Main Viewport */
	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);
		Out.vNormal = In[i].vNormal;
		Out.vTangent = In[i].vTangent;
		Out.vBiNormal = In[i].vBiNormal;
		Out.vTexUV = In[i].vTexUV;
		Out.vTexUV_2 = In[i].vTexUV_2;

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
		Out.vTexUV_2 = In[j].vTexUV_2;

		Out.vProjPosition = Out.vPosition;
		Out.vWorldPosition = In[j].vPosition;
		Out.iViewportIndex = 2;

		TriStream.Append(Out);
	}
	TriStream.RestartStrip();
}

struct GS_OUT_FRESNEL
{
	float4 vPosition		: SV_POSITION;
	float4 vNormal			: NORMAL;
	float2 vTexUV			: TEXCOORD0;
	float4 vRefl			: TEXCOORD1;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_FRESNEL(triangle VS_OUT_FRESNEL In[3], inout TriangleStream<GS_OUT_FRESNEL> TriStream)
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

struct PS_IN_DOUBLE_UV
{
	float4 vPosition			: SV_POSITION;
	float4 vNormal				: NORMAL;
	float3 vTangent				: TANGENT;
	float3 vBiNormal			: BINORMAL;
	float2 vTexUV				: TEXCOORD0;
	float2 vTexUV_2				: TEXCOORD1;
	float4 vProjPosition		: TEXCOORD2;
	float4 vWorldPosition		: TEXCOORD3;
	uint   iViewportIndex		: SV_VIEWPORTARRAYINDEX;
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
	vector	vDiffuse			: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse.rgb = vMtrlDiffuse.b;
	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT	PS_COLOR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse.rgb = g_vColor.rgb;
	Out.vDiffuse.a = g_fAlpha;

	return Out;
}

PS_OUT	PS_DASH(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vMtrlDiffuse = g_DiffuseTexture_Second.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse = vMtrlDiffuse;

	Out.vDiffuse.a = Out.vDiffuse.r * g_fTime;
	
	return Out;
}

PS_OUT	PS_PIPELOCKER_BALL(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vDiffuse = g_vColor * 2.f;
	return Out;
}

PS_OUT	PS_MASK(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;
	Out.vDiffuse.rgb = 0.f;
	Out.vDiffuse.a = 1.f;

	return Out;
}

PS_OUT	PS_MAIN_RESPAWN_PORTAL(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vFX_tex = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV_2);
	float fWeight = (vFX_tex.b * 0.8f);

	float4 vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV + fWeight);
	float4 vColor = g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, g_vColorRamp_UV.xy);

	Out.vDiffuse.rgb = (vMtrlDiffuse.r - (vMtrlDiffuse.g * 0.5f)) * vColor.rgb * 2.f; // 10.f
	Out.vDiffuse.a = Out.vDiffuse.b * 0.75f * g_fAlpha;

	return Out;
}

PS_OUT	PS_MAIN_GRAVITYPIPE(PS_IN_DOUBLE_UV In)
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
	Out.vDiffuse.a = Out.vDiffuse.r * g_fAlpha;

	return Out;
}

PS_OUT	PS_MAIN_WORMHOLE(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 vFlipUV;
	vFlipUV.x = In.vTexUV.y;
	vFlipUV.y = In.vTexUV.x;
	In.vTexUV.x += g_fTime * 0.25;
	In.vTexUV.y += g_fTime;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);


	vector vColor = g_ColorRampTexture.Sample(Mirror_MinMagMipLinear_Sampler, vFlipUV);
	Out.vDiffuse.rgb = (vMtrlDiffuse.r * 2.f) * vColor.rgb;
	Out.vDiffuse.a = Out.vDiffuse.r;

	return Out;
}

PS_OUT	PS_MAIN_RESPAWNTENNEL(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 vDistortionUV = In.vTexUV;
	vDistortionUV.y -= g_fTime;
	float4 vFX_tex = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistortionUV);
	float fWeight = (vFX_tex.b);

	float2 vDiffTexUV = In.vTexUV;
	vDiffTexUV.y += fWeight;
	vector vMtrlDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDiffTexUV);


	vector vColor = g_ColorRampTexture.Sample(Clamp_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vDiffuse.rgb = fWeight * vColor.rgb * 1.f; //2.5
	Out.vDiffuse.a = 1;// Out.vDiffuse.r + Out.vDiffuse.g + Out.vDiffuse.b;

	return Out;
}

PS_OUT	PS_MAIN_UMBRELLAPIPE(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;
	float2 vDistortionUV = In.vTexUV;
	vDistortionUV.x += g_fTime * 0.33333333f;
	vDistortionUV.y += g_fTime;
	float4 vFX_tex = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistortionUV);
	float fWeight = (vFX_tex.b * 0.5f);

	vector vMtrlDiffuse = g_DiffuseTexture_Second.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV + fWeight);

	float2 vflipUV = { In.vTexUV.y, In.vTexUV.x };
	vflipUV.x += g_fTime * 0.33333333f;
	vflipUV.y += g_fTime;
	vector vColor = g_ColorRampTexture.Sample(Wrap_MinMagMipLinear_Sampler, vflipUV - fWeight);
	Out.vDiffuse.rgb = vMtrlDiffuse.r * vColor.rgb;
	Out.vDiffuse.a = Out.vDiffuse.r * g_fAlpha;

	Out.vDiffuse.rgb *= g_fAlpha;

	float fCheck_V = 0.3f;
	if (In.vTexUV.y < fCheck_V)
		Out.vDiffuse.rgb *= In.vTexUV.y / fCheck_V;

	return Out;
}

PS_OUT	PS_MAIN_BOSS_GRAVITOTIONAL_BOMB_EXPLOSION(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	if (0.499f >= In.vTexUV.y)
		discard;

	float fDistRatio_U = (In.vTexUV.x - 0.5f) / 0.5f;
	float fDistRatio_V = (In.vTexUV.y - 0.5f) / 0.5f;

	float2 vDistUV = In.vTexUV;
	vDistUV.x = fDistRatio_U;
	vDistUV.y = fDistRatio_V * 1.25f;
	vDistUV.y += g_fTime;
	vector vDistortion = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);
	float fDistPower = vDistortion.r * 0.1f;

	Out.vDiffuse = g_vColor;
	if (0.505f >= In.vTexUV.y)
		Out.vDiffuse.rg *= 3.f;
	else
		Out.vDiffuse.rgb *= vDistortion.rgb * (1.f - fDistRatio_V);

	if(fDistRatio_V > g_fAlpha)
		Out.vDiffuse.rgb *= g_fAlpha;

	if (0.f >= g_fAlpha)
		Out.vDiffuse.rgb = 0.f;

	return Out;
}

PS_OUT	PS_MAIN_BOSS_GRAVITOTIONAL_BOMB(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	float fDistRatio_U = (In.vTexUV.x - 0.5f) / 0.5f;
	float fDistRatio_V = (In.vTexUV.y - 0.5f) / 0.5f;

	float2 vDistUV = In.vTexUV;
	vDistUV.x = fDistRatio_U;
	vDistUV.y = fDistRatio_V * 1.25f;
	vDistUV.y += g_fTime;
	vector vDistortion = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);
	float fDistPower = vDistortion.r * 0.1f;

	Out.vDiffuse = g_vColor;
	Out.vDiffuse.rgb *= vDistortion.rgb * (1.f - fDistRatio_V);

	if (fDistRatio_V > g_fAlpha)
		Out.vDiffuse.rgb *= g_fAlpha;

	if (0.f >= g_fAlpha)
		Out.vDiffuse.rgb = 0.f;

	return Out;
}

PS_OUT	PS_MAIN_MOONBABOON_SHILED(PS_IN_DOUBLE_UV In)
{
	PS_OUT Out = (PS_OUT)0;

	In.vTexUV.x += g_fTime;
	vector vDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	float2 vDistUV = (float2)0.f;
	vDistUV.y = vDiffuse.b;

	vector vColor = g_ColorRampTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);

	Out.vDiffuse.rgb = vColor.rgb * vDiffuse.b * 2.5f;
	Out.vDiffuse.a = vDistUV.y * 0.25f * g_fAlpha;

	return Out;
}

PS_OUT PS_FRESNEL(PS_IN_FRESNEL In)
{
	PS_OUT Out = (PS_OUT)0;

	vector vOutColor = vector(0.5f, 0.5f, 0.8f, 0.5f);
	vector vInColor = vector(0.3f, 0.3f, 0.7f, 0.2f);

	Out.vDiffuse = lerp(vInColor, vOutColor, In.vRefl);
	return Out;
}

PS_OUT	PS_MAIN_BOSS_GRAOUNDPOUND(PS_IN_DOUBLE_UV In)
{
	//PS_OUT Out = (PS_OUT)0;

	//if (0.499f >= In.vTexUV.y)
	//	discard;

	//float fDistRatio_U = (In.vTexUV.x - 0.5f) / 0.5f;
	//float fDistRatio_V = (In.vTexUV.y - 0.5f) / 0.5f;

	//float2 vDistUV = In.vTexUV;
	//vDistUV.x = fDistRatio_U;
	//vDistUV.y = fDistRatio_V * 1.25f;
	//vDistUV.y += g_fTime;
	//vector vDistortion = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);
	//float fDistPower = vDistortion.r * 0.2f;

	//vDistUV.y += fDistPower;
	//vector vDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);


	//Out.vDiffuse = g_vColor;
	//if (0.505f >= In.vTexUV.y)
	//{
	//	Out.vDiffuse.rg *= 3.f;
	//}
	//else
	//	Out.vDiffuse.rgb *= vDiffuse.rgb * vDistortion.rgb  * ((1.f - fDistRatio_V) * 1.f);

	//if (fDistRatio_V > g_fAlpha)
	//	Out.vDiffuse.rgb = 0.f;

	//if (0.f >= g_fAlpha)
	//	Out.vDiffuse.rgb = 0.f;

	//return Out;

	PS_OUT Out = (PS_OUT)0;

	if (0.499f >= In.vTexUV.y)
		discard;

	float fDistRatio_U = (In.vTexUV.x - 0.5f) / 0.5f;
	float fDistRatio_V = (In.vTexUV.y - 0.5f) / 0.5f;

	float2 vDistUV = In.vTexUV;
	vDistUV.x = fDistRatio_U;
	vDistUV.y = fDistRatio_V * 1.25f;
	vDistUV.y += g_fTime;
	vector vDiffuse = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);
	float fDistPower = vDiffuse.r * g_fTimeWeight * 0.2f;

	vDistUV.y -= fDistPower;
	vDistUV.x += fDistPower;
	vector vDistortion = g_DistortionTexture.Sample(Wrap_MinMagMipLinear_Sampler, vDistUV);

	Out.vDiffuse = g_vColor;
	if (0.505f >= In.vTexUV.y)
	{
		Out.vDiffuse.rg *= 3.f;
	}
	else
		Out.vDiffuse.rgb *= vDistortion.rgb  * ((1.f - fDistRatio_V) * 0.1f);

	Out.vDiffuse.rgb *= g_fAlpha;

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
		VertexShader = compile		vs_5_0 VS_MAIN();
		GeometryShader = compile	gs_5_0 GS_MAIN();
		PixelShader = compile		ps_5_0 PS_MAIN();
	}

	pass Default_Alpha // 2
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass Default_Alpha_Opaque // 3
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass RespawnPortal // 4
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_RESPAWN_PORTAL();
	}

	pass GravityPipe // 5
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_GRAVITYPIPE();
	}

	pass Wormhole // 6
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_WORMHOLE();
	}

	pass RespawnTennel // 7
	{
		SetRasterizerState(Rasterizer_CWW);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_RESPAWNTENNEL();
	}

	pass RespawnTennel_Mask // 8
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MASK();
	}

	pass Umbrella_Pipe // 9
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_UMBRELLAPIPE();
	}

	pass Boss_Gravitational_Bomb_Explosion // 10
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_BOSS_GRAVITOTIONAL_BOMB_EXPLOSION();
	}

	pass Boss_GroundPound // 11
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_BOSS_GRAOUNDPOUND();
	}

	pass Boss_Gravitational_Bomb_Ball // 12
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_BOSS_GRAVITOTIONAL_BOMB();
	}

	pass Boss_MoonBaboon_Shield // 13
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_DOUBLE_UV();
		GeometryShader = compile gs_5_0 GS_DOUBLE_UV();
		PixelShader = compile ps_5_0 PS_MAIN_MOONBABOON_SHILED();
	}

	pass BCody_Size_ShockWave// 14
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_COLOR();
	}

	pass Dash// 15
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_DASH();
	}

	pass PipeLocker_Ball// 16
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PIPELOCKER_BALL();
	}
};