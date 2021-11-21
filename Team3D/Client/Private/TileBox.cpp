#include "stdafx.h"
#include "..\public\TileBox.h"
#include "GameInstance.h"

CTileBox::CTileBox(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTileBox::CTileBox(const CTileBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTileBox::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTileBox::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	CModel_Instance::ARG_DESC Arg;
	Arg.iInstanceCount = 10;
	Arg.fCullingRadius = 10.f;
	Arg.pActorName = "TileBox";
	Arg.pWorldMatrices = new _float4x4[Arg.iInstanceCount];
	Arg.pMaterial = m_pGameInstance->Get_BasePxMaterial();

	for (_uint i = 0; i < Arg.iInstanceCount; ++i)
	{
		Arg.pWorldMatrices[i] = MH_XMFloat4x4Identity();
		Arg.pWorldMatrices[i]._41 = ((i % 100) * 10.f);
		Arg.pWorldMatrices[i]._43 = ((i / 100) * 10.f);
	}

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_TileBox"), TEXT("Com_Model"), (CComponent**)&m_pModelCom, &Arg), E_FAIL);

	_matrix TransformMatrix = XMMatrixIdentity();
	//TransformMatrix.r[3] = XMVectorSet(0.f, 0.f, 10.f, 1.f);
	TransformMatrix.r[3] = XMVectorSet(0.f, 30.f, 10.f, 1.f);
	m_pModelCom->Update_Model(TransformMatrix);

	/* RenderGroup, 메시별 그룹 지정 */
	//m_pModelCom->Set_MeshRenderGroup(0, RENDER_GROUP::RENDER_NONALPHA);
	//m_pModelCom->Set_MeshRenderGroup(1, RENDER_GROUP::RENDER_ALPHA);

	return S_OK;
}

_int CTileBox::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	_matrix TransformMatrix= XMMatrixRotationZ(XMConvertToRadians((_float)-dTimeDelta * 10.f));

	if (m_pGameInstance->Key_Down(DIK_0))
		m_pModelCom->Update_Model(TransformMatrix);

	if (m_pGameInstance->Key_Pressing(DIK_9))
		m_pModelCom->Update_Model(TransformMatrix);

	//if (m_pGameInstance->Key_Down(DIK_8))
	//{
	//	PxRigidDynamic* pTest = CPhysX::GetInstance()->Create_DynamicActor(PxTransform(PxVec3(0.f, 10.f, 0.f)), PxSphereGeometry(3.f), m_pGameInstance->Create_PxMaterial(0.5f, 0.5f, 0.5f), "Test", PxVec3(3.f, 3.f, 3.f));

	//}

	return NO_EVENT;
}

_int CTileBox::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	/* RenderGroup, 여러 그룹에 그려야 하는 경우 모두 추가 */
	//m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_ALPHA, this);
	m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CTileBox::Render(RENDER_GROUP::Enum eGroup)
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective();
	// Alpha : Not Process Shadow 
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0, m_iRenderNum, false);
	/* RenderGroup, 메시별로 렌더 그룹을 지정해줘야 하는 경우 eGroup인자 넘겨줘야함. */
	//m_pModelCom->Render_Model(0, m_iRenderNum, false, eGroup);

	/* Window Render Code */
	//_uint iRenderCount = m_pModelCom->Frustum_Culling();
	//m_pModelCom->Bind_GBuffers(iRenderCount);

	//// Window
	//m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", 0, aiTextureType_DIFFUSE, m_iRenderNum);
	//m_pModelCom->Set_ShaderResourceView("g_NormalTexture", 0, aiTextureType_NORMALS, m_iRenderNum);
	//m_pModelCom->Render_ModelByPass(iRenderCount, 0, 2);

	//// Window Frames
	//m_pModelCom->Set_ShaderResourceView("g_DiffuseTexture", 1, aiTextureType_DIFFUSE, m_iRenderNum);
	//m_pModelCom->Set_ShaderResourceView("g_NormalTexture", 1, aiTextureType_NORMALS, m_iRenderNum);
	//m_pModelCom->Render_ModelByPass(iRenderCount, 1, 3);

	return S_OK;
}

HRESULT CTileBox::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	m_pModelCom->Render_Model(1,0, true);

	return S_OK;
}

CTileBox * CTileBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTileBox* pInstance = new CTileBox(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTileBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTileBox::Clone_GameObject(void * pArg)
{
	CTileBox* pInstance = new CTileBox(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTileBox");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTileBox::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}