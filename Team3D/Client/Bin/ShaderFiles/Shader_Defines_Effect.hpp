/*For.Effect_Shader*/
cbuffer Effect
{
	// Mesh_Effect
	float	g_fTime;
	float	g_fAlpha;
	float	g_fTimeWeight;
	float	g_fRadianAngle;
	float4	g_vColor;
	float4	g_vColorRamp_UV;
// PointCustom
	float2			g_vSize;
	float4			g_vUV;
	int				g_IsBillBoard;
// 	float4			g_vColor;
// 	float			g_fTime;
// 	float4			g_vColorRamp_UV;

// PointCoutom_STT
// 	float2			g_vSize;
// 	float4			g_vColor;
// 	float			g_fTime;
// 	float4			g_vUV;

}

// 해당 축에 대한 Angle만큼의 회전 값을 return;
float4 Rotate_Radian_Axis(float angle, float3 axis)
{
	float sn = sin(angle * 0.5);
	float cs = cos(angle * 0.5);
	return float4(axis * sn, cs);
}