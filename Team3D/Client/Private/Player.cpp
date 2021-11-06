#include "stdafx.h"
#include "..\public\Player.h"
#include "GameInstance.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CPlayer::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Cody"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pModelCom->Set_Animation(0, m_pTransformCom);
	m_pModelCom->Set_NextAnimIndex(0);

	return S_OK;
}

_int CPlayer::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if(m_pGameInstance->Key_Pressing(DIK_L))
		m_pTransformCom->Go_Straight(dTimeDelta);

	m_pModelCom->Update_NodeMatrices(dTimeDelta, m_pTransformCom);

	return NO_EVENT;
}

_int CPlayer::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_SHADOWTARGET, this);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CPlayer::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Bind_VIBuffer();

	_uint iMeshCount = m_pModelCom->Get_MeshCount();

	for (_uint iIndex = 0; iIndex < iMeshCount; ++iIndex)
		m_pModelCom->Render_Model(iIndex, 0);


	return S_OK;
}

HRESULT CPlayer::Render_ShadowMap(_uint iPassIndex)
{
	_matrix			WorldMatrix, LightViewMatrix, LightProjMatrix = XMMatrixIdentity();

	_vector vCamPosition = CPipeline::GetInstance()->Get_MainCamPosition();
	WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_vector vLightPos = XMVectorSet(-100.f, 100.f, 100.f, 1.f);
	LightViewMatrix = XMMatrixLookAtLH(vLightPos, XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	LightProjMatrix = XMMatrixOrthographicLH(1280.f / 12.f, 720.f / 12.f, 0.3f, 300.f);

	//m_pModelCom->Set_Variable("WorldMatrix", &XMMatrixTranspose(WorldMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("LightPos", &vLightPos, sizeof(_vector));
	m_pModelCom->Set_Variable("LightViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("LightProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	m_pModelCom->Bind_VIBuffer();

	_uint iMeshCount = m_pModelCom->Get_MeshCount();

	for (_uint iIndex = 0; iIndex < iMeshCount; ++iIndex)
		m_pModelCom->Render_Model(iIndex, iPassIndex);
	
	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer::Clone_GameObject(void * pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}