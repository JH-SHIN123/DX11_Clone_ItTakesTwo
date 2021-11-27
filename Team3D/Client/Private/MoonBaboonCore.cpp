#include "stdafx.h"
#include "..\Public\MoonBaboonCore.h"
#include "MoonBaboonCore_Pillar.h"
#include "MoonBaboonCore_Button.h"
#include "MoonBaboonCore_Shield.h"
#include "MoonBaboonCore_Glass.h"

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
	CGameObject::NativeConstruct(pArg);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(4.f, 0.f)), E_FAIL);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(5.f, 0.f, 5.f, 1.f));

	m_pCorePillar = CMoonBaboonCore_Pillar::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreButton = CMoonBaboonCore_Button::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreShield = CMoonBaboonCore_Shield::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreGlass = CMoonBaboonCore_Glass::Create(m_pDevice, m_pDeviceContext, this);

    return S_OK;
}

_int CMoonBaboonCore::Tick(_double TimeDelta)
{
	if (m_bBroken)
	{
		if (false == m_bBrokenStart)
		{
			if (m_dBrokenWaitingDeltaT >= 2.0)
			{
				m_bBrokenStart = true;
				m_dBrokenWaitingDeltaT = 0.0;
			}
			else
			{
				m_dBrokenWaitingDeltaT += TimeDelta;
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
			}
			else
			{
				m_iActiveCore = 0;
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

void CMoonBaboonCore::Active_Pillar(_double TimeDelta)
{
	if (m_iActiveCore == 1)
	{
		if (m_fMoveDelta < 1.4f)
		{
			m_pTransformCom->Go_Up(TimeDelta);
			m_fMoveDelta += (_float)TimeDelta;
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
		}
	}
	else m_iActiveCore = 0;
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
	Safe_Release(m_pCorePillar);
	Safe_Release(m_pCoreButton);
	Safe_Release(m_pCoreShield);
	Safe_Release(m_pCoreGlass);

	Safe_Release(m_pTransformCom);

	CGameObject::Free();
}
