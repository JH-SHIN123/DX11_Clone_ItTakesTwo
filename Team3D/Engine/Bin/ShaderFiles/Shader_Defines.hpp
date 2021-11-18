
#define aiTextureType_NONE 0
#define aiTextureType_DIFFUSE 1
#define aiTextureType_SPECULAR 2
#define aiTextureType_AMBIENT 3
#define aiTextureType_EMISSIVE 4
#define aiTextureType_HEIGHT 5
#define aiTextureType_NORMALS 6
#define aiTextureType_SHININESS 7
#define aiTextureType_OPACITY 8
#define aiTextureType_DISPLACEMENT 9
#define aiTextureType_LIGHTMAP 10
#define aiTextureType_REFLECTION 11
#define aiTextureType_BASE_COLOR 12
#define aiTextureType_NORMAL_CAMERA 13
#define aiTextureType_EMISSION_COLOR 14
#define aiTextureType_METALNESS 15
#define aiTextureType_DIFFUSE_ROUGHNESS 16
#define aiTextureType_AMBIENT_OCCLUSION 17
#define aiTextureType_UNKNOWN  18
#define AI_TEXTURE_TYPE_MAX  aiTextureType_UNKNOWN

#define		MAX_CASCADES		4
#define		MAIN_VIEWPORT_INDEX 1
#define		SUB_VIEWPORT_INDEX	2

/* struct */
struct ISMATERIALS
{
	uint Is_None;
	uint Is_Diffuse;
	uint Is_Specular;
	uint Is_Ambient;
	uint Is_Emissive;
	uint Is_Height;
	uint Is_Normals;
	uint Is_Shininess;
	uint Is_Opacity;
	uint Is_Display;
	uint Is_Lightmap;
	uint Is_Reflection;
	uint Is_Base_color;
	uint Is_Normal_camera;
	uint Is_Emission_color;
	uint Is_Metalness;
	uint Is_Diffuse_roughness;
	uint Is_Ambient_occlusion;
	uint Is_Unknown;
};

struct BONEMATRICES
{
	matrix	Matrices[256];
};

struct Material
{
	float4 vDiffuse;
	float4 vAmbient;
	float4 vSpecular; // w = SpecPower
	float4 vReflect;
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

cbuffer Materials
{
	Material	g_Material = (Material)(1.f);
	ISMATERIALS g_IsMaterials;
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
	/* x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
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

SamplerComparisonState ShadowSampler = sampler_state
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS;
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