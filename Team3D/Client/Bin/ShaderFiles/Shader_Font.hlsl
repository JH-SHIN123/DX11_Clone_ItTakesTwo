
#include "Shader_Defines.hpp"

texture2D		g_DiffuseTexture;
matrix			g_UIWorldMatrix;
matrix			g_UIViewMatrix;
matrix			g_UIProjMatrix;

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
	float2 vSize : PSIZE;

	/* 인스턴스 정점버퍼 */
	float3 vInsPosition : TEXCOORD1;
	float4 vInsTexUV : TEXCOORD2;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;
	float2 vSize : PSIZE;
	float4 vTexUV : TEXCOORD0;
};


VS_OUT VS_FONT(VS_IN_FONT In)
{
	VS_OUT				Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vInsPosition, 1.f), g_WorldMatrix);
	Out.vSize = In.vSize;
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

[maxvertexcount(6)]
void  GS_MAIN(/*입력*/ point  VS_OUT In[1], /*출력*/ inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT		Out[4];

	// 어쩌피 뷰 매트릭스는 항등
	matrix matVP = mul(g_MainViewMatrix, g_MainProjMatrix);

	float2		vHalfSize = float2(0.5f, 0.5f);


	/* 좌상 */
	Out[0].vPosition = mul(In[0].vPosition, matVP);
	Out[0].vPosition.x = Out[0].vPosition.x - vHalfSize.x;
	Out[0].vPosition.y = Out[0].vPosition.y + vHalfSize.y;
	Out[0].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
	Out[0].iViewportIndex = 1;

	/* 우상 */
	Out[1].vPosition = mul(In[0].vPosition, matVP);
	Out[1].vPosition.x = Out[1].vPosition.x + vHalfSize.x;
	Out[1].vPosition.y = Out[1].vPosition.y + vHalfSize.y;
	//Out[1].vPosition = mul(In[0].vPosition, matVP);
	Out[1].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
	Out[1].iViewportIndex = 1;

	/* 우하 */
	Out[2].vPosition = mul(In[0].vPosition, matVP);
	Out[2].vPosition.x = Out[2].vPosition.x + vHalfSize.x;
	Out[2].vPosition.y = Out[2].vPosition.y - vHalfSize.y;
	//Out[2].vPosition = mul(In[0].vPosition, matVP);
	Out[2].vTexUV = float2(In[0].vTexUV.z , In[0].vTexUV.w);
	Out[2].iViewportIndex = 1;

	/* 좌하 */
	Out[3].vPosition = mul(In[0].vPosition, matVP);
	Out[3].vPosition.x = Out[3].vPosition.x - vHalfSize.x;
	Out[3].vPosition.y = Out[3].vPosition.y - vHalfSize.y;
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
	/////////////////////////////////////////////////////////////////////////////// 
	///* 좌상 */
	//Out[0].vPosition = mul(In[0].vPosition, matVP);
	//Out[0].vPosition.x = Out[0].vPosition.x -  vHalfSize.x;
	//Out[0].vPosition.y = Out[0].vPosition.y -  vHalfSize.y;
	//Out[0].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
	//Out[0].iViewportIndex = 1;
	//TriStream.Append(Out[0]);

	///* 우상 */
	//Out[1].vPosition = mul(In[0].vPosition, matVP);
	//Out[1].vPosition.x = Out[1].vPosition.x +  vHalfSize.x;
	//Out[1].vPosition.y = Out[1].vPosition.y -  vHalfSize.y;
	////Out[1].vPosition = mul(In[0].vPosition, matVP);
	//Out[1].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
	//Out[1].iViewportIndex = 1;
	//TriStream.Append(Out[1]);

	///* 우하 */
	//Out[2].vPosition = mul(In[0].vPosition, matVP);
	//Out[2].vPosition.x = Out[2].vPosition.x + vHalfSize.x;
	//Out[2].vPosition.y = Out[2].vPosition.y + vHalfSize.y;
	////Out[2].vPosition = mul(In[0].vPosition, matVP);
	//Out[2].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.w);
	//Out[2].iViewportIndex = 1;
	//TriStream.Append(Out[2]);

	//TriStream.RestartStrip();

	///* 좌하 */
	//Out[3].vPosition = mul(In[0].vPosition, matVP);
	//Out[3].vPosition.x = Out[3].vPosition.x - vHalfSize.x;
	//Out[3].vPosition.y = Out[3].vPosition.y + vHalfSize.y;
	//Out[3].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.w);
	//Out[3].iViewportIndex = 1;
	//TriStream.Append(Out[3]);
	//TriStream.Append(Out[2]);
	//TriStream.Append(Out[1]);
	///////////////////////////////////////////////////////////////////////////////
	//TriStream.RestartStrip();
	// Sub View 0,1

	// 어쩌피 뷰 매트릭스는 항등
	//matVP = g_SubProjMatrix;

	//Out[4].vPosition.x = In[0].vPosition.x - vHalfSize.x;
	//Out[4].vPosition.y = In[0].vPosition.y - vHalfSize.y;
	//Out[4].vPosition = mul(Out[4].vPosition, matVP);
	////Out[4].vPosition = mul(In[0].vPosition, matVP);
	//Out[4].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.y);
	//Out[4].iViewportIndex = 2;
	//TriStream.Append(Out[4]);

	/* 우상 */
	//Out[5].vPosition.x = In[0].vPosition.x + vHalfSize.x;
	//Out[5].vPosition.y = In[0].vPosition.y - vHalfSize.y;
	//Out[5].vPosition = mul(Out[5].vPosition, matVP);
	//Out[5].vPosition = mul(In[0].vPosition, matVP);
	//Out[5].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.y);
	//Out[5].iViewportIndex = 2;
	//TriStream.Append(Out[5]);

	///* 우하 */
	//Out[6].vPosition.x = In[0].vPosition.x + vHalfSize.x;
	//Out[6].vPosition.y = In[0].vPosition.y + vHalfSize.y;
	//Out[6].vPosition = mul(Out[6].vPosition, matVP);
	//Out[6].vPosition = mul(In[0].vPosition, matVP);
	//Out[6].vTexUV = float2(In[0].vTexUV.z, In[0].vTexUV.w);
	//Out[6].iViewportIndex = 2;
	//TriStream.Append(Out[6]);

	//TriStream.RestartStrip();

	///* 좌하 */
	//Out[7].vPosition.x = In[0].vPosition.x - vHalfSize.x;
	//Out[7].vPosition.y = In[0].vPosition.y + vHalfSize.y;
	//Out[7].vPosition = mul(Out[7].vPosition, matVP);
	//Out[7].vPosition = mul(In[0].vPosition, matVP);
	//Out[7].vTexUV = float2(In[0].vTexUV.x, In[0].vTexUV.w);
	//Out[7].iViewportIndex = 2;
	//TriStream.Append(Out[4]);
	//TriStream.Append(Out[6]);
	//TriStream.Append(Out[7]);
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
};



