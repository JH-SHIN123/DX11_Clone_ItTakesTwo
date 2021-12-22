#include "Shader_Defines.hpp"
#include "Shader_Defines_Effect.hpp"

texture2D		g_DiffuseTexture;
texture2D		g_SecondTexture;
texture2D		g_ColorTexture;
texture2D		g_DissolveTexture;

BlendState BlendState_Add2
{
	BlendEnable[0] = true;
	BlendEnable[1] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = MAX;
	SrcBlendAlpha = ONE;
	DestBlendAlpha = ONE;
	BlendOpAlpha = Add;
};

BlendState BlendState_Alpha2
{
	BlendEnable[0] = true;
	SrcBlend = ONE;
	DestBlend = ONE;
	BlendOp = MAX;
};

sampler DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = wrap;
	AddressV = wrap;
};

sampler ColorSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;

	AddressU = mirror/*_once*/;
	AddressV = mirror/*_once*/;
};
//////////////////////////////////////////////////////////////////////////////////////////
struct VS_IN
{
	/* 본래 정점버퍼 */
	float3 vPosition	: POSITION;

	/* 인스턴스할 정점버퍼 */
	row_major matrix WorldMatrix	: WORLD;
	float2 vSize					: PSIZE;
	float4 vTextureUV_LTRB			: TEXCOORD0;
	float fTime						: TEXCOORD1;
};

struct VS_OUT
{
	float4	vPosition		: POSITION;
	float2	vSize			: PSIZE;
	float4	vTextureUV_LTRB	: TEXCOORD0;
	float	fTime			: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition			= mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB		= In.vTextureUV_LTRB;
	Out.vSize				= In.vSize;
	Out.fTime				= In.fTime;

	return Out;
}

struct VS_OUT_ROTATE
{
	float4	vPosition		: POSITION;
	float2	vSize			: PSIZE;
	float4	vTextureUV_LTRB	: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float	fAngle			: TEXCOORD2;
};

VS_OUT_ROTATE VS_ROTATE_X(VS_IN In)
{
	VS_OUT_ROTATE			Out = (VS_OUT_ROTATE)0;

	Out.vPosition			= mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB		= In.vTextureUV_LTRB;
	Out.vSize				= In.vSize;
	Out.fTime				= In.fTime;
	Out.fAngle				= In.WorldMatrix._11;

	return Out;
}

struct VS_OUT_NOBILL_DIR
{
	float4	vPosition		: POSITION;
	float2	vSize			: PSIZE;
	float4	vTextureUV_LTRB	: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float3	vNoBill_Dir		: TEXCOORD2;
};
VS_OUT_NOBILL_DIR VS_MAIN_NOBILL_Y(VS_IN In)
{
	VS_OUT_NOBILL_DIR			Out = (VS_OUT_NOBILL_DIR)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;
	Out.fTime = In.fTime;
	Out.vNoBill_Dir = In.WorldMatrix._21_22_23;

	return Out;
}

struct VS_OUT_TRIPLE_UV
{
	float4	vPosition		: POSITION;
	float2	vSize			: PSIZE;
	float4	vTextureUV_LTRB	: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float4	vTexUV_Right	: TEXCOORD2;
	float4	vTexUV_Up		: TEXCOORD3;
};

VS_OUT_TRIPLE_UV VS_TRIPLE_UV(VS_IN In)
{
	VS_OUT_TRIPLE_UV			Out = (VS_OUT_TRIPLE_UV)0;

	Out.vPosition		= mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize			= In.vSize;
	Out.fTime			= In.fTime;
	Out.vTexUV_Right	= In.WorldMatrix._11_12_13_14;
	Out.vTexUV_Up		= In.WorldMatrix._21_22_23_24;

	return Out;
}
//////////////////////////////////////////////////////////////////////////////////////////
struct  GS_OUT
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float4	vProjPosition	: TEXCOORD2;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_MAIN(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	Out[0].fTime = In[0].fTime;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	Out[1].fTime = In[0].fTime;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	Out[2].fTime = In[0].fTime;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	Out[3].fTime = In[0].fTime;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	Out[4].fTime = In[0].fTime;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	Out[5].fTime = In[0].fTime;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	Out[6].fTime = In[0].fTime;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	Out[7].fTime = In[0].fTime;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

[maxvertexcount(12)]
void  GS_MAIN_FLIP_U(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	float4 RotateUV = In[0].vTextureUV_LTRB;
	RotateUV.x = In[0].vTextureUV_LTRB.z;
	RotateUV.z = In[0].vTextureUV_LTRB.x;

	Out[0].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[4].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[1].vTexUV = float2(RotateUV.z, RotateUV.y);
	Out[5].vTexUV = float2(RotateUV.z, RotateUV.y);
	Out[2].vTexUV = float2(RotateUV.z, RotateUV.w);
	Out[6].vTexUV = float2(RotateUV.z, RotateUV.w);
	Out[3].vTexUV = float2(RotateUV.x, RotateUV.w);
	Out[7].vTexUV = float2(RotateUV.x, RotateUV.w);

	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;

	for (uint i = 0; i < 4; ++i)
	{
		Out[i].vPosition = mul(Out[i].vPosition, matVP);
		Out[i].vProjPosition = Out[i].vPosition;
		Out[i].iViewportIndex = 1;
		Out[i].fTime = In[0].fTime;
	}

	TriStream.Append(Out[0]);
	TriStream.Append(Out[1]);
	TriStream.Append(Out[2]);
	TriStream.RestartStrip();

	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);
	TriStream.RestartStrip();


	vLook	= normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight	= normalize(cross(vAxisY, vLook));
	vUp		= normalize(cross(vLook, vRight));
	matVP	= mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;

	for (uint j = 4; j < 8; ++j)
	{
		Out[j].vPosition = mul(Out[j].vPosition, matVP);
		Out[j].vProjPosition = Out[j].vPosition;
		Out[j].iViewportIndex = 2;
		Out[j].fTime = In[0].fTime;
	}

	TriStream.Append(Out[4]);
	TriStream.Append(Out[5]);
	TriStream.Append(Out[6]);
	TriStream.RestartStrip();

	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}


[maxvertexcount(12)]
void  GS_MAIN_ROTATE_ANGLE_X(/*입력*/ point  VS_OUT_ROTATE In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vLook	= normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight	= normalize(cross(vAxisY, vLook));
	float3		vUp		= normalize(cross(vLook, vRight));
	matrix		matVP	= mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	float fRadian = radians(In[0].fAngle);
	float fSin = sin(fRadian);
	float fCos = cos(fRadian);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In[0].vTextureUV_LTRB.xy - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;

	Out[0].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[4].vTexUV = float2(RotateUV.x, RotateUV.y);

	RotateUV = In[0].vTextureUV_LTRB.zy - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out[1].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[5].vTexUV = float2(RotateUV.x, RotateUV.y);

	RotateUV = In[0].vTextureUV_LTRB.zw - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out[2].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[6].vTexUV = float2(RotateUV.x, RotateUV.y);

	RotateUV = In[0].vTextureUV_LTRB.xw - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out[3].vTexUV = float2(RotateUV.x, RotateUV.y);
	Out[7].vTexUV = float2(RotateUV.x, RotateUV.y);

	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;

	[unroll]
	for (uint i = 0; i < 4; ++i)
	{
		Out[i].vPosition		= mul(Out[i].vPosition, matVP);
		Out[i].vProjPosition	= Out[i].vPosition;
		Out[i].iViewportIndex	= 1;
		Out[i].fTime			= In[0].fTime;
	}

	TriStream.Append(Out[0]);
	TriStream.Append(Out[1]);
	TriStream.Append(Out[2]);
	TriStream.RestartStrip();

	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);
	TriStream.RestartStrip();


	vLook	= normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight	= normalize(cross(vAxisY, vLook));
	vUp		= normalize(cross(vLook, vRight));
	matVP	= mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;

	[unroll]
	for (uint j = 4; j < 8; ++j)
	{
		Out[j].vPosition		= mul(Out[j].vPosition, matVP);
		Out[j].vProjPosition	= Out[j].vPosition;
		Out[j].iViewportIndex	= 2;
		Out[j].fTime			= In[0].fTime;
	}

	TriStream.Append(Out[4]);
	TriStream.Append(Out[5]);
	TriStream.Append(Out[6]);
	TriStream.RestartStrip();

	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

[maxvertexcount(12)]
void  GS_MAIN_NOBILL_Y(/*입력*/ point  VS_OUT_NOBILL_DIR In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	// Main View 0,0
	float3		vUp = In[0].vNoBill_Dir;

	float3		vCamDir = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(vCamDir, vUp));
	float3		vLook = normalize(cross(vRight, vUp));

	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	Out[0].fTime = In[0].fTime;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	Out[1].fTime = In[0].fTime;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	Out[2].fTime = In[0].fTime;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	Out[3].fTime = In[0].fTime;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vUp = In[0].vNoBill_Dir;

	vCamDir = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vRight = normalize(cross(vCamDir, vUp));
	vLook = normalize(cross(vRight, vUp));

	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	Out[4].fTime = In[0].fTime;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	Out[5].fTime = In[0].fTime;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	Out[6].fTime = In[0].fTime;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	Out[7].fTime = In[0].fTime;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

struct  GS_OUT_DOUBLEUV
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float2	vTexUV2			: TEXCOORD2;
	float4	vProjPosition	: TEXCOORD3;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_DOUBLEUV(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLEUV> TriStream)
{
	GS_OUT_DOUBLEUV		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].vTexUV2 = float2(g_vUV.x, g_vUV.y);
	Out[0].iViewportIndex = 1;
	Out[0].fTime = In[0].fTime;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].vTexUV2 = float2(g_vUV.z, g_vUV.y);
	Out[1].iViewportIndex = 1;
	Out[1].fTime = In[0].fTime;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].vTexUV2 = float2(g_vUV.z, g_vUV.w);
	Out[2].iViewportIndex = 1;
	Out[2].fTime = In[0].fTime;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].vTexUV2 = float2(g_vUV.x, g_vUV.w);
	Out[3].iViewportIndex = 1;
	Out[3].fTime = In[0].fTime;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].vTexUV2 = float2(g_vUV.x, g_vUV.y);
	Out[4].iViewportIndex = 2;
	Out[4].fTime = In[0].fTime;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].vTexUV2 = float2(g_vUV.z, g_vUV.y);
	Out[5].iViewportIndex = 2;
	Out[5].fTime = In[0].fTime;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].vTexUV2 = float2(g_vUV.z, g_vUV.w);
	Out[6].iViewportIndex = 2;
	Out[6].fTime = In[0].fTime;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].vTexUV2 = float2(g_vUV.x, g_vUV.w);
	Out[7].iViewportIndex = 2;
	Out[7].fTime = In[0].fTime;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

[maxvertexcount(12)]
void  GS_DOUBLEUV_TIME(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLEUV> TriStream)
{
	GS_OUT_DOUBLEUV		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[0].iViewportIndex = 1;
	Out[0].fTime = In[0].fTime;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[1].iViewportIndex = 1;
	Out[1].fTime = In[0].fTime;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[2].iViewportIndex = 1;
	Out[2].fTime = In[0].fTime;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[3].iViewportIndex = 1;
	Out[3].fTime = In[0].fTime;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[4].iViewportIndex = 2;
	Out[4].fTime = In[0].fTime;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[5].iViewportIndex = 2;
	Out[5].fTime = In[0].fTime;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[6].iViewportIndex = 2;
	Out[6].fTime = In[0].fTime;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].vTexUV2 = float2(In[0].fTime, 0.f);
	Out[7].iViewportIndex = 2;
	Out[7].fTime = In[0].fTime;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

[maxvertexcount(12)]
void  GS_DOUBLEUV_LEFT_ROTATE_BILL_Y_UP_SIZE(/*입력*/ point  VS_OUT_NOBILL_DIR In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLEUV> TriStream)
{
	GS_OUT_DOUBLEUV		Out[8];

	// Main View 0,0
	float3		vUp = In[0].vNoBill_Dir;

	float3		vCamDir = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(vCamDir, vUp));
	float3		vLook = normalize(cross(vRight, vUp));

	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].vTexUV2 = float2(g_vUV.x, g_vUV.y);
	Out[0].iViewportIndex = 1;
	Out[0].fTime = In[0].fTime;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].vTexUV2 = float2(g_vUV.z, g_vUV.y);
	Out[1].iViewportIndex = 1;
	Out[1].fTime = In[0].fTime;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].vTexUV2 = float2(g_vUV.z, g_vUV.w);
	Out[2].iViewportIndex = 1;
	Out[2].fTime = In[0].fTime;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].vTexUV2 = float2(g_vUV.x, g_vUV.w);
	Out[3].iViewportIndex = 1;
	Out[3].fTime = In[0].fTime;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vUp = In[0].vNoBill_Dir;

	vCamDir = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vRight = normalize(cross(vCamDir, vUp));
	vLook = normalize(cross(vRight, vUp));

	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].vTexUV2 = float2(g_vUV.y, g_vUV.z);
	Out[4].iViewportIndex = 2;
	Out[4].fTime = In[0].fTime;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].vTexUV2 = float2(g_vUV.w, g_vUV.z);
	Out[5].iViewportIndex = 2;
	Out[5].fTime = In[0].fTime;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].vTexUV2 = float2(g_vUV.w, g_vUV.z);
	Out[6].iViewportIndex = 2;
	Out[6].fTime = In[0].fTime;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].vTexUV2 = float2(g_vUV.y, g_vUV.x);
	Out[7].iViewportIndex = 2;
	Out[7].fTime = In[0].fTime;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}


struct  GS_OUT_TRIPLE_UV
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV_Diff		: TEXCOORD0;
	float	fTime			: TEXCOORD1;
	float2	vTexUV_Diss		: TEXCOORD2;
	float2	vTexUV_Dist		: TEXCOORD3;
	float4	vProjPosition	: TEXCOORD4;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(12)]
void  GS_TRIPLE_UV(/*입력*/ point  VS_OUT_TRIPLE_UV In[1], /*출력*/ inout TriangleStream<GS_OUT_TRIPLE_UV> TriStream)
{
	GS_OUT_TRIPLE_UV		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[0].vPosition	= In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vTexUV_Diff	= float2(In[0].vTextureUV_LTRB.x,	In[0].vTextureUV_LTRB.y);
	Out[0].vTexUV_Diss	= float2(In[0].vTexUV_Right.x,		In[0].vTexUV_Right.y);
	Out[0].vTexUV_Dist	= float2(In[0].vTexUV_Up.x,			In[0].vTexUV_Up.y);

	Out[1].vPosition	= In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vTexUV_Diff	= float2(In[0].vTextureUV_LTRB.z,	In[0].vTextureUV_LTRB.y);
	Out[1].vTexUV_Diss	= float2(In[0].vTexUV_Right.z,		In[0].vTexUV_Right.y);
	Out[1].vTexUV_Dist	= float2(In[0].vTexUV_Up.z,			In[0].vTexUV_Up.y);

	Out[2].vPosition	= In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vTexUV_Diff	= float2(In[0].vTextureUV_LTRB.z,	In[0].vTextureUV_LTRB.w);
	Out[2].vTexUV_Diss	= float2(In[0].vTexUV_Right.z,		In[0].vTexUV_Right.w);
	Out[2].vTexUV_Dist	= float2(In[0].vTexUV_Up.z,			In[0].vTexUV_Up.w);

	Out[3].vPosition	= In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vTexUV_Diff	= float2(In[0].vTextureUV_LTRB.x,	In[0].vTextureUV_LTRB.w);
	Out[3].vTexUV_Diss	= float2(In[0].vTexUV_Right.x,		In[0].vTexUV_Right.w);
	Out[3].vTexUV_Dist	= float2(In[0].vTexUV_Up.x,			In[0].vTexUV_Up.w);

	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		Out[i].vPosition = mul(Out[i].vPosition, matVP);
		Out[i].vProjPosition = Out[i].vPosition;
		Out[i].iViewportIndex = 1;
		Out[i].fTime = In[0].fTime;
	}

	TriStream.Append(Out[0]);
	TriStream.Append(Out[1]);
	TriStream.Append(Out[2]);
	TriStream.RestartStrip();

	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);
	TriStream.RestartStrip();


	vLook	= normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight	= normalize(cross(vAxisY, vLook));
	vUp		= normalize(cross(vLook, vRight));
	matVP	= mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;

	[unroll]
	for (int j = 4; j < 8; ++j)
	{
		Out[j].vTexUV_Diff = Out[j - 4].vTexUV_Diff;
		Out[j].vTexUV_Diss = Out[j - 4].vTexUV_Diss;
		Out[j].vTexUV_Dist = Out[j - 4].vTexUV_Dist;
			
		Out[j].vPosition = mul(Out[j].vPosition, matVP);
		Out[j].vProjPosition = Out[j].vPosition;
		Out[j].iViewportIndex = 2;
		Out[j].fTime = In[0].fTime;
	}

	TriStream.Append(Out[4]);
	TriStream.Append(Out[5]);
	TriStream.Append(Out[6]);
	TriStream.RestartStrip();

	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

///////////////////////////////////////////////////////////////////////////////
struct  PS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float	fTime		: TEXCOORD1;
};

struct  PS_IN_DOUBLEUV
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float	fTime		: TEXCOORD1;
	float2	vTexUV2		: TEXCOORD2;
};

struct  PS_IN_TRIPLEUV
{
	// Diffuse, Dissolve, Distortion
	float4	vPosition	: SV_POSITION;
	float2	vTexUV_Diff	: TEXCOORD0;
	float	fTime		: TEXCOORD1;
	float2	vTexUV_Diss	: TEXCOORD2;
	float2	vTexUV_Dist	: TEXCOORD3;

};

struct  PS_OUT
{
	vector	vColor : SV_TARGET;
};

PS_OUT  PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.01f >= vColor.a)
		discard;

	Out.vColor.rgba = vColor.a * In.fTime;
	//Out.vColor.a = In.fTime;

	return Out;
}

PS_OUT  PS_ONLY_TEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.01f >= vColor.a)
		discard;

	Out.vColor = vColor;
	Out.vColor.a = vColor.r;

	return Out;
}

PS_OUT  PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	//float fRGB = Out.vColor.r + Out.vColor.g + Out.vColor.b;
	//if (0.1f >= Out.vColor.a)
	//	discard;
	Out.vColor.rgb *= g_vColor.rgb;
	Out.vColor.a = Out.vColor.r;

	Out.vColor.rgba *= In.fTime;

	return Out;
}

PS_OUT  PS_CIRCLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = (float4)0;
	float2 vCenter = In.vTexUV - 0.5f;
	vCenter = abs(vCenter);
	float fLenght = length(vCenter);
	vDiffuse.a = 1.f;

	fLenght = fLenght / 0.5f; // normalize
	float2 vColorUV = (float2)((fLenght * -1.f) + 1.f);
	float4 vColor = g_SecondTexture.Sample(DiffuseSampler, vColorUV);

	vDiffuse.rgb = vColor.rgb * 1.2f;
	vDiffuse.a *= In.fTime;
	Out.vColor = vDiffuse;

	if (0.5f < length(vCenter))
		discard;

	return Out;
}

PS_OUT  PS_PINBALL_DUST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = (float4)0;
	float2 vCenter = In.vTexUV - 0.5f;
	vCenter = abs(vCenter);
	float fLenght = length(vCenter);
	vDiffuse.a = 1.f;

	fLenght = fLenght / 0.5f; // normalize
	float2 vColorUV = (float2)((fLenght * -1.f) + 1.f);

	vDiffuse.rgb = g_vColor.rgb;
	vDiffuse.a *= In.fTime;
	Out.vColor = vDiffuse;

	if (0.5f < length(vCenter))
		discard;

	return Out;
}

PS_OUT  PS_DIFFUSE_ALPHA_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.a)
		discard;

	Out.vColor.rgb = vDiff.a;

	float2 vColorUV = (float2)vDiff.a;
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vColorUV);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.rgb *= In.fTime * g_fTime;
	Out.vColor.a	= In.fTime;

	return Out;
}

PS_OUT  PS_ENV_DUST(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;

	Out.vColor.rgba = vDiff.r;

	float2 vColorUV = (float2)vDiff.r;
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vColorUV);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.rgb *= In.fTime * g_fTime;
	Out.vColor.a = In.fTime;

	return Out;
}


PS_OUT  PS_MOONBABOON_BOOSTER(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor = vDiff;

	Out.vColor.a = Out.vColor.r * In.fTime * g_fTime;

	return Out;
}

PS_OUT  PS_MAIN_COLORTEXTURE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV2);
	if (0.01f >= vDiff.r)
		discard;

	Out.vColor.rgb = vDiff.rgb;

	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.rgb *= In.fTime * g_fTime;
	Out.vColor.a = In.fTime;

	return Out;
}

PS_OUT  PS_MAIN_LASER_SMOKE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	//float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	//if (0.01f >= vDiff.r)
	//	discard;

	//Out.vColor.rgba = vDiff.rgba;
	//float2 vTexUV_2 = { In.fTime , 0.f };
	//float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_2);
	//float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV2);

	//Out.vColor.rgb *= vColor.rgb * vDistortion.rgb;
	//Out.vColor.a *= Out.vColor.r;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV2);

	Out.vColor = vDiff;
	Out.vColor.rgb = vDistortion.rgb;

	float2 vTexUV_3 = { In.fTime , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_3);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.a *= Out.vColor.r * g_fTime;

	return Out;
}

PS_OUT  PS_BOSS_LASER_SMOKE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;

	In.vTexUV2.y += In.fTime * 2.f;
	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV2);

	Out.vColor = vDiff;
	Out.vColor.rgb = vDistortion.rgb;

	float2 vTexUV_3 = { In.fTime , 0.f };
	if (0.f > In.fTime)
		vTexUV_3.x = 0.f;
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_3);

	Out.vColor.rgb *= vColor.rgb *  In.fTime;
	Out.vColor.a = Out.vColor.r * g_fTime *  In.fTime;

	return Out;
}

PS_OUT  PS_MAIN_MISSILE_SMOKE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	float2 vUV = In.vTexUV2;
	vUV.y += In.fTime * 2.f + vUV.x;
	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, vUV);

	Out.vColor = vDiff;
	if(In.fTime > vDistortion.r)
		Out.vColor.rgb *= vDistortion.rgb * In.fTime * 10.f;

	float2 vTexUV_3 = { In.fTime , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_3);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.a *= In.fTime * g_fAlpha * Out.vColor.r  * 0.5;

	return Out;
}

PS_OUT  PS_MAIN_MISSILE_SMOKE_BLACK(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vDiff.rgb *= In.fTime;

	vDiff.a = (vDiff.r + vDiff.g)* In.fTime * 0.5f;

	Out.vColor = vDiff;

	return Out;
}

PS_OUT  PS_MAIN_MISSILE_EXPLOSION(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	float2 vUV = In.vTexUV2;
	vUV.y += In.fTime * 2.f + vUV.x;
	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, vUV);

	Out.vColor = vDiff;
	if (In.fTime > vDistortion.r)
		Out.vColor.rgb *= vDistortion.rgb * In.fTime * 10.f;

	float2 vTexUV_3 = { vDiff.r , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_3);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.a *= In.fTime * g_fAlpha * Out.vColor.r;

	return Out;
}

PS_OUT  PS_MAIN_LASER_PARTICLE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	Out.vColor = vDiff;
	Out.vColor.a = vDiff.r;

	float2 vTexUV_2 = { In.fTime , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_2);

	Out.vColor.rgb *= vColor.rgb;
	Out.vColor.a *= vColor.r;

	return Out;
}

PS_OUT  PS_COLOR_UV(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	Out.vColor = vDiff;
	Out.vColor.a = vDiff.r;

	float2 vTexUV_2 = { vDiff.r , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_2);

	Out.vColor.rgb *= vColor.rgb * 1.5f *  In.fTime * g_fTime;

	return Out;
}

PS_OUT  PS_MAIN_PARTICLE_RGB(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	Out.vColor = vDiff;
	Out.vColor.a = vDiff.r;

	float2 vTexUV_2 = { In.fTime , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_2);

	Out.vColor.rgb *= vColor.rgb * 1.5f;
	Out.vColor.a *= (vColor.r + vColor.g + vColor.b) * 0.5f * In.fTime * g_fTime;

	return Out;
}

PS_OUT  PS_MAIN_PARTICLE_UFO(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= vDiff.r)
		discard;
	Out.vColor = vDiff;
	Out.vColor.a = vDiff.r * 2.f;

	float2 vTexUV_2 = { In.fTime , 0.f };
	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vTexUV_2);

	Out.vColor.rgb *= vColor.rgb * 1.5f;
	Out.vColor.a *= (vColor.r + vColor.g + vColor.b) * 1.f * In.fTime * g_fTime;

	return Out;
}

PS_OUT  PS_MAIN_DEFAULT_SMOKE(PS_IN_DOUBLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.01f >= (vDiff.r + vDiff.g + vDiff.b) / 3.f)
		discard;

	Out.vColor = vDiff;

	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, In.vTexUV2);

	Out.vColor.rgb *= vColor.rgb * In.fTime * g_fTime;
	Out.vColor.a = Out.vColor.r * In.fTime * g_fTime;

	return Out;
}

PS_OUT  PS_MAIN_MISSILE_SMOKE_COLOR(PS_IN_TRIPLEUV In)
{
	PS_OUT		Out = (PS_OUT)0;

	// 왜곡을 먹은 디졸브를 디퓨즈(스모크)틀 에다가 곱하고 그 색상의 r값을 UV로 해서 색상을 입히자

	In.vTexUV_Dist.y += g_fAlpha;
	float4 vDist = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV_Dist);
	float fWeight = vDist.r * 0.5f;
	
	In.vTexUV_Diss.y += fWeight;
	float4 vDiss = g_DissolveTexture.Sample(DiffuseSampler, In.vTexUV_Diss);

	float4 vDiff = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV_Diff);

	vDiff.a = vDiff.r * In.fTime * g_fAlpha;
	vDiff.rgb = vDiss.rgb;

	if (vDiff.r + 0.05f < In.fTime)
	{
		float2 vUV = (float2)0;
		vUV.x = In.fTime;
		vDiff.rgb *= g_ColorTexture.Sample(DiffuseSampler, vUV).rgb;
		vDiff.rgb *= 1.5f;
	}
	else
		vDiff.rgb *= 0.2f;

	Out.vColor = vDiff;

	return Out;
}

technique11		DefaultTechnique
{
	pass PointInstance_Default // 0
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha2, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN();
	}

	pass PS_G_COLOR // 1
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_COLOR();
	}

	pass PS_COLORTEXTURE // 2
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_COLORTEXTURE();
	}

	pass PS_BOSSLASER_SMOKE // 3
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_LASER_SMOKE();
	}

	pass PS_BOSSLASER_PARTICLE // 4
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NOBILL_Y();
		GeometryShader = compile gs_5_0  GS_MAIN_NOBILL_Y();
		PixelShader = compile ps_5_0  PS_MAIN_LASER_PARTICLE();
	}

	pass PS_BOSSLASER_SMOKE_LOOP // 5
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_ONLY_TEX();
	}

	pass PS_PARTICLE // 6
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NOBILL_Y();
		GeometryShader = compile gs_5_0  GS_MAIN_NOBILL_Y();
		PixelShader = compile ps_5_0  PS_MAIN_PARTICLE_RGB();
	}

	pass PS_COLOR_SPRITE // 7
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_DEFAULT_SMOKE();
	}

	pass PS_BOSS_CORE_LIGHTNING // 8
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NOBILL_Y();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV_LEFT_ROTATE_BILL_Y_UP_SIZE();
		PixelShader = compile ps_5_0  PS_MAIN_DEFAULT_SMOKE();
	}

	pass PS_BOSSMISSILE_SMOKE // 9
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_MISSILE_SMOKE();
	}

	pass PS_BOSSMISSILE_EXPLOSION // 10
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_MISSILE_EXPLOSION();
	}

	pass PS_UFO_INSIDE_BATTERY_PARTICLE // 11
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NOBILL_Y();
		GeometryShader = compile gs_5_0  GS_MAIN_NOBILL_Y();
		PixelShader = compile ps_5_0  PS_COLOR_UV();
	}

	pass PS_PINBALL_MOVE // 12
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_CIRCLE();
	}

	pass PS_BASIC // 13
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_DIFFUSE_ALPHA_COLOR();
	}

	pass PS_ENV_DUST // 14
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_ENV_DUST();
	}

	pass PS_ENV_STAR // 15
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_ROTATE_X();
		GeometryShader = compile gs_5_0  GS_MAIN_ROTATE_ANGLE_X();
		PixelShader = compile ps_5_0  PS_DIFFUSE_ALPHA_COLOR();
	}

	pass MOONBABBON_BOOSTER // 16
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NOBILL_Y();
		GeometryShader = compile gs_5_0  GS_MAIN_NOBILL_Y();
		PixelShader = compile ps_5_0  PS_MOONBABOON_BOOSTER();
	}

	pass PS_PINBALL_DUST // 17
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_PINBALL_DUST();
	}

	pass Missile_Smoke_Black // 18
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_MAIN_MISSILE_SMOKE_BLACK();
	}

	pass Missile_Smoke_Color // 19
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_TRIPLE_UV();
		GeometryShader = compile gs_5_0  GS_TRIPLE_UV();
		PixelShader = compile ps_5_0  PS_MAIN_MISSILE_SMOKE_COLOR();
	}

	pass BOSSLASER_SMOKE // 20
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_DOUBLEUV();
		PixelShader = compile ps_5_0  PS_BOSS_LASER_SMOKE();
	}
}