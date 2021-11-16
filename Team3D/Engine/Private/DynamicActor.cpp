#include "..\Public\DynamicActor.h"
#include "PhysX.h"

CDynamicActor::CDynamicActor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CActor(pDevice, pDeviceContext)
{
}

CDynamicActor::CDynamicActor(const CDynamicActor & rhs)
	: CActor(rhs)
{
}

HRESULT CDynamicActor::NativeConstruct_Prototype()
{
	CActor::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CDynamicActor::NativeConstruct(void * pArg)
{
	CActor::NativeConstruct(pArg);

	return S_OK;
}

CDynamicActor * CDynamicActor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDynamicActor* pInstance = new CDynamicActor(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CDynamicActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CDynamicActor::Clone_Component(void * pArg)
{
	CDynamicActor* pInstance = new CDynamicActor(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CDynamicActor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDynamicActor::Free()
{
	if (true == m_isClone)
		m_pPhysX->Remove_Actor(&m_pActor);

	CActor::Free();
}