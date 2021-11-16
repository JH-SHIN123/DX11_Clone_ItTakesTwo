
#define MAX_CASCADES	3
#define MAIN_VIEWPORT_INDEX 1
#define SUB_VIEWPORT_INDEX 2

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

	matrix	g_MainProjMatrixInverse;
	matrix	g_MainViewMatrixInverse;
	matrix	g_SubProjMatrixInverse;
	matrix	g_SubViewMatrixInverse;
}

cbuffer Camera
{
	float	g_fMainCamFar;
	float	g_fSubCamFar;
	float4	g_vMainCamPosition;
	float4	g_vSubCamPosition;
}

cbuffer Viewport
{
	float4	g_vMainViewportUVInfo;
	float4	g_vSubViewportUVInfo;
};

/* For. Shadow */
texture2D g_CascadedShadowDepthTexture;
cbuffer ShadowDesc
{
	float	g_CascadeEnds[MAX_CASCADES + 1];
	matrix	g_ShadowTransforms_Main[MAX_CASCADES];
	matrix	g_ShadowTransforms_Sub[MAX_CASCADES];
};

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