////////////////////////////////////////////////////////////
#include "Shader_Include.hpp"

texture2D g_DiffuseTexture;
StructuredBuffer<float> g_HDRDebugBuffer;

////////////////////////////////////////////////////////////

struct VS_IN
{
	float4 vPosition : POSITION;	
	float2 vTexUV : TEXCOORD;
};

struct VS_OUT
{
	float4 vPosition : SV_POSITION;	
	float2 vTexUV : TEXCOORD;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;
	
	Out.vPosition = In.vPosition;
	Out.vTexUV = In.vTexUV;

	return Out;
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4 vPosition : SV_POSITION;
	float2 vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector	vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(Wrap_Sampler, In.vTexUV);

	return Out;
}

PS_OUT PS_HDR(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_HDRDebugBuffer[0];

	return Out;
}
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass Default
	{		
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}	

	pass HDR
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZTest, 0);
		SetBlendState(BlendState_None, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HDR();
	}
};



