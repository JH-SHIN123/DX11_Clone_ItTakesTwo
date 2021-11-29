#include "stdafx.h"
#include "..\public\BigButtonFrame.h"
#include "UI_Generator.h"
#include "Cody.h"
#include "May.h"
#include "RobotParts.h"

CBigButtonFrame::CBigButtonFrame(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CBigButtonFrame::CBigButtonFrame(const CBigButtonFrame & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBigButtonFrame::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CBigButtonFrame::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_BigButtonFrame"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	ROBOTDESC BigButtonFrameDesc;
	if (nullptr != pArg)
		memcpy(&BigButtonFrameDesc, (ROBOTDESC*)pArg, sizeof(ROBOTDESC));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, BigButtonFrameDesc.vPosition);

	CStaticActor::ARG_DESC StaticDesc;
	m_UserData = USERDATA(GameID::eBIGBUTTONFRAME, this);
	StaticDesc.pModel = m_pModelCom;
	StaticDesc.pTransform = m_pTransformCom;
	StaticDesc.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Static"), (CComponent**)&m_pStaticActorCom, &StaticDesc), E_FAIL);

	return S_OK;
}

_int CBigButtonFrame::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return NO_EVENT;
}


_int CBigButtonFrame::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);
	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 5.f))
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);

	return NO_EVENT;
}

HRESULT CBigButtonFrame::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CBigButtonFrame::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

CBigButtonFrame * CBigButtonFrame::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CBigButtonFrame* pInstance = new CBigButtonFrame(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CBigButtonFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBigButtonFrame::Clone_GameObject(void * pArg)
{
	CBigButtonFrame* pInstance = new CBigButtonFrame(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CBigButtonFrame");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBigButtonFrame::Free()
{
	Safe_Release(m_pStaticActorCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}