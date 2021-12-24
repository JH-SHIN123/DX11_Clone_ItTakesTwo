#include "stdafx.h"
#include "..\Public\MoonBaboonCore.h"
#include "MoonBaboonCore_Pillar.h"
#include "MoonBaboonCore_Button.h"
#include "MoonBaboonCore_Shield.h"
#include "MoonBaboonCore_Glass.h"
#include "Effect_Boss_Core.h"
#include "Effect_Generator.h"
#include "UFO.h"

CMoonBaboonCore::CMoonBaboonCore(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore::CMoonBaboonCore(const CMoonBaboonCore& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonBaboonCore::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CMoonBaboonCore::NativeConstruct(void* pArg)
{
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(m_tDesc));

	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(4.f, 0.f)), E_FAIL);
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	m_pCorePillar = CMoonBaboonCore_Pillar::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreButton = CMoonBaboonCore_Button::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreShield = CMoonBaboonCore_Shield::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreGlass = CMoonBaboonCore_Glass::Create(m_pDevice, m_pDeviceContext, this);

	// Effect
	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect_BossCore"), Level::LEVEL_STAGE, TEXT("GameObject_2D_Boss_Core"),nullptr, (CGameObject**)&m_pEffectBossCore), E_FAIL);

	DATABASE->Set_MoonBaboonCore(this);

	if (m_tDesc.iIndex == 1) // 중력 발판 앞 코어
		m_fMoveTime *= 2.5f;

    return S_OK;
}

_int CMoonBaboonCore::Tick(_double TimeDelta)
{
	NULL_CHECK_RETURN(m_pCorePillar, E_FAIL);
	NULL_CHECK_RETURN(m_pCoreButton, E_FAIL);
	NULL_CHECK_RETURN(m_pCoreShield, E_FAIL);
	NULL_CHECK_RETURN(m_pCoreGlass, E_FAIL);

	if (m_bBroken)
	{
		if (false == m_bBrokenStart)
		{
			if (m_dBrokenWaitingDeltaT >= 1.0)
			{
				m_bBrokenStart = true;
				m_dBrokenWaitingDeltaT = 0.0;
			}
			else
			{
				m_dBrokenWaitingDeltaT += TimeDelta;
				m_bMove = false;
			}
		}
		else if (true == m_bBrokenStart)
		{
			if (m_fMoveDelta > 0.f)
			{
				m_iActiveCore = -1;
				m_bArrived = false;
				// 내려가는중에, 실드 올라가기 트리거 실행
				m_pTransformCom->Go_Down(TimeDelta);
				m_fMoveDelta -= (_float)TimeDelta;
				m_bMove = true;
			}
			else
			{
				m_iActiveCore = 0;
				m_bMove = false;
			}
		}
	}
	else
	{
		Active_Pillar(TimeDelta);
	}

	m_pCorePillar->Tick(TimeDelta);
	m_pCoreButton->Tick(TimeDelta);
	m_pCoreShield->Tick(TimeDelta);
	m_pCoreGlass->Tick(TimeDelta);

	if (m_pEffectBossCore) 
		m_pEffectBossCore->Set_Pos(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	// 2 Phase시 위로 올라가는거
	GoUp(TimeDelta);

    return _int();
}

_int CMoonBaboonCore::Late_Tick(_double TimeDelta)
{
	m_pCorePillar->Late_Tick(TimeDelta);
	m_pCoreButton->Late_Tick(TimeDelta);
	m_pCoreShield->Late_Tick(TimeDelta);
	m_pCoreGlass->Late_Tick(TimeDelta);

    return _int();
}

//void CMoonBaboonCore::Reset()
//{
//	if (m_bResetOnce) return;
//	if (nullptr != m_pEffectBossCore) return;
//
//	if (nullptr == m_pEffectBossCore)
//	{
//		FAILED_CHECK(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Effect_BossCore"), Level::LEVEL_STAGE, TEXT("GameObject_2D_Boss_Core"), nullptr, (CGameObject**)&m_pEffectBossCore));
//	}
//
//	m_bBroken = false;
//	m_bBrokenStart = false;
//	m_dBrokenWaitingDeltaT = 0.f;
//	m_iActiveCore = 0;
//
//	m_pCoreGlass->Reset();
//
//	m_bResetOnce = true;
//}

void CMoonBaboonCore::GoUp(_double dTimeDelta)
{
	if (false == m_IsGoUp)
	{
		m_pTransformCom->Set_Speed(4.f, 0.f);
		return;
	}

	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);

	_float fDist = (_float)dTimeDelta * m_fUpSpeed;
	m_fDistance += fDist;

	if (m_fMaxY <= m_fDistance)
	{
		//Reset();
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vMaxPos), 1.f));
		m_fMaxY = 0.f;
		m_IsGoUp = false;
		m_fDistance = 0.f;
		m_iActiveCore = 0;
		m_bMove = false;
		m_b2Floor = true;
		return;
	}

	m_pTransformCom->Go_Up(dTimeDelta);
	m_bMove = true;

}

void CMoonBaboonCore::Active_Pillar(_double TimeDelta)
{
	if (true == m_IsGoUp)
		return;

	CUFO* pUFO = (CUFO*)DATABASE->Get_BossUFO();
	if (nullptr != pUFO)
	{
		if (false == m_b2Floor && m_tDesc.iIndex == 1)
			return;

		if (CUFO::LASER != pUFO->Get_BossPatern())
			return;
	}

	if (m_iActiveCore == 1)
	{
		if (m_fMoveDelta < m_fMoveTime)
		{
			m_pTransformCom->Go_Up(TimeDelta);
			m_fMoveDelta += (_float)TimeDelta;
			m_bMove = true;
		}
		else
			m_bArrived = true;
	}
	else if (m_iActiveCore == -1)
	{
		if (m_fMoveDelta > 0.f)
		{
			m_bArrived = false;
			// 내려가는중에, 실드 올라가기 트리거 실행
			m_pTransformCom->Go_Down(TimeDelta);
			m_fMoveDelta -= (_float)TimeDelta;
			m_bMove = true;
		}
	}
	else
	{
		m_iActiveCore = 0;
		m_bMove = false;
	}
}

void CMoonBaboonCore::Set_Broken()
{
	if (m_bBroken) return;

	m_bBroken = true;
	if (m_pEffectBossCore) 
	{
		m_pEffectBossCore->HitOn();
		Safe_Release(m_pEffectBossCore);
		m_pEffectBossCore = nullptr;
	}
}

void CMoonBaboonCore::Set_MoonBaboonCoreUp(_float fMaxDistance, _float fSpeed)
{
	XMStoreFloat3(&m_vMaxPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vMaxPos.y += fMaxDistance;

	m_fMaxY = fMaxDistance;
	m_IsGoUp = true;
	m_fUpSpeed = fSpeed;

	m_pTransformCom->Set_Speed(m_fUpSpeed, 0.f);
	m_iActiveCore = 1;
}

CMoonBaboonCore* CMoonBaboonCore::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CMoonBaboonCore* pInstance = new CMoonBaboonCore(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CMoonBaboonCore");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CMoonBaboonCore::Clone_GameObject(void* pArg)
{
	CMoonBaboonCore* pInstance = new CMoonBaboonCore(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CMoonBaboonCore");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMoonBaboonCore::Free()
{
	if (m_pEffectBossCore)
	{
		m_pEffectBossCore->Set_Dead();
		Safe_Release(m_pEffectBossCore);
	}
	Safe_Release(m_pCorePillar);
	Safe_Release(m_pCoreButton);
	Safe_Release(m_pCoreShield);
	Safe_Release(m_pCoreGlass);

	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
