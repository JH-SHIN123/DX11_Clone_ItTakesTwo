#define MAX_GROUPS 64

groupshared float g_SharedPositions[1024];
groupshared float g_SharedAvgFinal[MAX_GROUPS];

Texture2D g_HDRTex;								// Input : HDR 텍스쳐
RWStructuredBuffer<float>	g_AverageLum;		// Output : UAV
StructuredBuffer<float>		g_AverageValues1D;	// Output : SRV (픽셀셰이더에서 사용할 리소스)


// 컴퓨트 셰이더가 실행되는 동안, 우리는 쓰레드 그룹의 공유 메모리에 즉시 저장할거다.
// 결과 : groupshared float SharedPositions[1024];

static const float4 LUM_FACTOR = float4(0.299, 0.587, 0.114, 0);

cbuffer DownScaleDesc
{
	uint2 g_Res = { 1280 / 4 ,720 / 4 };		// 다운스케일 해상도 계산 : width. height
						// 백 버퍼의 높이와 너비를 4로 나눈 값
	
	uint g_Domain = 1280 * 720 / 16;		// 다운스케일 이미지의 총 픽셀 수
						// 백 버퍼의 높이와 너비를 곱한 후 16으로 나눈 값

	uint g_GroupSize = 1280 * 720 / 16 * 1024;	// 첫 패스에 적용된 그룹 수 계산
						// 백 버퍼의 높이와 너비를 곱한 후 16으로 나눈 다음 1024를 곱한값
};

// 각 스레드에 대해 4x4 다운스케일 수행
float4 DownScale4x4(uint2 curPixel, uint groupThreadID)
{
	float avgLum = 0.0;
	
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
				downScaled += g_HDRTex.Load(nFullResPos, int2(j, i));
			}
		}
		downScaled /= 16.0;

		// 픽셀별 휘도 값 계산
		avgLum = dot(downScaled, LUM_FACTOR);

		// 공유 메모리에 결과 기록
		g_SharedPositions[groupThreadID] = avgLum;
	}

	// 동기화 후 다음 단계로
	GroupMemoryBarrierWithGroupSync();

	return avgLum;
}

float DownScale1024to4(uint dispatchThreadID, uint groupThreadID, float avgLum)
{
	// 다운스케일 코드를 확장
	[unroll]
	for (uint groupSize = 4, step1 = 1, step2 = 2, step3 = 3;
		groupSize < 1024;
		groupSize *= 4, step1 *= 4, step2 *= 4, step3 *= 4)
	{
		// 픽셀 결합 계산 생략
		if (groupThreadID % g_GroupSize == 0)
		{
			// 휘도 값 합산
			float stepAvgLum = avgLum;
			stepAvgLum += dispatchThreadID + step1 < g_Domain ?
				g_SharedPositions[groupThreadID+step1] : avgLum;
			stepAvgLum += dispatchThreadID + step2 < g_Domain ?
				g_SharedPositions[groupThreadID + step2] : avgLum;
			stepAvgLum += dispatchThreadID + step3 < g_Domain ?
				g_SharedPositions[groupThreadID + step3] : avgLum;

			// 결과 값 저장
			avgLum = stepAvgLum;
			g_SharedPositions[groupThreadID] = stepAvgLum;
		}

		// 동기화 후 다음 단계로
		GroupMemoryBarrierWithGroupSync();
	}

	return avgLum;
}

// 네 개의 값을 하나의 평균값으로 다운스케일 한후 저장
void DownScale4to1(uint dispatchThreadID, uint groupThreadID, uint groupID, float avgLum)
{
	if (groupThreadID == 0)
	{
		// 스레드 그룹에 대한 평균 휘도 값 계산
		float fFinalAvgLum = avgLum;
		fFinalAvgLum += dispatchThreadID + 256 < g_Domain ?
			g_SharedPositions[groupThreadID + 256] : avgLum;
		fFinalAvgLum += dispatchThreadID + 512 < g_Domain ?
			g_SharedPositions[groupThreadID + 512] : avgLum;
		fFinalAvgLum += dispatchThreadID + 768 < g_Domain ?
			g_SharedPositions[groupThreadID + 768] : avgLum;
		fFinalAvgLum /= 1024.0;

		// 최종 값을 1D UAV에 기록 후, 다음 과정으로
		g_AverageLum[groupID] = fFinalAvgLum;
	}
}

// 이 컴퓨트 셰이더는 X값 (Total back buffer pixels / 16*1024)에 따라 처리할 수 있다.
[numthreads(1024,1,1)]
void CS_DOWNSCALE_FIRSTPASS(uint3 groupID : SV_GroupID, uint3 dispatchThreadID : SV_DispatchThreadID, uint3 groupThreadID : SV_GroupThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadID.x % g_Res.x, dispatchThreadID.x / g_Res.x);

	// 16 픽셀 그룹을 하나의 픽셀로 줄여, 공유메모리에 저장
	float avgLum = DownScale4x4(CurPixel, groupThreadID.x);

	// 1024에서 4로 다운스케일
	avgLum = DownScale1024to4(dispatchThreadID.x, groupThreadID.x, avgLum);

	// 4에서 1로 다운스케일
	DownScale4to1(dispatchThreadID.x, groupThreadID.x, groupID.x, avgLum);
}

// 첫 번째 컴퓨트 셰이더의 실행이 완료되면 동일한 상수버퍼를 사용한 두번째 컴퓨트 셰이더를 실행하되
// 중간 값 휘도 SRV와 평균휘도 UAV값을 지정해 사용한다.
[numthreads(MAX_GROUPS, 1, 1)]
void CS_DOWNSCALE_SECONDPASS(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
	// 공유 메모리에 1D 값 저장
	float avgLum = 0.0;
	if (dispatchThreadID.x < g_GroupSize)
	{
		avgLum = g_AverageValues1D[dispatchThreadID.x]; /*first pass의 중간휘도값 텍스쳐*/
	}
	g_SharedAvgFinal[dispatchThreadID.x] = avgLum;

	// 동기화 후 다음 과정으로
	GroupMemoryBarrierWithGroupSync();

	// 64에서 16으로 다운스케일
	if (dispatchThreadID.x % 4 == 0)
	{
		// 휘도 값 합산
		float stepAvgLum = avgLum;
		stepAvgLum += dispatchThreadID.x + 1 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 1] : avgLum;
		stepAvgLum += dispatchThreadID.x + 2 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 2] : avgLum;
		stepAvgLum += dispatchThreadID.x + 3 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 3] : avgLum;

		// 결과 값 저장
		avgLum = stepAvgLum;
		g_SharedAvgFinal[dispatchThreadID.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync(); // 동기화 후 다음 과정으로

	// 16에서 4로 다운스케일
	if (dispatchThreadID.x % 16 == 0)
	{
		// 휘도 값 합산
		float stepAvgLum = avgLum;
		stepAvgLum += dispatchThreadID.x + 4 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 4] : avgLum;
		stepAvgLum += dispatchThreadID.x + 8 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 8] : avgLum;
		stepAvgLum += dispatchThreadID.x + 12 < g_GroupSize ?
			g_SharedAvgFinal[dispatchThreadID.x + 12] : avgLum;

		// 결과 값 저장
		avgLum = stepAvgLum;
		g_SharedAvgFinal[dispatchThreadID.x] = stepAvgLum;
	}

	GroupMemoryBarrierWithGroupSync(); // 동기화 후 다음 과정으로

	// 4에서 1로 다운스케일
	if (dispatchThreadID.x == 0)
	{
		// 평균 휘도 값 계산
		float fFinalLumValue = avgLum;
		fFinalLumValue += dispatchThreadID.x + 16 < g_GroupSize.x ?
			g_SharedAvgFinal[dispatchThreadID.x + 16] : avgLum;
		fFinalLumValue += dispatchThreadID.x + 32 < g_GroupSize.x ?
			g_SharedAvgFinal[dispatchThreadID.x + 32] : avgLum;
		fFinalLumValue += dispatchThreadID.x + 48 < g_GroupSize.x ?
			g_SharedAvgFinal[dispatchThreadID.x + 48] : avgLum;
		fFinalLumValue /= 64.0;

		g_AverageLum[0] = max(fFinalLumValue, 0.001);
	}
}


technique11		DefaultTechnique
{
	pass DownScaleFirstPass
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_FIRSTPASS();
	}

	pass DownScaleSecondPass
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_SECONDPASS();
	}
};



