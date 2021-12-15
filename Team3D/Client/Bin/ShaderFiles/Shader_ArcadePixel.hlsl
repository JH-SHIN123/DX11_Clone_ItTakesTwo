#include "Shader_Defines.hpp"

////////////////////////////////////////////////////////////

float		g_fGauge;
float		g_fDistFromUFO;

texture2D	g_DiffuseTexture;
texture2D	g_AlphaTexture;

////////////////////////////////////////////////////////////

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT	VS_CHARGEBARGAUGE(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;


	return Out;
}

////////////////////////////////////////////////////////////

struct GS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct GS_OUT
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	for (uint i = 0; i < 3; i++)
	{
		matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

		Out.vPosition = mul(In[i].vPosition, matVP);
		Out.vTexUV = In[i].vTexUV;
		Out.iViewportIndex = 1;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();

	for (uint j = 0; j < 3; j++)
	{
		matrix matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

		Out.vPosition = mul(In[j].vPosition, matVP);
		Out.vTexUV = In[j].vTexUV;
		Out.iViewportIndex = 2;

		TriStream.Append(Out);
	}

	TriStream.RestartStrip();
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct PS_OUT
{
	float4	vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (Out.vColor.a < 0.02f)
		discard;

	Out.vColor.rgba = vector(0.f, 0.7f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_CHARGEBAR(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (Out.vColor.a < 0.2f)
		discard;

	Out.vColor.rgba = vector(0.f, 0.7f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_CHARGEBARGAUGE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (Out.vColor.a < 0.2f)
		discard;

	if (In.vTexUV.x > g_fGauge)
		discard;

	Out.vColor.rgba = vector(0.f, 0.7f, 0.f, 1.f);

	return Out;
}

PS_OUT	PS_PIXELMOONBABOON(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);

	if (Out.vColor.a < 0.2f)
		discard;

	Out.vColor.rgba = vector(0.f, 0.7f, 0.f, 1.f);
	return Out;
}
////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}
	pass ChargeBar // 1
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_CHARGEBAR();
	}
	pass ChargeBarGauge // 2
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_CHARGEBARGAUGE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_CHARGEBARGAUGE();
	}

	pass PixelMoonBaboon // 3
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_CHARGEBARGAUGE();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PIXELMOONBABOON();
	}
};