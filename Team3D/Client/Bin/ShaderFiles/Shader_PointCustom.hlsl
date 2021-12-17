#include "Shader_Defines.hpp"
#include "Shader_Defines_Effect.hpp"

texture2D		g_DiffuseTexture;
texture2D		g_SecondTexture;
texture2D		g_ColorTexture;

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

cbuffer Light_Effect
{
	// 채도 0.25, 명암 1.0이  가장 괜찮았음
	float g_fSaturation_Power;	
	float g_fContrast_Power;	
}

struct VS_IN
{
	/* 본래 정점버퍼 */
	float3 vPosition	: POSITION;

	/* 인스턴스할 정점버퍼 */
	row_major matrix WorldMatrix	: WORLD;
	float2 vSize					: PSIZE;
	float4 vTextureUV_LTRB			: TEXCOORD0;
};

struct VS_OUT
{
	float4 vPosition		: POSITION;
	float2 vSize			: PSIZE;
	float4 vTextureUV_LTRB	: TEXCOORD0;
	float3 vPointAxis		: TEXCOORD1; // BillBoard
};

struct VS_OUT_NOBILLBOARD
{
	float4 vPosition		: POSITION;
	float2 vSize			: PSIZE;
	float4 vTextureUV_LTRB	: TEXCOORD0;
	float3 vRight			: TEXCOORD1;
	float3 vUp				: TEXCOORD2;
	float3 vLook			: TEXCOORD3;

};

struct VS_OUT_ROTATE_POWER
{
	float4 vPosition			: POSITION;
	float2 vSize				: PSIZE;
	float4 vTextureUV_LTRB		: TEXCOORD0;
	float2 vTextureUV_Rotate	: TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;

	Out.vPointAxis = -In.WorldMatrix._31_32_33;

	return Out;
}

VS_OUT VS_MAIN_AXIS_R(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;

	Out.vPointAxis = -In.WorldMatrix._11_12_13;

	return Out;
}

VS_OUT VS_MAIN_AXIS_Y(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = g_vUV;
	Out.vSize = In.vSize;

	Out.vPointAxis = In.vTextureUV_LTRB.xyz;

	return Out;
}

VS_OUT VS_MAIN_AXIS_Y_NO_GLOBAL_UV(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;

	Out.vPointAxis = In.WorldMatrix._21_22_23;

	return Out;
}

VS_OUT_NOBILLBOARD VS_MAIN_NO_BILL(VS_IN In)
{
	VS_OUT_NOBILLBOARD	Out = (VS_OUT_NOBILLBOARD)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize	= In.vSize;
	Out.vRight	= In.WorldMatrix._11_12_13;
	Out.vUp		= In.WorldMatrix._21_22_23;
	Out.vLook	= In.WorldMatrix._31_32_33;

	return Out;
}

VS_OUT_ROTATE_POWER VS_MAIN_DOUBLE_UV_ROTATE(VS_IN In)
{
	VS_OUT_ROTATE_POWER			Out = (VS_OUT_ROTATE_POWER)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In.vTextureUV_LTRB.xy - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	Out.vTextureUV_Rotate = RotateUV;
	return Out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct  GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	float4 vProjPosition	: TEXCOORD1;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_MAIN(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;

	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

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
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook	= normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;
	vAxisY	= vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight	= normalize(cross(vAxisY, vLook));
	vUp		= normalize(cross(vLook, vRight));
	matVP	= mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;
	
	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_NO_BILL_Y(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vUp = In[0].vPointAxis;

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
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	// Sub View 0,1

	vUp = In[0].vPointAxis;
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
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_NO_BILL_Y_ROTATE_UV(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vUp = In[0].vPointAxis;

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
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.w, In[0].vTextureUV_LTRB.x);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.w, In[0].vTextureUV_LTRB.z);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.y, In[0].vTextureUV_LTRB.z);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.y, In[0].vTextureUV_LTRB.x);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	// Sub View 0,1

	vUp = In[0].vPointAxis;
	vCamDir = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vRight = normalize(cross(vCamDir, vUp));
	vLook = normalize(cross(vRight, vUp));

	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.w, In[0].vTextureUV_LTRB.x);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.w, In[0].vTextureUV_LTRB.z);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.y, In[0].vTextureUV_LTRB.z);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.y, In[0].vTextureUV_LTRB.z);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_NO_BILL_Y_DOWN_SIZE(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vUp = In[0].vPointAxis;

	float3		vCamDir = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	float3		vRight = normalize(cross(vCamDir, vUp));
	float3		vLook = normalize(cross(vRight, vUp));

	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	// Sub View 0,1

	vUp = In[0].vPointAxis;
	vCamDir = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	vRight = normalize(cross(vCamDir, vUp));
	vLook = normalize(cross(vRight, vUp));

	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN__LASER(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vUp = In[0].vPointAxis;

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
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	// Sub View 0,1

	vUp = In[0].vPointAxis;
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
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_NO_BILL_UPSIZE(/*입력*/ point  VS_OUT_NOBILLBOARD In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	float3		vRight	= In[0].vRight;
	float3		vUp		= In[0].vUp;
	float3		vLook	= In[0].vLook;

	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	// Sub View 0,1
	vRight	= In[0].vRight;
	vUp		= In[0].vUp;
	vLook	= In[0].vLook;
	matVP	= mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
////////////
struct  GS_OUT_DOUBLE_TEX
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0; // 전역변수 0011
	float2 vTexUV_2			: TEXCOORD1; // 버퍼 하나가 각개적으로 들고있는 UV
	float4 vProjPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_MAIN_DOUBLE_TEX_GLOBAL(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLE_TEX> TriStream)
{
	GS_OUT_DOUBLE_TEX		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;

	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

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
	Out[0].vTexUV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[0].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[1].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[2].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV  = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[3].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;
	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[4].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[5].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[6].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[7].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_DOUBLE_UV(/*입력*/ point  VS_OUT_ROTATE_POWER In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLE_TEX> TriStream)
{
	GS_OUT_DOUBLE_TEX		Out[8];

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
	Out[0].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
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
	Out[4].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vTexUV_2 = In[0].vTextureUV_Rotate;
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
[maxvertexcount(12)]
void  GS_MAIN_DOUBLE_TEX_ANGLE_NOT_BILL(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLE_TEX> TriStream)
{
	GS_OUT_DOUBLE_TEX		Out[8];

	float3		vUp = In[0].vPointAxis;
	float3		vAxisX = float3(1.f, 0.f, 0.f);
	float3		vLook = normalize(cross(vAxisX, vUp));
	float3		vRight = normalize(cross(vUp, vLook));

	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float2 RotateUV = In[0].vTextureUV_LTRB.xy - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	float4 vAngle_LTRB;
	vAngle_LTRB.xy = RotateUV;

	RotateUV = In[0].vTextureUV_LTRB.zw - 0.5f;
	RotateUV = mul(RotateUV, RotateMatrix);
	RotateUV += 0.5f;
	vAngle_LTRB.zw = RotateUV;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(vAngle_LTRB.x, vAngle_LTRB.y);
	Out[0].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(vAngle_LTRB.z, vAngle_LTRB.y);
	Out[1].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(vAngle_LTRB.z, vAngle_LTRB.w);
	Out[2].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(vAngle_LTRB.x, vAngle_LTRB.w);
	Out[3].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(vAngle_LTRB.x, vAngle_LTRB.y);
	Out[4].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(vAngle_LTRB.z, vAngle_LTRB.y);
	Out[5].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(vAngle_LTRB.z, vAngle_LTRB.w);
	Out[6].vTexUV_2 = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(vAngle_LTRB.x, vAngle_LTRB.w);
	Out[7].vTexUV_2 = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

/*
*/
////////////
struct  GS_OUT_DIST
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	float2 vWightUV			: TEXCOORD1;
	float2 vColorRamp_UV	: TEXCOORD2;
	float4 vProjPosition	: TEXCOORD3;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_MAIN_DIST(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DIST> TriStream)
{
	GS_OUT_DIST		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

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
	Out[0].vWightUV = float2(g_vUV.x, g_vUV.y);
	Out[0].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vWightUV = float2(g_vUV.z, g_vUV.y);
	Out[1].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vWightUV = float2(g_vUV.z, g_vUV.w);
	Out[2].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vWightUV = float2(g_vUV.x, g_vUV.w);
	Out[3].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vWightUV = float2(g_vUV.x, g_vUV.y);
	Out[4].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vWightUV = float2(g_vUV.z, g_vUV.y);
	Out[5].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vWightUV = float2(g_vUV.z, g_vUV.w);
	Out[6].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vWightUV = float2(g_vUV.x, g_vUV.w);
	Out[7].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

[maxvertexcount(12)]
void  GS_MAIN_DIST_ROTATION(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DIST> TriStream)
{
	GS_OUT_DIST		Out[8];

	// Main View 0,0
	float3		vLook = normalize(g_vMainCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

	float3		vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	float3		vRight = normalize(cross(vAxisY, vLook));
	float3		vUp = normalize(cross(vLook, vRight));
	matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

	float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

	float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	float4		vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	float fSin = sin(g_fRadianAngle);
	float fCos = cos(g_fRadianAngle);
	float2x2 RotateMatrix = float2x2(fCos, -fSin, fSin, fCos);
	float4 RotateUV = In[0].vTextureUV_LTRB - 0.5f;

	/* 좌상 */
	Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[0].vPosition = mul(Out[0].vPosition, matVP);
	Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vWightUV = mul(RotateUV.xy, RotateMatrix).xy + 0.5f;
	Out[0].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vWightUV = mul(RotateUV.zy, RotateMatrix).xy + 0.5f;
	Out[1].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vWightUV = mul(RotateUV.zw, RotateMatrix).xy + 0.5f;
	Out[2].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[3].vWightUV = mul(RotateUV.xw, RotateMatrix).xy + 0.5f;
	Out[3].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[3].vProjPosition = Out[3].vPosition;
	Out[3].iViewportIndex = 1;
	TriStream.Append(Out[0]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();
	// Sub View 0,1

	vLook = normalize(g_vSubCamPosition - In[0].vPosition).xyz;
	if (0 == g_IsBillBoard) // false
		vLook = In[0].vPointAxis;

	vAxisY = vector(0.f, 1.f, 0.f, 0.f).xyz;
	vRight = normalize(cross(vAxisY, vLook));
	vUp = normalize(cross(vLook, vRight));
	matVP = mul(g_SubViewMatrix, g_SubProjMatrix);

	vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
	vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

	Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[4].vPosition = mul(Out[4].vPosition, matVP);
	Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vWightUV = mul(RotateUV.xy, RotateMatrix).xy + 0.5f;
	Out[4].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vWightUV = mul(RotateUV.zy, RotateMatrix).xy + 0.5f;
	Out[5].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vWightUV = mul(RotateUV.zw, RotateMatrix).xy + 0.5f;
	Out[6].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vWightUV = mul(RotateUV.xw, RotateMatrix).xy + 0.5f;
	Out[7].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct  PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct  PS_OUT
{
	vector	vColor : SV_TARGET;
};

/* 픽셀의 최종적인 색을 결정하낟. */
PS_OUT  PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	//Out.vColor.a *= 2;
	//if (0.01f >= Out.vColor.a)
	//	discard;

	return Out;
}

PS_OUT  PS_MAIN_COLOR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor *= g_vColor;

	float fColor = Out.vColor.r + Out.vColor.g + Out.vColor.b;

	if (0.01f >= fColor)
		discard;

	if (0.01f >= Out.vColor.a)
		discard;

	return Out;
}

PS_OUT  PS_MAIN_COLOR_NOALPHATEX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r;
	Out.vColor *= g_vColor;

	return Out;
}

PS_OUT  PS_UFO_ELECTRICBOX(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r * 2.f;
	Out.vColor *= g_vColor * 3.f;

	return Out;
}

PS_OUT  PS_MAIN_COLOR_NOALPHATEX_TIME(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r * g_fTime;
	Out.vColor *= g_vColor;

	return Out;
}

PS_OUT  PS_MAIN_PRESS_SMOKE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.a *= g_fTime;
	Out.vColor.rgb *= g_vColor.rgb;
	return Out;
}

PS_OUT  PS_MAIN_ALPHATIME(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= Out.vColor.a)
		discard;

	return Out;
}

PS_OUT  PS_MAIN_PILLAR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV );
	Out.vColor.rgb *= g_vColor.rgb;
	Out.vColor.rgb *= g_fTime;
	Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;

	return Out;
}

PS_OUT  PS_SMOKE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV);
	Out.vColor.a *= g_fTime;

	return Out;
}

PS_OUT  PS_EXPLOSION(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV);

	float2 vTex_UV = In.vTexUV;
	vTex_UV.y += vDiffuse.r * 0.5f;

	float4 vDist = g_SecondTexture.Sample(ColorSampler, In.vTexUV);
	vDiffuse *= vDist;
	Out.vColor = vDiffuse;

	float4 vColorRamp = g_ColorTexture.Sample(ColorSampler, vTex_UV);
	Out.vColor.rgb *= vColorRamp.rgb;

	Out.vColor.a *= g_fAlpha;

	return Out;
}

PS_OUT  PS_MAIN_LASER(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.g;		

	if (Out.vColor.g > 0.6f)
	{
		Out.vColor.b = 1.f;
		Out.vColor.a = 0.9f;
	}

	return Out;
}

PS_OUT  PS_MAIN_LASER_OUTSIDE(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV);
	Out.vColor = vector(Out.vColor.r, 0.f, 0.f, g_fAlpha);

	return Out;
}

PS_OUT  PS_MAIN_LIGHT(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = (float4)0;
	float2 vCenter = In.vTexUV - 0.5f;
	vCenter = abs(vCenter);

	float fLenght = length(vCenter);
	if (0.5f < fLenght)
		discard;

	vDiffuse.a = 1.f;

	fLenght = fLenght / 0.5f; // normalize
	float2 vColorUV = (float2)((fLenght * -1.f) + 1.f);
	float4 vColor = g_SecondTexture.Sample(DiffuseSampler, vColorUV);

	vDiffuse.rgb = vColor.rgb * g_fSaturation_Power;
	vDiffuse.a *= g_fContrast_Power; 
	Out.vColor = vDiffuse;

	return Out;
}

PS_OUT  PS_MAIN_LASER_CIRCLE(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	float2 vCenter = In.vTexUV - 0.5f;
	vCenter = abs(vCenter);

	float fLength = length(vCenter);
	
	if (0.5f < fLength)
		discard;

	fLength = fLength / 0.5f; // normalize
	float2 vColorUV = (float2)((fLength * -1.f) + 1.f);
	float4 vColor = g_DiffuseTexture.Sample(DiffuseSampler, vColorUV);

	Out.vColor = vColor;
	Out.vColor.a = Out.vColor.g * 3;

	if (Out.vColor.g > 0.6f)
	{
		Out.vColor.b = 1.f;
		Out.vColor.a = 0.9f;
	}

	return Out;
}


struct  PS_IN_DOUBLE_UV
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float2 vColorUV : TEXCOORD1;
};

PS_OUT  PS_MAIN_DOUBLE_TEX(PS_IN_DOUBLE_UV In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV);
	if (0.01f >= Out.vColor.r)
		discard;

	vector vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vColorUV);
	Out.vColor = vColor;
	Out.vColor.a = 1.f;
	//Out.vColor.a = 1.f;
	float fColor = Out.vColor.r + Out.vColor.g + Out.vColor.b;

	if (0.01f >= fColor)
		discard;

	return Out;
}

PS_OUT  PS_MAIN_DIFF_SPRITE(PS_IN_DOUBLE_UV In)
{
	PS_OUT		Out = (PS_OUT)0;

	vector vDiffuse = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vector vSprite = g_SecondTexture.Sample(DiffuseSampler, In.vColorUV);

	if (0.f >= vSprite.a)
		discard;

	vSprite *= vDiffuse * g_fTime;
	Out.vColor = vSprite;

	return Out;
}

PS_OUT  PS_MAIN_ROTATE_DISTORTION(PS_IN_DOUBLE_UV In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, In.vColorUV);
	float fWeight = 0.f;
	//fWeight = (vDistortion.r) * 0.5f;

	float4 vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV + fWeight);

	Out.vColor = vColor;
	Out.vColor *= g_vColor;

	return Out;
}

struct  PS_IN_DIST
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
	float2 vWeightUV : TEXCOORD1;
	float2 vColorRamp_UV	: TEXCOORD2;

};

PS_OUT  PS_DISTORTION(PS_IN_DIST In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vFX_tex = g_SecondTexture.Sample(DiffuseSampler, In.vWeightUV);
	float4 vColor = (float4)0.f;
	float fWeight = vFX_tex.r * 0.3f;

	vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV + fWeight);

	vColor.a = abs(vColor.a - 1.f);
	//vColor.r += 0.2f;
	//vector pattern1 = { 1.f,0.f,0.f,0.f };
	//pattern1 *= vFX_tex;


	Out.vColor = vColor;// +pattern1;
	return Out;
}

PS_OUT  PS_DISTORTION_COLOR(PS_IN_DIST In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vFX_tex = g_SecondTexture.Sample(DiffuseSampler, In.vWeightUV);
	float4 vColor = (float4)0.f;
	float fWeight = vFX_tex.r * 0.5f;

	float4 vColorRamp = g_ColorTexture.Sample(ColorSampler, In.vColorRamp_UV );
	vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV + fWeight);
	vColor.rgb *= vColorRamp.rgb;

	//vColor.a = abs(vColor.a - 1.f);
	Out.vColor = vColor;
	return Out;
}

PS_OUT  PS_BOSS_CORE(PS_IN_DIST In)
{
	PS_OUT		Out = (PS_OUT)0;

	float4 vDiffuse = (float4)0;
	float2 vCenter = In.vColorRamp_UV - 0.5f;
	vCenter = abs(vCenter);

	float fLenght = length(vCenter);

	float2 vUV = In.vColorRamp_UV;
	vUV.y += g_fTime;
	float4 vDistortion = g_SecondTexture.Sample(DiffuseSampler, vUV);
	fLenght = fLenght / 0.5f; // normalize
	float fWeight = vDistortion.r * fLenght;

	float2 vColorUV = (float2)((fLenght * -1.f) + 1.f);
	float fAlpha = vColorUV.x;
	vColorUV += fWeight;
	if (0.f >= vColorUV.x)
	{
		vColorUV.x = 0.f;
		vColorUV.y = 0.f;
	}
	if (1.f <= vColorUV.x)
	{
		vColorUV.x = 0.99f;
		vColorUV.y = 0.99f;
	}

	float4 vColor = g_ColorTexture.Sample(DiffuseSampler, vColorUV);
//	vColor.rgb *= vDistortion.rgb;
	vColor.a = (vColor.r + vColor.g + vColor.b);// / 3.f;
	vColor.a *= fAlpha;
	Out.vColor = vColor;


	return Out;
}

technique11		DefaultTechnique
{
	pass PointInstance_Default // 0
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN();
	}

	pass PointInstance_G_COLOR // 1
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_COLOR();
	}

	pass Distortion // 2
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN_DIST();
		PixelShader = compile ps_5_0  PS_DISTORTION();
	}

	pass Distortion_Color // 3
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN_DIST();
		PixelShader = compile ps_5_0  PS_DISTORTION_COLOR();
	}

	pass DoubleTex // 4
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN_DOUBLE_TEX_GLOBAL();
		PixelShader = compile ps_5_0  PS_MAIN_DOUBLE_TEX();
	}

	pass BLENDADD_G_COLOR // 5
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_COLOR();
	}

	pass Diffuse_Sprite // 6
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN_DOUBLE_TEX_GLOBAL();
		PixelShader = compile ps_5_0  PS_MAIN_DIFF_SPRITE();
	}

	pass Default_ALPHATIME // 7
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_ALPHATIME();
	}

	pass Robot_Battery_Spark // 8
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NO_BILL();
		GeometryShader = compile gs_5_0  GS_MAIN_NO_BILL_UPSIZE();
		PixelShader = compile ps_5_0  PS_MAIN_COLOR_NOALPHATEX();
	}

	pass WallLaserTrap // 9
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_AXIS_Y();
		GeometryShader = compile gs_5_0  GS_MAIN_NO_BILL_Y();
		PixelShader = compile ps_5_0  PS_MAIN_COLOR_NOALPHATEX_TIME();
	}

	pass PointLight // 10
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_LIGHT();
	}

	pass Laser // 11
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_AXIS_Y_NO_GLOBAL_UV();
		GeometryShader = compile gs_5_0  GS_MAIN__LASER();
		PixelShader = compile ps_5_0  PS_MAIN_LASER();
	}
	pass Laser_Circle // 12
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_LASER_CIRCLE();
	}
	pass Laser_Outside // 13
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_AXIS_Y_NO_GLOBAL_UV();
		GeometryShader = compile gs_5_0  GS_MAIN__LASER();
		PixelShader = compile ps_5_0  PS_MAIN_LASER_OUTSIDE();
	}
	pass BossRoom_Core // 14
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_Default, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN_DIST_ROTATION();
		PixelShader = compile ps_5_0  PS_BOSS_CORE();
	}

	pass BossBomb_Pillar // 15
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_AXIS_Y_NO_GLOBAL_UV();
		GeometryShader = compile gs_5_0  GS_MAIN_NO_BILL_Y_ROTATE_UV();
		PixelShader = compile ps_5_0  PS_MAIN_PILLAR();
	}

	pass BossGroundPound_Smoke // 16
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_SMOKE();
	}

	pass Dead_Explosion// 17
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_EXPLOSION();
	}

	pass UFO_ElectricBox_Spark // 18
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN_NO_BILL();
		GeometryShader = compile gs_5_0  GS_MAIN_NO_BILL_UPSIZE();
		PixelShader = compile ps_5_0  PS_UFO_ELECTRICBOX();
	}

	pass UFO_Press_Smoke // 19
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0  VS_MAIN();
		GeometryShader = compile gs_5_0  GS_MAIN();
		PixelShader = compile ps_5_0  PS_MAIN_PRESS_SMOKE();
	}
};

/*
PS_OUT  PS_DISTORTION_DASH(PS_IN_DIST In)
{
PS_OUT		Out = (PS_OUT)0;

float4 vFX_tex = g_SecondTexture.Sample(DiffuseSampler, In.vWeightUV);
float4 vColor = (float4)0.f;
float fWeight = vFX_tex.r * 0.5f;

float4 vColorRamp = g_ColorTexture.Sample(ColorSampler, In.vColorRamp_UV);
vColor = g_DiffuseTexture.Sample(ColorSampler, In.vTexUV + fWeight);
vColor.rgb *= (vColorRamp.rgb);

//if (0.1f >= vFX_tex.r)
//	discard;

Out.vColor = vColor;
return Out;
}

////////////
struct  GS_OUT_AXIS
{
float4 vPosition		: SV_POSITION;
float2 vTexUV			: TEXCOORD0;
float2 vWightUV			: TEXCOORD1;
float2 vColorRamp_UV	: TEXCOORD2;
float4 vProjPosition	: TEXCOORD3;
float4 vAxisRotate		: TEXCOORD4; // 해당 축으로만 회전
uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
/*
/*
void  GS_MAIN_DIST_AXIS(point  VS_OUT In[1], inout TriangleStream<GS_OUT_DIST> TriStream)
{
GS_OUT_DIST		Out[8];

// Main View 0,0
float3		vLook = normalize(In[0].vPointAxis).xyz;

float3		vUp = normalize(g_vMainCamUp).xyz;
float3		vRight = normalize(cross(vUp, vLook)).xyz;

matrix		matVP = mul(g_MainViewMatrix, g_MainProjMatrix);;

float2		vHalfSize = float2(In[0].vSize.x * 0.5f, In[0].vSize.y * 0.5f);

float4		vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
float4		vWolrdPointPos_Y = vector(vLook, 0.f)		*	vHalfSize.y;

Out[0].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
Out[0].vPosition = mul(Out[0].vPosition, matVP);
Out[0].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
Out[0].vWightUV = float2(g_vUV.x, g_vUV.y);
Out[0].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
Out[0].vProjPosition = Out[0].vPosition;
Out[0].iViewportIndex = 1;
TriStream.Append(Out[0]);

Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
Out[1].vPosition = mul(Out[1].vPosition, matVP);
Out[1].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
Out[1].vWightUV = float2(g_vUV.z, g_vUV.y);
Out[1].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
Out[1].vProjPosition = Out[1].vPosition;
Out[1].iViewportIndex = 1;
TriStream.Append(Out[1]);
Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
Out[2].vPosition = mul(Out[2].vPosition, matVP);
Out[2].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
Out[2].vWightUV = float2(g_vUV.z, g_vUV.w);
Out[2].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
Out[2].vProjPosition = Out[2].vPosition;
Out[2].iViewportIndex = 1;
TriStream.Append(Out[2]);

TriStream.RestartStrip();
Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
Out[3].vPosition = mul(Out[3].vPosition, matVP);
Out[3].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
Out[3].vWightUV = float2(g_vUV.x, g_vUV.w);
Out[3].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
Out[3].vProjPosition = Out[3].vPosition;
Out[3].iViewportIndex = 1;
TriStream.Append(Out[0]);
TriStream.Append(Out[2]);
TriStream.Append(Out[3]);

TriStream.RestartStrip();

// Sub View 0,1

vLook = normalize(In[0].vPointAxis).xyz;
vUp = normalize(g_vMainCamUp).xyz;
vRight = normalize(cross(vUp, vLook)).xyz;
matVP = mul(g_SubViewMatrix, g_SubProjMatrix);;

vWolrdPointPos_X = vector(vRight, 0.f)	*	vHalfSize.x;
vWolrdPointPos_Y = vector(vUp, 0.f)		*	vHalfSize.y;

Out[4].vPosition = In[0].vPosition + vWolrdPointPos_X + vWolrdPointPos_Y;
Out[4].vPosition = mul(Out[4].vPosition, matVP);
Out[4].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
Out[4].vWightUV = float2(g_vUV.x, g_vUV.y);
Out[4].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.y);
Out[4].vProjPosition = Out[4].vPosition;
Out[4].iViewportIndex = 2;
TriStream.Append(Out[4]);

Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
Out[5].vPosition = mul(Out[5].vPosition, matVP);
Out[5].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
Out[5].vWightUV = float2(g_vUV.z, g_vUV.y);
Out[5].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
Out[5].vProjPosition = Out[5].vPosition;
Out[5].iViewportIndex = 2;
TriStream.Append(Out[5]);

Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
Out[6].vPosition = mul(Out[6].vPosition, matVP);
Out[6].vTexUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
Out[6].vWightUV = float2(g_vUV.z, g_vUV.w);
Out[6].vColorRamp_UV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
Out[6].vProjPosition = Out[6].vPosition;
Out[6].iViewportIndex = 2;
TriStream.Append(Out[6]);

TriStream.RestartStrip();

Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
Out[7].vPosition = mul(Out[7].vPosition, matVP);
Out[7].vTexUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
Out[7].vWightUV = float2(g_vUV.x, g_vUV.w);
Out[7].vColorRamp_UV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
Out[7].vProjPosition = Out[7].vPosition;
Out[7].iViewportIndex = 2;
TriStream.Append(Out[4]);
TriStream.Append(Out[6]);
TriStream.Append(Out[7]);
}

*/

// 중심에서 멀어 질 수록 알파를 낮추자

//float4 vDiffuse = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

//vDiffuse *= g_vColor;
//vDiffuse.a = 1.f;
//float2 vCenter = In.vTexUV - 0.5f;
//vCenter = abs(vCenter);
////vCenter -= 0.5f;
////vCenter = abs(vCenter) * 2.f;
//float fDist = (vCenter.x + vCenter.y) *0.5f;
////float4 vAlphaTex = g_SecondTexture.Sample(DiffuseSampler, In.vTexUV);
////
////vAlphaTex.r -= 1.f;
////vAlphaTex.r = abs(vAlphaTex.r);

//vDiffuse.a = 0.5f * COS_ARR(3.14f * pow(fDist, 1.5f)) + 0.5f;//vAlphaTex.r * g_fAlpha;

//Out.vColor = vDiffuse;


//float4 vDiffuse =  { 0.498039246f, 1.000000000f, 0.831372619f, 1.000000000f };
//float2 vCenter = In.vTexUV - 0.5f;
//vCenter = abs(vCenter);
////vCenter -= 0.5f;
////vCenter = abs(vCenter) * 2.f;

//float fLenght = length(vCenter);
//if (0.5f < fLenght)
//	discard;

//vDiffuse.a = 0.5f;

//fLenght = fLenght / 0.5f; // normalize
//float2 vColorUV = float2((fLenght * -1.f) + 1.f);
//float4 vColor = g_SecondTexture.Sample(DiffuseSampler, vColorUV);

//fLenght *= 100.f; // 원하는 만큼 곱하기 (길이)
//vDiffuse.rgb *= (100.f - fLenght);// *0.25f;// +0.5f;//0.5f * COS_ARR(3.14f * pow(fDist, 1.5f)) + 0.5f;//vAlphaTex.r * g_fAlpha;
//vDiffuse.rgb /= 1000.f;	// 감쇠 파워?
//Out.vColor = vDiffuse;



//float2 vCenter = In.vColorRamp_UV - 0.5f;
//vCenter = abs(vCenter);
//float fLength = length(vCenter);
//if (0.5f < fLength)
//	discard;
//if (0.4f < fLength && 0.45f > fLength)
//discard;
//if (0.28f < fLength && 0.33f > fLength)
//discard;