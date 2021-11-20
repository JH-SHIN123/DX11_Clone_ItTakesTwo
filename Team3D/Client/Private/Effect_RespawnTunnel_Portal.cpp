#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel_Portal.h"
#include "GameInstance.h"

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel_Portal::CEffect_RespawnTunnel_Portal(const CEffect_RespawnTunnel_Portal & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel_Portal::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	CModel_Instance::ARG_DESC Data;
	char szActorName[MAX_PATH] = "";
	WideCharToMultiByte(CP_ACP, 0, m_EffectDesc_Prototype.ModelName, MAX_PATH, szActorName, MAX_PATH, NULL, NULL);
	Data.pActorName = szActorName;
	Data.fCullingRadius = m_EffectDesc_Clone.fCullingRadius;
	Data.pWorldMatrices = m_pInstanceBuffer;
	Data.iInstanceCount = m_EffectDesc_Prototype.iInstanceCount;

	for (_uint i = 0; i < Data.iInstanceCount; ++i)
	{
		Data.pWorldMatrices[i] = MH_XMFloat4x4Identity();
		Data.pWorldMatrices[i]._41 = _float((i % 100) * 10.f);
		Data.pWorldMatrices[i]._43 = _float((i / 100) * 10.f);
	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelInstanceCom, &Data), E_FAIL);
	//m_pTexturesCom_ColorRamp
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Color_Ramp", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp, &Data), E_FAIL);

	m_vColor = { 0.392156899f, 0.584313750f, 0.929411829f, 1.000000000f };
	return S_OK;
}

_int CEffect_RespawnTunnel_Portal::Tick(_double TimeDelta)
{
	m_dAngle += 0.8;
	if (720.0 <= m_dAngle)
		m_dAngle = 0.0;

	m_fColorRamp_U += (_float)TimeDelta * 0.15f;

	if (2.f <= m_fColorRamp_U)
		m_fColorRamp_U = 0.f;

	return _int();
}

_int CEffect_RespawnTunnel_Portal::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_ALPHA, this);
}

HRESULT CEffect_RespawnTunnel_Portal::Render()
{
	NULL_CHECK_RETURN(m_pModelInstanceCom, E_FAIL);

	_float fRadian = XMConvertToRadians((_float)m_dAngle);
	//g_vColor

	_float2 vColorRampUV = { m_fColorRamp_U, 0.f };
	m_pModelInstanceCom->Set_Variable("g_vColorRamp_UV",&vColorRampUV,	sizeof(_float2));
	m_pModelInstanceCom->Set_Variable("g_fRadianAngle",&fRadian,	sizeof(_float));
	m_pModelInstanceCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(11));
	m_pModelInstanceCom->Set_DefaultVariables_Perspective();
	m_pModelInstanceCom->Render_Model(4);

	return S_OK;
}

void CEffect_RespawnTunnel_Portal::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_RespawnTunnel_Portal::Ready_Instance()
{



	return S_OK;
}

void CEffect_RespawnTunnel_Portal::Color_Check(_double TimeDelta)
{
}

CEffect_RespawnTunnel_Portal * CEffect_RespawnTunnel_Portal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel_Portal*	pInstance = new CEffect_RespawnTunnel_Portal(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel_Portal::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel_Portal* pInstance = new CEffect_RespawnTunnel_Portal(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel_Portal");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel_Portal::Free()
{
	Safe_Release(m_pTexturesCom_ColorRamp);
	__super::Free();
}
