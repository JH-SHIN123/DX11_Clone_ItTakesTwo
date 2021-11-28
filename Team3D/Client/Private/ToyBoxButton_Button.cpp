#include "stdafx.h"
#include "ToyBoxButton_Button.h"
#include "GameInstance.h"
#include "ToyBoxButton.h"
#include "May.h"
#include "Cody.h"

CToyBoxButton_Button::CToyBoxButton_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CToyBoxButton_Button::CToyBoxButton_Button(const CToyBoxButton_Button& rhs)
	: CGameObject(rhs)
{
}

void CToyBoxButton_Button::Set_ButtonOrigin()
{
	CTransform* pParentTransform = m_pParent->Get_Transform();
	if (nullptr == pParentTransform) return;

	_matrix WorldMat, ScaleMat,RotMat, TransMat, ParentMat;
	ScaleMat = XMMatrixScaling(0.43f, 0.43f, 0.43f);
	RotMat = XMMatrixRotationX(XMConvertToRadians(-90.f));
	TransMat = XMMatrixTranslation(0.f, 1.95f, -6.f);
	ParentMat = pParentTransform->Get_WorldMatrix();
	WorldMat = ScaleMat * RotMat * TransMat * ParentMat;
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_pStaticActorCom->Update_StaticActor();
}

HRESULT CToyBoxButton_Button::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		m_pParent = (CToyBoxButton*)pArg;

	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(2.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_BigButton"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr == m_pParent) return -1;

	CTransform* pParentTransform = m_pParent->Get_Transform();
	if (nullptr == pParentTransform) return -1;

	_matrix WorldMat, ScaleMat, RotMat, TransMat, ParentMat;
	ScaleMat = XMMatrixScaling(0.43f, 0.43f, 0.43f);
	RotMat = XMMatrixRotationX(XMConvertToRadians(-90.f));
	TransMat = XMMatrixTranslation(0.f, 1.95f, -6.f);
	ParentMat = pParentTransform->Get_WorldMatrix();
	WorldMat = ScaleMat * RotMat * TransMat * ParentMat;
	m_pTransformCom->Set_WorldMatrix(WorldMat);

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_StaticActor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	CTriggerActor::ARG_DESC tTriggerArg;
	tTriggerArg.pGeometry = new PxBoxGeometry(1.5f,0.75f,1.5f);
	tTriggerArg.pTransform = m_pTransformCom;
	tTriggerArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArg), E_FAIL);
	Safe_Delete(tTriggerArg.pGeometry);

	return S_OK;
}

_int CToyBoxButton_Button::Tick(_double TimeDelta)
{
	if (nullptr == m_pParent) return -1;

	CGameObject::Tick(TimeDelta);

	if (m_pParent->Get_Trigger())
	{
		CTransform* pParentTransform = m_pParent->Get_Transform();
		if (nullptr == pParentTransform) return -1;

		_float fPressedButtonOffsetY = 0.5f;
		_matrix WorldMat, ScaleMat, RotMat, TransMat, ParentMat;
		ScaleMat = XMMatrixScaling(0.43f, 0.43f, 0.43f);
		RotMat = XMMatrixRotationX(XMConvertToRadians(-90.f));
		TransMat = XMMatrixTranslation(0.f, 1.95f, -6.f + fPressedButtonOffsetY);
		ParentMat = pParentTransform->Get_WorldMatrix();
		WorldMat = ScaleMat * RotMat * TransMat * ParentMat;
		m_pTransformCom->Set_WorldMatrix(WorldMat);

		m_pStaticActorCom->Update_StaticActor();
	}
	else
	{
		// 버튼 눌림처리 
		OnPressed_Button(TimeDelta);
	}

	return NO_EVENT;
}

_int CToyBoxButton_Button::Late_Tick(_double TimeDelta)
{
	if (nullptr == m_pParent) return -1;

	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CToyBoxButton_Button::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CToyBoxButton_Button::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CToyBoxButton_Button::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject)
{
	if (nullptr == m_pParent) return;

	switch (eStatus)
	{
	case Engine::TriggerStatus::eFOUND:
	{
		// 버튼 밑으로 이동
		if (eID == GameID::eMAY)
		{
			CMay* pMay = (CMay*)pGameObject;
			//if (ANI_M_GroundPound_Land == pMay->Get_CurState())
				m_pParent->Set_Trigger();

			m_iCheckTrigger = 1;
		}
		else if (eID == GameID::eCODY)
		{
			CCody* pCody = (CCody*)pGameObject;
			if (ANI_C_Bhv_GroundPound_Land == pCody->Get_CurState() ||
				ANI_C_Bhv_ChangeSize_GroundPound_Land_MH == pCody->Get_CurState() ||
				ANI_C_Bhv_ChangeSize_GroundPound_Falling == pCody->Get_CurState() ||
				ANI_C_Bhv_GroundPound_Land_MH == pCody->Get_CurState() ||
				ANI_C_Bhv_GroundPound_Start == pCody->Get_CurState())
				m_pParent->Set_Trigger();
			
			m_iCheckTrigger = 1;
		}
		break;
	}
	case Engine::TriggerStatus::eLOST:
	{
		if (eID == GameID::eMAY || eID == GameID::eCODY)
			m_iCheckTrigger = -1;
		break;
	}
	}
}

void CToyBoxButton_Button::OnPressed_Button(_double TimeDelta)
{
	_float fTriggerSpeed = 3.f;

	if (1 == m_iCheckTrigger) // 위에 올라가있다.
	{
		if (m_fTriggerDeltaMove > 0.2)
		{
			m_iCheckTrigger = 0;
			m_fTriggerDeltaMove = 0.2f;
		}
		else
		{
			m_fTriggerDeltaMove += (_float)TimeDelta * fTriggerSpeed;
			m_pTransformCom->Go_Down(TimeDelta * fTriggerSpeed);
		}
	}

	if (-1 == m_iCheckTrigger) // 내려가있다.
	{
		if (m_fTriggerDeltaMove < 0)
		{
			m_iCheckTrigger = 0;
			m_fTriggerDeltaMove = 0;
		}
		else
		{
			m_fTriggerDeltaMove -= (_float)TimeDelta * fTriggerSpeed;
			m_pTransformCom->Go_Up(TimeDelta * fTriggerSpeed);
		}
	}

	if(1 == m_iCheckTrigger || -1 == m_iCheckTrigger)
		m_pStaticActorCom->Update_StaticActor();
}

CToyBoxButton_Button* CToyBoxButton_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{ 
	CToyBoxButton_Button* pInstance = new CToyBoxButton_Button(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Create Instance - CToyBoxButton_Button");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToyBoxButton_Button::Free()
{
	m_pParent = nullptr;

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
