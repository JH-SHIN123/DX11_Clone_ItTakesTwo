#include "stdafx.h"
#include "..\Public\Effect_Boss_Gravitational_Bomb.h"

CEffect_Boss_Gravitational_Bomb::CEffect_Boss_Gravitational_Bomb(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Boss_Gravitational_Bomb::CEffect_Boss_Gravitational_Bomb(const CEffect_Boss_Gravitational_Bomb & rhs)
	: CInGameEffect_Model(rhs)
{
}


HRESULT CEffect_Boss_Gravitational_Bomb::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_Gravitational_Bomb::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_vDefulat_Scale = { 0.75f, 0.75f, 0.75f, 0.f };
	m_pTransformCom->Set_Scale(XMLoadFloat4(&m_vDefulat_Scale));
	m_fScale_Pow = 6;
	return S_OK;
}

_int CEffect_Boss_Gravitational_Bomb::Tick(_double TimeDelta)
{
	if (0.0 >= m_dLifeTime)
		return EVENT_DEAD;

	m_dLifeTime -= TimeDelta;
	if(4 > m_dLifeTime)
		m_fAlphaTime -= (_float)TimeDelta * 0.333333f;

	m_fTime -= (_float)TimeDelta * 0.1f;
	if (0.f >= m_fTime)
		m_fTime = 1.f;

	if (0.f >= m_fAlphaTime)
		m_fAlphaTime = 0.f;

	Explosion_Check();
	Scale_Check(TimeDelta);

	return _int();
}

_int CEffect_Boss_Gravitational_Bomb::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Gravitational_Bomb::Render(RENDER_GROUP::Enum eGroup)
{
	_uint iShaderPass = 10;
	if (false == m_IsExplosion)
		iShaderPass = 12;

	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;

	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(iShaderPass);

	return S_OK;
}

void CEffect_Boss_Gravitational_Bomb::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_Boss_Gravitational_Bomb::Scale_Check(_double TimeDelta)
{
	if (true == m_IsScaling_End || false == m_IsExplosion)
		return;

	_float fScale = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);
	_float fScalePower = (m_fScale_Max - fScale) * 3.f + 0.5f;

	fScale += (_float)TimeDelta * fScalePower;
	m_fScale_Pow -= (_float)TimeDelta;

	if (m_fScale_Max <= fScale)
	{
		fScale = m_fScale_Max;
		m_IsScaling_End = true;
	}

	_vector vScale = XMVectorSet(fScale, fScale, fScale, 0.f);
	m_pTransformCom->Set_Scale(vScale);
}

void CEffect_Boss_Gravitational_Bomb::Explosion_Check()
{
	// ¶¥¿¡ ´ê¾Ò´Ù¸é m_IsExplosion = true;
}

CEffect_Boss_Gravitational_Bomb * CEffect_Boss_Gravitational_Bomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Gravitational_Bomb*	pInstance = new CEffect_Boss_Gravitational_Bomb(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Gravitational_Bomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Gravitational_Bomb::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Gravitational_Bomb* pInstance = new CEffect_Boss_Gravitational_Bomb(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Gravitational_Bomb");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Gravitational_Bomb::Free()
{
	Safe_Release(m_pTexture_Distortion);

	__super::Free();
}
