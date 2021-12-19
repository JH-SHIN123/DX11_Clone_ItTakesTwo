#include "stdafx.h"
#include "..\Public\Effect_Cody_Size_ShockWave.h"

CEffect_Cody_Size_ShockWave::CEffect_Cody_Size_ShockWave(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Cody_Size_ShockWave::CEffect_Cody_Size_ShockWave(const CEffect_Cody_Size_ShockWave & rhs)
	: CInGameEffect_Model(rhs)
{
}


HRESULT CEffect_Cody_Size_ShockWave::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Cody_Size_ShockWave::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	if (EFFECT_DESC_CLONE::PV_CODY == m_EffectDesc_Clone.iPlayerValue) // Ä¿ÁöÀÚ
	{
		m_vScale = { 0.01f, 0.01f, 0.01f };
		m_IsScaling_Bigger = true;
	}
	else if (EFFECT_DESC_CLONE::PV_CODY_L == m_EffectDesc_Clone.iPlayerValue) // wkrdkwlwk	
		m_vScale = { 10.75f, 10.75f, 10.75f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

	m_fAlphaTime = 0.0175f;

	return S_OK;
}

_int CEffect_Cody_Size_ShockWave::Tick(_double TimeDelta)
{
	if (2.0 <= m_dLifeTime)
		return EVENT_DEAD;

	m_dLifeTime += TimeDelta;

	m_fAlphaTime -= (_float)TimeDelta * 0.05f;
	if (0.f > m_fAlphaTime) m_fAlphaTime = 0.f;

	if (true == m_IsScaling_Bigger)
	{
		m_vScale.x += (_float)TimeDelta * 30.f;
		m_vScale.y = m_vScale.x;
		m_vScale.z = m_vScale.x;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
	}
	else
	{
		m_vScale.x -= (_float)TimeDelta * 25.f;
		if (0.f > m_vScale.x) m_vScale.x = 0.f;
		m_vScale.y = m_vScale.x;
		m_vScale.z = m_vScale.x;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
	}

	return _int();
}

_int CEffect_Cody_Size_ShockWave::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Cody_Size_ShockWave::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;

	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(14);

	return S_OK;
}

void CEffect_Cody_Size_ShockWave::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

CEffect_Cody_Size_ShockWave * CEffect_Cody_Size_ShockWave::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Cody_Size_ShockWave*	pInstance = new CEffect_Cody_Size_ShockWave(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Cody_Size_ShockWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Cody_Size_ShockWave::Clone_GameObject(void * pArg)
{
	CEffect_Cody_Size_ShockWave* pInstance = new CEffect_Cody_Size_ShockWave(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Cody_Size_ShockWave");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Cody_Size_ShockWave::Free()
{
	__super::Free();
}
