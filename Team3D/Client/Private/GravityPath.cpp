#include "stdafx.h"
#include "..\Public\GravityPath.h"
#include "SepdStaticActor.h"

CGravityPath::CGravityPath(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CGravityPath::CGravityPath(const CGravityPath & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGravityPath::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CGravityPath::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	//클라
	NULL_CHECK_RETURN(pArg, E_FAIL);
	memcpy(&m_Static_Env_Desc, pArg, sizeof(STATIC_ENV_DESC));

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);

	//클라
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, m_Static_Env_Desc.szModelTag, TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	//테스트
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath_01_Bend_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_GravityPath"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	//클라
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_Static_Env_Desc.WorldMatrix));
	//테스트
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(60.f, 5.f, 15.f, 1.f));

	_uint iMeshCount = m_pModelCom->Get_MeshCount();

	m_arrUserData = new USERDATA[iMeshCount];

	for (_uint iMeshIndex = 0; iMeshIndex < iMeshCount; ++iMeshIndex)
	{
		m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_SIDE;
		m_arrUserData[iMeshIndex].pGameObject = this;

		//클라
		if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath01")) && (iMeshIndex == 3 || iMeshIndex == 4))
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
		else if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath02")) && iMeshIndex == 2)
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
		else if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath03")) && (iMeshIndex == 2 || iMeshIndex == 5 || iMeshIndex == 7 || iMeshIndex == 10))
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
		else if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath04")) && (iMeshIndex == 3 || iMeshIndex == 4 || iMeshIndex == 10))
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
		else if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath05")) && (iMeshIndex == 0 || iMeshIndex == 2))
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
		else if (!lstrcmp(m_Static_Env_Desc.szModelTag, TEXT("Component_Model_GravityPath06")) && (iMeshIndex == 2 || iMeshIndex == 5))
			m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;

		//테스트
		//if (iMeshIndex == 2)
		//	m_arrUserData[iMeshIndex].eID = GameID::eGRAVITYPATH_CENTER;
	}

	CSepdStaticActor::ARG_DESC ArgDesc;
	ArgDesc.pModel = m_pModelCom;
	ArgDesc.pTransform = m_pTransformCom;
	ArgDesc.arrUserData = m_arrUserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_SepdStaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &ArgDesc), E_FAIL);

	
	return S_OK;
}

_int CGravityPath::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	return NO_EVENT;
}

_int CGravityPath::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CGravityPath::Render(RENDER_GROUP::Enum eRender)
{
	CGameObject::Render(eRender);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CGravityPath::Render_ShadowDepth()
{
	CGameObject::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CGravityPath * CGravityPath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CGravityPath* pInstance = new CGravityPath(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CGravityPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGravityPath::Clone_GameObject(void * pArg)
{
	CGravityPath* pInstance = new CGravityPath(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CGravityPath");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGravityPath::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	Safe_Delete_Array(m_arrUserData);

	CGameObject::Free();
}