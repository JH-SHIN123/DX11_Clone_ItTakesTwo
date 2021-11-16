#include "..\public\Frustum.h"
#include "Pipeline.h"

IMPLEMENT_SINGLETON(CFrustum)

HRESULT CFrustum::Ready_Frustum()
{
	m_vPoints[0] = _float3(-1.f, 1.f, 0.f);
	m_vPoints[1] = _float3(1.f, 1.f, 0.f);
	m_vPoints[2] = _float3(1.f, -1.f, 0.f);
	m_vPoints[3] = _float3(-1.f, -1.f, 0.f);
	m_vPoints[4] = _float3(-1.f, 1.f, 1.f);
	m_vPoints[5] = _float3(1.f, 1.f, 1.f);
	m_vPoints[6] = _float3(1.f, -1.f, 1.f);
	m_vPoints[7] = _float3(-1.f, -1.f, 1.f);

	return S_OK;
}

void CFrustum::Transform_ToWorldSpace()
{
	CPipeline* pPipeline = CPipeline::GetInstance();

	_matrix	InverseProjMatrix;
	_matrix	InverseViewMatrix;
	_matrix CombinedMatrix;
	_vector vPoints_World[8];

	/* For.MainViewport */
	InverseProjMatrix	= pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	InverseViewMatrix	= pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	CombinedMatrix		= InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_World[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[FRUSTUM_MAIN].vPoints_World[iIndex], vPoints_World[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[0], XMPlaneFromPoints(vPoints_World[1], vPoints_World[5], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[1], XMPlaneFromPoints(vPoints_World[4], vPoints_World[0], vPoints_World[3]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[2], XMPlaneFromPoints(vPoints_World[4], vPoints_World[5], vPoints_World[1]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[3], XMPlaneFromPoints(vPoints_World[3], vPoints_World[2], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[4], XMPlaneFromPoints(vPoints_World[5], vPoints_World[4], vPoints_World[7]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[5], XMPlaneFromPoints(vPoints_World[0], vPoints_World[1], vPoints_World[2]));

	m_Frustum[FRUSTUM_MAIN].fFrustumDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[4]) - XMLoadFloat4(&m_Frustum[FRUSTUM_MAIN].PlaneWorld[5])));

	/* For.SubViewport */
	InverseProjMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	InverseViewMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	CombinedMatrix = InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_World[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[FRUSTUM_SUB].vPoints_World[iIndex], vPoints_World[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[0], XMPlaneFromPoints(vPoints_World[1], vPoints_World[5], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[1], XMPlaneFromPoints(vPoints_World[4], vPoints_World[0], vPoints_World[3]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[2], XMPlaneFromPoints(vPoints_World[4], vPoints_World[5], vPoints_World[1]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[3], XMPlaneFromPoints(vPoints_World[3], vPoints_World[2], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[4], XMPlaneFromPoints(vPoints_World[5], vPoints_World[4], vPoints_World[7]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[5], XMPlaneFromPoints(vPoints_World[0], vPoints_World[1], vPoints_World[2]));

	m_Frustum[FRUSTUM_SUB].fFrustumDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[4]) - XMLoadFloat4(&m_Frustum[FRUSTUM_SUB].PlaneWorld[5])));

	/* For.FullScreen_Main */
	InverseProjMatrix = pPipeline->Get_Transform(CPipeline::TS_FULLSCREEN_PROJ_INVERSE);
	InverseViewMatrix = pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	CombinedMatrix = InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_World[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].vPoints_World[iIndex], vPoints_World[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[0], XMPlaneFromPoints(vPoints_World[1], vPoints_World[5], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[1], XMPlaneFromPoints(vPoints_World[4], vPoints_World[0], vPoints_World[3]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[2], XMPlaneFromPoints(vPoints_World[4], vPoints_World[5], vPoints_World[1]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[3], XMPlaneFromPoints(vPoints_World[3], vPoints_World[2], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[4], XMPlaneFromPoints(vPoints_World[5], vPoints_World[4], vPoints_World[7]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[5], XMPlaneFromPoints(vPoints_World[0], vPoints_World[1], vPoints_World[2]));

	m_Frustum[FRUSTUM_FULLSCREEN_MAIN].fFrustumDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[4]) - XMLoadFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_MAIN].PlaneWorld[5])));

	/* For.FullScreen_Sub */
	InverseProjMatrix = pPipeline->Get_Transform(CPipeline::TS_FULLSCREEN_PROJ_INVERSE);
	InverseViewMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	CombinedMatrix = InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_World[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].vPoints_World[iIndex], vPoints_World[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[0], XMPlaneFromPoints(vPoints_World[1], vPoints_World[5], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[1], XMPlaneFromPoints(vPoints_World[4], vPoints_World[0], vPoints_World[3]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[2], XMPlaneFromPoints(vPoints_World[4], vPoints_World[5], vPoints_World[1]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[3], XMPlaneFromPoints(vPoints_World[3], vPoints_World[2], vPoints_World[6]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[4], XMPlaneFromPoints(vPoints_World[5], vPoints_World[4], vPoints_World[7]));
	XMStoreFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[5], XMPlaneFromPoints(vPoints_World[0], vPoints_World[1], vPoints_World[2]));

	m_Frustum[FRUSTUM_FULLSCREEN_SUB].fFrustumDepth = XMVectorGetX(XMVector3Length(XMLoadFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[4]) - XMLoadFloat4(&m_Frustum[FRUSTUM_FULLSCREEN_SUB].PlaneWorld[5])));
}

void CFrustum::Transform_ToLocalSpace(_fmatrix WorldMatrix)
{
	CPipeline* pPipeline = CPipeline::GetInstance();

	_matrix	InverseProjMatrix;
	_matrix	InverseViewMatrix;
	_matrix CombinedMatrix;
	_vector vPoints_Local[8];

	/* For.MainViewport */
	InverseProjMatrix = pPipeline->Get_Transform(CPipeline::TS_MAINPROJ_INVERSE);
	InverseViewMatrix	= pPipeline->Get_Transform(CPipeline::TS_MAINVIEW_INVERSE);
	CombinedMatrix		= InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_Local[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[0].vPoints_Local[iIndex], vPoints_Local[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[0].PlaneLocal[0], XMPlaneFromPoints(vPoints_Local[1], vPoints_Local[5], vPoints_Local[6]));
	XMStoreFloat4(&m_Frustum[0].PlaneLocal[1], XMPlaneFromPoints(vPoints_Local[4], vPoints_Local[0], vPoints_Local[3]));
	XMStoreFloat4(&m_Frustum[0].PlaneLocal[2], XMPlaneFromPoints(vPoints_Local[4], vPoints_Local[5], vPoints_Local[1]));
	XMStoreFloat4(&m_Frustum[0].PlaneLocal[3], XMPlaneFromPoints(vPoints_Local[3], vPoints_Local[2], vPoints_Local[6]));
	XMStoreFloat4(&m_Frustum[0].PlaneLocal[4], XMPlaneFromPoints(vPoints_Local[5], vPoints_Local[4], vPoints_Local[7]));
	XMStoreFloat4(&m_Frustum[0].PlaneLocal[5], XMPlaneFromPoints(vPoints_Local[0], vPoints_Local[1], vPoints_Local[2]));

	/* For.SubViewport */
	InverseProjMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBPROJ_INVERSE);
	InverseViewMatrix = pPipeline->Get_Transform(CPipeline::TS_SUBVIEW_INVERSE);
	CombinedMatrix = InverseProjMatrix * InverseViewMatrix;

	for (_uint iIndex = 0; iIndex < 8; ++iIndex)
	{
		vPoints_Local[iIndex] = XMVector3TransformCoord(XMLoadFloat3(&m_vPoints[iIndex]), CombinedMatrix);
		XMStoreFloat3(&m_Frustum[1].vPoints_Local[iIndex], vPoints_Local[iIndex]);
	}

	XMStoreFloat4(&m_Frustum[1].PlaneLocal[0], XMPlaneFromPoints(vPoints_Local[1], vPoints_Local[5], vPoints_Local[6]));
	XMStoreFloat4(&m_Frustum[1].PlaneLocal[1], XMPlaneFromPoints(vPoints_Local[4], vPoints_Local[0], vPoints_Local[3]));
	XMStoreFloat4(&m_Frustum[1].PlaneLocal[2], XMPlaneFromPoints(vPoints_Local[4], vPoints_Local[5], vPoints_Local[1]));
	XMStoreFloat4(&m_Frustum[1].PlaneLocal[3], XMPlaneFromPoints(vPoints_Local[3], vPoints_Local[2], vPoints_Local[6]));
	XMStoreFloat4(&m_Frustum[1].PlaneLocal[4], XMPlaneFromPoints(vPoints_Local[5], vPoints_Local[4], vPoints_Local[7]));
	XMStoreFloat4(&m_Frustum[1].PlaneLocal[5], XMPlaneFromPoints(vPoints_Local[0], vPoints_Local[1], vPoints_Local[2]));
}

_bool CFrustum::IsIn_WorldSpace(_fvector vPosition, _float fRadius)
{
	_bool isIn_MainViewport	= true;
	_bool isIn_SubViewport	= true;

	for (_uint iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Frustum[0].PlaneWorld[iIndex]), XMVectorSetW(vPosition, 1.f))))
		{
			isIn_MainViewport = false;
			break;
		}
	}

	for (_uint iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Frustum[1].PlaneWorld[iIndex]), XMVectorSetW(vPosition, 1.f))))
		{
			isIn_SubViewport = false;
			break;
		}
	}

	if (isIn_MainViewport || isIn_SubViewport)
		return true;
	
	return false;
}

_bool CFrustum::IsIn_LocalSpace(_fvector vPosition, _float fRadius)
{
	_bool isIn_MainViewport = true;
	_bool isIn_SubViewport = true;

	for (_uint iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Frustum[0].PlaneLocal[iIndex]), XMVectorSetW(vPosition, 1.f))))
		{
			isIn_MainViewport = false;
			break;
		}
	}

	for (_uint iIndex = 0; iIndex < 6; ++iIndex)
	{
		if (fRadius < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_Frustum[1].PlaneLocal[iIndex]), XMVectorSetW(vPosition, 1.f))))
		{
			isIn_SubViewport = false;
			break;
		}
	}

	if (isIn_MainViewport || isIn_SubViewport)
		return true;

	return false;
}

void CFrustum::Free()
{
}