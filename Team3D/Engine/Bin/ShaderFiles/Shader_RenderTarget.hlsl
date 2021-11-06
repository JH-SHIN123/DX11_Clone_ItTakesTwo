////////////////////////////////////////////////////////////

texture2D g_DiffuseTexture;

sampler DiffuseSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

////////////////////////////////////////////////////////////

struct VS_IN
{
	float4 vPosition : POSITION;	
	float2 vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;	
	float2 vTexUV : TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	Out.vPosition = In.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	return Out;
}

////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Default
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_ZEnable, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		GeometryShader = NULL;
		VertexShader = compile vs_5_0 VS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}	
};



