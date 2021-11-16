#include "stdafx.h"
#include "..\public\SubCamera.h"
#include "GameInstance.h"

CSubCamera::CSubCamera(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CCamera(pDevice, pDeviceContext)
{
}

CSubCamera::CSubCamera(const CSubCamera & rhs)
	: CCamera(rhs)
{
}

HRESULT CSubCamera::NativeConstruct_Prototype()
{
	CCamera::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSubCamera::NativeConstruct(void * pArg)
{
	CCamera::NativeConstruct(pArg);

	return S_OK;
}

_int CSubCamera::Tick(_double dTimeDelta)
{
	//if (m_pGameInstance->Key_Pressing(DIK_W))
	//	m_pTransformCom->Go_Straight(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_A))
	//	m_pTransformCom->Go_Left(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_S))
	//	m_pTransformCom->Go_Backward(dTimeDelta);
	//if (m_pGameInstance->Key_Pressing(DIK_D))
	//	m_pTransformCom->Go_Right(dTimeDelta);

	//_long	MouseMove = 0;

	//if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_X))
	//	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), dTimeDelta * MouseMove * 0.1);

	//if (MouseMove = m_pGameInstance->Mouse_Move(CInput_Device::DIMS_Y))
	//	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * MouseMove * 0.1);

	return CCamera::Tick(dTimeDelta);
}

_int CSubCamera::Late_Tick(_double TimeDelta)
{
	CCamera::Late_Tick(TimeDelta);

	return NO_EVENT;
}

CSubCamera * CSubCamera::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CSubCamera* pInstance = new CSubCamera(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSubCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSubCamera::Clone_GameObject(void * pArg)
{
	CSubCamera* pInstance = new CSubCamera(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSubCamera");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSubCamera::Free()
{
	CCamera::Free();
}
