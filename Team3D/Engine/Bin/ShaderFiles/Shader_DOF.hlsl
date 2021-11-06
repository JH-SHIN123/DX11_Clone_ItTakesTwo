
float g_Width = 1280;
float g_Height = 720;

float g_Weight[13] = {0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561};
float g_Total = 6.2108;

texture2D     g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float4 vPosition : POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;

	Out.vPosition = In.vPosition;
	Out.vTexUV = In.vTexUV ;

	return Out;
}

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 UVPos = In.vTexUV;
	float2 uv = 0;
	float2 uv2 = 0;

	float texeluvV = 1.f / (g_Width) * 2.f;
	float texeluvH = 1.f / (g_Height) * 2.f;

	for (int i = -6; i < 6; ++i)
	{
		uv = UVPos + float2(texeluvV * i, 0);
		uv2 = UVPos + float2(0, texeluvH * i);
		Out.vColor += g_Weight[6 + i] * g_DiffuseTexture.Sample(DiffuseSampler, uv);
		Out.vColor += g_Weight[6 + i] * g_DiffuseTexture.Sample(DiffuseSampler, uv2);
	}

	Out.vColor /= (g_Total * 2.f);
	Out.vColor.a = 1.f;

	return  Out;
}

BlendState BlendState_Alpha
{
	BlendEnable[0] = true;
	SrcBlend = SRC_ALPHA;
	DestBlend = INV_SRC_ALPHA;
	BlendOp = Add;
};

RasterizerState Rasterizer_Solid
{
	FillMode = solid;
	CullMode = back;
	FrontCounterClockwise = false;
};

DepthStencilState DepthStecil_ZEnable
{
	DepthEnable = false;
};

BlendState BlendState_None
{
	BlendEnable[0] = false;
};

technique11		DefaultTechnique
{
	pass DepthofField
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_ZEnable, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};