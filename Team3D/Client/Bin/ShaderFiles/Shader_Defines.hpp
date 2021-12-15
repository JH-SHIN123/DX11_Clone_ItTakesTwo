
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
	uint	g_iViewportDrawInfo;
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
sampler	Point_Sampler = sampler_state
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	AddressW = CLAMP;
	Filter = MIN_MAG_MIP_POINT;
};

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

sampler	Clamp_MinMagMipLinear_Sampler = sampler_state
{
	AddressU = CLAMP;
	AddressV = CLAMP;
	Filter = MIN_MAG_MIP_LINEAR;
};

sampler	Mirror_MinMagMipLinear_Sampler = sampler_state
{
	AddressU = mirror;
	AddressV = mirror;
	Filter = MIN_MAG_MIP_LINEAR;
};

SamplerComparisonState ShadowSampler = sampler_state
{
	Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	mipLODBias = 0.f;
	maxAnisotropy = 16.f;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	ComparisonFunc = LESS;
	//ComparisonFunc = LESS_EQUAL;
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
RasterizerState Rasterizer_CW
{
	FillMode = Solid;
	CullMode = Front;
	//FrontCounterClockwise = true; // 둘중에 하나만 켜야함.
};
RasterizerState Rasterizer_Shadow
{
	FillMode = Solid;
	CullMode = Back;
	FrontCounterClockwise = true;

	DepthBias = 50000;
	DepthBiasClamp = 0.0f;
	SlopeScaledDepthBias = 1.0f;
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

float cos_value[] =
{
	1				,
	0.99995			,
	0.9998			,
	0.99955			,
	0.9992			,
	0.99875			,
	0.998201		,
	0.997551		,
	0.996802		,
	0.995953		,
	0.995004		,
	0.993956		,
	0.992809		,
	0.991562		,
	0.990216		,
	0.988771		,
	0.987227		,
	0.985585		,
	0.983844		,
	0.982004		,
	0.980067		,
	0.978031		,
	0.975897		,
	0.973666		,
	0.971338		,
	0.968912		,
	0.96639			,
	0.963771		,
	0.961055		,
	0.958244		,
	0.955337		,
	0.952334		,
	0.949235		,
	0.946042		,
	0.942755		,
	0.939373		,
	0.935897		,
	0.932327		,
	0.928665		,
	0.924909		,
	0.921061		,
	0.917121		,
	0.913089		,
	0.908966		,
	0.904752		,
	0.900447		,
	0.896052		,
	0.891568		,
	0.886995		,
	0.882333		,
	0.877583		,
	0.872745		,
	0.867819		,
	0.862807		,
	0.857709		,
	0.852525		,
	0.847255		,
	0.841901		,
	0.836463		,
	0.830941		,
	0.825336		,
	0.819648		,
	0.813878		,
	0.808028		,
	0.802096		,
	0.796084		,
	0.789992		,
	0.783822		,
	0.777573		,
	0.771246		,
	0.764842		,
	0.758362		,
	0.751806		,
	0.745174		,
	0.738469		,
	0.731689		,
	0.724836		,
	0.717911		,
	0.710914		,
	0.703845		,
	0.696707		,
	0.689498		,
	0.682221		,
	0.674876		,
	0.667463		,
	0.659983		,
	0.652438		,
	0.644827		,
	0.637151		,
	0.629412		,
	0.62161			,
	0.613746		,
	0.60582			,
	0.597834		,
	0.589788		,
	0.581683		,
	0.57352			,
	0.5653			,
	0.557023		,
	0.54869			,
	0.540302		,
	0.531861		,
	0.523366		,
	0.514819		,
	0.50622			,
	0.497571		,
	0.488872		,
	0.480124		,
	0.471328		,
	0.462485		,
	0.453596		,
	0.444661		,
	0.435682		,
	0.42666			,
	0.417595		,
	0.408487		,
	0.39934			,
	0.390152		,
	0.380925		,
	0.37166			,
	0.362358		,
	0.353019		,
	0.343646		,
	0.334238		,
	0.324796		,
	0.315322		,
	0.305817		,
	0.296281		,
	0.286715		,
	0.277121		,
	0.267499		,
	0.25785			,
	0.248176		,
	0.238476		,
	0.228753		,
	0.219007		,
	0.209239		,
	0.19945			,
	0.189641		,
	0.179813		,
	0.169967		,
	0.160104		,
	0.150226		,
	0.140332		,
	0.130424		,
	0.120503		,
	0.11057			,
	0.100626		,
	0.0906716		,
	0.0807084		,
	0.0707372		,
	0.0607589		,
	0.0507745		,
	0.040785		,
	0.0307915		,
	0.0207949		,
	0.0107962		,
	0.000796393		,
	-0.00920347	,
	-0.0192024		,
	-0.0291994		,
	-0.0391936		,
	-0.0491838		,
	-0.0591691		,
	-0.0691484		,
	-0.0791209		,
	-0.0890854		,
	-0.099041		,
	-0.108987		,
	-0.118922		,
	-0.128844		,
	-0.138754		,
	-0.148651		,
	-0.158532		,
	-0.168397		,
	-0.178246		,
	-0.188077		,
	-0.197889		,
	-0.207681		,
	-0.217452		,
	-0.227202		,
	-0.236929		,
	-0.246632		,
	-0.256311		,
	-0.265964		,
	-0.27559		,
	-0.285189		,
	-0.294759		,
	-0.3043		,
	-0.313811		,
	-0.32329		,
	-0.332736		,
	-0.34215		,
	-0.351529		,
	-0.360873		,
	-0.370181		,
	-0.379452		,
	-0.388685		,
	-0.397879		,
	-0.407033		,
	-0.416147		,
	-0.425219		,
	-0.434248		,
	-0.443234		,
	-0.452176		,
	-0.461073		,
	-0.469923		,
	-0.478726		,
	-0.487482		,
	-0.496189		,
	-0.504846		,
	-0.513453		,
	-0.522008		,
	-0.530511		,
	-0.538961		,
	-0.547358		,
	-0.555699		,
	-0.563985		,
	-0.572215		,
	-0.580387		,
	-0.588501		,
	-0.596557		,
	-0.604552		,
	-0.612488		,
	-0.620362		,
	-0.628174		,
	-0.635923		,
	-0.643608		,
	-0.65123		,
	-0.658786		,
	-0.666276		,
	-0.6737		,
	-0.681056		,
	-0.688344		,
	-0.695563		,
	-0.702713		,
	-0.709792		,
	-0.716801		,
	-0.723738		,
	-0.730602		,
	-0.737394		,
	-0.744111		,
	-0.750754		,
	-0.757323		,
	-0.763815		,
	-0.770231		,
	-0.77657		,
	-0.782832		,
	-0.789015		,
	-0.795119		,
	-0.801144		,
	-0.807088		,
	-0.812952		,
	-0.818735		,
	-0.824435		,
	-0.830054		,
	-0.835589		,
	-0.84104		,
	-0.846408		,
	-0.851691		,
	-0.856889		,
	-0.862001		,
	-0.867027		,
	-0.871966		,
	-0.876818		,
	-0.881582		,
	-0.886258		,
	-0.890846		,
	-0.895344		,
	-0.899753		,
	-0.904072		,
	-0.908301		,
	-0.912438		,
	-0.916485		,
	-0.92044		,
	-0.924302		,
	-0.928073		,
	-0.93175		,
	-0.935335		,
	-0.938825		,
	-0.942222		,
	-0.945525		,
	-0.948733		,
	-0.951846		,
	-0.954865		,
	-0.957787		,
	-0.960614		,
	-0.963345		,
	-0.965979		,
	-0.968517		,
	-0.970958		,
	-0.973302		,
	-0.975549		,
	-0.977698		,
	-0.979749		,
	-0.981702		,
	-0.983557		,
	-0.985314		,
	-0.986972		,
	-0.988532		,
	-0.989992		,
	-0.991354		,
	-0.992617		,
	-0.99378		,
	-0.994844		,
	-0.995808		,
	-0.996673		,
	-0.997438		,
	-0.998104		,
	-0.998669		,
	-0.999135		,
	-0.999501		,
	-0.999767		,
	-0.999933		,
	-0.999999		,
};


float COS_ARR(float _fRadian)
{
	int iIndex = clamp(floor(_fRadian * 100.f), 0, 315);

	return cos_value[iIndex];
}