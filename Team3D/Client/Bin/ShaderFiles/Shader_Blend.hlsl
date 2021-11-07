////////////////////////////////////////////////////////////

texture2D	g_DiffuseTexture;
texture2D	g_ShadeTexture;
texture2D	g_SpecularTexture;
texture2D   g_DepthTexture;
texture2D   g_DOFTexture;
texture2D   g_ShadowMapTexture;

sampler ShadowSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};


sampler DOFSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

sampler DepthSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

sampler DiffuseSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

sampler ShadeSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

sampler SpecularSampler = sampler_state
{
	AddressU = wrap;
	AddressV = wrap;
};

////////////////////////////////////////////////////////////

struct VS_IN
{
	float4 vPosition	: POSITION;	
	float2 vTexUV		: TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition	: SV_POSITION;	
	float2 vTexUV		: TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0;
	
	Out.vPosition	= In.vPosition;
	Out.vTexUV		= In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition	: SV_POSITION;
	float2 vTexUV		: TEXCOORD;
};

struct PS_OUT
{
	vector vColor	: SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector	vDiffuseDesc	= g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vector	vShadeDesc		= g_ShadeTexture.Sample(ShadeSampler, In.vTexUV);
	vector	vSpecularDesc	= g_SpecularTexture.Sample(SpecularSampler, In.vTexUV);
	vector	vDOFDesc        = g_DOFTexture.Sample(DOFSampler, In.vTexUV / 2.f);
	vector	vDepthDesc      = g_DepthTexture.Sample(DepthSampler, In.vTexUV);
	vector	vShadowDesc	    = g_ShadowMapTexture.Sample(ShadowSampler, In.vTexUV);
	
	vector vShadowFactor;
	if (vDepthDesc.y < vShadowDesc.y + 0.05f)
	{
		vShadowFactor.rgb = 1.f;
		vShadowFactor.a = 0.f;
	}
	else
	{
		vShadowFactor.rgb = 0.f;
		vShadowFactor.a = 1.f;
	}

	Out.vColor = vDiffuseDesc * vShadeDesc + vSpecularDesc;

	if (Out.vColor.a == 0.f)
		discard;

	Out.vColor = lerp(Out.vColor, vDOFDesc, saturate(5.f * abs(vDepthDesc.x)));
	Out.vColor *= vShadowFactor;

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
	pass Directional
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_ZEnable, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader	= NULL;
		PixelShader		= compile ps_5_0 PS_MAIN();
	}	
};



