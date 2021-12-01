#include "stdafx.h"
#include "..\public\UmbrellaBeam_Joystick.h"
#include "Cody.h"
#include "May.h"
#include "DataStorage.h"
#include "UmbrellaBeam.h"

CUmbrellaBeam_Joystick::CUmbrellaBeam_Joystick(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CUmbrellaBeam_Joystick::CUmbrellaBeam_Joystick(const CUmbrellaBeam_Joystick & rhs)
	: CGameObject(rhs)
{
}

HRESULT CUmbrellaBeam_Joystick::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CUmbrellaBeam_Joystick::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_UmbrellaBeam_Joystick"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	FAILED_CHECK_RETURN(Ready_Layer_UmbrellaBeam(TEXT("Layer_UmbrellaBeam")), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-790.943604f, 767.497498f, 188.182541f, 1.f));
	m_pTransformCom->Set_RotateAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-45.f));

	m_UserData = USERDATA(GameID::eUMBRELLABEAMJOYSTICK, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	CTriggerActor::ARG_DESC TriggerArgDesc;

	TriggerArgDesc.pUserData = &m_UserData;
	TriggerArgDesc.pTransform = m_pTransformCom;
	TriggerArgDesc.pGeometry = new PxSphereGeometry(1.f);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_TriggerActor"), TEXT("Com_Trigger"), (CComponent**)&m_pTriggerCom, &TriggerArgDesc), E_FAIL);
	Safe_Delete(TriggerArgDesc.pGeometry);

	return S_OK;
}

_int CUmbrellaBeam_Joystick::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-790.943604f, 767.057498f, 188.182541f, 1.f));


	/*CCody* pCody = (CCody*)DATABASE->GetCody();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, pCody->Get_Position());
*/
	return NO_EVENT;
}

_int CUmbrellaBeam_Joystick::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CUmbrellaBeam_Joystick::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

void CUmbrellaBeam_Joystick::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{

}

HRESULT CUmbrellaBeam_Joystick::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

HRESULT CUmbrellaBeam_Joystick::Ready_Layer_UmbrellaBeam(const _tchar * pLayerTag)
{
	CGameObject* pGameObject = nullptr;
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, pLayerTag, Level::LEVEL_STAGE, TEXT("GameObject_UmbrellaBeam"), nullptr, &pGameObject), E_FAIL);
	m_pUmbrellaBeam = static_cast<CUmbrellaBeam*>(pGameObject);

	return S_OK;
}


CUmbrellaBeam_Joystick * CUmbrellaBeam_Joystick::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CUmbrellaBeam_Joystick* pInstance = new CUmbrellaBeam_Joystick(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CUmbrellaBeam_Joystick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUmbrellaBeam_Joystick::Clone_GameObject(void * pArg)
{
	CUmbrellaBeam_Joystick* pInstance = new CUmbrellaBeam_Joystick(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CUmbrellaBeam_Joystick");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUmbrellaBeam_Joystick::Free()
{
	Safe_Release(m_pUmbrellaBeam);
	Safe_Release(m_pTriggerCom);
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}