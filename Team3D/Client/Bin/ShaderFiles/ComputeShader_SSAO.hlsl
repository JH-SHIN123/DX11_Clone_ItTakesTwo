#define WINCX 1280
#define WINCY 720

Texture2D	g_NormalsTexture;
Texture2D	g_DepthTexture;

RWStructuredBuffer<float4> g_RWDownScaledDepth;

cbuffer Matrices
{
	matrix	g_MainProjMatrixInverse;
	matrix	g_SubProjMatrixInverse;

	matrix	g_MainViewMatrix;
	matrix	g_SubViewMatrix;
}

cbuffer Camera
{
	float	g_fMainCamFar;
	float	g_fSubCamFar;
}

cbuffer Viewport
{
	/* x = TopLeftX, y = TopLeftY, z = Width, w = Height, 0.f ~ 1.f */
	float4	g_vMainViewportUVInfo;
	float4	g_vSubViewportUVInfo;
};

cbuffer DownScaleConstants
{
	uint2	g_Res = { WINCX / 2, WINCY / 2 };				// 1/4 가로 세로 해상도
	float2	g_ResRcp = {1.f / (WINCX / 2.f), 1.f / (WINCY / 2.f) };		// Res의 역수 값
	float	g_OffsetRadius = 10.f;		// 랜덤 위치를 찾기위한 반지름 범위
	float	g_Radius = 13.f;			// 앰비언트 반구의 반지름값.
}

float3 ConvertNormalToViewSpace(uint3 CurPixel, float3 vNormalWorld)
{
	float2 vUV = { CurPixel.x / g_Res.x, CurPixel.y / g_Res.y };

	if (vUV.x >= g_vMainViewportUVInfo.x && vUV.x <= g_vMainViewportUVInfo.z &&
		vUV.y >= g_vMainViewportUVInfo.y && vUV.y <= g_vMainViewportUVInfo.w)
	{
		vNormalWorld = mul(vNormalWorld, (float3x3)g_MainViewMatrix);
	}
	else if (vUV.x >= g_vSubViewportUVInfo.x && vUV.x <= g_vSubViewportUVInfo.z &&
		vUV.y >= g_vSubViewportUVInfo.y && vUV.y <= g_vSubViewportUVInfo.w)
	{
		vNormalWorld = mul(vNormalWorld, (float3x3)g_SubViewMatrix);
	}

	return vNormalWorld;
}

float ConvertZToViewDepth(uint3 CurPixel, float fViewZ, float fProjZ)
{
	vector vViewPos = vector(0.f, 0.f, fProjZ, 1.f);
	float2 vUV = { CurPixel.x / g_Res.x, CurPixel.y / g_Res.y };

	if (vUV.x >= g_vMainViewportUVInfo.x && vUV.x <= g_vMainViewportUVInfo.z &&
		vUV.y >= g_vMainViewportUVInfo.y && vUV.y <= g_vMainViewportUVInfo.w)
	{
		fViewZ *= g_fMainCamFar;
		vViewPos = vViewPos * fViewZ;
		vViewPos = mul(vViewPos, g_MainProjMatrixInverse);
	}
	else if (vUV.x >= g_vSubViewportUVInfo.x && vUV.x <= g_vSubViewportUVInfo.z &&
		vUV.y >= g_vSubViewportUVInfo.y && vUV.y <= g_vSubViewportUVInfo.w)
	{
		fViewZ *= g_fSubCamFar;
		vViewPos = vViewPos * fViewZ;
		vViewPos = mul(vViewPos, g_SubProjMatrixInverse);
	}

	return vViewPos.z;
}

[numthreads(1024, 1, 1)]
void CS_DOWNSCALE_DEPTH(uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint3 CurPixel = uint3(dispatchThreadId.x % g_Res.x, dispatchThreadId.x / g_Res.x, 0);

	// Skip out of bound pixels
	if (CurPixel.y < g_Res.y)
	{
		// DepthTex x: ViewZ / y: ProjZ
		float minViewZ = 1.0;
		float minProjZ = 1.0;

		float3 avgNormalWorld = float3(0.0, 0.0, 0.0);
		uint3 FullResPixel = CurPixel * 2;

		[unroll]
		for (int i = 0; i < 2; i++)
		{
			[unroll]
			for (int j = 0; j < 2; j++)
			{
				// Get the pixels depth and store the minimum depth
				float4 curDepth = g_DepthTexture.Load(FullResPixel, int2(j, i)); // x: ViewZ / CamFar, y: ProjZ / ViewZ
				minViewZ = min(curDepth.x, minViewZ);
				minProjZ = min(curDepth.y, minProjZ);

				// 4개의 픽셀에 대한 평균 노말값 구하기
				float3 normalWorld = g_NormalsTexture.Load(FullResPixel, int2(j, i)).xyz;
				avgNormalWorld += normalize(normalWorld * 2.f - 1.f);
			}
		}

		g_RWDownScaledDepth[dispatchThreadId.x].x = ConvertZToViewDepth(CurPixel, minViewZ, minProjZ);
		float3 avgNormalView = ConvertNormalToViewSpace(CurPixel, avgNormalWorld * 0.25f);
		g_RWDownScaledDepth[dispatchThreadId.x].yzw = avgNormalView;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
StructuredBuffer<float4>	g_DownScaledDepth;
RWTexture2D<float>			g_RWSSAO;

groupshared float g_SharedDepths[1024];

// Possion disc sampling pattern
static const float	g_NumSamplesRcp = 1.0 / 8.0;
static const uint	g_NumSamples = 8;
static const float2 g_SampleOffsets[g_NumSamples] = {
	float2(0.2803166, 0.08997212),
	float2(-0.5130632, 0.6877457),
	float2(0.425495, 0.8665376),
	float2(0.8732584, 0.3858971),
	float2(0.0498111, -0.6287371),
	float2(-0.9674183, 0.1236534),
	float2(-0.3788098, -0.09177673),
	float2(0.6985874, -0.5610316),
};

float GetDepth(int2 pos)
{
	// Clamp the input pixel position
	float x = clamp(pos.x, 0, g_Res.x - 1);
	float y = clamp(pos.y, 0, g_Res.y - 1);

	// find the mini-depth index position and retrive the detph value
	int dsDepthIdx = x + y * g_Res.x;
	return g_DownScaledDepth[dsDepthIdx].x;
}

float3 GetNormal(int2 pos)
{
	// Clamp the input pixel position
	float x = clamp(pos.x, 0, g_Res.x - 1);
	float y = clamp(pos.y, 0, g_Res.y - 1);

	int dsDepthIdx = x + y * g_Res.x; // find the mini-depth index position
	return g_DownScaledDepth[dsDepthIdx].yzw;
}

float GetCamFar(int2 CurPixel)
{
	float fCamFar = 0.f;
	float2 vUV = { CurPixel.x / g_Res.x, CurPixel.y / g_Res.y };

	if (vUV.x >= g_vMainViewportUVInfo.x && vUV.x <= g_vMainViewportUVInfo.z &&
		vUV.y >= g_vMainViewportUVInfo.y && vUV.y <= g_vMainViewportUVInfo.w)
	{
		fCamFar = g_fMainCamFar;
	}
	else if (vUV.x >= g_vSubViewportUVInfo.x && vUV.x <= g_vSubViewportUVInfo.z &&
		vUV.y >= g_vSubViewportUVInfo.y && vUV.y <= g_vSubViewportUVInfo.w)
	{
		fCamFar = g_fSubCamFar;
	}

	return fCamFar;
}

float3 ConvertToViewSpace(int2 CurPixel, float2 ClipPos, float fViewZ)
{
	vector vViewPos = vector(ClipPos.xy, 0.f, 1.f);
	float2 vUV = { CurPixel.x / g_Res.x, CurPixel.y / g_Res.y };

	if (vUV.x >= g_vMainViewportUVInfo.x && vUV.x <= g_vMainViewportUVInfo.z &&
		vUV.y >= g_vMainViewportUVInfo.y && vUV.y <= g_vMainViewportUVInfo.w)
	{
		vViewPos = vViewPos * fViewZ;
		vViewPos = mul(vViewPos, g_MainProjMatrixInverse);
	}
	else if (vUV.x >= g_vSubViewportUVInfo.x && vUV.x <= g_vSubViewportUVInfo.z &&
		vUV.y >= g_vSubViewportUVInfo.y && vUV.y <= g_vSubViewportUVInfo.w)
	{
		vViewPos = vViewPos * fViewZ;
		vViewPos = mul(vViewPos, g_SubProjMatrixInverse);
	}

	vViewPos.z = fViewZ;

	return vViewPos.xyz;
}

float ComputeAO(int2 cetnerPixelPos, float2 centerClipPos)
{
	// Get the depths for the normal calculation
	float centerDepth = GetDepth(cetnerPixelPos.xy);
	float fCamFar = GetCamFar(cetnerPixelPos.xy);
	float isNotSky = centerDepth < fCamFar;

	// Find the center pixel veiwspace position
	float3 centerPos;
	centerPos = ConvertToViewSpace(cetnerPixelPos, centerClipPos, centerDepth);

	// Get the view space normal for the center pixel
	float3 centerNormal = GetNormal(cetnerPixelPos.xy);
	centerNormal = normalize(centerNormal);

	// Prepare for random sampling offset
	float rotationAngle = 0.0;
	//float rotationAngle = dot(float2(centerClipPos), float2(73.0, 197.0));
	float2 randSinCos;
	sincos(rotationAngle, randSinCos.x, randSinCos.y);
	float2x2 randRotMat = float2x2(randSinCos.y, -randSinCos.x, randSinCos.x, randSinCos.y);

	// Take the samples and calculate the ambient occlusion value for each
	float ao = 0.0;
	[unroll]
	for (uint i = 0; i < g_NumSamples; i++)
	{
		// Find the texture space position and depth
		float2 sampleOff = g_OffsetRadius.xx * mul(g_SampleOffsets[i], randRotMat);
		float curDepth = GetDepth(cetnerPixelPos + int2(sampleOff.x, -sampleOff.y));

		// Calculate the view space position
		 float3 curPos = ConvertToViewSpace(cetnerPixelPos + int2(sampleOff.x, -sampleOff.y), centerClipPos + 2.0 * sampleOff * g_ResRcp, curDepth);

		float3 centerToCurPos = curPos - centerPos;
		float lenCenterToCurPos = length(centerToCurPos);
		float angleFactor = 1.0 - dot(centerToCurPos / lenCenterToCurPos, centerNormal);
		float distFactor = lenCenterToCurPos / g_Radius;

		ao += saturate(max(distFactor, angleFactor)) * isNotSky;
		//ao += saturate(max(distFactor, angleFactor));
	}

	return ao * g_NumSamplesRcp;
}

[numthreads(1024, 1, 1)]
void CS_SSAO_COMPUTE(uint3 groupThreadId : SV_GroupThreadID, uint3 dispatchThreadId : SV_DispatchThreadID)
{
	uint2 CurPixel = uint2(dispatchThreadId.x % g_Res.x, dispatchThreadId.x / g_Res.x);

	g_SharedDepths[groupThreadId.x] = g_DownScaledDepth[dispatchThreadId.x].x;

	GroupMemoryBarrierWithGroupSync();

	// Skip out of bound pixels
	if (CurPixel.y < g_Res.y)
	{
		// Find the XY clip space position for the current pixel
		// Y has to be inverted
		float2 centerClipPos = 2.0 * float2(CurPixel) * g_ResRcp;
		centerClipPos = float2(centerClipPos.x - 1.0, 1.0 - centerClipPos.y);

		g_RWSSAO[CurPixel] = ComputeAO(CurPixel, centerClipPos);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

technique11		DefaultTechnique
{
	pass DepthDownScale // 0
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_DOWNSCALE_DEPTH();
	}

	pass SSAOCompute // 1
	{
		VertexShader = NULL;
		GeometryShader = NULL;
		PixelShader = NULL;
		ComputeShader = compile cs_5_0 CS_SSAO_COMPUTE();
	}
};



