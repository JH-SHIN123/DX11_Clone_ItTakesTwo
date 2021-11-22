#include "stdafx.h"
#include "..\Public\Effect_RespawnTunnel.h"
#include "GameInstance.h"

CEffect_RespawnTunnel::CEffect_RespawnTunnel(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_RespawnTunnel::CEffect_RespawnTunnel(const CEffect_RespawnTunnel & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_RespawnTunnel::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_RespawnTunnel::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Color_Ramp", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Tilling_Noise", TEXT("Com_Texture_Distor"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	return S_OK;
}

_int CEffect_RespawnTunnel::Tick(_double TimeDelta)
{
	m_fTime_UV += (_float)TimeDelta * 0.05f;
	if (1.f <= m_fTime_UV)
		m_fTime_UV = 0.f;


	return _int();
}

_int CEffect_RespawnTunnel::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
}

HRESULT CEffect_RespawnTunnel::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

// 	_float fRadian = XMConvertToRadians((_float)m_dAngle);
// 	//g_vColor
// 
// 	m_pModelCom->Set_Variable("g_vColorRamp_UV", &vColorRampUV, sizeof(_float2));
 	m_pModelCom->Set_Variable("g_fTime", &m_fTime_UV, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(2));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(12));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(7);

	return S_OK;
}

void CEffect_RespawnTunnel::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

CEffect_RespawnTunnel * CEffect_RespawnTunnel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_RespawnTunnel*	pInstance = new CEffect_RespawnTunnel(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_RespawnTunnel::Clone_GameObject(void * pArg)
{
	CEffect_RespawnTunnel* pInstance = new CEffect_RespawnTunnel(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_RespawnTunnel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_RespawnTunnel::Free()
{
	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_ColorRamp);

	__super::Free();
}
