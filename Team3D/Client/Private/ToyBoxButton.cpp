#include "stdafx.h"
#include "ToyBoxButton.h"
#include "GameInstance.h"
#include "ToyBoxButton_Button.h"

CToyBoxButton::CToyBoxButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CToyBoxButton::CToyBoxButton(const CToyBoxButton& rhs)
	: CGameObject(rhs)
{
}

CTransform* CToyBoxButton::Get_Transform() const
{
	if (nullptr == m_pTransformCom) return nullptr;
	return m_pTransformCom;
}

HRESULT CToyBoxButton::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CToyBoxButton::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(2.f, 0.f)), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_ToyBox09_Stars"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	
	// TEST
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f,0.f, 5.f, 1.f));
	//m_pTransformCom->Set_RotateAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));

	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	m_pToyBoxButton_Button = CToyBoxButton_Button::Create(m_pDevice, m_pDeviceContext, this);

	return S_OK;
}

_int CToyBoxButton::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	OnTrigger(TimeDelta);

	m_pToyBoxButton_Button->Tick(TimeDelta);

	return NO_EVENT;
}

_int CToyBoxButton::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pToyBoxButton_Button->Late_Tick(TimeDelta);
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}
	
	return NO_EVENT;
}

HRESULT CToyBoxButton::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CToyBoxButton::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CToyBoxButton::OnTrigger(_double TimeDelta)
{
	if (m_bTrigger)
	{
		if (0 == m_bCheckTrigger)
		{
			_float fTriggerSpeed = 5.f;
			if (m_fTriggerRatio >= 2.0)
			{
				m_bCheckTrigger = 1;
				m_fTriggerRatio = 0;
			}
			else
			{
				m_fTriggerRatio += (_float)TimeDelta * fTriggerSpeed;
				m_pTransformCom->Go_Straight(TimeDelta * fTriggerSpeed);
			}
		}
		else if (1 == m_bCheckTrigger)
		{
			// 2.5 ÃÊ ´ë±â
			if (m_fTriggerRatio >= 2.5f)
			{
				m_bCheckTrigger = 2;
				m_fTriggerRatio = 0;
			}
			else
			{
				m_fTriggerRatio += (_float)TimeDelta;
			}
		}
		else if (2 == m_bCheckTrigger)
		{
			_float fTriggerSpeed = 1.f;
			if (m_fTriggerRatio >= 2.0)
			{
				// Set Button
				if(m_pToyBoxButton_Button) m_pToyBoxButton_Button->Set_ButtonOrigin();

				m_bCheckTrigger = 0;
				m_fTriggerRatio = 0;
				m_bTrigger = false;
			}
			else
			{
				m_fTriggerRatio += (_float)TimeDelta * fTriggerSpeed;
				m_pTransformCom->Go_Backward(TimeDelta * fTriggerSpeed);
			}
		}

		m_pStaticActorCom->Update_StaticActor();
	}
}

CToyBoxButton* CToyBoxButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CToyBoxButton* pInstance = new CToyBoxButton(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CToyBoxButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CToyBoxButton::Clone_GameObject(void* pArg)
{
	CToyBoxButton* pInstance = new CToyBoxButton(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CToyBoxButton");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CToyBoxButton::Free()
{
	Safe_Release(m_pToyBoxButton_Button);

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
