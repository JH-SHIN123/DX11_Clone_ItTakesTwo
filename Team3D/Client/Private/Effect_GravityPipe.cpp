#include "stdafx.h"
#include "Effect_GravityPipe.h"
#include "GameInstance.h"
#include "TriggerActor.h"
#include "Cody.h"
#include "May.h"
#include "Effect_Env_Particle.h"

CEffect_GravityPipe::CEffect_GravityPipe(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_GravityPipe::CEffect_GravityPipe(const CEffect_GravityPipe & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_GravityPipe::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_GravityPipe::NativeConstruct(void * pArg)
{
	__super::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_EffectDesc_Prototype.ModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_flowmaptest", TEXT("Com_Texture_Color"), (CComponent**)&m_pTexturesCom_ColorRamp), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, L"Component_Texture_Wormhole_Noise", TEXT("Com_Texture_Distortion"), (CComponent**)&m_pTexturesCom_Distortion), E_FAIL);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Env_Effect", Level::LEVEL_STAGE, L"GameObject_2D_Env_Particle", nullptr, (CGameObject**)&m_pParticle);
	m_pParticle->Set_InstanceCount(5000);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pPhysxTransformCom), E_FAIL);

	_matrix PhysxWorldMatrix = XMMatrixIdentity();
	_vector vTrans = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	PhysxWorldMatrix = XMMatrixRotationZ(XMConvertToRadians(90.f)) * XMMatrixTranslation(XMVectorGetX(vTrans), XMVectorGetY(vTrans), XMVectorGetZ(vTrans));
	m_pPhysxTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pPhysxTransformCom->Set_WorldMatrix(PhysxWorldMatrix);


	CTriggerActor::ARG_DESC ArgDesc;
	m_UserData = USERDATA(GameID::eGRAVITYPIPE, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pPhysxTransformCom;
	ArgDesc.pGeometry = new PxCapsuleGeometry(4.f, 20.f);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);
	Safe_Delete(ArgDesc.pGeometry);

	return S_OK;
}

_int CEffect_GravityPipe::Tick(_double TimeDelta)
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

_int CEffect_GravityPipe::Late_Tick(_double TimeDelta)
{
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
}

HRESULT CEffect_GravityPipe::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelCom->Set_Variable("g_fTime", &m_fTime, sizeof(_float));
	m_pModelCom->Set_ShaderResourceView("g_ColorRampTexture", m_pTexturesCom_ColorRamp->Get_ShaderResourceView(0));
	m_pModelCom->Set_ShaderResourceView("g_DistortionTexture", m_pTexturesCom_Distortion->Get_ShaderResourceView(0));
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Render_Model(5);

	return S_OK;
}

void CEffect_GravityPipe::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

void CEffect_GravityPipe::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	// Cody

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eCODY)
	{
		((CCody*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}

	// May

	if (eStatus == TriggerStatus::eFOUND && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, true, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
	else if (eStatus == TriggerStatus::eLOST && eID == GameID::Enum::eMAY)
	{
		((CMay*)pGameObject)->SetTriggerID(GameID::Enum::eGRAVITYPIPE, false, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	}
}

HRESULT CEffect_GravityPipe::Ready_Instance()
{
	return S_OK;
}

CEffect_GravityPipe * CEffect_GravityPipe::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_GravityPipe*	pInstance = new CEffect_GravityPipe(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_GravityPipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_GravityPipe::Clone_GameObject(void * pArg)
{
	CEffect_GravityPipe* pInstance = new CEffect_GravityPipe(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_GravityPipe");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_GravityPipe::Free()
{
	Safe_Release(m_pPhysxTransformCom);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTexturesCom_Distortion);
	Safe_Release(m_pTexturesCom_ColorRamp);

	Safe_Release(m_pParticle); 
	__super::Free();
}
