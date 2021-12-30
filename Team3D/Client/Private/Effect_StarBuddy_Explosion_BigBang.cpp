#include "stdafx.h"
#include "..\Public\Effect_StarBuddy_Explosion_BigBang.h"
#include "Effect_Generator.h"

CEffect_StarBuddy_Explosion_BigBang::CEffect_StarBuddy_Explosion_BigBang(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_StarBuddy_Explosion_BigBang::CEffect_StarBuddy_Explosion_BigBang(const CEffect_StarBuddy_Explosion_BigBang & rhs)
	: CInGameEffect_Model(rhs)
{
}


HRESULT CEffect_StarBuddy_Explosion_BigBang::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_StarBuddy_Explosion_BigBang::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody_Size_ShokeWave"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));

	m_fAlphaTime = 0.0175f;

	return S_OK;
}

_int CEffect_StarBuddy_Explosion_BigBang::Tick(_double TimeDelta)
{
	if (1.2 <= m_dLifeTime)
		return EVENT_DEAD;

	m_dLifeTime += TimeDelta;
	if (1.2 < m_dLifeTime && true == m_IsScaling)
	{
		EFFECT->Add_Effect(Effect_Value::StarBuddy_Explosion_Particle, m_pTransformCom->Get_WorldMatrix());
		EFFECT->Add_Effect(Effect_Value::StarBuddy_Ring, m_pTransformCom->Get_WorldMatrix());
		//////m_fScalePow = 10.f;
		m_IsScaling = false;
	}
 
	if (true == m_IsScaling)
	{
		m_fAlphaTime += (_float)TimeDelta * 0.75f;
		if (1.f < m_fAlphaTime) m_fAlphaTime = 1.f;

		//////m_fScalePow -= (_float)TimeDelta * 8.f;
		//////if (0.f > m_fScalePow)
		//////	m_fScalePow = 0.f;

		m_vScale.x += (_float)TimeDelta/* * m_fScalePow * m_fScalePow */* 7.f;
		m_vScale.y = m_vScale.x;
		m_vScale.z = m_vScale.x;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
	}
	else
	{
		//////m_fAlphaTime -= (_float)TimeDelta;
		//////if (0.f > m_fAlphaTime) m_fAlphaTime = 0.f;

		//m_fScalePow -= (_float)TimeDelta * 15.f;
		//if (2.5f > m_fScalePow)
		//	m_fScalePow = 2.5f;

		//m_vScale.x += (_float)TimeDelta * m_fScalePow * m_fScalePow;
		//m_vScale.y = m_vScale.x;
		//m_vScale.z = m_vScale.x;
		//m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
	}
	//else
	//{
	//	m_fAlphaTime -= (_float)TimeDelta * 2.f;
	//	if (0.f >= m_fAlphaTime) m_fAlphaTime = 0.f;
	//
	//	m_fScalePow -= (_float)TimeDelta * 5.f;
	//	if (0.f >= m_fScalePow)
	//		m_fScalePow = 0.f;
	//
	//	m_vScale.x -= (_float)TimeDelta * m_fScalePow * m_fScalePow;
	//	if (0.f >= m_vScale.x)
	//		m_vScale.x = 0.f;
	//
	//	m_vScale.y = m_vScale.x;
	//	m_vScale.z = m_vScale.x;
	//
	//
	//	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vScale));
	//}

	return _int();
}

_int CEffect_StarBuddy_Explosion_BigBang::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_StarBuddy_Explosion_BigBang::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;

	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(14);

	return S_OK;
}

void CEffect_StarBuddy_Explosion_BigBang::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

CEffect_StarBuddy_Explosion_BigBang * CEffect_StarBuddy_Explosion_BigBang::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_StarBuddy_Explosion_BigBang*	pInstance = new CEffect_StarBuddy_Explosion_BigBang(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_StarBuddy_Explosion_BigBang");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_StarBuddy_Explosion_BigBang::Clone_GameObject(void * pArg)
{
	CEffect_StarBuddy_Explosion_BigBang* pInstance = new CEffect_StarBuddy_Explosion_BigBang(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_StarBuddy_Explosion_BigBang");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_StarBuddy_Explosion_BigBang::Free()
{
	__super::Free();
}
