#include "stdafx.h"
#include "..\public\StarBuddy.h"
#include "GameInstance.h"

CStarBuddy::CStarBuddy(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CStarBuddy::CStarBuddy(const CStarBuddy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStarBuddy::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CStarBuddy::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_StarBuddy"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(35.f, 1.f, 25.f, 1.f));

	CTriggerActor::ARG_DESC ArgDesc;

	m_UserData = USERDATA(GameID::eSTARBUDDY, this);
	ArgDesc.pUserData = &m_UserData;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pGeometry = &PxSphereGeometry(0.5f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &ArgDesc), E_FAIL);

	return S_OK;
}

_int CStarBuddy::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_E) && m_IsCollide)
	{
		m_bLaunch = true;
	}

	if (m_bLaunch == false)
	{
		m_pTransformCom->RotateYaw(dTimeDelta * 0.5f);
		m_pTransformCom->RotatePitch(dTimeDelta * 0.2f);
	}

	else if (m_bLaunch == true)
	{
		Launch_StarBuddy(dTimeDelta);
		m_fLifeTime += (_float)dTimeDelta;
		if (m_fLifeTime > 3.5f)
			return EVENT_DEAD; // 
	}

	return NO_EVENT;
}

_int CStarBuddy::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CStarBuddy::Render()
{
	CGameObject::Render();
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CStarBuddy::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	//// ui 생성
	//1.pGameObject; -> 플레이어
	//2.this; -> 별이야
    


	//// 별의 포인터 
	//// UI 내부에선 IF(m_pGameInstance->Key_Down(DIKI))
	//{
	//	pPlayer->신호
	//	pStar->신호
	//}

	if (eStatus == TriggerStatus::eFOUND)
		m_IsCollide = true;
	else
		m_IsCollide = false;

}

HRESULT CStarBuddy::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CStarBuddy::Launch_StarBuddy(_double dTimeDelta)
{
	// 실제로 상호작용 할땐 Player -> StarBuddy Dir 방향으로 이동

	m_pTransformCom->Move_ToTarget(XMVectorSet(100.f, 10.f, 100.f, 0.f), dTimeDelta * 5.f);
	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), dTimeDelta * 4.f);
	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), dTimeDelta * 4.f);
	
}


CStarBuddy * CStarBuddy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CStarBuddy* pInstance = new CStarBuddy(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStarBuddy::Clone_GameObject(void * pArg)
{
	CStarBuddy* pInstance = new CStarBuddy(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CStarBuddy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStarBuddy::Free()
{
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}