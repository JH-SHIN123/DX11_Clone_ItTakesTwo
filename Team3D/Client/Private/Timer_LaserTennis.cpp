#include "stdafx.h"
#include "..\Public\Timer_LaserTennis.h"
#include "LaserTennis_Manager.h"

CTimer_LaserTennis::CTimer_LaserTennis(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CTimer_LaserTennis::CTimer_LaserTennis(const CTimer_LaserTennis & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTimer_LaserTennis::NativeConstruct_Prototype()
{
	CGameObject::NativeConstruct_Prototype();

	return S_OK;
}

HRESULT CTimer_LaserTennis::NativeConstruct(void * pArg)
{
	CGameObject::NativeConstruct(pArg);

	LASERTENNIS->Set_Timer(this);

	return S_OK;
}

_int CTimer_LaserTennis::Tick(_double dTimeDelta)
{
	CGameObject::Tick(dTimeDelta);

	if (true == m_bOnOff)
	{
		m_dButton_Time += dTimeDelta;
		m_dLargeButton_Time += dTimeDelta;
		m_dLaserActivation_Time += dTimeDelta;

		/* 작은버튼 */
		if (3.0 <= m_dButton_Time)
		{
			LASERTENNIS->Active_LaserButton();
			m_dButton_Time = 0.0;
		}
		/* 큰버튼 */
		if (30.0 <= m_dLargeButton_Time)
		{
			LASERTENNIS->Active_LaserButtonLarge_Gate(false);
			m_dLargeButton_Time = 0.0;
		}
		/* 레이저 생성기 */
		if (15.0 <= m_dLaserActivation_Time)
		{
			if (0 == m_iLaserActivation_Count)
			{
				LASERTENNIS->Active_LaserActivation(0);
				LASERTENNIS->Active_LaserActivation(4);
				++m_iLaserActivation_Count;
			}
			else if (1 == m_iLaserActivation_Count)
			{
				LASERTENNIS->Active_LaserActivation(2);
				LASERTENNIS->Active_LaserActivation(6);
				++m_iLaserActivation_Count;
			}
			else if (2 == m_iLaserActivation_Count)
			{
				LASERTENNIS->Active_LaserActivation(3);
				++m_iLaserActivation_Count;
			}
			m_dLaserActivation_Time = 0.0;
		}
	}
	return NO_EVENT;
}

_int CTimer_LaserTennis::Late_Tick(_double dTimeDelta)
{
	CGameObject::Late_Tick(dTimeDelta);

	return NO_EVENT;
}

CTimer_LaserTennis * CTimer_LaserTennis::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CTimer_LaserTennis* pInstance = new CTimer_LaserTennis(pDevice, pDeviceContext);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX("Failed to Create Instance - CTimer_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTimer_LaserTennis::Clone_GameObject(void * pArg)
{
	CTimer_LaserTennis* pInstance = new CTimer_LaserTennis(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX("Failed to Clone Instance - CTimer_LaserTennis");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTimer_LaserTennis::Free()
{
	CGameObject::Free();
}
