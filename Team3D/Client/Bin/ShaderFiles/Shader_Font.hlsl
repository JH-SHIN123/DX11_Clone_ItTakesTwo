
#include "Shader_Defines.hpp"

texture2D		g_DiffuseTexture;

matrix			g_UIWorldMatrix;
matrix			g_UIViewMatrix;
matrix			g_UIProjMatrix;

float2			g_MainViewPort;
float2			g_SubViewPort;
float2			g_DefaultViewPort;
int				g_iGSOption;

float			g_fAlpha;

sampler DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN_FONT
{
	/* 본래 정점버퍼 */
	float3 vPosition : POSITION;

	/* 인스턴스 정점버퍼 */
	float3 vInsPosition : TEXCOORD1;
	float2 vScale : TEXCOORD2;
	float4 vInsTexUV : TEXCOORD3;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vScale : TEXCOORD0;
	float4 vTexUV : TEXCOORD1;
};


VS_OUT VS_FONT(VS_IN_FONT In)
{
	VS_OUT				Out = (VS_OUT)0;

	Out.vPosition = vector(In.vInsPosition, 1.f);
	Out.vScale = In.vScale;
	Out.vTexUV = In.vInsTexUV;

	return Out;
}

struct  GS_OUT
{
	float4 vPosition		: SV_POSITION;
	float2 vTexUV			: TEXCOORD0;
	uint   iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

/////////////////////////////////////////////////////////////////////////

[maxvertexcount(12)]
void  GS_MAIN(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[8];

	// 어쩌피 뷰 매트릭스는 항등
	matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

	float2	vSize = float2(In[0].vScale.x / g_MainViewPort.x, In[0].vScale.y / g_MainViewPort.y);

	if (0 == g_iGSOption)
	{
		/* 좌상 */
		Out[0].vPosition = mul(In[0].vPosition, matVP);
		Out[0].vPosition.x = Out[0].vPosition.x - vSize.x;
		Out[0].vPosition.y = Out[0].vPosition.y + vSize.y;
		Out[0].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
		Out[0].iViewportIndex = 1;

		/* 우상 */
		Out[1].vPosition = mul(In[0].vPosition, matVP);
		Out[1].vPosition.x = Out[1].vPosition.x + vSize.x;
		Out[1].vPosition.y = Out[1].vPosition.y + vSize.y;
		Out[1].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
		Out[1].iViewportIndex = 1;

		/* 우하 */
		Out[2].vPosition = mul(In[0].vPosition, matVP);
		Out[2].vPosition.x = Out[2].vPosition.x + vSize.x;
		Out[2].vPosition.y = Out[2].vPosition.y - vSize.y;
		Out[2].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.w);
		Out[2].iViewportIndex = 1;

		/* 좌하 */
		Out[3].vPosition = mul(In[0].vPosition, matVP);
		Out[3].vPosition.x = Out[3].vPosition.x - vSize.x;
		Out[3].vPosition.y = Out[3].vPosition.y - vSize.y;
		Out[3].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.w);
		Out[3].iViewportIndex = 1;

		TriStream.Append(Out[1]);
		TriStream.Append(Out[0]);
		TriStream.Append(Out[3]);

		TriStream.RestartStrip();

		TriStream.Append(Out[3]);
		TriStream.Append(Out[2]);
		TriStream.Append(Out[1]);

		TriStream.RestartStrip();
	}
	else
	{
		// 어쩌피 뷰 매트릭스는 항등
		matVP = g_SubProjMatrix;

		vSize = float2(In[0].vScale.x / g_SubViewPort.x, In[0].vScale.y / g_SubViewPort.y);

		/* 좌상 */
		Out[4].vPosition = mul(In[0].vPosition, matVP);
		Out[4].vPosition.x = Out[4].vPosition.x - vSize.x;
		Out[4].vPosition.y = Out[4].vPosition.y + vSize.y;
		Out[4].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
		Out[4].iViewportIndex = 2;

		/* 우상 */
		Out[5].vPosition = mul(In[0].vPosition, matVP);
		Out[5].vPosition.x = Out[5].vPosition.x + vSize.x;
		Out[5].vPosition.y = Out[5].vPosition.y + vSize.y;
		Out[5].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
		Out[5].iViewportIndex = 2;

		/* 우하 */
		Out[6].vPosition = mul(In[0].vPosition, matVP);
		Out[6].vPosition.x = Out[6].vPosition.x + vSize.x;
		Out[6].vPosition.y = Out[6].vPosition.y - vSize.y;
		Out[6].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.w);
		Out[6].iViewportIndex = 2;

		/* 좌하 */
		Out[7].vPosition = mul(In[0].vPosition, matVP);
		Out[7].vPosition.x = Out[7].vPosition.x - vSize.x;
		Out[7].vPosition.y = Out[7].vPosition.y - vSize.y;
		Out[7].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.w);
		Out[7].iViewportIndex = 2;

		TriStream.Append(Out[5]);
		TriStream.Append(Out[4]);
		TriStream.Append(Out[7]);

		TriStream.RestartStrip();

		TriStream.Append(Out[7]);
		TriStream.Append(Out[6]);
		TriStream.Append(Out[5]);

		TriStream.RestartStrip();
	}
}

////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////

[maxvertexcount(6)]
void  GS_Default(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[4];

	// 어쩌피 뷰 매트릭스는 항등
	matrix matVP = mul(g_UIViewMatrix, g_UIProjMatrix);
	float2	vSize = float2(In[0].vScale.x / g_DefaultViewPort.x - 0.0075f, In[0].vScale.y / g_DefaultViewPort.y - 0.0075f);

	/* 좌상 */
	Out[0].vPosition = mul(In[0].vPosition, matVP);
	Out[0].vPosition.x = Out[0].vPosition.x - vSize.x;
	Out[0].vPosition.y = Out[0].vPosition.y + vSize.y;
	Out[0].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
	Out[0].iViewportIndex = 0;

	/* 우상 */
	Out[1].vPosition = mul(In[0].vPosition, matVP);
	Out[1].vPosition.x = Out[1].vPosition.x + vSize.x;
	Out[1].vPosition.y = Out[1].vPosition.y + vSize.y;
	Out[1].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
	Out[1].iViewportIndex = 0;

	/* 우하 */
	Out[2].vPosition = mul(In[0].vPosition, matVP);
	Out[2].vPosition.x = Out[2].vPosition.x + vSize.x;
	Out[2].vPosition.y = Out[2].vPosition.y - vSize.y;
	Out[2].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.w);
	Out[2].iViewportIndex = 0;

	/* 좌하 */
	Out[3].vPosition = mul(In[0].vPosition, matVP);
	Out[3].vPosition.x = Out[3].vPosition.x - vSize.x;
	Out[3].vPosition.y = Out[3].vPosition.y - vSize.y;
	Out[3].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.w);
	Out[3].iViewportIndex = 0;

	TriStream.Append(Out[1]);
	TriStream.Append(Out[0]);
	TriStream.Append(Out[3]);

	TriStream.RestartStrip();

	TriStream.Append(Out[3]);
	TriStream.Append(Out[2]);
	TriStream.Append(Out[1]);

	TriStream.RestartStrip();
	
}

////////////////////////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD0;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET;
};

/* 픽셀의 최종적인 색을 결정하낟. */
PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	return Out;
}

technique11		DefaultTechnique
{
	pass Font
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FONT();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	pass FontDefalut
	{
		SetRasterizerState(Rasterizer_NoCull);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_FONT();
		GeometryShader = compile gs_5_0 GS_Default();
		PixelShader = compile ps_5_0 PS_MAIN();
	}
};



