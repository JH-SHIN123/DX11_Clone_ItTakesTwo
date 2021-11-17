#include "stdafx.h"
#include "..\public\Rocket.h"
#include "GameInstance.h"
#include "DataStorage.h"
CRocket::CRocket(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CRocket::CRocket(const CRocket & rhs)
	: CGameObject(rhs)
{
}

HRESULT CRocket::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CRocket::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(5.f, XMConvertToRadians(90.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_Rocket"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.f, 0.f, 10.f, 1.f));

	return S_OK;
}

_int CRocket::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (m_pGameInstance->Key_Down(DIK_9))
		m_bLaunch = true;

	if (m_bLaunch == true)
	{
		Launch_Rocket(dTimeDelta);
		m_fLifeTime += (_float)dTimeDelta;
		if (m_fLifeTime > 3.5f)
			return EVENT_DEAD;
	}

	return NO_EVENT;
}

_int CRocket::Late_Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);
	return m_pRendererCom->Add_GameObject_ToRenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CRocket::Render()
{
	CGameObject::Render();
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CRocket::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth();

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);
	return S_OK;
}

void CRocket::Launch_Rocket(_double TimeDelta)
{
	m_fUpAcceleration += (_float)TimeDelta * 0.2f;

	//m_pTransformCom->Go_Straight(TimeDelta);

	if (m_fUpAcceleration < 0.092f)
	{
		// 실제로 상호작용 할땐 Player <-> Rocket Dir 을 축으로 회전해야함.
		m_pTransformCom->Rotate_Axis(XMVectorSet(1.f, 0.f, 0.f, 0.f), TimeDelta * 1.75f);
		m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (m_fUpAcceleration - 0.06f) * (m_fUpAcceleration - 0.06f)/*/ 4.f*/);
	}

	m_pTransformCom->Rotate_Axis(m_pTransformCom->Get_State(CTransform::STATE_UP), m_fUpAcceleration);
	m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), (m_fUpAcceleration - 0.06f) * (m_fUpAcceleration - 0.06f)/*/ 4.f*/);
	m_pTransformCom->Go_Up(m_fUpAcceleration / 8.f);
}


CRocket * CRocket::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CRocket* pInstance = new CRocket(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CRocket::Clone_GameObject(void * pArg)
{
	CRocket* pInstance = new CRocket(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CRocket");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRocket::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}