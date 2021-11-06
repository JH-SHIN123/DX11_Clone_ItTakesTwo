

cbuffer TransformDesc
{
	matrix		WorldMatrix, LightViewMatrix, LightProjMatrix;
	vector		LightPos;
};

int g_iScreenX = 1280;
int g_iScreenY = 720;



texture2D		g_DepthTexture;

sampler DepthSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

texture2D		g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

struct VS_IN
{
	float4 vPosition : POSITION;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float4 mClipPosition : TEXCOORD1;
};


VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = mul(In.vPosition, WorldMatrix);
	Out.vPosition = mul(Out.vPosition, LightViewMatrix);
	Out.vPosition = mul(Out.vPosition, LightProjMatrix);
	Out.mClipPosition = Out.vPosition;

	return Out;
}



struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float4 mClipPosition : TEXCOORD1;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float depth = In.mClipPosition.z / In.mClipPosition.w;
	Out.vColor = vector(depth.xxx, 1.f);

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

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

};






