#include "stdafx.h"
#include "..\Public\Effect_Gravity_Umbrella.h"
#include "Effect_Env_Particle.h"

CEffect_Gravity_Umbrella::CEffect_Gravity_Umbrella(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Gravity_Umbrella::CEffect_Gravity_Umbrella(const CEffect_Gravity_Umbrella & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_Gravity_Umbrella::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Gravity_Umbrella::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_flowmaptest", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Wormhole_Noise", TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Effect", Level::LEVEL_STAGE, L"GameObject_2D_Env_Particle", nullptr, (CGameObject**)&m_pParticle);
	m_pParticle->Set_InstanceCount(5000);
	m_pTransformCom->Set_Scale(XMVectorSet(3.05f, 1.65f, 3.05f, 0.f));

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(63.75f, 72.35f, 196.f, 1.f));
	return S_OK;
}

_int CEffect_Gravity_Umbrella::Tick(_double TimeDelta)
{

	m_fTime += (_float)TimeDelta * 0.1f;

	if (3.f <= m_fTime)
		m_fTime = 0.f;

	if (m_pGameInstance->Key_Down(DIK_NUMPAD5))
		m_pParticle->Set_IsActivateParticles(true);
	if (m_pGameInstance->Key_Down(DIK_NUMPAD6))
		m_pParticle->Set_IsActivateParticles(false);

	m_pParticle->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrix());

	m_pParticle->Set_Particle_Radius(_float3(5.f, 40.f, 5.f));
	return _int();
}

_int CEffect_Gravity_Umbrella::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_Gravity_Umbrella::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(5);

	return S_OK;
}

void CEffect_Gravity_Umbrella::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_Gravity_Umbrella::Ready_Instance()
{
	return S_OK;
}

CEffect_Gravity_Umbrella * CEffect_Gravity_Umbrella::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Gravity_Umbrella*	pInstance = new CEffect_Gravity_Umbrella(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Gravity_Umbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Gravity_Umbrella::Clone_GameObject(void * pArg)
{
	CEffect_Gravity_Umbrella* pInstance = new CEffect_Gravity_Umbrella(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Gravity_Umbrella");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Gravity_Umbrella::Free()
{
	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_ColorRamp);

	Safe_Release(m_pParticle);
	__super::Free();
}
