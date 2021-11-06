#include "..\public\GameObject.h"
#include "GameInstance.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: m_pDevice(pDevice)
	, m_pDeviceContext(pDeviceContext)
	, m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pDeviceContext(rhs.m_pDeviceContext)
	, m_pGameInstance(rhs.m_pGameInstance)
	, m_isDead(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pGameInstance);
}

_float CGameObject::Get_DistanceFromCamera()
{
	return 0.f;
}

void CGameObject::Set_Dead()
{
	m_isDead = true;
}

HRESULT CGameObject::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::NativeConstruct(void * pArg)
{
	return S_OK;
}

_int CGameObject::Tick(_double dTimeDelta)
{
	return NO_EVENT;
}

_int CGameObject::Late_Tick(_double dTimeDelta)
{
	return NO_EVENT;
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

HRESULT CGameObject::Add_Component(_uint iPrototypeLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent ** ppOut, void * pArg)
{
	CComponent*	pComponent = Find_Component(pComponentTag);
	NOT_NULL_CHECK_RETURN(pComponent, E_FAIL);

	pComponent = m_pGameInstance->Add_Component_Clone(iPrototypeLevelIndex, pPrototypeTag, pArg);
	NULL_CHECK_RETURN(pComponent, E_FAIL);

	m_Components.emplace(pComponentTag, pComponent);

	*ppOut = pComponent;
	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& rPair : m_Components)
		Safe_Release(rPair.second);

	m_Components.clear();

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pGameInstance);
}