#include "stdafx.h"
#include "..\Public\RespawnTunnel.h"
#include "GameInstance.h"

CRespawnTunnel::CRespawnTunnel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameEffect(pDevice, pDeviceContext)
{
}

CRespawnTunnel::CRespawnTunnel(const CRespawnTunnel & rhs)
	: CGameEffect(rhs)
{
}

HRESULT CRespawnTunnel::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CRespawnTunnel::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	return S_OK;
}

_int CRespawnTunnel::Tick(_double TimeDelta)
{
	return _int();
}

_int CRespawnTunnel::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CRespawnTunnel::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	//m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CRespawnTunnel::Set_ShaderConstant_Default()
{
	return __super::Set_ShaderConstant_Default();
}

HRESULT CRespawnTunnel::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	return __super::Set_ShaderConstant_Shadow(LightViewMatrix, LightViewMatrix);
}

CRespawnTunnel * CRespawnTunnel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CRespawnTunnel*	pInstance = new CRespawnTunnel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CRespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRespawnTunnel::Clone_GameObject(void * pArg)
{
	CRespawnTunnel* pInstance = new CRespawnTunnel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRespawnTunnel::Free()
{
	__super::Free();
}
