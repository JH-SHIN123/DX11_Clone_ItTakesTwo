#define MAX_GROUPS 64
#define WINCX 1280
#define WINCY 720

Texture2D					g_InputTex;					// Input : HDR 텍스쳐
RWTexture2D<float4>			g_OutputDownScaleTex;				

cbuffer DownScaleDesc
{
	uint2	g_Res = { WINCX / 4 ,WINCY / 4 };									// 다운스케일 해상도 계산 : width. height & 백 버퍼의 높이와 너비를 4로 나눈 값
};

// 각 스레드에 대해 4x4 다운스케일 수행
void DownScale4x4(uint2 curPixel, uint groupThreadID)
{
	// 픽셀 결합 생략
	if (curPixel.y < g_Res.y)
	{
		// 4x4 픽셀 그룹 값 합산
		int3 nFullResPos = int3(curPixel * 4, 0);
		float4 downScaled = float4(0.0, 0.0, 0.0, 0.0);
		
		[unroll]
		for (int i = 0; i < 4; i++)
		{
			[unroll]
			for (int j = 0; j < 4; j++)
			{
				downScaled += g_InputTex.Load(nFullResPos, int2(j, i));
			}
		}
		downScaled /= 16.0; // 평균
		g_OutputDownScaleTex[curPixel.xy] = downScaled; // 블룸 : HDR Tex 다운스케일한값 따로 저장
	}
}

// 이 컴퓨트 셰이더는 X값 (Total back buffer pixels / 16*1024)에 따라 처리할 수 있다.
// witdh x height -> 1x1
[numthreads(1024,1,1)]
void CS_DOWNSCALED(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadID.x % g_Res.x, dispatchThreadID.x / g_Res.x);

	// 16 픽셀 그룹을 하나의 픽셀로 줄여, 공유메모리에 저장
	DownScale4x4(CurPixel, groupThreadID.x);

}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* For. Blur */
static const float g_SampleWeights[13] = {
	0.002216,
	0.008764,
	0.026995,
	0.064759,
	0.120985,
	0.176033,
	0.199471,
	0.176033,
	0.120985,
	0.064759,
	0.026995,
	0.008764,
	0.002216,
};

#define KERNERLHALF 6
#define GROUPTHREADS 128

Texture2D <float4 > g_Input;
RWTexture2D <float4 > g_Output;
groupshared float4 g_SharedInput[GROUPTHREADS];

[numthreads(GROUPTHREADS, 1, 1)]
void CS_HORIZONTAL_FILTER(uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	int2 coord = int2(GI - KERNERLHALF + (GROUPTHREADS - KERNERLHALF * 2) * Gid.x, Gid.y);
	coord = clamp(coord, int2(0, 0), int2(g_Res.x - 1, g_Res.y - 1));
	g_SharedInput[GI] = g_Input.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	// 수평방향 블러
	if (GI >= KERNERLHALF && GI < (GROUPTHREADS - KERNERLHALF) &&
		((Gid.x * (GROUPTHREADS - 2 * KERNERLHALF) + GI - KERNERLHALF) < g_Res.x))
	{
		float4 vOut = 0;

		[unroll]
		for (int i = -KERNERLHALF; i <= KERNERLHALF; ++i)
		{
			vOut += g_SharedInput[GI + i] * g_SampleWeights[i + KERNERLHALF];
		}

		g_Output[coord] = float4(vOut.rgb, 1.f);
	}
}

[numthreads(GROUPTHREADS, 1, 1)]
void CS_VERTICAL_FILTER(uint3 Gid : SV_GroupID, uint GI : SV_GroupIndex)
{
	uint2 coord = int2(Gid.x, GI - KERNERLHALF + (GROUPTHREADS - KERNERLHALF * 2) * Gid.y);
	coord = clamp(coord, int2(0, 0), int2(g_Res.x - 1, g_Res.y - 1));
	g_SharedInput[GI] = g_Input.Load(int3(coord, 0));

	GroupMemoryBarrierWithGroupSync();

	// 수직방향 블러
	if (GI >= KERNERLHALF && GI < (GROUPTHREADS - KERNERLHALF) &&
		((GI - KERNERLHALF + (GROUPTHREADS - KERNERLHALF * 2) * Gid.y) < g_Res.y))
	{
		float4 vOut = 0;

		[unroll]
		for (int i = -KERNERLHALF; i <= KERNERLHALF; ++i)
		{
			vOut += g_SharedInput[GI + i] * g_SampleWeights[i + KERNERLHALF];
		}

		g_Output[coord] = float4(vOut.rgb, 1.f);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
technique11		DefaultTechnique
{
	pass DownScaled // 0
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALED();
	}
	
	pass HorizontalFilter // 1
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_HORIZONTAL_FILTER();
	}

	pass VerticalFilter // 2
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_VERTICAL_FILTER();
	}
};



