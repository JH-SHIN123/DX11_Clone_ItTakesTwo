#include "stdafx.h"
#include "..\Public\Effect_Env_Particle_Field_Dust.h"


CEffect_Env_Particle_Field_Dust::CEffect_Env_Particle_Field_Dust(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CEffect_Env_Particle_Field(pDevice, pDeviceContext)
{
}

CEffect_Env_Particle_Field_Dust::CEffect_Env_Particle_Field_Dust(const CEffect_Env_Particle_Field & rhs)
	: CEffect_Env_Particle_Field(rhs)
{
}

HRESULT CEffect_Env_Particle_Field_Dust::NativeConstruct_Prototype(void * pArg)
{
	return __super::NativeConstruct_Prototype(pArg);
}

HRESULT CEffect_Env_Particle_Field_Dust::NativeConstruct(void * pArg)
{
	return __super::NativeConstruct(pArg);
}

_int CEffect_Env_Particle_Field_Dust::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_int CEffect_Env_Particle_Field_Dust::Late_Tick(_double TimeDelta)
{	
	return __super::Late_Tick(TimeDelta);
}

HRESULT CEffect_Env_Particle_Field_Dust::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = (_float)m_dControl_Time;
	_float4 vUV = { 0.f, 0.f, 1.f, 1.f };
	m_pPointInstanceCom_STT->Set_DefaultVariables();

	m_pPointInstanceCom_STT->Set_Variable("g_fTime", &fAlpha, sizeof(_float));
	m_pPointInstanceCom_STT->Set_Variable("g_vUV", &vUV, sizeof(_float4));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Set_ShaderResourceView("g_ColorTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	m_pPointInstanceCom_STT->Render(2, m_pInstanceBuffer_STT, m_EffectDesc_Prototype.iInstanceCount);

	return S_OK;
}

CEffect_Env_Particle_Field_Dust * CEffect_Env_Particle_Field_Dust::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Env_Particle_Field_Dust*	pInstance = new CEffect_Env_Particle_Field_Dust(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Env_Particle_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Env_Particle_Field_Dust::Clone_GameObject(void * pArg)
{
	CEffect_Env_Particle_Field_Dust* pInstance = new CEffect_Env_Particle_Field_Dust(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Env_Particle_Field");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Env_Particle_Field_Dust::Free()
{
	__super::Free();
}
