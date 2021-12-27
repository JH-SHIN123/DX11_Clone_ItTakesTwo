#include "stdafx.h"
#include "MoonBaboon_SubLaser.h"
#include "DataStorage.h"
#include "Laser_TypeB.h"

CMoonBaboon_SubLaser::CMoonBaboon_SubLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboon_SubLaser::CMoonBaboon_SubLaser(const CMoonBaboon_SubLaser& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonBaboon_SubLaser::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboon_SubLaser::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(3.5f, XMConvertToRadians(40.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MainLaser_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_vecLaser_TypeB.reserve(4);

	return S_OK;
}

_int CMoonBaboon_SubLaser::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (true == m_IsLaserOperation)
		Laser_AttackPattern(TimeDelta);
	else if(false == m_IsLaserOperation && true == DATABASE->Get_LaserTypeB_Recovery())
		Laser_Down(TimeDelta);

	return NO_EVENT;
}

_int CMoonBaboon_SubLaser::Late_Tick(_double TimeDelta)
{
	CGameObject::Late_Tick(TimeDelta);

	if (0 < m_pModelCom->Culling(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 20.f)) {
		m_pRendererCom->Add_GameObject_ToRenderGroup(RENDER_GROUP::RENDER_NONALPHA, this);
	}

	return NO_EVENT;
}

HRESULT CMoonBaboon_SubLaser::Render(RENDER_GROUP::Enum eGroup)
{
	CGameObject::Render(eGroup);
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);
	m_pModelCom->Set_DefaultVariables_Perspective(m_pTransformCom->Get_WorldMatrix());
	m_pModelCom->Set_DefaultVariables_Shadow();
	m_pModelCom->Render_Model(1);

	return S_OK;
}

HRESULT CMoonBaboon_SubLaser::Render_ShadowDepth()
{
	NULL_CHECK_RETURN(m_pModelCom, E_FAIL);

	m_pModelCom->Set_DefaultVariables_ShadowDepth(m_pTransformCom->Get_WorldMatrix());

	// Skinned: 2 / Normal: 3
	m_pModelCom->Render_Model(3, 0, true);

	return S_OK;
}

void CMoonBaboon_SubLaser::Set_LaserOperation(_bool IsActive)
{
	m_IsLaserOperation = IsActive; 

	if (false == m_IsLaserOperation)
	{
		for (auto pLaserTypeB : m_vecLaser_TypeB)
			pLaserTypeB->Set_Dead();

		m_vecLaser_TypeB.clear();
	}
}

void CMoonBaboon_SubLaser::Laser_AttackPattern(_double TimeDelta)
{
	if (0 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 1.5)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 1;
			m_IsLaserUp = true;
		}
		else
		{
			m_pTransformCom->Go_Up(TimeDelta);
			m_dPatternDeltaT += TimeDelta;
			//m_pStaticActorCom->Update_StaticActor();
		}
	}
	else if (1 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 10.0)
			m_dPatternDeltaT = 0.0;
		else
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f,1.f,0.f,0.f), -TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}

	/* 레이저 발사!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	if (true == m_IsLaserUp && true == m_IsLaserCreate)
	{
		CGameObject* pGameObject = nullptr;

		_float4 vDir;
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4 vConvertPos;
		XMStoreFloat4(&vConvertPos, vPos);
		vConvertPos.y += 0.7f;

		for (_uint i = 0; i < 4; ++i)
		{
			m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeB"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeB"), nullptr, &pGameObject);
			m_vecLaser_TypeB.emplace_back(static_cast<CLaser_TypeB*>(pGameObject));
			m_vecLaser_TypeB[i]->Set_StartPoint(vConvertPos);
			m_vecLaser_TypeB[i]->SetUp_SubLaserDirection(i);
			m_vecLaser_TypeB[i]->Set_RotateSpeed(15.f);
		}

		m_IsLaserCreate = false;
	}
}

void CMoonBaboon_SubLaser::Laser_Down(_double TimeDelta)
{
	if (m_dDownTime <= 1.5)
	{
		m_pTransformCom->Go_Down(TimeDelta);
		m_dDownTime += TimeDelta;
		/*m_pStaticActorCom->Update_StaticActor();*/
	}
	else
	{
		m_IsLaserUp = false;
		m_IsLaserCreate = true;
		m_dDownTime = 0.0;
		DATABASE->Set_LaserTypeB_Recovery(false);

		/* 다음에도 또 올라와야하기 때문에 초기화 해주자 ㅇㅇ */
		m_dPatternDeltaT = 0.0;
		m_iPatternState = 0;
	}
}

void CMoonBaboon_SubLaser::SetUp_SubLaserPosition(_uint iOption)
{
	if (0 == iOption)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(78.945f, 439.f, 158.95f, 1.f));
	else if (1 == iOption)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(22.008f, 439.f, 212.40f, 1.f));
	else if (2 == iOption)
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(100.05f, 439.f, 209.95f, 1.f));
}

CMoonBaboon_SubLaser* CMoonBaboon_SubLaser::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMoonBaboon_SubLaser* pInstance = new CMoonBaboon_SubLaser(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboon_SubLaser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboon_SubLaser::Clone_GameObject(void* pArg)
{
	CMoonBaboon_SubLaser* pInstance = new CMoonBaboon_SubLaser(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboon_SubLaser");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboon_SubLaser::Free()
{
	for (auto pLaserTypeB : m_vecLaser_TypeB)
		Safe_Release(pLaserTypeB);

	m_vecLaser_TypeB.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	CGameObject::Free();
}
