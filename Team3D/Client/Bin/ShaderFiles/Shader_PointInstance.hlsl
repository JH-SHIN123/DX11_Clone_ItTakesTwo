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

[maxvertexcount(12)]
void  GS_GATESMOKE(point  VS_OUT In[1], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out[8];
	for (int i = 0; i < 8; i++) { Out[i] = (GS_OUT)0; }

	matrix ChildWorld	= In[0].ChildWorld;
	for (int j = 0; j < 8; j++) { Out[j].vMaskUV.x = ChildWorld[3].w; }
	ChildWorld[3].w		= 1.f;

	matrix WorldMatrix	= mul(ChildWorld, g_WorldMatrix);

	float3 vLook	= normalize(g_vMainCamPosition - WorldMatrix[3]).xyz;
	float3 vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3 vRight	= normalize(cross(vAxisY, vLook));
	float3 vUp		= normalize(cross(vLook, vRight));

	Out[0].vPosition = vector(vRight, 0.f) * 0.5f + vector(vUp, 0.f) * 0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[1].vPosition = vector(vRight, 0.f) * -0.5f + vector(vUp, 0.f) * 0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[2].vPosition = vector(vRight, 0.f) * -0.5f + vector(vUp, 0.f) * -0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[3].vPosition = vector(vRight, 0.f) * 0.5f + vector(vUp, 0.f) * -0.5f + vector(0.f, 0.f, 0.f, 1.f);

	vLook	= normalize(g_vSubCamPosition - WorldMatrix[3]).xyz;
	vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight	= normalize(cross(vAxisY, vLook));
	vUp		= normalize(cross(vLook, vRight));

	Out[4].vPosition = vector(vRight, 0.f) * 0.5f + vector(vUp, 0.f) * 0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[5].vPosition = vector(vRight, 0.f) * -0.5f + vector(vUp, 0.f) * 0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[6].vPosition = vector(vRight, 0.f) * -0.5f + vector(vUp, 0.f) * -0.5f + vector(0.f, 0.f, 0.f, 1.f);
	Out[7].vPosition = vector(vRight, 0.f) * 0.5f + vector(vUp, 0.f) * -0.5f + vector(0.f, 0.f, 0.f, 1.f);

	matrix MainWVP		= mul(mul(WorldMatrix, g_MainViewMatrix), g_MainProjMatrix);
	matrix SubWVP		= mul(mul(WorldMatrix, g_SubViewMatrix), g_SubProjMatrix);

	Out[0].vPosition = mul(Out[0].vPosition, MainWVP);
	Out[1].vPosition = mul(Out[1].vPosition, MainWVP);
	Out[2].vPosition = mul(Out[2].vPosition, MainWVP);
	Out[3].vPosition = mul(Out[3].vPosition, MainWVP);
	Out[4].vPosition = mul(Out[4].vPosition, SubWVP);
	Out[5].vPosition = mul(Out[5].vPosition, SubWVP);
	Out[6].vPosition = mul(Out[6].vPosition, SubWVP);
	Out[7].vPosition = mul(Out[7].vPosition, SubWVP);

	Out[0].vTexUV = float2(0.f, 0.f);
	Out[1].vTexUV = float2(0.125f, 0.f);
	Out[2].vTexUV = float2(0.125f, 0.125f);
	Out[3].vTexUV = float2(0.f, 0.125f);
	Out[4].vTexUV = Out[0].vTexUV;
	Out[5].vTexUV = Out[1].vTexUV;
	Out[6].vTexUV = Out[2].vTexUV;
	Out[7].vTexUV = Out[3].vTexUV;

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
	Out.vColor.rgb *= 3.f;
	Out.vColor.a *= g_MaskTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vMaskUV).r;

	return Out;
}

PS_OUT  PS_GATESMOKE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_MinMagMipLinear_Sampler, In.vTexUV);
	Out.vColor.rgb = 1.f;
	Out.vColor.a *= Out.vColor.a * 0.1f * In.vMaskUV.x;

	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	pass PipeLocker_Connected
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0  VS_MAIN();
		GeometryShader	= compile gs_5_0  GS_MAIN();
		PixelShader		= compile ps_5_0  PS_MAIN();
	}

	pass GateSmoke
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0  VS_MAIN();
		GeometryShader	= compile gs_5_0  GS_GATESMOKE();
		PixelShader		= compile ps_5_0  PS_GATESMOKE();
	}
};