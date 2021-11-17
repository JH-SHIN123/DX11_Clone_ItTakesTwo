#include "stdafx.h"
#include "..\public\MoonBaboon.h"
#include "GameInstance.h"
#include "DataStorage.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"

CMoonBaboon::CMoonBaboon(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	
}

CMoonBaboon::CMoonBaboon(const CMoonBaboon & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMoonBaboon::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_ControllableActor"), TEXT("Com_Actor"), (CComponent**)&m_pActorCom, &CControllableActor::ARG_DESC(m_pTransformCom)), E_FAIL);

	m_pCodyTransform = ((CCody*)CDataStorage::GetInstance()->GetCody())->Get_Transform();
	if (nullptr == m_pCodyTransform)
		return E_FAIL;
	Safe_AddRef(m_pCodyTransform);

	m_pMayTransform = ((CMay*)CDataStorage::GetInstance()->GetMay())->Get_Transform();
	if (nullptr == m_pMayTransform)
		return E_FAIL;
	Safe_AddRef(m_pMayTransform);

	m_pUFOModel = ((CUFO*)CDataStorage::GetInstance()->Get_UFO())->Get_Model();
	if (nullptr == m_pUFOModel)
		return E_FAIL;
	Safe_AddRef(m_pUFOModel);

	m_pUFOTransform = ((CUFO*)CDataStorage::GetInstance()->Get_UFO())->Get_Transform();
	if (nullptr == m_pUFOTransform)
		return E_FAIL;
	Safe_AddRef(m_pUFOModel);
	
	
	m_pModelCom->Set_Animation(15);
	m_pModelCom->Set_NextAnimIndex(15);	

	return S_OK;
}

_int CMoonBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	Check_State(dTimeDelta);
	Change_State(dTimeDelta);
	During_Animation_Behavior(dTimeDelta);
	Fix_MoonBaboon_Chair(dTimeDelta);

	PxMaterial* pMaterial = CPhysX::GetInstance()->Create_Material(0.5f, 0.5f, 0.f);
	//m_pActorCom->Update(dTimeDelta);
	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}



CMoonBaboon::MOON_STATE CMoonBaboon::Check_State(_double TimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		m_pModelCom->Set_Animation(m_eNextState);
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CMoonBaboon::Change_State(_double TimeDelta)
{
	if (m_eTarget == TARGET_CODY)
	{
		int i = 0;
	}
	else
	{
		int i = 0;
	}
}

void CMoonBaboon::During_Animation_Behavior(_double TimeDelta)
{
}

void CMoonBaboon::Fix_MoonBaboon_Chair(_double TimeDelta)
{
	_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
	_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
	XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f)  * BoneChair * m_pUFOTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
}

HRESULT CMoonBaboon::Render()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Render_Model(0);
	return S_OK;
}

HRESULT CMoonBaboon::Set_ShaderConstant_Default()
{
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	return S_OK;
}

HRESULT CMoonBaboon::Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix)
{
	m_pModelCom->Set_Variable("g_WorldMatrix", &XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix()), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_MainProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubViewMatrix", &XMMatrixTranspose(LightViewMatrix), sizeof(_matrix));
	m_pModelCom->Set_Variable("g_SubProjMatrix", &XMMatrixTranspose(LightProjMatrix), sizeof(_matrix));

	return S_OK;
}

HRESULT CMoonBaboon::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
}

CMoonBaboon * CMoonBaboon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonBaboon* pInstance = new CMoonBaboon(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoonBaboon::Clone_GameObject(void * pArg)
{
	CMoonBaboon* pInstance = new CMoonBaboon(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon::Free()
{
	Safe_Release(m_pUFOTransform);
	Safe_Release(m_pUFOModel);
	Safe_Release(m_pMayTransform);
	Safe_Release(m_pCodyTransform);
	//Safe_Release(m_pActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}