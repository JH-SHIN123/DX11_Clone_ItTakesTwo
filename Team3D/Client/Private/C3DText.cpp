#include "stdafx.h"
#include "..\Public\C3DText.h"


C3DText::C3DText(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

C3DText::C3DText(const C3DText & rhs)
	: CGameObject(rhs)
{
}

HRESULT C3DText::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT C3DText::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_UserData.eID = GameID::e3DTEXT;
	m_UserData.pGameObject = this;

	FAILED_CHECK_RETURN(Ready_Component(pArg), E_FAIL);

	return S_OK;
}

_int C3DText::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return NO_EVENT;
}

_int C3DText::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_EFFECT, this);

	return NO_EVENT;
}

HRESULT C3DText::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(16, 0);

	return S_OK;
}

HRESULT C3DText::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	/* Skinned: 2 / Normal: 3 */
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void C3DText::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
	CGameObject::Trigger(eStatus, eID, pGameObject);
}

HRESULT C3DText::Ready_Component(void * pArg)
{
	ARG_DESC tArg;
	if(nullptr != pArg)
		if (nullptr != pArg)
			memcpy(&tArg, pArg, sizeof(ARG_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, tArg.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&tArg.vPosition), 1.f));
	m_pTransformCom->Set_Scale(XMVectorSet(1.5f, 1.5f, 1.5f, 1.f));
	m_IsBoost = tArg.IsBoost;

	/* Trigger */
	PxGeometry* TriggerGeom = new PxBoxGeometry(1.f, 1.f, 1.f);
	CTriggerActor::ARG_DESC tTriggerArgDesc;
	tTriggerArgDesc.pGeometry = TriggerGeom;
	tTriggerArgDesc.pTransform = m_pTransformCom;
	tTriggerArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_TriggerActor"), (CComponent**)&m_pTriggerActorCom, &tTriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerGeom);

	return S_OK;
}

C3DText * C3DText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	C3DText* pInstance = new C3DText(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - C3DText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * C3DText::Clone_GameObject(void * pArg)
{
	C3DText* pInstance = new C3DText(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - C3DText");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void C3DText::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTriggerActorCom);

	CGameObject::Free();
}
