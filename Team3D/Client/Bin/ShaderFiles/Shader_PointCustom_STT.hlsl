#include "Shader_Defines.hpp"
#include "Shader_Defines_Effect.hpp"

texture2D		g_DiffuseTexture;
texture2D		g_SecondTexture;
texture2D		g_ColorTexture;

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

	Out.vPosition = mul(vector(In.vPosition, 1.f), In.WorldMatrix);
	Out.vTextureUV_LTRB = In.vTextureUV_LTRB;
	Out.vSize = In.vSize;
	Out.fTime = In.fTime;

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
}