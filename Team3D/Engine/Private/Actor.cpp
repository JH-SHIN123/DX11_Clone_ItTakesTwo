#include "..\Public\Actor.h"
#include "PhysX.h"

CActor::CActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CComponent(pDevice, pDeviceContext)
	, m_pPhysX(CPhysX::GetInstance())
{
	Safe_AddRef(m_pPhysX);
}

CActor::CActor(const CActor & rhs)
	: CComponent(rhs)
	, m_pPhysX(rhs.m_pPhysX)
{
	Safe_AddRef(m_pPhysX);
}

HRESULT CActor::NativeConstruct_Prototype()
{
	CComponent::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CActor::NativeConstruct(void * pArg)
{
	CComponent::NativeConstruct(pArg);

	return S_OK;
}

void CActor::Free()
{
	Safe_Release(m_pPhysX);

	CComponent::Free();
}
