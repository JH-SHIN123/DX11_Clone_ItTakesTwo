#include "stdafx.h"
#include "..\Public\Effect_Boss_GroundPound_Ring.h"

#include "stdafx.h"
#include "..\Public\Effect_Boss_Gravitational_Bomb_Pillar.h"

CEffect_Boss_GroundPound_Ring::CEffect_Boss_GroundPound_Ring(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_GroundPound_Ring::CEffect_Boss_GroundPound_Ring(const CEffect_Boss_GroundPound_Ring & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_GroundPound_Ring::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_GroundPound_Ring::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_VIBuffer_Rect_TripleUV"), TEXT("Com_VIBuffer_Rect"), (CComponent**)&m_pBufferRectCom), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Slime_Cloud"), TEXT("Com_Tex1"), (CComponent**)&m_pTexturesCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_T_Ring"), TEXT("Com_Tex2"), (CComponent**)&m_pTexturesCom_Second), E_FAIL);

	_matrix WorldMatrix = XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix);
	for (_int i = 0; i < 3; ++i)
		WorldMatrix.r[i] = XMVector3Normalize(WorldMatrix.r[i]);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vPos.m128_f32[1] += 0.3f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Scale(XMVectorSet(13.8f, 1.f, 13.8f, 0.f));

	m_EffectDesc_Prototype.fLifeTime = 3.f;

	return S_OK;
}

_int CEffect_Boss_GroundPound_Ring::Tick(_double TimeDelta)
{
	if (false == m_IsActivate && 0.f >= m_fTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;
	if (true == m_IsActivate)
	{
		m_fTime += (_float)TimeDelta * 0.25f;
		if (0.5f < m_fTime)
			m_fTime = 0.5f;
	}
	else
	{
		m_fTime -= (_float)TimeDelta * 0.25f;
		if (0.0f > m_fTime)
			m_fTime = 0.0f;
	}

	m_dRotateTime += TimeDelta * 20;
	if (360.0 < m_dRotateTime)
		m_dRotateTime = 0.0;

	return _int();
}

_int CEffect_Boss_GroundPound_Ring::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_GroundPound_Ring::Render(RENDER_GROUP::Enum eGroup)
{
	_int i = 0;
	_float fRadian = XMConvertToRadians((_float)m_dRotateTime);
	m_pBufferRectCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());

	m_pBufferRectCom->Set_Variable("g_fRadianAngle", &fRadian, sizeof(_float));
	m_pBufferRectCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pBufferRectCom->Set_Variable("g_fAlpha", &m_fTime, sizeof(_float));

	m_pBufferRectCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Second->Get_ShaderResourceView(0));
	m_pBufferRectCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(0));
	m_pBufferRectCom->Render(2);

	return S_OK;
}

void CEffect_Boss_GroundPound_Ring::Instance_Size(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_GroundPound_Ring::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_GroundPound_Ring::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Boss_GroundPound_Ring::Ready_Instance()
{
	m_pInstanceBuffer = new VTXMATRIX_CUSTOM_ST;

	m_pInstanceBuffer->vRight = { 1.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vUp = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vLook = { 0.f, 0.f, 1.f, 0.f };
	m_pInstanceBuffer->vSize = m_vSize;
	m_pInstanceBuffer->vTextureUV = { 0.f, -1.f, 1.f, 3.f };
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4(&m_pInstanceBuffer->vPosition, vPos);

	return S_OK;
}

void CEffect_Boss_GroundPound_Ring::Check_RotateUV(_double TimeDelta)
{
	m_dRotateTime += TimeDelta * 30;
	if (360.0 < m_dRotateTime)
		m_dRotateTime = 0.0;

	_float fRadian = XMConvertToRadians((_float)m_dRotateTime);



}

CEffect_Boss_GroundPound_Ring * CEffect_Boss_GroundPound_Ring::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_GroundPound_Ring*	pInstance = new CEffect_Boss_GroundPound_Ring(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_GroundPound_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_GroundPound_Ring::Clone_GameObject(void * pArg)
{
	CEffect_Boss_GroundPound_Ring* pInstance = new CEffect_Boss_GroundPound_Ring(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_GroundPound_Ring");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_GroundPound_Ring::Free()
{
	Safe_Release(m_pBufferRectCom);

	__super::Free();
}
