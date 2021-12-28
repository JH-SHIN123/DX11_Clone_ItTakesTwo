#include "stdafx.h"
#include "..\Public\Effect_StarBuddy_Explosion_Pillar.h"
#include "Effect_Generator.h"

CEffect_StarBuddy_Explosion_Pillar::CEffect_StarBuddy_Explosion_Pillar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_StarBuddy_Explosion_Pillar::CEffect_StarBuddy_Explosion_Pillar(const CEffect_StarBuddy_Explosion_Pillar & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_StarBuddy_Explosion_Pillar::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_StarBuddy_Explosion_Pillar::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);


	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_PointInstance_Custom"), TEXT("Com_VIBuffer"), (CComponent**)&m_pPointInstanceCom), E_FAIL);

	_matrix  WolrdMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);

	m_pTransformCom->Set_WorldMatrix(WolrdMatrix);	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Tex"), (CComponent**)&m_pTexturesCom), E_FAIL);

	Ready_Instance();

	return S_OK;
}

_int CEffect_StarBuddy_Explosion_Pillar::Tick(_double TimeDelta)
{
	if (4.0 <= m_dActivateTime)
		return EVENT_DEAD;

	m_dActivateTime += TimeDelta;
	if (0.75 < m_dActivateTime && false == m_IsEffect)
	{
		m_IsEffect = true;
		EFFECT->Add_Effect(Effect_Value::StarBuddy_Explosion_BigBang, m_pTransformCom->Get_WorldMatrix());
	}
	if (3.0 < m_dActivateTime && true == m_IsActivate)
		m_IsActivate = false;

	m_fTime -= (_float)TimeDelta * 0.5f;

	Instance_Size((_float)TimeDelta);

	return _int();
}

_int CEffect_StarBuddy_Explosion_Pillar::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_StarBuddy_Explosion_Pillar::Render(RENDER_GROUP::Enum eGroup)
{
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));

	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(13));

	m_pPointInstanceCom->Render(23, m_pInstanceBuffer, 1);

	return S_OK;
}

void CEffect_StarBuddy_Explosion_Pillar::Instance_Size(_float TimeDelta, _int iIndex)
{
	_float2 vSize_Now = m_pInstanceBuffer[iIndex].vSize;
	if (true == m_IsActivate)
	{
		vSize_Now.x += (_float)TimeDelta * 2.25f;
		vSize_Now.y += (_float)TimeDelta * 500.f;
		if (m_vSize_Max.x <= vSize_Now.x) vSize_Now.x = m_vSize_Max.x;
	}
	else
	{
		vSize_Now.x -= (_float)TimeDelta * 2.25f;
		if (0.f >= vSize_Now.x) vSize_Now.x = 0.f;
	}

	m_pInstanceBuffer[iIndex].vSize = vSize_Now;
}

void CEffect_StarBuddy_Explosion_Pillar::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_StarBuddy_Explosion_Pillar::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_StarBuddy_Explosion_Pillar::Ready_Instance()
{
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST;

	m_pInstanceBuffer->vRight = { 1.f, 0.f, 0.f, 0.f };
	m_pInstanceBuffer->vUp = { 0.f, 1.f, 0.f, 0.f };
	m_pInstanceBuffer->vLook = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vSize = m_vSize;
	m_pInstanceBuffer->vTextureUV = { 0.f, 1.f, 1.f, 3.f };
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_pInstanceBuffer->vPosition, vPos);


	return S_OK;
}

CEffect_StarBuddy_Explosion_Pillar * CEffect_StarBuddy_Explosion_Pillar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_StarBuddy_Explosion_Pillar*	pInstance = new CEffect_StarBuddy_Explosion_Pillar(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_StarBuddy_Explosion_Pillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_StarBuddy_Explosion_Pillar::Clone_GameObject(void * pArg)
{
	CEffect_StarBuddy_Explosion_Pillar* pInstance = new CEffect_StarBuddy_Explosion_Pillar(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_StarBuddy_Explosion_Pillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_StarBuddy_Explosion_Pillar::Free()
{
	__super::Free();
}
