#include "stdafx.h"
#include "..\Public\Effect_Boss_Gravitational_Bomb_Pillar.h"

CEffect_Boss_Gravitational_Bomb_Pillar::CEffect_Boss_Gravitational_Bomb_Pillar(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect(pDevice, pDeviceContext)
{
}

CEffect_Boss_Gravitational_Bomb_Pillar::CEffect_Boss_Gravitational_Bomb_Pillar(const CEffect_Boss_Gravitational_Bomb_Pillar & rhs)
	: CInGameEffect(rhs)
{
}

HRESULT CEffect_Boss_Gravitational_Bomb_Pillar::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Boss_Gravitational_Bomb_Pillar::NativeConstruct(void * pArg)
{
	__super::Ready_Component(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Texture_Color_Ramp"), TEXT("Com_Tex"), (CComponent**)&m_pTexturesCom), E_FAIL);

	Ready_Instance();

	m_EffectDesc_Prototype.fLifeTime = 3.f;

	return S_OK;
}

_int CEffect_Boss_Gravitational_Bomb_Pillar::Tick(_double TimeDelta)
{
	if (0.f >= m_EffectDesc_Prototype.fLifeTime)
		return EVENT_DEAD;

	m_EffectDesc_Prototype.fLifeTime -= (_float)TimeDelta;

	m_fTime -= (_float)TimeDelta * 0.125f;

	Instance_Size((_float)TimeDelta);

	return _int();
}

_int CEffect_Boss_Gravitational_Bomb_Pillar::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Boss_Gravitational_Bomb_Pillar::Render(RENDER_GROUP::Enum eGroup)
{
	m_pPointInstanceCom->Set_DefaultVariables();
	m_pPointInstanceCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));

	m_pPointInstanceCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pPointInstanceCom->Set_ShaderResourceView("g_DiffuseTexture", m_pTexturesCom->Get_ShaderResourceView(13));

	m_pPointInstanceCom->Render(15, m_pInstanceBuffer, 1);

	return S_OK;
}

void CEffect_Boss_Gravitational_Bomb_Pillar::Instance_Size(_float TimeDelta, _int iIndex)
{
	_float2 vSize_Now = m_pInstanceBuffer[iIndex].vSize;

	vSize_Now.x += (_float)TimeDelta * 2.25f;
	vSize_Now.y += (_float)TimeDelta * 70.f;

	if (m_vSize_Max.x <= vSize_Now.x)
		vSize_Now.x = m_vSize_Max.x;
	//if (m_vSize_Max.y <= vSize_Now.y)
	//	vSize_Now.y = m_vSize_Max.y;

	m_pInstanceBuffer[iIndex].vSize = vSize_Now;
}

void CEffect_Boss_Gravitational_Bomb_Pillar::Instance_Pos(_float TimeDelta, _int iIndex)
{
}

void CEffect_Boss_Gravitational_Bomb_Pillar::Instance_UV(_float TimeDelta, _int iIndex)
{
}

HRESULT CEffect_Boss_Gravitational_Bomb_Pillar::Ready_Instance()
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

CEffect_Boss_Gravitational_Bomb_Pillar * CEffect_Boss_Gravitational_Bomb_Pillar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Boss_Gravitational_Bomb_Pillar*	pInstance = new CEffect_Boss_Gravitational_Bomb_Pillar(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Boss_Gravitational_Bomb_Pillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Boss_Gravitational_Bomb_Pillar::Clone_GameObject(void * pArg)
{
	CEffect_Boss_Gravitational_Bomb_Pillar* pInstance = new CEffect_Boss_Gravitational_Bomb_Pillar(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Boss_Gravitational_Bomb_Pillar");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Boss_Gravitational_Bomb_Pillar::Free()
{
	__super::Free();
}
