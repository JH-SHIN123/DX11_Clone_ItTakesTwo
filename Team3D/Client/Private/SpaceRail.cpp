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

HRESULT CSpaceRail::Start_Path(CPath::STATE eState, _uint iAnimFrame)
{
	if (nullptr == m_pPathCom) return E_FAIL;

	return m_pPathCom->Start_Path(eState, iAnimFrame);
}

_bool CSpaceRail::Take_Path(_double dTimeDelta, _matrix& WorldMatrix)
{
	if (nullptr == m_pPathCom) return false;

	// 속도는 프레임개수로 조절하자.
	return m_pPathCom->Update_Animation(dTimeDelta, WorldMatrix);
}


HRESULT CSpaceRail::NativeConstruct_Prototype()
{
	CDynamic_Env::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CSpaceRail::NativeConstruct(void* pArg)
{
	ARG_DESC tTest;
	tTest.iMatrialIndex = 0;
	tTest.iOption = 0;
	lstrcpy(tTest.szModelTag, L"Component_Model_GrindRail02");
	tTest.WorldMatrix = MH_XMFloat4x4Identity();
	XMStoreFloat4x4(&tTest.WorldMatrix, XMMatrixTranslation(0.f, 5.f, 0.f));

	CDynamic_Env::NativeConstruct(&tTest);

	// Set Rail Tag
	lstrcpy(m_szRailTag, m_tDynamic_Env_Desc.szModelTag);

	/* Set Path */
	CPath::PATH_DESC pathDesc;
	XMStoreFloat4x4(&pathDesc.WorldMatrix, m_pTransformCom->Get_WorldMatrix());

	// 모델태그에 따라, 패스 지정해주기
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Path_GrindRail02"), TEXT("Com_Path"), (CComponent**)&m_pPathCom, (void*)&pathDesc), E_FAIL);

	/* Space Rail Node 구성 */
	vector<_uint> FrameIndices;
	vector<_float4x4> FrameMatrices;
	m_pPathCom->Get_FramesWorldMatrices(FrameIndices, FrameMatrices, m_iPerNodesInteract);

	CSpaceRail_Node::SPACERAILNODE_DESC nodeDesc;

	_uint iFrameIndex = 0;
	_float fEdgeRadio = 0.1f;
	_uint iNumFrames = (_uint)FrameMatrices.size();
	for (auto& pFrameMat : FrameMatrices)
	{
		lstrcpy(nodeDesc.szRailTag, m_szRailTag);
		nodeDesc.iFrameIndex = FrameIndices[iFrameIndex];
		nodeDesc.WorldMatrix = pFrameMat;

		if(iFrameIndex < iNumFrames * fEdgeRadio)
			nodeDesc.iEdgeState = EDGE::EDGE_START;
		else if(iFrameIndex >= iNumFrames * (1.f - fEdgeRadio))
			nodeDesc.iEdgeState = EDGE::EDGE_END;
		else
			nodeDesc.iEdgeState = EDGE::EDGE_MID;

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

	Safe_Release(m_pPathCom);

	CDynamic_Env::Free();
}
