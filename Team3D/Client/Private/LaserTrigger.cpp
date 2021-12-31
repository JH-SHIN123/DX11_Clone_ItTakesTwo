#include "stdafx.h"
#include "..\Public\LaserTrigger.h"
#include "Effect_Generator.h"
#include "Effect_StarBuddy_Move.h"

CLaserTrigger::CLaserTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CLaserTrigger::CLaserTrigger(const CLaserTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CLaserTrigger::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CLaserTrigger::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::eLASERTRIGGER;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	m_pTransformCom->Set_Scale(XMVectorSet(15.f, 15.f, 15.f, 1.f));
	m_pTransformCom->Set_Speed(40.f, 0.f);
	m_fEndY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fJumpY = m_fEndY;

	return S_OK;
}

_int CLaserTrigger::Tick(_double dTimeDelta)
{
	if (true == m_isDead)
		return EVENT_DEAD;

	CGameObject::Tick(dTimeDelta);

	if (false == LASERTENNIS->Get_StartGame())
	{
		m_pMoveEffect->Set_Dead();
		Set_Dead();
	}

	Movement(dTimeDelta);

	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);

	vMyPos = XMVectorSetY(vMyPos, 0.f);
	vTargetPos = XMVectorSetY(vTargetPos, 0.f);

	if (0.2f > XMVectorGetX(XMVector3Length(XMVector3Length(vTargetPos - vMyPos))))
	{
		LASERTENNIS->Create_Laser(m_iActivationIndex, m_eTarget);
		m_pMoveEffect->Set_Dead();
		Set_Dead();
	}

	return NO_EVENT;
}

_int CLaserTrigger::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CLaserTrigger::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	if(m_eTarget == CLaserTennis_Manager::TARGET::TARGET_CODY)
		m_pModelCom->Render_Model(16, 0);
	else
		m_pModelCom->Render_Model(17, 0);

	return S_OK;
}

HRESULT CLaserTrigger::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CLaserTrigger::Movement(_double dTimeDelta)
{
	m_pMoveEffect->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());

	if (true == m_bBounce)
	{
		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		_vector vPos = XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f);
		m_pTransformCom->Move_ToTarget(vPos, dTimeDelta);

		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		fY += (_float)dTimeDelta;

		if (m_fEndY <= XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vMyPos, m_fEndY));
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vMyPos, fY));
	}
	else
	{
		_vector vPos = XMVectorSetW(XMLoadFloat3(&m_vTargetPos), 1.f);
		m_pTransformCom->Move_ToTarget(vPos, dTimeDelta);

		m_fJumpTime += (_float)dTimeDelta * m_fJumpSpeed;
		_float fY = m_fJumpY + ((m_fJumpPower * m_fJumpTime) - 0.5f * 9.8f * m_fJumpTime * m_fJumpTime);
		if (m_fMaxY > fY && false == m_bJumpEnd)
		{
			m_fEndY += 0.7f;
			m_bJumpEnd = true;
		}
		else
			m_fMaxY = fY;

		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		if (m_fEndY >= fY)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, m_fEndY));
			m_fEndY += 0.5f;
			m_bBounce = true;
		}
		else
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPos, fY));
	}
}

HRESULT CLaserTrigger::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if (nullptr != pArg)
		memcpy(&tArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_LaserBall"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tArg.vPosition), 1.f));
	m_vTargetPos = tArg.vTargetPosition;
	m_eTarget = tArg.eTarget;
	m_iActivationIndex = tArg.iActivationIndex;

	/* Effect */
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_StarBuddyEffect"), Level::LEVEL_STAGE, TEXT("GameObject_StarBuddy_Move"), nullptr, (CGameObject**)&m_pMoveEffect), E_FAIL);

	if (m_eTarget == CLaserTennis_Manager::TARGET::TARGET_CODY)
		m_pMoveEffect->Set_ColorIndex(2); //¸ÞÀÌ²¨
	else
		m_pMoveEffect->Set_ColorIndex(6);

	return S_OK;
}

CLaserTrigger * CLaserTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CLaserTrigger* pInstance = new CLaserTrigger(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CLaserTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CLaserTrigger::Clone_GameObject(void * pArg)
{
	CLaserTrigger* pInstance = new CLaserTrigger(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CLaserTrigger");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLaserTrigger::Free()
{
	Safe_Release(m_pMoveEffect);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
