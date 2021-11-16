/* struct */
struct BONEMATRICES
{
	matrix	Matrices[256];
};

/* cbuffer */
cbuffer Matrices
{
	matrix	g_WorldMatrix;
	
	matrix	g_MainViewMatrix;
	matrix	g_MainProjMatrix;
	matrix	g_SubViewMatrix;
	matrix	g_SubProjMatrix;
}
cbuffer Camera
{
	float	g_fMainCamFar;
	float	g_fSubCamFar;
	float4	g_vMainCamPosition;
	float4	g_vSubCamPosition;
}

/* Sampler */
sampler	Wrap_Sampler = sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
};

sampler	Wrap_MinMagMipLinear_Sampler = sampler_state
{
	AddressU = Wrap;
	AddressV = Wrap;
	Filter = MIN_MAG_MIP_LINEAR;
};

/* RasterizerState */
RasterizerState Rasterizer_Solid
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
};
RasterizerState Rasterizer_Wireframe
{
	FillMode = Wireframe;
	CullMode = None;
};
RasterizerState Rasterizer_NoCull
{
	FillMode = Solid;
	CullMode = None;
};
RasterizerState Rasterizer_Shadow
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = false;
	//DepthBias = 0.005;
	//DepthBiasClamp = 0.0f;
	//SlopeScaledDepthBias = 5;
};

/* DepthStencilState */
DepthStencilState DepthStecil_Default
{
	DepthEnable		= true;
	DepthWriteMask	= All;
	DepthFunc		= Less;
};
DepthStencilState DepthStecil_No_ZWrite
{
	DepthEnable		= true;
	DepthWriteMask	= Zero;
	DepthFunc		= Less;
};
DepthStencilState DepthStecil_No_ZTest
{
	DepthEnable		= false;
};

/* BlendState */
BlendState BlendState_Alpha
{
	BlendEnable[0]	= true;
	SrcBlend		= SRC_ALPHA;
	DestBlend		= INV_SRC_ALPHA;
	BlendOp			= Add;
};
BlendState BlendState_Add
{
	BlendEnable[0]	= true;
	BlendEnable[1]	= true;
	SrcBlend		= ONE;
	DestBlend		= ONE;
	BlendOp			= Add;
	SrcBlendAlpha	= ONE;
	DestBlendAlpha	= ONE;
	BlendOpAlpha	= Add;
};
BlendState BlendState_None
{
	BlendEnable[0] = false;
};