
#include "Shader_Defines.hpp"

texture2D		g_DiffuseTexture;
texture2D		g_SecondTexture;
texture2D		g_ColorTexture;

vector			g_vMainCamUp;
vector			g_vSubCamUp;

float2			g_vSize;
float4			g_vColor;
float			g_fTime;
float4			g_vUV;
float4			g_vColorRamp_UV;
int				g_IsBillBoard;
int				g_RotateAxis; // x0, y1, z2

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
////////////
struct  GS_OUT_DOUBLE_TEX
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0; // 전역변수 0011
	float2 vColorUV			: TEXCOORD1; // 버퍼 하나가 각개적으로 들고있는 UV
	float4 vProjPosition	: TEXCOORD2;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};
[maxvertexcount(12)]
void  GS_MAIN_DOUBLE_TEX(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DOUBLE_TEX> TriStream)
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
	Out[0].vColorUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[0].vProjPosition = Out[0].vPosition;
	Out[0].iViewportIndex = 1;
	TriStream.Append(Out[0]);

	/* 우상 */
	Out[1].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[1].vPosition = mul(Out[1].vPosition, matVP);
	Out[1].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[1].vColorUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[1].vProjPosition = Out[1].vPosition;
	Out[1].iViewportIndex = 1;
	TriStream.Append(Out[1]);

	/* 우하 */
	Out[2].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[2].vPosition = mul(Out[2].vPosition, matVP);
	Out[2].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[2].vColorUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[2].vProjPosition = Out[2].vPosition;
	Out[2].iViewportIndex = 1;
	TriStream.Append(Out[2]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[3].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[3].vPosition = mul(Out[3].vPosition, matVP);
	Out[3].vTexUV  = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[3].vColorUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
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
	Out[4].vColorUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.y);
	Out[4].vProjPosition = Out[4].vPosition;
	Out[4].iViewportIndex = 2;
	TriStream.Append(Out[4]);

	/* 우상 */
	Out[5].vPosition = In[0].vPosition - vWolrdPointPos_X + vWolrdPointPos_Y;
	Out[5].vPosition = mul(Out[5].vPosition, matVP);
	Out[5].vTexUV = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.y);
	Out[5].vColorUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.y);
	Out[5].vProjPosition = Out[5].vPosition;
	Out[5].iViewportIndex = 2;
	TriStream.Append(Out[5]);

	/* 우하 */
	Out[6].vPosition = In[0].vPosition - vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[6].vPosition = mul(Out[6].vPosition, matVP);
	Out[6].vTexUV  = float2(g_vColorRamp_UV.z, g_vColorRamp_UV.w);
	Out[6].vColorUV = float2(In[0].vTextureUV_LTRB.z, In[0].vTextureUV_LTRB.w);
	Out[6].vProjPosition = Out[6].vPosition;
	Out[6].iViewportIndex = 2;
	TriStream.Append(Out[6]);

	TriStream.RestartStrip();

	/* 좌하 */
	Out[7].vPosition = In[0].vPosition + vWolrdPointPos_X - vWolrdPointPos_Y;
	Out[7].vPosition = mul(Out[7].vPosition, matVP);
	Out[7].vTexUV = float2(g_vColorRamp_UV.x, g_vColorRamp_UV.w);
	Out[7].vColorUV = float2(In[0].vTextureUV_LTRB.x, In[0].vTextureUV_LTRB.w);
	Out[7].vProjPosition = Out[7].vPosition;
	Out[7].iViewportIndex = 2;
	TriStream.Append(Out[4]);
	TriStream.Append(Out[6]);
	TriStream.Append(Out[7]);
}
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
void  GS_MAIN_DIST_AXIS(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT_DIST> TriStream)
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

	if (0.01f >= Out.vColor.a)
		discard;

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
	//Out.vColor.a = 1.f;
	float fColor = Out.vColor.r + Out.vColor.g + Out.vColor.b;

	if (0.01f >= fColor)
		discard;


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
		GeometryShader = compile gs_5_0  GS_MAIN_DOUBLE_TEX();
		PixelShader = compile ps_5_0  PS_MAIN_DOUBLE_TEX();
	}
	//pass Distortion_Dash //4
	//{
	//	SetRasterizerState(Rasterizer_NoCull);
	//	SetDepthStencilState(DepthStecil_No_ZWrite, 0);
	//	SetBlendState(BlendState_Add, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
	//	VertexShader = compile vs_5_0  VS_MAIN();
	//	GeometryShader = compile gs_5_0  GS_MAIN_DIST_AXIS();
	//	PixelShader = compile ps_5_0  PS_DISTORTION_DASH();
	//	// size 10
	//	// 그라디언트 2
	//	// 리본 노이즈 0
	//}
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
*/