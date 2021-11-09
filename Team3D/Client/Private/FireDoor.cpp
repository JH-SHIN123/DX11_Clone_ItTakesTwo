#include "stdafx.h"
#include "..\Public\FireDoor.h"
#include "GameInstance.h"

CFireDoor::CFireDoor(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CFireDoor::CFireDoor(const CFireDoor & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CFireDoor::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CFireDoor::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CFireDoor::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	fT += TimeDelta * 0.01f;

	return _int();
}

_int CFireDoor::Late_Tick(_double TimeDelta)
{
	return  m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CFireDoor::Render()
{


	SetUp_Shader_Data();
	m_pPointInstanceCom->Set_Variable("g_fTime", &fT, sizeof(_float));

	m_pPointInstanceCom->Render(4, m_pInstanceBuffer, m_pEffectDesc_Prototype->iInstanceCount);

	return S_OK;
}

CFireDoor * CFireDoor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CFireDoor*	pInstance = new CFireDoor(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CFireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CFireDoor::Clone_GameObject(void * pArg)
{
	CFireDoor* pInstance = new CFireDoor(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CFireDoor");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFireDoor::Free()
{
	__super::Free();
}
