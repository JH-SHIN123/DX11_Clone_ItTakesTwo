/*For.Effect_Shader*/
cbuffer Effect
{
	// Mesh_Effect
	float	g_fTime;
	float	g_fAlpha;
	float4	g_vColor;
	float	g_fRadianAngle;
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
