#include "stdafx.h"
#include "MoonBaboon_MainLaser.h"

CMoonBaboon_MainLaser::CMoonBaboon_MainLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboon_MainLaser::CMoonBaboon_MainLaser(const CMoonBaboon_MainLaser& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonBaboon_MainLaser::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon_MainLaser::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(2.f, XMConvertToRadians(30.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MainLaser_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, -5.f, 5.f, 1.f));

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	CStaticActor::ARG_DESC tArg;
	tArg.pModel = m_pModelCom;
	tArg.pTransform = m_pTransformCom;
	tArg.pUserData = &m_UserData;

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	return S_OK;
}

_int CMoonBaboon_MainLaser::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	Laser_AttackPattern(TimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon_MainLaser::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMoonBaboon_MainLaser::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CMoonBaboon_MainLaser::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CMoonBaboon_MainLaser::Laser_AttackPattern(_double TimeDelta)
{
	// 위로 올라와서
	// 올라와서 왼쪽으롣 돌다가
	// 끝날때쯤 오른쪽으로 살짝돌면서
	// 밑으로 들어감
	if (0 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 1.5)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 1;
		}
		else
		{
			m_pTransformCom->Go_Up(TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}
	else if (1 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 10.0)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 2;
		}
		else
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f,1.f,0.f,0.f) ,TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}
	else if (2 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 1.5)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 3;
		}
		else
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}
	else if (3 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 1.5)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 4;
		}
		else
		{
			m_pTransformCom->Go_Down(TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}
}

CMoonBaboon_MainLaser* CMoonBaboon_MainLaser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMoonBaboon_MainLaser* pInstance = new CMoonBaboon_MainLaser(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon_MainLaser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboon_MainLaser::Clone_GameObject(void* pArg)
{
	CMoonBaboon_MainLaser* pInstance = new CMoonBaboon_MainLaser(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon_MainLaser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon_MainLaser::Free()
{
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
