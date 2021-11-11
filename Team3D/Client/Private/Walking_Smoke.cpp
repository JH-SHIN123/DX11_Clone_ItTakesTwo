#include "stdafx.h"
#include "..\Public\Walking_Smoke.h"
#include "GameInstance.h"

CWalking_Smoke::CWalking_Smoke(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CWalking_Smoke::CWalking_Smoke(const CWalking_Smoke & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CWalking_Smoke::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CWalking_Smoke::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CWalking_Smoke::Tick(_double TimeDelta)
{
	m_fLifeTime -= TimeDelta;

	for(_int iIndex = 0; iIndex < m_)
	m_pInstanceBuffer[0].vTextureUV = Check_UV(TimeDelta);




	return _int();
}

_int CWalking_Smoke::Late_Tick(_double TimeDelta)
{
	if (0.f >= m_fLifeTime)
		return EVENT_DEAD;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CWalking_Smoke::Render()
{
	SetUp_Shader_Data();

	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom->Render(0, m_pInstanceBuffer, m_pEffectDesc_Prototype->iInstanceCount);

	return S_OK;
}

CWalking_Smoke * CWalking_Smoke::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CWalking_Smoke*	pInstance = new CWalking_Smoke(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CWalking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWalking_Smoke::Clone_GameObject(void * pArg)
{
	CWalking_Smoke*	pInstance = new CWalking_Smoke(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CWalking_Smoke");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWalking_Smoke::Free()
{
	__super::Free();
}
