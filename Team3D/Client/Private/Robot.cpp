#include "stdafx.h"
#include "..\public\Robot.h"
#include "Effect_Generator.h"

CRobot::CRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRobotParts(pDevice, pDeviceContext)
{
}

CRobot::CRobot(const CRobotParts & rhs)
	: CRobotParts(rhs)
{
}

HRESULT CRobot::NativeConstruct_Prototype()
{
	CRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRobot::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Robot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_tRobotPartsDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));

	_vector vPosition = m_tRobotPartsDesc.vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	
	//피벗 Y축
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tRobotPartsDesc.RotY);

	m_UserData.eID = GameID::eROBOT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);
	
	Set_MeshRenderGroup();

	return S_OK;
}

_int CRobot::Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::RobotBattery_Spark, m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CRobot::Late_Tick(_double dTimeDelta)
{
	CRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CRobot::Render(RENDER_GROUP::Enum eGroup)
{
	CRobotParts::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();

	_uint iMaterialIndex = 0;
	m_pModelCom->Sepd_Bind_Buffer();

	/* 렌더순서 주의 - 논알파 -> 알파 */
	iMaterialIndex = 0;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 2;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);
	//
	iMaterialIndex = 3;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 4;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 5;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	// 4: Alpha 
	iMaterialIndex = 1;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);

	//m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CRobot::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CRobot::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CRobot::Set_MeshRenderGroup()
{
	m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_ALPHA);
	m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(3, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(4, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(5, tagRenderGroup::RENDER_NONALPHA);


	return S_OK;
}

HRESULT CRobot::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);

	return S_OK;
}

CRobot * CRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRobot* pInstance = new CRobot(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRobot::Clone_GameObject(void * pArg)
{
	CRobot* pInstance = new CRobot(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRobot");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRobot::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}