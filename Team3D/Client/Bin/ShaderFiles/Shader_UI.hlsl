#include "Shader_Defines.hpp"

////////////////////////////////////////////////////////////

texture2D	g_DiffuseTexture;
texture2D	g_DiffuseSubTexture;
texture2D	g_SubTexture;
texture2D   g_DiffuseNoiseTexture;
texture2D   g_DiffuseMaskTexture;
texture2D   g_DiffuseAlphaTexture;

matrix		g_UIWorldMatrix;
matrix		g_UIViewMatrix;
matrix		g_UIProjMatrix;

int		g_iShaderMouseOption;
int		g_iColorOption;
int		g_iGSOption;
int		g_iRespawnOption;
int		g_iHeaderBoxOption;
int		g_iAlphaOption;

float	g_fAlpha;
float	g_Time;
float	g_Angle;
float	g_fHeartTime;
float	g_fScreenAlpha;
float	g_fDistance;
float	g_fRatio;
float	g_fDecreaseRateRatio;
float2  g_UV;
float2  g_vScreenMaskUV;
 
sampler	DiffuseSampler = sampler_state
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

////////////////////////////////////////////////////////////

struct VS_IN
{
	float3	vPosition	: POSITION;
	float2	vTexUV		: TEXCOORD0;
};

struct VS_OUT
{           
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float2  vSubUV		: TEXCOORD1;
};

VS_OUT	VS_MAIN(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT	VS_RespawnCirle(VS_IN In)
{
	VS_OUT	Out = (VS_OUT)0;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
	Out.vTexUV = In.vTexUV;

	float2 SubUV = In.vTexUV;
	SubUV.x -= 0.5f;
	SubUV.y -= 0.5f;

	float fCos, fSin;

	fCos = cos(g_Angle);
	fSin = sin(g_Angle);

	float2x2 dd;

	dd = float2x2(fCos, -fSin, fSin, fCos);
	SubUV = mul(SubUV, dd);
	SubUV.x += 0.5f;
	SubUV.y += 0.5f;

	Out.vSubUV = SubUV;

	return Out;
}

VS_OUT	VS_LOGO(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix	matWV, matWVP;

	matWV = mul(g_UIWorldMatrix, g_UIViewMatrix);
	matWVP = mul(matWV, g_UIProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct GS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float2  vSubUV		: TEXCOORD1;
};

struct GS_OUT
{
	float4	vPosition		: SV_POSITION;
	float2	vTexUV			: TEXCOORD0;
	float2  vSubUV			: TEXCOORD1;
	uint	iViewportIndex	: SV_VIEWPORTARRAYINDEX;
};

[maxvertexcount(6)]
void GS_MAIN(triangle GS_IN In[3], inout TriangleStream<GS_OUT> TriStream)
{
	GS_OUT Out = (GS_OUT)0;

	if (0 == g_iGSOption)
	{

		for (uint i = 0; i < 3; i++)
		{
			matrix matVP = g_MainProjMatrix;

			Out.vPosition = mul(In[i].vPosition, matVP);
			Out.vTexUV = In[i].vTexUV;
			Out.vSubUV = In[i].vSubUV;
			Out.iViewportIndex = 1;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
	else
	{

		for (uint j = 0; j < 3; j++)
		{
			matrix matVP = g_SubProjMatrix;

			Out.vPosition = mul(In[j].vPosition, matVP);
			Out.vTexUV = In[j].vTexUV;
			Out.vSubUV = In[j].vSubUV;
			Out.iViewportIndex = 2;

			TriStream.Append(Out);
		}

		TriStream.RestartStrip();
	}
}

////////////////////////////////////////////////////////////

struct PS_IN
{
	float4	vPosition	: SV_POSITION;
	float2	vTexUV		: TEXCOORD0;
	float2  vSubUV		: TEXCOORD1;
};

struct PS_OUT
{
	float4	vColor		: SV_TARGET0;
};

PS_OUT	PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	return Out;
}

PS_OUT PS_Frame(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.b = 0.f;

	return Out;

}

PS_OUT PS_PC_Mouse(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	float4 vSubColor = g_SubTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.b = 0.f;
	
	return Out;
}

PS_OUT PS_Fill(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.rg = 1.f;


	return Out;
}

PS_OUT PS_PlayerMarker(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (Out.vColor.a < 0.35f)
		discard;
		
	float3 Green = float3(0.74f, 0.99f, 0.1f);
	float3 SkyBlue = float3(0.31f, 0.73f, 0.87f);

	if (0 == g_iColorOption && Out.vColor.a > 0.8f)
		Out.vColor.rgb = Green;
	else if (1 == g_iColorOption && Out.vColor.a > 0.8f)
		Out.vColor.rgb = SkyBlue;

	Out.vColor.a = g_fAlpha;

	return Out;
}

PS_OUT PS_RespawnCircle(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vector SubColor = g_DiffuseSubTexture.Sample(DiffuseSampler, In.vTexUV);

	//In.vTexUV += g_UV;
	vector Noise = g_DiffuseNoiseTexture.Sample(DiffuseSampler, In.vSubUV);


	if (0.f >= Out.vColor.r && 0.f >= Out.vColor.b)
		discard;

	float fColor = g_Time - SubColor.a;

	// 이거는 rgb에 r채널만 줘서 게이지를 띄위기 위함
	Out.vColor.rgb = Out.vColor.r/* + Out.vColor.b*/;
	// 여기서 이제 r채널만 띄웠으니까 알파 값을 조절해주자
	//Out.vColor.a = Out.vColor.r * 0.25f /* + Out.vColor.b + Out.vColor.a*/;
	Out.vColor.a = Out.vColor.r * 0.25f /* + Out.vColor.b + Out.vColor.a*/;

	if (Out.vColor.r <= 0.411f != Out.vColor.r >= 0.42f)
	{
		if (fColor <= 0.015f && Out.vColor.r == 1.f && Out.vColor.g == 1.f)
		{
			// 노란색 꼬랑지에만 노이즈
			Out.vColor.rgb *= Out.vColor.r * 50.f * Noise.r;
			// 게이지에 노이즈
			//Out.vColor.rgb *= Noise.rgb;
			//Out.vColor.r = Noise.a;
			//Out.vColor.a = 1.f;
			//Out.vColor.ra = 1.f;
			//Out.vColor.a = Noise.r;
		}

		if (SubColor.a <= g_Time)
		{
			Out.vColor.rgb *= float3(1.f, 0.09882f, 0.f) * 2.f * Noise.r;
			//Out.vColor.r = Noise.r;
			Out.vColor.a = Out.vColor.r;

		}
	}
	else
		Out.vColor.a = 0.8f;

	return Out;
}


PS_OUT PS_RespawnCircleHeart(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vector SubColor = g_DiffuseSubTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.f >= Out.vColor.a && 0.f >= Out.vColor.b)
		discard;

	float fHeartTime = abs(sin(g_fHeartTime));

	if (Out.vColor.b <= fHeartTime)
	{
		if (0 == g_iRespawnOption)
		{
			Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 3.f;
			Out.vColor *= vector(10.0f, 1.0f, 0.1f, 1.f);
		}
		else
		{
			Out.vColor.rgb = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 0.5f;
			Out.vColor *= vector(1000.0f, 1.9f, 0.0f, 1.f);
		}
	}
	else
		discard;

	return Out;
}

PS_OUT PS_AlphaScreen(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (1 == g_iAlphaOption)
		Out.vColor.a = 0.6f;
	else
		Out.vColor.a = 0.5f;

	return Out;
}

PS_OUT PS_SplashScreen(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.999f >= Out.vColor.a)
		discard;

	//if (Out.vColor.r >= 0.8f)
	//	In.vTexUV.x += g_vScreenMaskUV.x;
	//else if (Out.vColor.g >= 0.8f)
	//	In.vTexUV.y -= g_vScreenMaskUV.y;
	//else if (Out.vColor.b >= 0.8f)
	//	In.vTexUV.x -= g_vScreenMaskUV.x;

	//vector Mask = g_DiffuseMaskTexture.Sample(DiffuseSampler, In.vTexUV);

	Out.vColor.a = g_fScreenAlpha;

	//Out.vColor.rgb += Mask;

	return Out;
}


PS_OUT PS_SplashScreenMask(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	vector Screen = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	
	float2 vWeight = { g_vScreenMaskUV.x, g_vScreenMaskUV.y };

	if (0 == g_iColorOption)
	{
		In.vTexUV.x += vWeight.x;
		In.vTexUV.y += vWeight.y;
	}
	else if (1 == g_iColorOption)
	{
		In.vTexUV.x -= vWeight.x;
		In.vTexUV.y += vWeight.y;
	}
	else
	{
		In.vTexUV.x -= vWeight.x;
		In.vTexUV.y -= vWeight.y;
	}

	vector vColor = g_DiffuseMaskTexture.Sample(DiffuseSampler, In.vTexUV);

	if (vColor.r <= 0.1f && vColor.g <= 0.1f && vColor.b <= 0.1f)
		discard;

	if (Screen.a == 0.f)
		discard;

	if (vColor.r > 0.1f && 0 == g_iColorOption)
	{
		//Out.vColor.rgba = 1.f;
		Out.vColor.rgba = vector(1.f, 1.f, 0.1f, 1.f);
		//Out.vColor.rgb = 1.f;
		//Out.vColor.a = 0.7f;
	}
	else if (vColor.g > 0.1f && 1 == g_iColorOption)
	{
		//Out.vColor.rgba = 1.f;
		Out.vColor.rgba = vector(1.f, 1.f, 0.1f, 1.f);
		//Out.vColor.rgb = 1.f;
		//Out.vColor.a = 0.7f;
	}
	else if (vColor.b > 0.1f && 2 == g_iColorOption)
	{
		//Out.vColor.rgba = 1.f;
		Out.vColor.rgba = vector(1.f, 1.f, 0.1f, 1.f);
		//Out.vColor.rgb = 1.f;
		//Out.vColor.a = 0.7f;
	}
	return Out;
}

PS_OUT PS_HeaderBox(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	switch (g_iHeaderBoxOption)
	{
	case 0:
		break;
	case 1:
		Out.vColor.rgb = float3(0.41f, 0.f, 0.f);
		break;
	case 2:
		Out.vColor.rgb = float3(0.99f, 0.878f, 0.815f);
		break;
	case 3:
		Out.vColor.rgb = float3(1.f, 0.83f, 0.25f);
		break;
	}
	
	return Out;
}

PS_OUT PS_ChapterSelect(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	vector AlphaTex = g_DiffuseAlphaTexture.Sample(DiffuseSampler, In.vTexUV);

	if (AlphaTex.r < 1.f)
		Out.vColor.a = AlphaTex.r;

	return Out;
}


PS_OUT PS_Gauge_Cicle(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (Out.vColor.r <= g_fDistance && Out.vColor.r > 0.f)
	{
		Out.vColor.rgb = float3(1.f, 1.f, 0.f);
		Out.vColor.a = 0.5f;
	}
	else if (Out.vColor.r > g_fDistance)
		discard; 

	if (Out.vColor.a <= 0.1f)
		discard;

	Out.vColor.a = g_fAlpha;

	return Out;
}


PS_OUT PS_WhiteScreenFadeInOut(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.rgb = 1.f;
	Out.vColor.a = g_fAlpha;
	return Out;
}

PS_OUT PS_BlackScreenOut(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);
	Out.vColor.a = g_fAlpha;

	return Out;
}

PS_OUT PS_ContextIcon(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.1f >= Out.vColor.a)
		discard;

	Out.vColor.a = g_fAlpha;

	return Out;
}

PS_OUT PS_BossHPBarFrame(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.f == Out.vColor.r && 0.f == Out.vColor.g && 0.f == Out.vColor.b)
		discard;

	if (Out.vColor.g >= 0.2f)
	{
		Out.vColor.rgb = 0.f;
		Out.vColor.a = 0.5f;
	}
	else
	{
		Out.vColor.rgb = 0.f;
	}

	return Out;
}

PS_OUT PS_BossHPBar(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0;

	Out.vColor = g_DiffuseTexture.Sample(DiffuseSampler, In.vTexUV);

	if (0.f == Out.vColor.r && 0.f == Out.vColor.g && 0.f == Out.vColor.b)
		discard;


	if (Out.vColor.g >= 0.2f)
	{
		Out.vColor.r = 0.9f;
		Out.vColor.g = 0.14f;
		Out.vColor.b = 0.015f;
	}
	else
		discard;

	if (g_fRatio <= In.vTexUV.x)
		Out.vColor.rgb = 1.f;

	if (0.664f <= In.vTexUV.x && 0.668f >= In.vTexUV.x)
		Out.vColor.rgb = 0.f;

	if (0.331f <= In.vTexUV.x && 0.335f >= In.vTexUV.x)
		Out.vColor.rgb = 0.f;

	if (g_fDecreaseRateRatio <= In.vTexUV.x && Out.vColor.a != 0.f)
		Out.vColor.a = 0.f;


	return Out;
}

////////////////////////////////////////////////////////////

technique11 DefaultTechnique
{
	// 0
	pass Default
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader	= compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader		= compile ps_5_0 PS_MAIN();
	}

	// 1
	pass Frame
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_Frame();
	}

	// 2
	pass Mouse
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PC_Mouse();
	}
	
	// 3
	pass Fill
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_Fill();
	}

	// 4
	pass PlayerMarker
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.5f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_PlayerMarker();
	}

	// 5
	pass RespawnCircle
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_RespawnCirle();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_RespawnCircle();
	}

	// 6
	pass RespawnCircleHeart
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_RespawnCircleHeart();
	}

	// 7
	pass AlphaScreen
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_AlphaScreen();
	}

	// 8
	pass SplashScreenBack
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 9
	pass SplashScreen
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SplashScreen();
	}

	// 10
	pass SplashScreenMask
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_SplashScreenMask();
	}

	// 11
	pass Menu
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

	// 12
	pass AlphaScreenMenu
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_AlphaScreen();
	}

	// 13
	pass HeaderBox
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_HeaderBox();
	}

	// 14
	pass ChapterSelect
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_ChapterSelect();
	}

	// 15
	pass GaugeCircle
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_Gauge_Cicle();
	}

	// 16
	pass WhiteScreenFadeinout
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_WhiteScreenFadeInOut();
	}

	// 17
	pass BlackScreenFadeout
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_BlackScreenOut();
	}

	// 18
	pass ContextIcon
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = compile gs_5_0 GS_MAIN();
		PixelShader = compile ps_5_0 PS_ContextIcon();
	}

	// 19
	pass BossHpBarFrame
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BossHPBarFrame();
	}

	// 20
	pass BossHpBar
	{
		SetRasterizerState(Rasterizer_Solid);
		SetDepthStencilState(DepthStecil_No_ZWrite, 0);
		SetBlendState(BlendState_Alpha, vector(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_LOGO();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_BossHPBar();
	}
};