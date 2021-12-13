#include "stdafx.h"
#include "MoonBaboon_MainLaser.h"
#include "DataStorage.h"
#include "Laser_TypeB.h"

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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(3.5f, XMConvertToRadians(40.f))), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom), E_FAIL);
	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_Model_MoonBaboon_MainLaser_01"), TEXT("Com_Model"), (CComponent**)&m_pModelCom), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(64.f, 239.f, 195.f, 1.f));

	m_UserData.eID = GameID::eENVIRONMENT;
	m_UserData.pGameObject = this;

	//CStaticActor::ARG_DESC tArg;
	//tArg.pModel = m_pModelCom;
	//tArg.pTransform = m_pTransformCom;
	//tArg.pUserData = &m_UserData;
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STAGE, TEXT("Component_StaticActor"), TEXT("Com_Actor"), (CComponent**)&m_pStaticActorCom, &tArg), E_FAIL);

	DATABASE->Set_MoonBaboon_MainLaser(this);

	m_vecLaser_TypeB.reserve(8);

	return S_OK;
}

_int CMoonBaboon_MainLaser::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	if (true == m_IsLaserOperation)
		Laser_AttackPattern(TimeDelta);
	else if(false == m_IsLaserOperation && true == DATABASE->Get_LaserTypeB_Recovery())
		Laser_Down(TimeDelta);

	GoUp(TimeDelta);

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

void CMoonBaboon_MainLaser::Set_LaserOperation(_bool IsActive)
{
	m_IsLaserOperation = IsActive; 

	if (false == m_IsLaserOperation)
	{
		for (auto pLaserTypeB : m_vecLaser_TypeB)
			pLaserTypeB->Set_Dead();

		m_vecLaser_TypeB.clear();
	}
}

void CMoonBaboon_MainLaser::Laser_AttackPattern(_double TimeDelta)
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
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 2;

			for (auto pLaserTypeB : m_vecLaser_TypeB)
				pLaserTypeB->Set_RotateSpeed(-40.f);
		}
		else
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f,1.f,0.f,0.f) ,TimeDelta);
			m_dPatternDeltaT += TimeDelta;
		}
	}
	else if (2 == m_iPatternState)
	{
		if (m_dPatternDeltaT >= 10.0)
		{
			m_dPatternDeltaT = 0.0;
			m_iPatternState = 1;

			for (auto pLaserTypeB : m_vecLaser_TypeB)
				pLaserTypeB->Set_RotateSpeed(40.f);
		}
		else
		{
			m_pTransformCom->Rotate_Axis(XMVectorSet(0.f, 1.f, 0.f, 0.f), -TimeDelta);
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

		for (_uint i = 0; i < 8; ++i)
		{
			m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_LaserTypeB"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTypeB"), nullptr, &pGameObject);
			m_vecLaser_TypeB.emplace_back(static_cast<CLaser_TypeB*>(pGameObject));
			m_vecLaser_TypeB[i]->Set_StartPoint(vConvertPos);
			m_vecLaser_TypeB[i]->SetUp_MainLaserDirection(i);
			m_vecLaser_TypeB[i]->Set_RotateSpeed(40.f);
		}

		m_IsLaserCreate = false;
	}
}

void CMoonBaboon_MainLaser::Laser_Down(_double TimeDelta)
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

void CMoonBaboon_MainLaser::Set_MainLaserUp(_float fMaxDistance, _float fSpeed)
{
	XMStoreFloat3(&m_vMaxPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vMaxPos.y += fMaxDistance;

	m_fMaxY = fMaxDistance;
	m_IsGoUp = true;
	m_fUpSpeed = fSpeed;

	for (_uint i = 0; i < 8; ++i)
		m_vecLaser_TypeB[i]->Set_LaserTypeBUp(fMaxDistance, fSpeed);

	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);
}

void CMoonBaboon_MainLaser::GoUp(_double dTimeDelta)
{
	if (false == m_IsGoUp)
	{
		m_pTransformCom->Set_Speed(3.5f, 0.f);
		return;
	}

	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);

	_float fDist = (_float)dTimeDelta * m_fUpSpeed;
	m_fDistance += fDist;

	if (m_fMaxY <= m_fDistance)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vMaxPos), 1.f));
		m_fMaxY = 0.f;
		m_IsGoUp = false;
		m_fDistance = 0.f;

		if (2 == DATABASE->Get_FloorIndex())
			m_IsArrivalLastFloor = true;
		else
			m_IsArrivalLastFloor = false;

		return;
	}

	m_pTransformCom->Go_Up(dTimeDelta);
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
	for (auto pLaserTypeB : m_vecLaser_TypeB)
		Safe_Release(pLaserTypeB);

	m_vecLaser_TypeB.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	//Safe_Release(m_pStaticActorCom);

	CGameObject::Free();
}
