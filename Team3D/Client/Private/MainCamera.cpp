#include "stdafx.h"
#include "..\public\MainCamera.h"
#include "GameInstance.h"

CMainCamera::CMainCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CMainCamera::CMainCamera(const CMainCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CMainCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMainCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	return S_OK;
}

_int CMainCamera::Tick(_double dTimeDelta)
{
	if (m_pGameInstance->Key_Pressing(DIK_W))
		m_pTransformCom->Go_Straight(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_A))
		m_pTransformCom->Go_Left(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_S))
		m_pTransformCom->Go_Backward(dTimeDelta);
	if (m_pGameInstance->Key_Pressing(DIK_D))
		m_pTransformCom->Go_Right(dTimeDelta);

	_long	MouseMove = 0;

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * MouseMove * 0.1);

	if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
		m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * MouseMove * 0.1);

	return CCamera::Tick(dTimeDelta);
}

_int CMainCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);

	return NO_EVENT;
}

CMainCamera * CMainCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMainCamera* pInstance = new CMainCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMainCamera::Clone_GameObject(void * pArg)
{
	CMainCamera* pInstance = new CMainCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMainCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainCamera::Free()
{
	CCamera::Free();
}
