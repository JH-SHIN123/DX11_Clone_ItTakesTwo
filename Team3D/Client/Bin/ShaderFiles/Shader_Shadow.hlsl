

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

	matrix		matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	matrix matLightWVP = mul(g_matWorld, g_matLightVP);

	Out.vPosition = mul(float4(In.vPosition.xyz, 1), matWVP);
	Out.vDepth = mul(float4(In.vPosition.xyz, 1), matLightWVP);
	Out.vShadowUV = mul(Out.vDepth, g_matBias);

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

	float4 vTexCoord[9];

	float fTexelSizeX = 1.f / 1280.f;
	float fTexelSizeY = 1.f / 720.f;

	vTexCoord[0] = In.vShadowUV;
	vTexCoord[1] = In.vShadowUV + float4(-fTexelSizeX, 0.f, 0.f, 0.f);
	vTexCoord[2] = In.vShadowUV + float4(fTexelSizeX, 0.f, 0.f, 0.f);
	vTexCoord[3] = In.vShadowUV + float4(0.f, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[4] = In.vShadowUV + float4(0.f, fTexelSizeY, 0.f, 0.f);
	vTexCoord[5] = In.vShadowUV + float4(-fTexelSizeX, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[6] = In.vShadowUV + float4(fTexelSizeX, -fTexelSizeY, 0.f, 0.f);
	vTexCoord[7] = In.vShadowUV + float4(-fTexelSizeX, fTexelSizeY, 0.f, 0.f);
	vTexCoord[8] = In.vShadowUV + float4(fTexelSizeX, fTexelSizeY, 0.f, 0.f);;

	float fShadowTerms[9];
	float fShadowTerm = 0.f;

	for (int i = 0; i < 9; i++)
	{
		float fDepthValue = g_ShadowMapTexture.Sample(ShadowMapSampler, vTexCoord[i]).x;
		float cosTheta = dot(0.99f, 1.f);
		//float bias = 0.00025f * tan(acos(cosTheta));	// 카메라 10,10 일때
		float bias = 0.0025f * tan(acos(cosTheta));	// 카메라 64,36 일때

		bias = clamp(bias, 0, 0.01);

		//float a = (fDepthValue * In.vDepth.w < In.vDepth.z - bias) ? 0.2f : 1.f;
		//fShadowTerms[i] = float(a);

		fShadowTerm += (fDepthValue * In.vDepth.w < In.vDepth.z - bias) ? 0.3f : 1.f;
	}

	fShadowTerm /= 9.f;

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






