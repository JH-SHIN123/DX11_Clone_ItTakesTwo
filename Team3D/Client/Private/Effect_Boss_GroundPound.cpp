#include "stdafx.h"
#include "..\Public\Effect_Boss_GroundPound.h"

CEffect_Boss_GroundPound::CEffect_Boss_GroundPound(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Boss_GroundPound::CEffect_Boss_GroundPound(const CEffect_Boss_GroundPound & rhs)
	: CInGameEffect_Model(rhs)
{
}


HRESULT CEffect_Boss_GroundPound::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_GroundPound::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Tilling_Noise"), TEXT("Com_Textures_Distortion"), (CComponent**)&m_pTexture_Distortion), E_FAIL);

	m_pTransformCom->Set_Scale(XMVectorSet(3.75f, 1.25f, 3.75f, 0.f));

	return S_OK;
}

_int CEffect_Boss_GroundPound::Tick(_double TimeDelta)
{
	//if (0.0 >= m_dLifeTime)
	//	return EVENT_DEAD;

	m_fTime -= (_float)TimeDelta * 0.25f;
	if (0.f >= m_fTime)
		m_fTime = 1.f;

	m_dLifeTime -= TimeDelta;
	if (4 > m_dLifeTime)

	m_fAlphaTime += (_float)TimeDelta * 0.333333f;
	if (1.f <= m_fAlphaTime)
		m_fAlphaTime = 1.f;

	m_fTimeWeight += (_float)TimeDelta * 100.f;
	if (360.f <= m_fTimeWeight)
		m_fTimeWeight = 0.f;

	return _int();
}

_int CEffect_Boss_GroundPound::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_GroundPound::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = m_fAlphaTime;
	if (1.f < fAlpha) fAlpha = 1.f;

	_float fSin = (sinf(XMConvertToRadians(m_fTimeWeight)) + 1.f) / 2.f;
	
	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pModelCom->Set_Variable("g_fTimeWeight", &fSin, sizeof(_float));

	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexture_Distortion->Get_ShaderResourceView(1));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(11);

	return S_OK;
}

void CEffect_Boss_GroundPound::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_Boss_GroundPound::Scale_Check(_double TimeDelta)
{
	if (true == m_IsScaling_End)
		return;

	_float fScale = m_pTransformCom->Get_Scale(CTransform::STATE_RIGHT);

	fScale += (_float)TimeDelta * m_fScale_Pow;

	if (m_fScale_Max <= fScale)
	{
		fScale = m_fScale_Max;
		m_IsScaling_End = true;
	}

	_vector vScale = XMVectorSet(fScale, fScale, fScale, 0.f);
	m_pTransformCom->Set_Scale(vScale);
}

CEffect_Boss_GroundPound * CEffect_Boss_GroundPound::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_GroundPound*	pInstance = new CEffect_Boss_GroundPound(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_GroundPound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_GroundPound::Clone_GameObject(void * pArg)
{
	CEffect_Boss_GroundPound* pInstance = new CEffect_Boss_GroundPound(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_GroundPound");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_GroundPound::Free()
{
	Safe_Release(m_pTexture_Distortion);

	__super::Free();
}
