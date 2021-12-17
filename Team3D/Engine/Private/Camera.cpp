#include "..\public\Camera.h"
#include "Pipeline.h"
#include "Graphic_Device.h"

CCamera::CCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
	, m_pPipeline(CPipeline::GetInstance())
{
	Safe_AddRef(m_pPipeline);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeline(rhs.m_pPipeline)
{
	Safe_AddRef(m_pPipeline);
}

_fvector CCamera::Get_Position()
{
	if (nullptr == m_pTransformCom)
		return XMVectorSet(0.f, 0.f, 0.f, 1.f);

	return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

HRESULT CCamera::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CCamera::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERA_DESC));

	m_pTransformCom = CTransform::Create(m_pDevice, m_pDeviceContext);
	NULL_CHECK_RETURN(m_pTransformCom, E_FAIL);
	FAILED_CHECK_RETURN(m_pTransformCom->NativeConstruct(&m_CameraDesc.TransformDesc), E_FAIL);

	_vector	vEye	= XMLoadFloat4(&m_CameraDesc.vEye);
	_vector	vAt		= XMLoadFloat4(&m_CameraDesc.vAt);
	_vector	vAxisY	= XMLoadFloat4(&m_CameraDesc.vAxisY);
	_vector	vLook	= XMVector3Normalize(vAt - vEye);
	_vector	vRight	= XMVector3Normalize(XMVector3Cross(vAxisY, vLook));
	_vector	vUp		= XMVector3Normalize(XMVector3Cross(vLook, vRight));

	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vEye);

	return S_OK;
}

_int CCamera::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

#ifndef MAKE_VIEWMATRIX_BY_CAMERADESC
	/* Control By WorldMatrix */

	if (1 == m_CameraDesc.iViewportIndex)
	{
		m_CameraDesc.fAspect = CGraphic_Device::GetInstance()->Get_ViewportAspect(1);

		_matrix CameraMatrix		= m_pTransformCom->Get_WorldMatrix();
		_matrix ProjMatrix			= XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
		_matrix ProjMatrix_ForEarth	= XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, 50000.f);

		m_pPipeline->Set_Transform(CPipeline::TS_MAINVIEW, XMMatrixInverse(nullptr, CameraMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_MAINVIEW_INVERSE, CameraMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_MAINPROJ, ProjMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_MAINPROJ_INVERSE, XMMatrixInverse(nullptr, ProjMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_MAINPROJ_FOR_EARTH, ProjMatrix_ForEarth);

		m_pPipeline->Set_MainCamFar(m_CameraDesc.fFar);
	}
	else if (2 == m_CameraDesc.iViewportIndex)
	{
		m_CameraDesc.fAspect = CGraphic_Device::GetInstance()->Get_ViewportAspect(2);

		_matrix CameraMatrix		= m_pTransformCom->Get_WorldMatrix();
		_matrix ProjMatrix			= XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
		_matrix ProjMatrix_ForEarth = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, 50000.f);

		m_pPipeline->Set_Transform(CPipeline::TS_SUBVIEW, XMMatrixInverse(nullptr, CameraMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_SUBVIEW_INVERSE, CameraMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_SUBPROJ, ProjMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_SUBPROJ_INVERSE, XMMatrixInverse(nullptr, ProjMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_SUBPROJ_FOR_EARTH, ProjMatrix_ForEarth);

		m_pPipeline->Set_SubCamFar(m_CameraDesc.fFar);
	}

	// For. Full Screen
	_matrix FullScreenProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fFullScreenAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
	m_pPipeline->Set_Transform(CPipeline::TS_FULLSCREEN_PROJ, FullScreenProjMatrix);
	m_pPipeline->Set_Transform(CPipeline::TS_FULLSCREEN_PROJ_INVERSE, XMMatrixInverse(nullptr, FullScreenProjMatrix));
#else
	/* Control By Eye/At */

	if (1 == m_CameraDesc.iViewportIndex)
	{
		m_CameraDesc.fAspect = CGraphic_Device::GetInstance()->Get_ViewportAspect(1);

		_matrix ViewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&m_CameraDesc.vEye), XMLoadFloat4(&m_CameraDesc.vAt), XMLoadFloat4(&m_CameraDesc.vAxisY));
		_matrix ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

		m_pPipeline->Set_Transform(CPipeline::TS_MAINVIEW, ViewMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_MAINVIEW_INVERSE, XMMatrixInverse(nullptr, ViewMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_MAINPROJ, ProjMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_MAINPROJ_INVERSE, XMMatrixInverse(nullptr, ProjMatrix));
		m_pPipeline->Set_MainCamFar(m_CameraDesc.fFar);
	}
	else if (2 == m_CameraDesc.iViewportIndex)
	{
		m_CameraDesc.fAspect = CGraphic_Device::GetInstance()->Get_ViewportAspect(2);

		_matrix ViewMatrix = XMMatrixLookAtLH(XMLoadFloat4(&m_CameraDesc.vEye), XMLoadFloat4(&m_CameraDesc.vAt), XMLoadFloat4(&m_CameraDesc.vAxisY));
		_matrix ProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);

		m_pPipeline->Set_Transform(CPipeline::TS_SUBVIEW, ViewMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_SUBVIEW_INVERSE, XMMatrixInverse(nullptr, ViewMatrix));
		m_pPipeline->Set_Transform(CPipeline::TS_SUBPROJ, ProjMatrix);
		m_pPipeline->Set_Transform(CPipeline::TS_SUBPROJ_INVERSE, XMMatrixInverse(nullptr, ProjMatrix));
		m_pPipeline->Set_SubCamFar(m_CameraDesc.fFar);
	}

	// For. Full Screen
	_matrix FullScreenProjMatrix = XMMatrixPerspectiveFovLH(m_CameraDesc.fFovY, m_CameraDesc.fFullScreenAspect, m_CameraDesc.fNear, m_CameraDesc.fFar);
	m_pPipeline->Set_Transform(CPipeline::TS_FULLSCREEN_PROJ, FullScreenProjMatrix);
	m_pPipeline->Set_Transform(CPipeline::TS_FULLSCREEN_PROJ_INVERSE, XMMatrixInverse(nullptr, FullScreenProjMatrix));
#endif

	return NO_EVENT;
}

_int CCamera::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

void CCamera::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pPipeline);

	CGameObject::Free();
}
