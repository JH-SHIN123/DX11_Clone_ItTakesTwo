#include "stdafx.h"
#include "..\public\RotatedRobotBody.h"
#include "Effect_Generator.h"

CRotatedRobotBody::CRotatedRobotBody(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CRotatedRobotParts(pDevice, pDeviceContext)
{
}

CRotatedRobotBody::CRotatedRobotBody(const CRotatedRobotParts & rhs)
	: CRotatedRobotParts(rhs)
{
}

HRESULT CRotatedRobotBody::NativeConstruct_Prototype()
{
	CRotatedRobotParts::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRotatedRobotBody::NativeConstruct(void * pArg)
{
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_RotatedRobotBody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	if (nullptr != pArg)
		memcpy(&m_tRtRobotPartsDesc, (RTROBOTDESC*)pArg, sizeof(RTROBOTDESC));

	_vector vPosition = m_tRtRobotPartsDesc.vPosition;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	
	//피벗 Y축
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_tRtRobotPartsDesc.RotY);
	m_UserData = USERDATA(GameID::eROBOT, this);

	CStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	Set_MeshRenderGroup();

	return S_OK;
}

_int CRotatedRobotBody::Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_NUMPAD4))
		CEffect_Generator::GetInstance()->Add_Effect(Effect_Value::RobotBattery_Spark, m_pTransformCom->Get_WorldMatrix());

	return NO_EVENT;
}

_int CRotatedRobotBody::Late_Tick(_double dTimeDelta)
{
	CRotatedRobotParts::Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 25.f))
		Add_GameObject_ToRenderGroup();

	return NO_EVENT;
}

HRESULT CRotatedRobotBody::Render(RENDER_GROUP::Enum eGroup)
{
	CRotatedRobotParts::Render(eGroup);
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

	iMaterialIndex = 1;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);
	//
	iMaterialIndex = 2;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);
	m_pModelCom->Set_ShaderResourceView("g_SpecularTexture", iMaterialIndex, aiTextureType_SPECULAR, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 4;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_EmissiveTexture", iMaterialIndex, aiTextureType_EMISSIVE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 5;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);

	iMaterialIndex = 6;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Set_ShaderResourceView("g_NormalTexture", iMaterialIndex, aiTextureType_NORMALS, 0);

	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 1, false, eGroup);


	// 4: Alpha 
	iMaterialIndex = 3;
	m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", iMaterialIndex, aiTextureType_DIFFUSE, 0);
	m_pModelCom->Sepd_Render_Model(iMaterialIndex, 4, false, eGroup);

	return S_OK;
}

HRESULT CRotatedRobotBody::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true, RENDER_GROUP::RENDER_NONALPHA);

	return S_OK;
}

void CRotatedRobotBody::Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject * pGameObject)
{
}

HRESULT CRotatedRobotBody::Set_MeshRenderGroup()
{
	m_pModelCom->Set_MeshRenderGroup(3, tagRenderGroup::RENDER_ALPHA);
	m_pModelCom->Set_MeshRenderGroup(0, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(1, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(2, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(4, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(5, tagRenderGroup::RENDER_NONALPHA);
	m_pModelCom->Set_MeshRenderGroup(6, tagRenderGroup::RENDER_NONALPHA);
	return S_OK;
}

HRESULT CRotatedRobotBody::Add_GameObject_ToRenderGroup()
{
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_ALPHA, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(tagRenderGroup::RENDER_NONALPHA, this);
	return S_OK;
}

CRotatedRobotBody * CRotatedRobotBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRotatedRobotBody* pInstance = new CRotatedRobotBody(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRotatedRobotBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRotatedRobotBody::Clone_GameObject(void * pArg)
{
	CRotatedRobotBody* pInstance = new CRotatedRobotBody(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRotatedRobotBody");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRotatedRobotBody::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}