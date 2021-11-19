
float CalcAttenuation(float d, float falloffStart, float falloffEnd)
{
	// Linear falloff.
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

// Schlick gives an approximation to Fresnel reflectance (see pg. 233 "Real-Time Rendering 3rd Ed.").
// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction.
float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

vector TextureSampleToWorldSpace(float3 mapSample, float3 tangetW, float3 biNormalW, float3 normalW)
{
	vector vReturn = vector(mapSample, 0.f) * 2.f - 1.f;

	float3x3 TBN = transpose(float3x3(tangetW, biNormalW, normalW));
	vReturn = vector(mul(TBN, normalize(vReturn.xyz)), 0.f);
	vReturn = vector(normalize(vReturn.xyz) * 0.5f + 0.5f, 0.f);

	return vReturn;
}

int Get_CascadedShadowSliceIndex(uint iViewportIndex, vector vWorldPos) /* 1: Main 2: Sub*/
{
	int iIndex = -1;
	vector shadowPosNDC;

	for (uint i = 0; i < MAX_CASCADES; ++i)
	{
		if (MAIN_VIEWPORT_INDEX == iViewportIndex) shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Main[i]);
		else shadowPosNDC = mul(vWorldPos, g_ShadowTransforms_Sub[i]);
		shadowPosNDC.z /= shadowPosNDC.w;

		// 2. 절두체에 위치하는지 & 몇번째 슬라이스에 존재하는지 체크(cascadeEndWorld값과 비교해가며 슬라이스 인덱스 구하기)
		if (shadowPosNDC.x >= 0 && shadowPosNDC.x <= 1.0 && shadowPosNDC.y >= 0.0 && shadowPosNDC.y <= 1.0 && shadowPosNDC.z >= 0.0 && shadowPosNDC.z <= 1.0)
		{
			// 여기서 문제 발생 (Aspect 변경시)
			if (-shadowPosNDC.z <= g_CascadeEnds[i+1])
			{
				iIndex = i;
				break;
			}
		}
	}

	return iIndex;
}

float Get_ShadowFactor(uint iViewportIndex, uint iSliceIndex, vector vWorldPos)
{
	matrix shadowTransformMatrix;
	if (MAIN_VIEWPORT_INDEX == iViewportIndex) shadowTransformMatrix = g_ShadowTransforms_Main[iSliceIndex];
	else shadowTransformMatrix = g_ShadowTransforms_Sub[iSliceIndex];

	vector shadowPosH = mul(vWorldPos, shadowTransformMatrix);
	shadowPosH.xyz /= shadowPosH.w;

	float shadowFactor = 0.f;

	// Match up Deferred With Cascaded Shadow Depth
	float2 vShadowUV = shadowPosH.xy;
	
	// Set up to Slice Offset
	vShadowUV.y = (vShadowUV.y + float(iSliceIndex)) / float(MAX_CASCADES);

	// Set up to Viewport Offset
	if (MAIN_VIEWPORT_INDEX == iViewportIndex) {
		vShadowUV.x *= g_vMainViewportUVInfo.z;
		//vShadowUV.y *= g_vMainViewportUVInfo.w; // y는 나중에 잡자.
	}
	else {
		vShadowUV.x = vShadowUV.x * (g_vSubViewportUVInfo.z) + (1.f - g_vSubViewportUVInfo.z);
		if (vShadowUV.x > 1.f) return shadowFactor;
		//vShadowUV.y = vShadowUV.y * (g_vSubViewportUVInfo.w) + (1.f - g_vSubViewportUVInfo.w);
	}

	float percentLit = 0.0f;
	float depth = shadowPosH.z; // 그릴 객체들의 깊이값. (그림자 ndc로 이동한)

	// PCF 적용
	uint width, height, numMips;
	g_CascadedShadowDepthTexture.GetDimensions(0, width, height, numMips);

	// Texel size.
	float dx = 1.0f / (float)width;
	float dy = 1.0f / (float)height;

	const float2 offsets[9] =
	{
		float2(-dx, -dy), float2(0.0f, -dy), float2(dx, -dy),
		float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
		float2(-dx, +dy), float2(0.0f, +dy), float2(dx, +dy)
	};

	// - 0.002f
	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		percentLit += g_CascadedShadowDepthTexture.SampleCmpLevelZero(ShadowSampler,
			vShadowUV + offsets[i], depth).r;
	}
	percentLit /= 9.0f;
	shadowFactor = 1.f - percentLit;

	return shadowFactor * 0.3f/*ShadowColor*/;
}