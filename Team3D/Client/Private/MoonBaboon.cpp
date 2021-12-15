#include "stdafx.h"
#include "..\Public\MoonBaboon.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"
#include "CutScenePlayer.h"
#include "RobotParts.h"

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

	ROBOTDESC MoonBaboonDesc;
	if (nullptr != pArg)
		memcpy(&MoonBaboonDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, MoonBaboonDesc.vPosition);

	DATABASE->Set_MoonBaboon(this);

	m_pUFOModel = ((CUFO*)DATABASE->Get_BossUFO())->Get_Model();
	NULL_CHECK_RETURN(m_pUFOModel, E_FAIL);
	Safe_AddRef(m_pUFOModel);

	m_pUFOTransform = ((CUFO*)DATABASE->Get_BossUFO())->Get_Transform();
	NULL_CHECK_RETURN(m_pUFOTransform, E_FAIL);
	Safe_AddRef(m_pUFOTransform);

	
	m_pModelCom->Set_Animation(15);
	m_pModelCom->Set_NextAnimIndex(15);	

	return S_OK;
}

_int CMoonBaboon::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == CCutScenePlayer::GetInstance()->Get_IsPlayCutScene())
	{
		m_pModelCom->Update_Animation(dTimeDelta);
		return S_OK;
	}

	Fix_MoonBaboon_Chair(dTimeDelta);

	//m_pActorCom->Update(dTimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
		return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}



CMoonBaboon::MOON_STATE CMoonBaboon::Check_State(_double dTimeDelta)
{
	if (m_eNextState != m_eCurState)
	{
		m_pModelCom->Set_Animation(m_eNextState);
		m_eCurState = m_eNextState;
	}
	return m_eCurState;
}

void CMoonBaboon::Fix_MoonBaboon_Chair(_double dTimeDelta)
{
	_matrix BoneChair = m_pUFOModel->Get_BoneMatrix("Chair");
	_float4x4 matWorld, matScale; // 우주선 안에있을때 유리밖으로 꼬리 튀어나와서 100->95정도로 줄임.
	XMStoreFloat4x4(&matWorld, XMMatrixRotationY(-90.f) * XMMatrixScaling(95.f, 95.f, 95.f)  * BoneChair * m_pUFOTransform->Get_WorldMatrix());
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&matWorld));
}

HRESULT CMoonBaboon::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}


HRESULT CMoonBaboon::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
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
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}