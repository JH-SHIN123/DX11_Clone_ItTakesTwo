#include "stdafx.h"
#include "..\Public\Effect_Umbrella_Pipe.h"
#include "Effect_Env_Particle.h"

CEffect_Umbrella_Pipe::CEffect_Umbrella_Pipe(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_Umbrella_Pipe::CEffect_Umbrella_Pipe(const CEffect_Umbrella_Pipe & rhs)
	: CInGameEffect_Model(rhs)
{
}

void CEffect_Umbrella_Pipe::Set_ParentWorldMatrix(_matrix ParentMatrix)
{
	XMStoreFloat4x4(&m_matParent, ParentMatrix);
}

HRESULT CEffect_Umbrella_Pipe::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_Umbrella_Pipe::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Wormhole_Noise", TEXT("Com_Texture_Diffuse"), (CComponent**)&m_pTexturesCom_Diffuse), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_flowmaptest", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Wormhole_Noise", TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	CEffect_Env_Particle::EParticle_Type Particle_Type;
	Particle_Type = CEffect_Env_Particle::Umbrella;
	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Effect", Level::LEVEL_STAGE, L"GameObject_2D_Env_Particle", &Particle_Type, (CGameObject**)&m_pParticle);
	m_pParticle->Set_InstanceCount(2500);
	m_pParticle->Set_Particle_Radius(_float3(2.25f, 18.f, 2.25f));

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vPos.m128_f32[1] += 6.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(63.75f, 72.35f, 196.f, 1.f));
	XMStoreFloat4x4(&m_matParent, XMMatrixIdentity());

	return S_OK;
}

_int CEffect_Umbrella_Pipe::Tick(_double TimeDelta)
{
	m_fTime += (_float)TimeDelta * 0.1f;

	if (3.f <= m_fTime)
		m_fTime = 0.f;

	if (m_pGameInstance->Key_Down(DIK_V))
		m_IsActivate = false;
	if (m_pGameInstance->Key_Down(DIK_C))
		m_IsActivate = true;

	Activate_Check(TimeDelta);

	if (0.0 < m_dActivateTime)
	{
		_matrix matWorld, matScale, matRotX, matTrans;
		matScale = XMMatrixScaling(1.5f, 3.f, 1.5f);
		matTrans = XMMatrixTranslation(0.f, 0.5f, 3.f);
		matRotX = XMMatrixRotationX(XMConvertToRadians(90.f));
		matWorld = matScale * matRotX * matTrans * XMLoadFloat4x4(&m_matParent);
		m_pTransformCom->Set_WorldMatrix(matWorld);
	
		m_pParticle->Set_ParentMatrix(m_pTransformCom->Get_WorldMatrix());	
	}

	return NO_EVENT;
}

_int CEffect_Umbrella_Pipe::Late_Tick(_double TimeDelta)
{
	if (0.0 >= m_dActivateTime)
		return NO_EVENT;

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 50.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT CEffect_Umbrella_Pipe::Render(RENDER_GROUP::Enum eGroup)
{
	_float fAlpha = (_float)m_dActivateTime;

	m_pModelCom->Set_Variable("g_fAlpha", &fAlpha, sizeof(_float));
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture_Second", m_pTexturesCom_Diffuse->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(9);

	return S_OK;
}

void CEffect_Umbrella_Pipe::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_Umbrella_Pipe::Ready_Instance()
{
	return S_OK;
}

void CEffect_Umbrella_Pipe::Set_Activate(_bool IsActivate)
{
	m_IsActivate = IsActivate;
}

void CEffect_Umbrella_Pipe::Activate_Check(_double TimeDelta)
{
	if (true == m_IsActivate)
	{
		m_dActivateTime += TimeDelta;
		if (1.0 < m_dActivateTime)
			m_dActivateTime = 1.0;
	}
	else
	{
		m_dActivateTime -= TimeDelta;
		if (0.0 > m_dActivateTime)
			m_dActivateTime = 0.0;
	}

	m_pParticle->Set_ControlTime(m_dActivateTime);
}

CEffect_Umbrella_Pipe * CEffect_Umbrella_Pipe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_Umbrella_Pipe*	pInstance = new CEffect_Umbrella_Pipe(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_Umbrella_Pipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Umbrella_Pipe::Clone_GameObject(void * pArg)
{
	CEffect_Umbrella_Pipe* pInstance = new CEffect_Umbrella_Pipe(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_Umbrella_Pipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_Umbrella_Pipe::Free()
{
	Safe_Release(m_pTexturesCom_Diffuse);
	Safe_Release(m_pTexturesCom_ColorRamp);
	Safe_Release(m_pTexturesCom_Distortion);

	Safe_Release(m_pParticle);

	__super::Free();
}
