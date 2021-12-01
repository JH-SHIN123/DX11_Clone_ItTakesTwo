#include "stdafx.h"
#include "SpaceRail.h"
#include "SpaceRail_Node.h"

CSpaceRail::CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CDynamic_Env(pDevice, pDeviceContext)
{
}

CSpaceRail::CSpaceRail(const CSpaceRail& rhs)
	: CDynamic_Env(rhs)
{
}

HRESULT CSpaceRail::Start_Path(_uint eEdgeState, CPath::STATE eState, _uint iAnimFrame)
{
	if (nullptr == m_pPathCom) return E_FAIL;

	//CPath::STATE ePathState = CPath::STATE_END;
	//if (EDGE_END == eEdgeState)
	//{
	//	if (CPath::STATE_BACKWARD == eState)
	//		eState = CPath::STATE_FORWARD;
	//	else 
	//		eState = CPath::STATE_BACKWARD;
	//}
	//else ePathState = eState;

	return m_pPathCom->Start_Path(eState, iAnimFrame);
}

_bool CSpaceRail::Take_Path(_double dTimeDelta, _matrix& WorldMatrix)
{
	if (nullptr == m_pPathCom) return false;

	return m_pPathCom->Update_Animation(dTimeDelta, WorldMatrix);
}


HRESULT CSpaceRail::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSpaceRail::NativeConstruct(void* pArg)
{
	// TEST
	ARG_DESC tTest;
	tTest.iMatrialIndex = 0;
	tTest.iOption = 0;
	lstrcpy(tTest.szModelTag, L"Component_Model_GrindRail02");
	
	//_matrix Rotate = XMMatrixRotationY(XMConvertToRadians(90.f));
	//_matrix Trans = XMMatrixTranslation(0.f,30.f,0.f);
	//XMStoreFloat4x4(&tTest.WorldMatrix, Rotate * Trans);

	tTest.WorldMatrix = MH_XMFloat4x4Identity();

	CDynamic_Env::NativeConstruct(&tTest);

	//CDynamic_Env::NativeConstruct(pArg);

	/* Setting Pivot */
	//_matrix PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f) * (XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f)) * XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-90.f)));
	//_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	//m_pTransformCom->Set_WorldMatrix(WorldMatrix * PivotMatrix);

	m_UserData.eID = GameID::eSPACERAIL;
	m_UserData.pGameObject = this;

	/* Set Static */
	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	// Set Rail Tag
	lstrcpy(m_szRailTag, m_tDynamic_Env_Desc.szModelTag);

	/* Set Path */
	CPath::PATH_DESC pathDesc;
	XMStoreFloat4x4(&pathDesc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	// 모델태그에 따라, 패스 지정해주기
	if (0 == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail01")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail01"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	else if (0 == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail02")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail02"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	else if (false == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail03")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail03"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	else if (false == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail04")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail04"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	else if (false == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail05")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail05"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	else if (false == lstrcmp(m_szRailTag, TEXT("Component_Model_GrindRail06")))
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail06"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);
	}
	

	/* Space Rail Node 구성 */
	vector<_uint> FrameIndices;
	vector<_float4x4> FrameMatrices;
	m_pPathCom->Get_FramesWorldMatrices(FrameIndices, FrameMatrices, m_iPerNodesInteract);

	CSpaceRail_Node::SPACERAILNODE_DESC nodeDesc;

	_uint iFrameIndex = 0;
	_uint iFrameCount = FrameMatrices.size();
	for (auto& pFrameMat : FrameMatrices)
	{
		lstrcpy(nodeDesc.szRailTag, m_szRailTag);
		nodeDesc.iFrameIndex = FrameIndices[iFrameIndex];
		nodeDesc.WorldMatrix = pFrameMat;

		if(iFrameIndex < iFrameCount * 0.5)
			nodeDesc.iEdgeState = EDGE_START;
		else
			nodeDesc.iEdgeState = EDGE_END;

		CSpaceRail_Node* pSpaceRailNode = CSpaceRail_Node::Create(m_pDevice, m_pDeviceContext, &nodeDesc);
		m_vecSpaceRailNodes.push_back(pSpaceRailNode);

		++iFrameIndex;
	}

	// DB 세팅
	DATABASE->Set_SpaceRails(m_szRailTag, this);

	return S_OK;
}

_int CSpaceRail::Tick(_double dTimeDelta)
{
	CDynamic_Env::Tick(dTimeDelta);

	for (auto& pNode : m_vecSpaceRailNodes) {
		if (nullptr == pNode) continue;
		pNode->Tick(dTimeDelta);
	}

	return _int();
}

_int CSpaceRail::Late_Tick(_double dTimeDelta)
{
	CDynamic_Env::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 200.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return 0;
}

HRESULT CSpaceRail::Render(RENDER_GROUP::Enum eGroup)
{
	CDynamic_Env::Render(eGroup);

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1, m_tDynamic_Env_Desc.iMatrialIndex);

	return S_OK;
}

HRESULT CSpaceRail::Render_ShadowDepth()
{
	CDynamic_Env::Render_ShadowDepth();

	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());
	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CSpaceRail* CSpaceRail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CSpaceRail* pInstance = new CSpaceRail(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CSpaceRail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSpaceRail::Clone_GameObject(void* pArg)
{
	CSpaceRail* pInstance = new CSpaceRail(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CSpaceRail");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CSpaceRail::Free()
{
	for (auto& pNode : m_vecSpaceRailNodes)
		Safe_Release(pNode);
	m_vecSpaceRailNodes.clear();

	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pPathCom);

	CDynamic_Env::Free();
}
