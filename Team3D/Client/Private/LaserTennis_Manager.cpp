#include "stdafx.h"
#include "..\Public\LaserTennis_Manager.h"
#include "LaserTrigger.h"
#include "LaserActivation.h"
#include "LaserButton.h"
#include "LaserButtonLarge_Gate.h"
#include "LaserPowerCoord.h"
#include "Timer_LaserTennis.h"
#include "Wall_LaserTennis.h"
#include "LaserButtonLarge.h"
#include "Laser_LaserTennis.h"

IMPLEMENT_SINGLETON(CLaserTennis_Manager)

CLaserTennis_Manager::CLaserTennis_Manager()
{
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
}

HRESULT CLaserTennis_Manager::NativeConstruct_LaserTennis()
{
	m_LaserActivation_Cody.reserve(4);
	m_LaserActivation_May.reserve(4);
	m_LaserActivation.reserve(7);
	m_LaserButton_Cody.reserve(12);
	m_LaserButton_May.reserve(12);
	m_LaserButtonLarge_Gate.reserve(8);
	m_Wall.reserve(5);

	ZeroMemory(m_pLaserPowerCoord, sizeof(m_pLaserPowerCoord));
	ZeroMemory(m_pLaserButtonLarge, sizeof(m_pLaserButtonLarge));
	ZeroMemory(m_bKeyCheck, sizeof(m_bKeyCheck));

	return S_OK;
}

HRESULT CLaserTennis_Manager::Create_LaserTrigger(_fvector vPosition, TARGET eTarget)
{
	/* LaserTrigger 생성 */
	CLaserTrigger::ARG_DESC tArg;
	XMStoreFloat3(&tArg.vPosition, vPosition);
	tArg.eTarget = eTarget;

	if (TARGET_CODY == eTarget)
	{
		Get_RandomArr(4);
		for (_uint i = 0; i < 4; ++i)
		{
			if (true == m_LaserActivation_Cody[m_pRandomArr[i]]->Get_Active())
			{
				XMStoreFloat3(&tArg.vTargetPosition, m_LaserActivation_Cody[m_pRandomArr[i]]->Get_Position());
				tArg.iActivationIndex = m_pRandomArr[i];
				break;
			}
		}
	}
	else if (TARGET_MAY == eTarget)
	{
		Get_RandomArr(4);
		for (_uint i = 0; i < 4; ++i)
		{
			if (true == m_LaserActivation_May[m_pRandomArr[i]]->Get_Active())
			{
				XMStoreFloat3(&tArg.vTargetPosition, m_LaserActivation_May[m_pRandomArr[i]]->Get_Position());
				tArg.iActivationIndex = m_pRandomArr[i];
				break;
			}
		}
	}

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, TEXT("Layer_Environment"), Level::LEVEL_STAGE, TEXT("GameObject_LaserTrigger"), &tArg), E_FAIL);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Create_LaserTrigger_LargeButton(_fvector vPosition, TARGET eTarget)
{
	/* 큰버튼 눌렀을 때 LaserTrigger 생성 */
	Create_LaserTrigger(vPosition, eTarget);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Create_Laser(_uint iIndex, TARGET eTarget)
{
		/* Laser 생성 */
	CLaser_LaserTennis::ARG_DESC tArg;
	if (eTarget == TARGET_CODY)
	{
		XMStoreFloat3(&tArg.vPosition, m_LaserActivation_Cody[iIndex]->Get_Position());

		if (3 == m_LaserActivation_Cody[iIndex]->Get_Option())
			XMStoreFloat3(&tArg.vDirection, m_LaserActivation_Cody[iIndex]->Get_Direction() * -1.f);
		else
			XMStoreFloat3(&tArg.vDirection, m_LaserActivation_Cody[iIndex]->Get_Direction());

		tArg.eTarget = eTarget;
	}
	else
	{
		XMStoreFloat3(&tArg.vPosition, m_LaserActivation_May[iIndex]->Get_Position());
		XMStoreFloat3(&tArg.vDirection, m_LaserActivation_May[iIndex]->Get_Direction());
		tArg.eTarget = eTarget;
	}

	FAILED_CHECK_RETURN(m_pGameInstance->Add_GameObject_Clone(Level::LEVEL_STAGE, L"Layer_Environment", Level::LEVEL_STAGE, TEXT("GameObject_Laser_LaserTennis"), &tArg), E_FAIL);

	return S_OK;
}

void CLaserTennis_Manager::Get_RandomArr(_uint iIndex)
{
	Safe_Delete_Array(m_pRandomArr);

	m_pRandomArr = new _uint[iIndex];

	for (_uint i = 0; i < iIndex; ++i)
		m_pRandomArr[i] = i;

	for (_uint i = 0; i < 50; ++i)
	{
		int iDst = rand() % iIndex;
		int iSrc = rand() % iIndex;

		int iTemp = m_pRandomArr[iDst];
		m_pRandomArr[iDst] = m_pRandomArr[iSrc];
		m_pRandomArr[iSrc] = iTemp;
	}
}

void CLaserTennis_Manager::Increase_PowerCoord()
{
	++m_iPowerCoordCount;

	if (2 == m_iPowerCoordCount)
		Create_StartUI();
}

void CLaserTennis_Manager::Decrease_PowerCoord()
{
	--m_iPowerCoordCount;
}

void CLaserTennis_Manager::KeyCheck(TARGET eTarget)
{
	if (false == m_bReady)
		return;

	m_bKeyCheck[eTarget] = true;

	if (true == m_bKeyCheck[TARGET_CODY] && true == m_bKeyCheck[TARGET_MAY])
		m_bStartGame = true;
}

void CLaserTennis_Manager::Set_MayCount()
{
	++m_iMayCount;

	if (4 <= m_iMayCount)
	{
		Reset_Game();
		m_eWinner = CLaserTennis_Manager::TARGET_MAY;
	}
}

void CLaserTennis_Manager::Set_CodyCount()
{
	++m_iCodyCount;

	if (4 <= m_iCodyCount)
	{
		Reset_Game();
		m_eWinner = CLaserTennis_Manager::TARGET_CODY;
	}
}

void CLaserTennis_Manager::Active_LaserButton()
{
	/* 랜덤 인덱스 Swap */
	_uint iRandomIndex[12] = {};

	for (_uint i = 0; i < 12; ++i)
		iRandomIndex[i] = i;

	for (_uint i = 0; i < 50; ++i)
	{
		int iDst = rand() % 12;
		int iSrc = rand() % 12;

		int iTemp = iRandomIndex[iDst];
		iRandomIndex[iDst] = iRandomIndex[iSrc];
		iRandomIndex[iSrc] = iTemp;
	}

	for (_uint i = 0; i < 12; ++i)
	{
		if (true == m_LaserButton_Cody[iRandomIndex[i]]->Get_Active())
			continue;
		else
		{
			m_pGameInstance->Stop_Sound(CHANNEL_LASERBUTTON);
			m_pGameInstance->Play_Sound(TEXT("MiniGame_ButtonActivate_Green.wav"), CHANNEL_LASERTENNIS);

			m_LaserButton_Cody[iRandomIndex[i]]->Active_Button(true);
			break;
		}
	}

	for (_uint i = 0; i < 12; ++i)
	{
		if (true == m_LaserButton_May[iRandomIndex[i]]->Get_Active())
			continue;
		else
		{
			m_pGameInstance->Stop_Sound(CHANNEL_LASERBUTTON);
			m_pGameInstance->Play_Sound(TEXT("MiniGame_ButtonActivate_Blue.wav"), CHANNEL_LASERTENNIS);

			m_LaserButton_May[iRandomIndex[i]]->Active_Button(true);
			break;
		}
	}
}

void CLaserTennis_Manager::Active_LaserButtonLarge_Gate(_bool bActive)
{
	for (auto& pGate : m_LaserButtonLarge_Gate)
	{
		if (bActive == pGate->Get_Active())
		{
			pGate->Change_State();
		}
	}
}

void CLaserTennis_Manager::Active_LaserButtonLarge_Gate(_bool bActive, TARGET eTarget)
{
	_uint	iStart = 0;
	_uint	iEnd = 0;
	if (eTarget == CLaserTennis_Manager::TARGET_CODY)
	{
		iStart = 0;
		iEnd = iStart + 4;
	}
	else
	{
		iStart = 4;
		iEnd = iStart + 4;
	}
	for (; iStart < iEnd; ++iStart)
	{
		if (bActive == m_LaserButtonLarge_Gate[iStart]->Get_Active())
			m_LaserButtonLarge_Gate[iStart]->Change_State();
	}
}

void CLaserTennis_Manager::Active_LaserButtonLarge(_bool bActive)
{
	m_pGameInstance->Stop_Sound(CHANNEL_LASERBUTTONLARGE);
	m_pGameInstance->Play_Sound(TEXT("MiniGame_ButtonActivate_Center.wav"), CHANNEL_LASERBUTTONLARGE);

	m_pLaserButtonLarge[0]->Activation(bActive);
	m_pLaserButtonLarge[1]->Activation(bActive);
}

void CLaserTennis_Manager::Start_Game()
{
	m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
	m_pGameInstance->Play_Sound(TEXT("StartButton_Push.wav"), CHANNEL_LASERPOWERCOORD);

	/* 파워코드 상태변경 */
	for (_uint i = 0; i < 2; ++i)
		m_pLaserPowerCoord[i]->Change_State();

	/* 레이저 생성기 생성 */
	Active_LaserActivation(1);
	Active_LaserActivation(5);

	/* 충돌 벽 생성 */
	Active_CollisionWall();

	/* 타이머 세팅 */
	m_pTimer_LaserTennis->OnOff_Timer(true);
}

void CLaserTennis_Manager::Reset_Game()
{
	m_pGameInstance->Stop_Sound(CHANNEL_LASERACTIVATION);
	m_pGameInstance->Stop_Sound(CHANNEL_LASERBUTTON);
	m_pGameInstance->Stop_Sound(CHANNEL_LASERBUTTONLARGE);
	m_pGameInstance->Stop_Sound(CHANNEL_LASERPOWERCOORD);
	m_pGameInstance->Stop_Sound(CHANNEL_LASERTENNIS);

	/* 결과창 UI 생성 */
	Create_ResultUI();

	/* 뷰포트 쪼개기 */
	m_pGameInstance->Set_GoalViewportInfo(XMVectorSet(0.f, 0.f, 0.5, 1.f), XMVectorSet(0.5f, 0.f, 0.5f, 1.f), 3.f);

	/* 충돌 벽 비활성화 */
	Active_CollisionWall();

	/* 레이저버튼 비활성화 */
	for (auto& pLaserButton : m_LaserButton_Cody)
		pLaserButton->Active_Button(false);
	for (auto& pLaserButton : m_LaserButton_May)
		pLaserButton->Active_Button(false);

	/* 레이저 생성기 비활성화 */
	for (auto& pLasaerActivaion : m_LaserActivation)
	{
		if(true == pLasaerActivaion->Get_Active())
			pLasaerActivaion->Change_State();
	}

	/* 큰 레이저버튼 비활성화 */
	for (_uint i = 0; i < 2; ++i)
	{
		if (true == m_pLaserButtonLarge[i]->Get_Active())
			m_pLaserButtonLarge[i]->Activation(false);
	}

	/* 파워코드 초기화 */
	for (_uint i = 0; i < 2; ++i)
		m_pLaserPowerCoord[i]->Change_State();

	/* 타이머 종료 */
	m_pTimer_LaserTennis->OnOff_Timer(false);

	/* 변수 초기화 */
	ZeroMemory(m_bKeyCheck, sizeof(m_bKeyCheck));
	m_bStartGame = false;
	m_bReady = false;
	m_iPowerCoordCount = 0;

	m_iCodyCount = 0;
	m_iMayCount = 0;
}

HRESULT CLaserTennis_Manager::Add_LaserActivation(CLaserActivation * pLaserActivation)
{
	if (nullptr == pLaserActivation || 7 <= m_LaserActivation.size())
		return E_FAIL;

	m_LaserActivation.emplace_back(pLaserActivation);

	if (7 <= m_LaserActivation.size())
	{
		for (auto& pLaserActivation : m_LaserActivation)
		{
			if (0 == pLaserActivation->Get_Option()
				|| 1 == pLaserActivation->Get_Option()
				|| 2 == pLaserActivation->Get_Option()
				|| 3 == pLaserActivation->Get_Option())
				m_LaserActivation_Cody.emplace_back(pLaserActivation);

			if (3 == pLaserActivation->Get_Option()
				|| 4 == pLaserActivation->Get_Option()
				|| 5 == pLaserActivation->Get_Option()
				|| 6 == pLaserActivation->Get_Option())
				m_LaserActivation_May.emplace_back(pLaserActivation);
		}
	}

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButton(CLaserButton * pLaserActivation)
{
	if (nullptr == pLaserActivation || (12 <= m_LaserButton_Cody.size() && 12 <= m_LaserButton_May.size()))
		return E_FAIL;

	if (0 == pLaserActivation->Get_Option())
		m_LaserButton_Cody.emplace_back(pLaserActivation);
	else
		m_LaserButton_May.emplace_back(pLaserActivation);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserPowerCoord(_uint iIndex, CLaserPowerCoord * pLaserPowerCoord)
{
	if (nullptr == pLaserPowerCoord || 2 <= iIndex)
		return E_FAIL;

	m_pLaserPowerCoord[iIndex] = pLaserPowerCoord;

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButtonLarge(_uint iIndex, CLaserButtonLarge * pLaserButtonLarge)
{
	if (nullptr == pLaserButtonLarge || 2 <= iIndex)
		return E_FAIL;

	m_pLaserButtonLarge[iIndex] = pLaserButtonLarge;

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_LaserButtonLarge_Gate(CLaserButtonLarge_Gate * pLaserButtonLarge_Gate)
{
	if (nullptr == pLaserButtonLarge_Gate || 8 <= m_LaserButtonLarge_Gate.size())
		return E_FAIL;

	m_LaserButtonLarge_Gate.emplace_back(pLaserButtonLarge_Gate);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Add_Wall(CWall_LaserTennis * pWall)
{
	if (nullptr == pWall || 5 <= m_Wall.size())
		return E_FAIL;

	m_Wall.emplace_back(pWall);

	return S_OK;
}

HRESULT CLaserTennis_Manager::Create_StartUI()
{
	/* UI 다 띄우면 Ready = true */

	m_bReady = true;
	return S_OK;
}

HRESULT CLaserTennis_Manager::Create_ResultUI()
{
	return S_OK;
}

void CLaserTennis_Manager::Active_LaserActivation(_uint iOption)
{
	for (auto& pLaserActivation : m_LaserActivation)
	{
		if (iOption == pLaserActivation->Get_Option())
		{
			/* Sound */
			m_pGameInstance->Stop_Sound(CHANNEL_LASERACTIVATION);
			m_pGameInstance->Play_Sound(TEXT("MiniGame_RisingBlock.wav"), CHANNEL_LASERACTIVATION);

			pLaserActivation->Change_State();
		}
	}
}

void CLaserTennis_Manager::Active_CollisionWall()
{
	for (auto& pWall : m_Wall)
		pWall->Change_State();
}

void CLaserTennis_Manager::Free()
{
	Safe_Release(m_pGameInstance);
	Safe_Delete_Array(m_pRandomArr);
}
