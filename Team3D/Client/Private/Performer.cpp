#include "stdafx.h"
#include "..\public\Performer.h"
#include "GameInstance.h"
#include "DataStorage.h"
#ifdef _AFX
#include"PerformerOption.h"
#include"MainForm.h"
#endif
CPerformer::CPerformer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{

}

CPerformer::CPerformer(const CPerformer & rhs)
	: CGameObject(rhs)
{

}

HRESULT CPerformer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPerformer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	m_tDesc = *(PERFORMERDESC*)pArg;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	//if (m_tDesc.strModelTag == L"Component_Model_GravityPath")
	//{


	//	//¹Øµ¿
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Ramp_01"), TEXT("Com_Model_Ramp0"), (CComponent**)&m_pModelCom), E_FAIL);
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Medium"), TEXT("Com_Model_Medium0"), (CComponent**)&m_pModels[0]), E_FAIL);
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Entrance"), TEXT("Com_Model_Entrance0"), (CComponent**)&m_pModels[1]), E_FAIL);
	//	//À§¿¡
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Ramp_01"), TEXT("Com_Model_Ramp1"), (CComponent**)&m_pModels[2]), E_FAIL);
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Medium"), TEXT("Com_Model_Medium1"), (CComponent**)&m_pModels[3]), E_FAIL);
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Entrance"), TEXT("Com_Model_Entrance1"), (CComponent**)&m_pModels[4]), E_FAIL);
	//	//Áß¾Ó
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Medium"), TEXT("Com_Model_Medium2"), (CComponent**)&m_pModels[5]), E_FAIL);
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Medium"), TEXT("Com_Model_Medium3"), (CComponent**)&m_pModels[6]), E_FAIL);
	//	m_fRotX = -90.f;
	//}
	//else
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_tDesc.strModelTag.c_str(), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);


	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_tDesc.vScale));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_tDesc.vPosition), 1.f));

	return S_OK;
}

_int CPerformer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_matrix matWorld = XMMatrixIdentity();

	matWorld = XMMatrixScaling(m_tDesc.vScale.x, m_tDesc.vScale.y, m_tDesc.vScale.z) *
		XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*
		XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y, m_tDesc.vPosition.z);
	m_pTransformCom->Set_WorldMatrix(matWorld);
	//m_pActorCom->Update(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CPerformer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
}






HRESULT CPerformer::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	m_pModelCom->Render_Model(1);
		//if (m_tDesc.strModelTag == L"Component_Model_GravityPath")
		//{
		//	_matrix matMedium0 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x + 4.f, m_tDesc.vPosition.y, m_tDesc.vPosition.z);
		//	_matrix matEntrance0 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ), XMConvertToRadians(m_fRotX + 180.f), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x + 4.f, m_tDesc.vPosition.y, m_tDesc.vPosition.z - 4.f);

		//	_matrix matMedium2 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ - 90.f), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x - 4.f, m_tDesc.vPosition.y + 4.f, m_tDesc.vPosition.z);
		//	_matrix matMedium3 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ - 90.f), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x - 4.f, m_tDesc.vPosition.y + 8.f, m_tDesc.vPosition.z);

		//	_matrix matMedium1 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ + 180.f), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x, m_tDesc.vPosition.y + 16.f, m_tDesc.vPosition.z);
		//	_matrix matEntrance1 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ + 180.f), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x + 4.f, m_tDesc.vPosition.y + 16.f, m_tDesc.vPosition.z);
		//	_matrix matRamp1 = XMMatrixRotationRollPitchYaw(XMConvertToRadians(m_fRotZ - 90.f), XMConvertToRadians(m_fRotX), XMConvertToRadians(m_fRotY))*XMMatrixTranslation(m_tDesc.vPosition.x - 4.f, m_tDesc.vPosition.y + 12.f, m_tDesc.vPosition.z);
		//	/*enum Models {Medium0,Entrance0 , Ramp1,Medium1,Entrance1 , Medium2, Medium3,	Medium4
		//	};*/
		//	NULL_CHECK_RETURN(m_pModels[0], E_FAIL);
		//	m_pModels[0]->Set_DefaultVariables_Perspective(matMedium0);
		//	m_pModels[0]->Set_DefaultVariables_Shadow();
		//	m_pModels[0]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[1], E_FAIL);
		//	m_pModels[1]->Set_DefaultVariables_Perspective(matEntrance0);
		//	m_pModels[1]->Set_DefaultVariables_Shadow();
		//	m_pModels[1]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[2], E_FAIL);
		//	m_pModels[2]->Set_DefaultVariables_Perspective(matRamp1);
		//	m_pModels[2]->Set_DefaultVariables_Shadow();
		//	m_pModels[2]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[3], E_FAIL);
		//	m_pModels[3]->Set_DefaultVariables_Perspective(matMedium1);
		//	m_pModels[3]->Set_DefaultVariables_Shadow();
		//	m_pModels[3]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[4], E_FAIL);
		//	m_pModels[4]->Set_DefaultVariables_Perspective(matEntrance1);
		//	m_pModels[4]->Set_DefaultVariables_Shadow();
		//	m_pModels[4]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[5], E_FAIL);
		//	m_pModels[5]->Set_DefaultVariables_Perspective(matMedium2);
		//	m_pModels[5]->Set_DefaultVariables_Shadow();
		//	m_pModels[5]->Render_Model(1);
		//	NULL_CHECK_RETURN(m_pModels[6], E_FAIL);
		//	m_pModels[6]->Set_DefaultVariables_Perspective(matMedium3);
		//	m_pModels[6]->Set_DefaultVariables_Shadow();
		//	m_pModels[6]->Render_Model(1);

		//}
	
	return S_OK;
}


CPerformer * CPerformer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPerformer* pInstance = new CPerformer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPerformer::Clone_GameObject(void * pArg)
{
	CPerformer* pInstance = new CPerformer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPerformer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPerformer::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}