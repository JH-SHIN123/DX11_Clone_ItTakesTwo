

cbuffer TransformDesc
{
	matrix		g_matWorld, g_matView, g_matProj;
	matrix		g_matLightVP;
	matrix		g_matBias;
};


int g_iScreenX = 1280;
int g_iScreenY = 720;



texture2D		g_ShadowMapTexture;

sampler ShadowMapSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN_ShadowRender
{
	float3		vPosition	: POSITION;
};

struct VS_OUT_ShadowRender
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
	float4		vShadowUV	: TEXCOORD1;
};


VS_OUT_ShadowRender VS_SHADOW(VS_IN_ShadowRender In)
{
	VS_OUT_ShadowRender Out = (VS_OUT_ShadowRender)0;


	return Out;
}



struct PS_IN_ShadowRender
{
	float4		vPosition	: POSITION;
	float4		vDepth		: TEXCOORD0;
	float4		vShadowUV	: TEXCOORD1;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET0;
};

PS_OUT PS_SHADOW(PS_IN_ShadowRender In)
{
	PS_OUT Out = (PS_OUT)0;


	Out.vColor = fShadowTerm;

	return Out;
}

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
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_ZEnable, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_SHADOW();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SHADOW();
	}

};






