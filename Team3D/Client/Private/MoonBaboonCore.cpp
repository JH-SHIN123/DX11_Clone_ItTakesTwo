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
	if (nullptr != pArg)
		memcpy(&m_tDesc, pArg, sizeof(m_tDesc));

	CGameObject::NativeConstruct(nullptr);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(Level::LEVEL_STATIC, TEXT("Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &CTransform::TRANSFORM_DESC(4.f, 0.f)), E_FAIL);

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_tDesc.WorldMatrix));

	m_pCorePillar = CMoonBaboonCore_Pillar::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreButton = CMoonBaboonCore_Button::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreShield = CMoonBaboonCore_Shield::Create(m_pDevice, m_pDeviceContext, this);
	m_pCoreGlass = CMoonBaboonCore_Glass::Create(m_pDevice, m_pDeviceContext, this);

	DATABASE->Set_MoonBaboonCore(this);

    return S_OK;
}

_int CMoonBaboonCore::Tick(_double TimeDelta)
{
//#ifdef _DEBUG
//	_vector vPos = XMVectorZero();
//	TCHAR szBuff[256] = L"";
//	GetPrivateProfileString(L"Section_1", L"Key_1", L"0", szBuff, 256, L"../test.ini");
//	_float x = _ttof(szBuff);
//
//	GetPrivateProfileString(L"Section_1", L"Key_2", L"0", szBuff, 256, L"../test.ini");
//	_float y = _ttof(szBuff);
//
//	GetPrivateProfileString(L"Section_1", L"Key_3", L"0", szBuff, 256, L"../test.ini");
//	_float z = _ttof(szBuff);
//
//	vPos = XMVectorSet(x, y, z, 1.f);
//#endif // _DEBUG
//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

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

void CMoonBaboonCore::Active_Pillar(_double TimeDelta)
{
	if (true == m_IsGoUp)
		return;

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
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vMaxPos), 1.f));
		m_fMaxY = 0.f;
		m_IsGoUp = false;
		m_fDistance = 0.f;
		m_iActiveCore = 0;
		return;
	}

	m_pTransformCom->Go_Up(dTimeDelta);
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
