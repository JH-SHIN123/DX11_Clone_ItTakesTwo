#include "Shader_Defines.hpp"

texture2D	g_DiffuseTexture;
texture2D	g_MaskTexture;

struct VS_IN
{
	row_major matrix ChildWorld	: WORLD;
};

struct VS_OUT
{
	row_major matrix ChildWorld	: WORLD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.ChildWorld = In.ChildWorld;

	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct  GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	float2 vMaskUV			: TEXCOORD1;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(12)]
void  GS_MAIN(point  VS_OUT In[1], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out[8];

	matrix WorldMatrix	= mul(In[0].ChildWorld, g_WorldMatrix);
	matrix MainWVP		= mul(mul(WorldMatrix, g_MainViewMatrix), g_MainProjMatrix);
	matrix SubWVP		= mul(mul(WorldMatrix, g_SubViewMatrix), g_SubProjMatrix);

	Out[0].vPosition = vector(-0.5f, 0.f, 0.5f, 1.f);
	Out[1].vPosition = vector(0.5f, 0.f, 0.5f, 1.f);
	Out[2].vPosition = vector(0.5f, 0.f, -0.5f, 1.f);
	Out[3].vPosition = vector(-0.5f, 0.f, -0.5f, 1.f);
	Out[4].vPosition = Out[0].vPosition;
	Out[5].vPosition = Out[1].vPosition;
	Out[6].vPosition = Out[2].vPosition;
	Out[7].vPosition = Out[3].vPosition;

	Out[0].vPosition = mul(Out[0].vPosition, MainWVP);
	Out[1].vPosition = mul(Out[1].vPosition, MainWVP);
	Out[2].vPosition = mul(Out[2].vPosition, MainWVP);
	Out[3].vPosition = mul(Out[3].vPosition, MainWVP);
	Out[4].vPosition = mul(Out[4].vPosition, SubWVP);
	Out[5].vPosition = mul(Out[5].vPosition, SubWVP);
	Out[6].vPosition = mul(Out[6].vPosition, SubWVP);
	Out[7].vPosition = mul(Out[7].vPosition, SubWVP);

	Out[0].vTexUV = float2(1.f, 0.f);
	Out[1].vTexUV = float2(1.f, 1.f);
	Out[2].vTexUV = float2(0.f, 1.f);
	Out[3].vTexUV = float2(0.f, 0.f);
	Out[4].vTexUV = Out[0].vTexUV;
	Out[5].vTexUV = Out[1].vTexUV;
	Out[6].vTexUV = Out[2].vTexUV;
	Out[7].vTexUV = Out[3].vTexUV;

	Out[0].vMaskUV = float2(0.f, 0.f);
	Out[1].vMaskUV = float2(1.f, 0.f);
	Out[2].vMaskUV = float2(1.f, 1.f);
	Out[3].vMaskUV = float2(0.f, 1.f);
	Out[4].vMaskUV = Out[0].vMaskUV;
	Out[5].vMaskUV = Out[1].vMaskUV;
	Out[6].vMaskUV = Out[2].vMaskUV;
	Out[7].vMaskUV = Out[3].vMaskUV;

	Out[0].iViewportIndex = 1;
	Out[1].iViewportIndex = 1;
	Out[2].iViewportIndex = 1;
	Out[3].iViewportIndex = 1;
	Out[4].iViewportIndex = 2;
	Out[5].iViewportIndex = 2;
	Out[6].iViewportIndex = 2;
	Out[7].iViewportIndex = 2;

	TriStream.Append(Out[0]);
	TriStream.Append(Out[1]);
	TriStream.Append(Out[2]);
	TriStream.RestartStrip();

	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);
	TriStream.RestartStrip();

	TriStream.Append(Out[4]);
	TriStream.Append(Out[5]);
	TriStream.Append(Out[6]);
	TriStream.RestartStrip();

	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
	TriStream.RestartStrip();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct  PS_IN
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	float2	vMaskUV			: TEXCOORD1;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

struct  PS_OUT
{
	vector vColor : SV_TARGET;
};

PS_OUT  PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vColor.a *= g_MaskTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vMaskUV).r;

	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass Default // 0
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0  VS_MAIN();
		GeometryShader	= compile gs_5_0  GS_MAIN();
		PixelShader		= compile ps_5_0  PS_MAIN();
	}
};