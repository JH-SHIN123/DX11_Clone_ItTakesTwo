#include "stdafx.h"
#include "..\public\UFORadarSet.h"
#include "UFORadarScreen.h"
#include "UFORadarLever.h"

CUFORadarSet::CUFORadarSet(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUFORadarSet::CUFORadarSet(const CUFORadarSet& rhs)
	: CGameObject(rhs)
{
}

HRESULT CUFORadarSet::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	
	return S_OK;
}

HRESULT CUFORadarSet::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	if (nullptr != pArg)
		memcpy(&m_tRadarDesc, (RADARDESC*)pArg, sizeof(RADARDESC));

	DATABASE->Set_UFORadarSet(this);

	CGameObject* pUFORadarScreen = nullptr;
	CGameObject* pUFORadarLever = nullptr;

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_UFORadarScreen"), Level::LEVEL_STAGE, TEXT("GameObject_UFORadarScreen"), &m_tRadarDesc, &pUFORadarScreen), E_FAIL);

	m_tRadarDesc.vPosition.m128_f32[2] -= 1.5f;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_UFORadarLever"), Level::LEVEL_STAGE, TEXT("GameObject_UFORadarLever"), &m_tRadarDesc, &pUFORadarLever), E_FAIL);

	m_pUFORadarScreen = static_cast<CUFORadarScreen*>(pUFORadarScreen);
	m_pUFORadarLever = static_cast<CUFORadarLever*>(pUFORadarLever);

	return S_OK;
}

_int CUFORadarSet::Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

_int CUFORadarSet::Late_Tick(_double dTimeDelta)
{
	return CGameObject::Tick(dTimeDelta);
}

HRESULT CUFORadarSet::Render(RENDER_GROUP::Enum eGroup)
{
	return CGameObject::Render(eGroup);
}

HRESULT CUFORadarSet::Render_ShadowDepth()
{
	return CGameObject::Render_ShadowDepth();
}

void CUFORadarSet::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

CUFORadarSet * CUFORadarSet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUFORadarSet* pInstance = new CUFORadarSet(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - UFORadarSet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUFORadarSet::Clone_GameObject(void * pArg)
{
	CUFORadarSet* pInstance = new CUFORadarSet(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUFORadarSet");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUFORadarSet::Free()
{
	Safe_Release(m_pUFORadarLever);
	Safe_Release(m_pUFORadarScreen);
	CGameObject::Free();
}