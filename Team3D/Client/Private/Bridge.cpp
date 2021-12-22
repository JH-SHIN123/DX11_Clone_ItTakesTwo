#include "stdafx.h"
#include "..\Public\Bridge.h"

CBridge::CBridge(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBridge::CBridge(const CBridge & rhs)
	: CGameObject(rhs)
{
}

void CBridge::Call_Trigger()
{
	m_bTrigger = true;
}

HRESULT CBridge::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBridge::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_Ins_Env_Desc, pArg, sizeof(CInstancing_Env::ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(10.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Ins_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom, &m_Ins_Env_Desc.Instancing_Arg), E_FAIL);

	XMStoreFloat4x4(&m_matPivot, XMMatrixTranslation(64.f, 17.f, 155.f));

	_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPivot)) * XMMatrixRotationX(XMConvertToRadians(-70.f)) * XMLoadFloat4x4(&m_matPivot);
	m_pModelCom->Update_Model(matTransform);

	CDataStorage::GetInstance()->Set_BridgePtr(this);

	return S_OK;
}

_int CBridge::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_bUpdate == true)
	{
		if (true == m_bTrigger)
		{
			_float	fAngle = 45.f * (_float)dTimeDelta;

			if (m_fAngle >= 70.f)
			{
				fAngle = 70.f - m_fAngle;

				m_bTrigger = false;
				m_bUpdate = false;
				m_bIsUppended = true;
				m_fAngle = 0.f;
			}

			_matrix matTransform = XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_matPivot)) * XMMatrixRotationX(XMConvertToRadians(fAngle)) * XMLoadFloat4x4(&m_matPivot);
			m_pModelCom->Update_Model(matTransform);

			m_fAngle += fAngle;
		}

		if (DATABASE->Get_BigButtonPressed() == true)
		{
			if (m_bSoundOnce == false)
			{
				m_pGameInstance->Set_SoundVolume(CHANNEL_BRIDGE_ACTIVE, m_fBridgeVolume);
				m_pGameInstance->Play_Sound(TEXT("Bridge.wav"), CHANNEL_BRIDGE_ACTIVE, m_fBridgeVolume);
				m_bSoundOnce = true;
			}
			Call_Trigger();
		}
	}
	return NO_EVENT;
}

_int CBridge::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling())
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBridge::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	m_pModelCom->Set_DefaultVariables_Shadow();

	m_pModelCom->Render_Model(0, m_Ins_Env_Desc.iMaterialIndex);

	return S_OK;
}

CBridge * CBridge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBridge* pInstance = new CBridge(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBridge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CBridge::Clone_GameObject(void * pArg)
{
	CBridge* pInstance = new CBridge(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBridge");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CBridge::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
