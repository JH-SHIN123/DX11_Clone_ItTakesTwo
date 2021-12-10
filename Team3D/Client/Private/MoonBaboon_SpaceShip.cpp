#include "stdafx.h"
#include "..\Public\MoonBaboon_SpaceShip.h"
#include "May.h"
#include "Cody.h"
#include "UFO.h"
#include"CutScenePlayer.h"

CMoonBaboon_SpaceShip::CMoonBaboon_SpaceShip(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
	
}

CMoonBaboon_SpaceShip::CMoonBaboon_SpaceShip(const CMoonBaboon_SpaceShip & rhs)
	: CGameObject(rhs)
{

}

HRESULT CMoonBaboon_SpaceShip::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon_SpaceShip::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pModelCom->Set_Animation(28);
	m_pModelCom->Set_NextAnimIndex(28);

	m_pTransformCom->RotateRoll_Angle(90.f);
	m_pTransformCom->RotateYaw_Angle(170.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(63.9f, 599.15f, 999.9f, 1.f));

	DATABASE->Set_MoonBaboon_SpaceShip(this);

	return S_OK;
}

_int CMoonBaboon_SpaceShip::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	m_pModelCom->Update_Animation(dTimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon_SpaceShip::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 30.f))
	return m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CMoonBaboon_SpaceShip::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(0);

	return S_OK;
}

HRESULT CMoonBaboon_SpaceShip::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(2, 0, true);
	return S_OK;
}

CMoonBaboon_SpaceShip * CMoonBaboon_SpaceShip::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CMoonBaboon_SpaceShip* pInstance = new CMoonBaboon_SpaceShip(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon_SpaceShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMoonBaboon_SpaceShip::Clone_GameObject(void * pArg)
{
	CMoonBaboon_SpaceShip* pInstance = new CMoonBaboon_SpaceShip(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon_SpaceShip");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon_SpaceShip::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}