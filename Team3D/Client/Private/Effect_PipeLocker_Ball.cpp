#include "stdafx.h"
#include "..\Public\Effect_PipeLocker_Ball.h"
#include "DataStorage.h"
#include "Effect_PipeLocker_Particle.h"
#include "PressureBigPlate.h"
#include "ControlRoom_Battery.h"

CEffect_PipeLocker_Ball::CEffect_PipeLocker_Ball(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CInGameEffect_Model(pDevice, pDeviceContext)
{
}

CEffect_PipeLocker_Ball::CEffect_PipeLocker_Ball(const CEffect_PipeLocker_Ball & rhs)
	: CInGameEffect_Model(rhs)
{
}

HRESULT CEffect_PipeLocker_Ball::NativeConstruct_Prototype(void * pArg)
{
	__super::NativeConstruct_Prototype(pArg);

	return S_OK;
}

HRESULT CEffect_PipeLocker_Ball::NativeConstruct(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_EffectDesc_Clone, pArg, sizeof(EFFECT_DESC_CLONE));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody_Size_ShokeWave"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_EffectDesc_Clone.WorldMatrix));
	m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 0.f));

	FAILED_CHECK_RETURN(Ready_InstanceBuffer(), E_FAIL);
	FAILED_CHECK_RETURN(Ready_Nodes(), E_FAIL);

	m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_PipeBall_Effect"), Level::LEVEL_STAGE, TEXT("GameObject_2D_PipeLocker_Button_Particle"), nullptr, (CGameObject**)&m_pParticle);
	m_pParticle->Set_IsActivate(true);

	return S_OK;
}

_int CEffect_PipeLocker_Ball::Tick(_double TimeDelta)
{
	if (m_isDead)
	{
		if(true == ((CControlRoom_Battery*)DATABASE->Get_ControlRoom_Battery())->Get_BatteryHolding())
			static_cast<CPressureBigPlate*>(DATABASE->Get_PressureBigPlate())->Set_Effect(true);

		return EVENT_DEAD;
	}

	Node_Check(TimeDelta);

	return _int();
}

_int CEffect_PipeLocker_Ball::Late_Tick(_double TimeDelta)
{
	if (m_isDead) return NO_EVENT;

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);
}

HRESULT CEffect_PipeLocker_Ball::Render(RENDER_GROUP::Enum eGroup)
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_Variable("g_vColor", &m_vColor, sizeof(_float4));
	m_pModelCom->Render_Model(16);

	return S_OK;
}

void CEffect_PipeLocker_Ball::SetUp_WorldMatrix(_fmatrix WorldMatrix)
{
}

HRESULT CEffect_PipeLocker_Ball::Ready_Nodes()
{
	m_vNodes[0]  = { 79.50f, 216.2f, 217.25f, 1.f };
	m_vNodes[1]  = { 72.50f, 216.2f, 217.25f, 1.f };
	m_vNodes[2]  = { 72.50f, 216.2f, 227.25f, 1.f };
	m_vNodes[3]  = { 57.50f, 216.2f, 227.25f, 1.f };
	m_vNodes[4]  = { 57.50f, 216.2f, 222.25f, 1.f };
	m_vNodes[5]  = { 67.50f, 216.2f, 222.25f, 1.f };
	m_vNodes[6]  = { 67.50f, 216.2f, 217.40f, 1.f };
	m_vNodes[7]  = { 55.25f, 216.2f, 217.40f, 1.f };
	m_vNodes[8]  = { 55.25f, 216.2f, 224.75f, 1.f };
	m_vNodes[9]  = { 50.25f, 216.2f, 224.75f, 1.f };
	m_vNodes[10] = { 50.25f, 221.4f, 224.75f, 1.f };
	m_vNodes[11] = { 47.00f, 221.4f, 224.75f, 1.f };

	return S_OK;
}

void CEffect_PipeLocker_Ball::Set_Pos(_vector vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

void CEffect_PipeLocker_Ball::Node_Check(_double TimeDelta)
{
	if (11 < m_iNodeCount)
	{
		m_isDead = true;
		m_IsEffect_End = true;
		m_pParticle->Set_IsActivate(false);
		return;
	}

	_vector vNodePos = XMLoadFloat4(&m_vNodes[m_iNodeCount]);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vDir = XMVector3Normalize(vNodePos - vPos);

	m_pParticle->Set_Dir(vDir);
	m_pParticle->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	vPos += vDir * (_float)TimeDelta * m_fSpeedPerSec;

	_float fDist = XMVector3Length(vNodePos - vPos).m128_f32[0];
	if (0.25f > fDist)
	{
		vPos = vNodePos;
		++m_iNodeCount;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
}

CEffect_PipeLocker_Ball * CEffect_PipeLocker_Ball::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext, void * pArg)
{
	CEffect_PipeLocker_Ball*	pInstance = new CEffect_PipeLocker_Ball(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CEffect_PipeLocker_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_PipeLocker_Ball::Clone_GameObject(void * pArg)
{
	CEffect_PipeLocker_Ball* pInstance = new CEffect_PipeLocker_Ball(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CEffect_PipeLocker_Ball");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEffect_PipeLocker_Ball::Free()
{
	Safe_Release(m_pParticle);

	__super::Free();
}
