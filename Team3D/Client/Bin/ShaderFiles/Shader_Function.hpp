
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
			if (-shadowPosNDC.z <= g_CascadeEnds[i + 1])
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

	// Match up Deferred With Cascaded Shadow Depth
	float2 vShadowUV = shadowPosH.xy;
	
	// Set up to Slice Offset
	vShadowUV.y = (vShadowUV.y + float(iSliceIndex)) / float(MAX_CASCADES);

	// Set up to Viewport Offset
	if(MAIN_VIEWPORT_INDEX == iViewportIndex)
		vShadowUV.x *= g_vMainViewportUVInfo.z;
	else
		vShadowUV.x *= g_vMainViewportUVInfo.z + 0.5f;


	float shadowFactor = 0.f;
	float percentLit = 0.0f;
	float depth = shadowPosH.z; // 그릴 객체들의 깊이값. (그림자 ndc로 이동한)

	percentLit = g_CascadedShadowDepthTexture.Sample(Wrap_Sampler, vShadowUV).r;
	if (percentLit < depth)
		shadowFactor = percentLit;

	if (shadowPosH.z < shadowFactor + 0.01f)
	{
		shadowFactor = 0.f;
	}

	return shadowFactor;
}